/*
 *  dosthread.c
 *
 *  This file is part of AED - Aqua/Mirix Kernel Extensions for DOS.
 *
 *  AED is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  AED is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with AED.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Cooperative/preemptive threading for MS-DOS targeting DOS/32A + DJGPP.
 *
 *  Context switching: setjmp/longjmp with manual jmp_buf patching.
 *  Preemption:        timer IRQ 0x08 hooked via raw DPMI 0.9 calls
 *                     (__dpmi_set_protected_mode_interrupt_vector) so the
 *                     code works under DOS/32A's built-in DPMI host as well
 *                     as cwsdpmi.
 *
 *  DJGPP jmp_buf layout (i386 flat, 2.04):
 *    int[0] = ebx
 *    int[1] = esi
 *    int[2] = edi
 *    int[3] = ebp
 *    int[4] = esp   <-- stack pointer
 *    int[5] = eip   <-- instruction pointer
 */

#include "dosthread.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <dos.h>
#include <dpmi.h>       /* __dpmi_*  — standard DJGPP DPMI header           */
#include <go32.h>       /* _go32_my_cs()                                    */

/* -------------------------------------------------------------------------
 * Assembler compatibility macros
 *
 * Intel syntax uses PUSHAD/POPAD; AT&T syntax (used by GNU as on macOS/Linux)
 * uses PUSHAL/POPAL.  Both encode identically — it's purely a mnemonic
 * difference.  We normalise here so the inline asm below works when
 * cross-compiling from macOS with a DJGPP toolchain that drives the host
 * system assembler.
 * ---------------------------------------------------------------------- */

#ifdef __APPLE__
#  define ASM_PUSHAD "pushal"
#  define ASM_POPAD  "popal"
#else
#  define ASM_PUSHAD "pushal"   /* GNU as also accepts pushal on Linux      */
#  define ASM_POPAD  "popal"
#endif

/* -------------------------------------------------------------------------
 * Tunables
 * ---------------------------------------------------------------------- */

#define MAX_THREADS   32
#define STACK_SIZE    (64 * 1024)   /* bytes per thread stack               */

/* -------------------------------------------------------------------------
 * jmp_buf field indices (DJGPP 2.04 i386)
 * If you ever see threads immediately crash on the first switch, these are
 * the first thing to verify against your toolchain's <setjmp.h>.
 * ---------------------------------------------------------------------- */

#define JB_EBX  0
#define JB_ESI  1
#define JB_EDI  2
#define JB_EBP  3
#define JB_ESP  4
#define JB_EIP  5

/* -------------------------------------------------------------------------
 * Thread states
 * ---------------------------------------------------------------------- */

#define TS_FREE     0
#define TS_RUNNING  1
#define TS_READY    2
#define TS_BLOCKED  3   /* waiting on a mutex                               */
#define TS_ZOMBIE   4   /* exited, waiting to be joined                     */

/* -------------------------------------------------------------------------
 * Internal structures
 * ---------------------------------------------------------------------- */

typedef struct {
    int        state;
    jmp_buf    ctx;
    char      *stack;           /* NULL for the main thread                 */
    void      *(*func)(void *);
    void      *arg;
    void      *retval;
    pthread_t  join_waiter;     /* thread blocked in pthread_join on us     */
    int        id;
} thread_t;

/* Concrete mutex — pthread_mutex_t is a pointer to this */
struct dos_mutex {
    volatile int locked;
    pthread_t    owner;
    pthread_t    waiters[MAX_THREADS];
    int          nwaiters;
};

/* -------------------------------------------------------------------------
 * Global state
 * ---------------------------------------------------------------------- */

static thread_t  threads[MAX_THREADS];
static pthread_t current_thread    = 0;
static volatile int scheduler_lock = 0;
static int       threading_inited  = 0;

/* Saved DPMI interrupt descriptor for IRQ 0 (INT 08h) */
static __dpmi_paddr old_timer_vector;

/* -------------------------------------------------------------------------
 * Interrupt enable/disable
 * ---------------------------------------------------------------------- */

static inline void cli(void) { __asm__ volatile ("cli" ::: "memory"); }
static inline void sti(void) { __asm__ volatile ("sti" ::: "memory"); }

/* -------------------------------------------------------------------------
 * 8259 PIC — send non-specific EOI to master
 * ---------------------------------------------------------------------- */

static inline void pic_eoi(void)
{
    outportb(0x20, 0x20);
}

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */

static void scheduler(void);
static void thread_wrapper(void);

/* -------------------------------------------------------------------------
 * Timer ISR
 *
 * We cannot use __attribute__((interrupt)) here because:
 *   a) Older DJGPP/GCC requires the handler to accept a struct pointer arg.
 *   b) GCC refuses to emit 80387 instructions inside an interrupt handler,
 *      which trips even with no floating-point code present.
 *
 * Instead we use a naked asm wrapper that:
 *   1. Saves all general-purpose registers (pushad/popad).
 *   2. Calls timer_isr_body() — a plain C function — for the real work.
 *   3. Restores registers and issues IRET.
 *
 * timer_isr is the address installed into the IDT via DPMI.
 * ---------------------------------------------------------------------- */

/* The actual work, called from the asm wrapper below */
static void timer_isr_body(void)
{
    /*
     * Chain to the original INT 08h handler via a 48-bit far call.
     * The struct must be packed so there is no padding between the
     * 32-bit offset and the 16-bit selector.
     */
    struct { unsigned long off; unsigned short sel; }
        __attribute__((packed)) fp;

    fp.off = old_timer_vector.offset32;
    fp.sel = old_timer_vector.selector;     /* __dpmi_paddr field is 'selector' */

    __asm__ volatile ("lcall *%0" : : "m"(fp) : "memory");

    if (!scheduler_lock)
        scheduler();

    pic_eoi();
}

/* Naked asm ISR wrapper — no C prologue/epilogue generated */
static void timer_isr(void) __attribute__((naked));
static void timer_isr(void)
{
    __asm__ volatile (
        ASM_PUSHAD "\n\t"       /* save all GP registers                    */
        "call %P0\n\t"          /* call timer_isr_body()                    */
        ASM_POPAD  "\n\t"       /* restore all GP registers                 */
        "iret\n\t"              /* return from interrupt                    */
        :
        : "i"(timer_isr_body)
        : "memory"
    );
}

/* -------------------------------------------------------------------------
 * Scheduler
 *
 * Round-robin: find the next READY thread after current_thread and switch
 * to it.  Called either from the timer ISR or directly from pthread_yield().
 * ---------------------------------------------------------------------- */

static void scheduler(void)
{
    scheduler_lock = 1;

    int start = ((int)current_thread + 1) % MAX_THREADS;
    int next  = -1;

    for (int i = 0; i < MAX_THREADS; i++) {
        int idx = (start + i) % MAX_THREADS;
        if (threads[idx].state == TS_READY ||
            threads[idx].state == TS_RUNNING) {
            next = idx;
            break;
        }
    }

    if (next == -1 || next == (int)current_thread) {
        scheduler_lock = 0;
        return;
    }

    int prev       = (int)current_thread;
    current_thread = (pthread_t)next;

    if (threads[prev].state == TS_RUNNING)
        threads[prev].state = TS_READY;
    threads[next].state = TS_RUNNING;

    scheduler_lock = 0;

    /* Save current context; longjmp returns us here later with value 1 */
    if (setjmp(threads[prev].ctx) == 0)
        longjmp(threads[next].ctx, 1);
}

/* -------------------------------------------------------------------------
 * thread_wrapper
 *
 * Entry point executed on the new thread's stack.
 * ---------------------------------------------------------------------- */

static void thread_wrapper(void)
{
    thread_t *t = &threads[current_thread];
    void *ret   = t->func(t->arg);
    pthread_exit(ret);
}

/* -------------------------------------------------------------------------
 * dosthread_init — called lazily on first pthread_create
 * ---------------------------------------------------------------------- */

static void dosthread_init(void)
{
    if (threading_inited)
        return;

    memset(threads, 0, sizeof(threads));

    /* Thread 0 = the main program; inherits the process stack */
    threads[0].state       = TS_RUNNING;
    threads[0].id          = 0;
    threads[0].stack       = NULL;
    threads[0].join_waiter = (pthread_t)-1;
    current_thread         = 0;

    /* Save old INT 08h and install ours */
    __dpmi_get_protected_mode_interrupt_vector(0x08, &old_timer_vector);

    __dpmi_paddr new_vec;
    new_vec.selector = _go32_my_cs();
    new_vec.offset32   = (unsigned long)timer_isr;
    __dpmi_set_protected_mode_interrupt_vector(0x08, &new_vec);

    threading_inited = 1;
}

/* -------------------------------------------------------------------------
 * dosthread_shutdown — restore original timer vector; call before exit()
 * ---------------------------------------------------------------------- */

void dosthread_shutdown(void)
{
    if (!threading_inited)
        return;

    cli();
    __dpmi_set_protected_mode_interrupt_vector(0x08, &old_timer_vector);
    threading_inited = 0;
    sti();
}

/* -------------------------------------------------------------------------
 * pthread_create
 * ---------------------------------------------------------------------- */

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg)
{
    (void)attr;

    cli();

    if (!threading_inited)
        dosthread_init();

    /* Find a free slot (slot 0 is reserved for the main thread) */
    int id = -1;
    for (int i = 1; i < MAX_THREADS; i++) {
        if (threads[i].state == TS_FREE) { id = i; break; }
    }
    if (id == -1) { sti(); return EAGAIN; }

    thread_t *t = &threads[id];
    memset(t, 0, sizeof(*t));

    t->stack = (char *)malloc(STACK_SIZE);
    if (!t->stack) { sti(); return ENOMEM; }

    t->func        = start_routine;
    t->arg         = arg;
    t->id          = id;
    t->join_waiter = (pthread_t)-1;

    /*
     * Bootstrap the thread context.
     *
     * We call setjmp() to fill t->ctx with a valid register snapshot, then
     * overwrite the saved ESP and EIP so that the first longjmp() into this
     * context will:
     *
     *   - Set ESP to the top of the new stack (16-byte aligned, with a
     *     fake zero return address already pushed, matching the i386 ABI
     *     expectation that [esp] == return address on function entry).
     *
     *   - Jump to thread_wrapper.
     *
     * Stack grows downward; base is t->stack + STACK_SIZE.
     */
    char *sp = t->stack + STACK_SIZE;
    sp = (char *)((unsigned long)sp & ~15UL);   /* align to 16 bytes        */
    sp -= 4;
    *(unsigned long *)sp = 0;                   /* fake return address       */

    setjmp(t->ctx);
    ((int *)t->ctx)[JB_ESP] = (int)(unsigned long)sp;
    ((int *)t->ctx)[JB_EIP] = (int)(unsigned long)thread_wrapper;

    t->state = TS_READY;
    *thread  = (pthread_t)id;

    sti();
    return 0;
}

/* -------------------------------------------------------------------------
 * pthread_exit
 * ---------------------------------------------------------------------- */

void pthread_exit(void *retval)
{
    cli();

    thread_t *t = &threads[current_thread];
    t->retval   = retval;
    t->state    = TS_ZOMBIE;

    pthread_t jw = t->join_waiter;
    if (jw != (pthread_t)-1 && threads[jw].state == TS_BLOCKED)
        threads[jw].state = TS_READY;

    sti();

    for (;;)
        pthread_yield();
}

/* -------------------------------------------------------------------------
 * pthread_join
 * ---------------------------------------------------------------------- */

int pthread_join(pthread_t thread, void **retval)
{
    if ((int)thread < 0 || thread >= MAX_THREADS ||
        threads[thread].state == TS_FREE)
        return ESRCH;

    thread_t *target = &threads[thread];

    cli();
    target->join_waiter = current_thread;

    while (target->state != TS_ZOMBIE) {
        threads[current_thread].state = TS_BLOCKED;
        sti();
        pthread_yield();
        cli();
    }
    sti();

    if (retval)
        *retval = target->retval;

    cli();
    free(target->stack);
    target->stack = NULL;
    target->state = TS_FREE;
    sti();

    return 0;
}

/* -------------------------------------------------------------------------
 * pthread_self
 * ---------------------------------------------------------------------- */

pthread_t pthread_self(void)
{
    return current_thread;
}

/* -------------------------------------------------------------------------
 * pthread_yield
 * ---------------------------------------------------------------------- */

void pthread_yield(void)
{
    cli();
    scheduler();
    sti();
}

/* -------------------------------------------------------------------------
 * pthread_mutex_init
 * ---------------------------------------------------------------------- */

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    (void)attr;

    struct dos_mutex *m = (struct dos_mutex *)malloc(sizeof(*m));
    if (!m)
        return ENOMEM;

    m->locked   = 0;
    m->owner    = (pthread_t)-1;
    m->nwaiters = 0;
    *mutex      = (pthread_mutex_t)m;
    return 0;
}

/* -------------------------------------------------------------------------
 * pthread_mutex_destroy
 * ---------------------------------------------------------------------- */

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    if (!mutex || !*mutex)
        return EINVAL;
    free(*mutex);
    *mutex = NULL;
    return 0;
}

/* -------------------------------------------------------------------------
 * pthread_mutex_lock
 * ---------------------------------------------------------------------- */

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (!mutex || !*mutex)
        return EINVAL;

    struct dos_mutex *m = (struct dos_mutex *)*mutex;

    for (;;) {
        cli();

        if (!m->locked) {
            m->locked = 1;
            m->owner  = current_thread;
            sti();
            return 0;
        }

        /* Enqueue ourselves and block until the owner unlocks */
        if (m->nwaiters < MAX_THREADS)
            m->waiters[m->nwaiters++] = current_thread;

        threads[current_thread].state = TS_BLOCKED;
        sti();
        pthread_yield();
    }
}

/* -------------------------------------------------------------------------
 * pthread_mutex_trylock
 * ---------------------------------------------------------------------- */

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    if (!mutex || !*mutex)
        return EINVAL;

    struct dos_mutex *m = (struct dos_mutex *)*mutex;

    cli();
    if (m->locked) { sti(); return EBUSY; }
    m->locked = 1;
    m->owner  = current_thread;
    sti();
    return 0;
}

/* -------------------------------------------------------------------------
 * pthread_mutex_unlock
 * ---------------------------------------------------------------------- */

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    if (!mutex || !*mutex)
        return EINVAL;

    struct dos_mutex *m = (struct dos_mutex *)*mutex;

    cli();

    if (!m->locked || m->owner != current_thread) {
        sti();
        return EPERM;
    }

    m->locked = 0;
    m->owner  = (pthread_t)-1;

    /* Wake the first waiter (FIFO order) */
    if (m->nwaiters > 0) {
        pthread_t w = m->waiters[0];
        for (int i = 0; i < m->nwaiters - 1; i++)
            m->waiters[i] = m->waiters[i + 1];
        m->nwaiters--;

        if (threads[w].state == TS_BLOCKED)
            threads[w].state = TS_READY;
    }

    sti();
    return 0;
}