#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#include "host_interface.h"

// Host interface state
static struct {
    int kqueue_fd;
    bool initialized;
} host_state;

// Initialize host interface
int host_interface_init(void) {
    if (host_state.initialized) {
        return 0;
    }
    
    // Create kqueue for event handling
    host_state.kqueue_fd = kqueue();
    if (host_state.kqueue_fd == -1) {
        return -1;
    }
    
    host_state.initialized = true;
    return 0;
}

// Cleanup host interface
void host_interface_cleanup(void) {
    if (host_state.kqueue_fd != -1) {
        close(host_state.kqueue_fd);
        host_state.kqueue_fd = -1;
    }
    host_state.initialized = false;
}

// Poll for host events
int host_interface_poll_events(void) {
    if (!host_state.initialized) {
        return -1;
    }
    
    struct kevent events[10];
    int nev = kevent(host_state.kqueue_fd, NULL, 0, events, 10, NULL);
    
    if (nev == -1) {
        perror("kevent");
        return -1;
    }
    
    for (int i = 0; i < nev; i++) {
        // Handle different types of events
        switch (events[i].filter) {
            case EVFILT_READ:
                handle_read_event(&events[i]);
                break;
            case EVFILT_WRITE:
                handle_write_event(&events[i]);
                break;
            case EVFILT_TIMER:
                handle_timer_event(&events[i]);
                break;
            default:
                printf("Unknown event filter: %d\n", events[i].filter);
                break;
        }
    }
    
    return nev;
}

// Add file descriptor to monitor
int host_interface_monitor_fd(int fd, void (*callback)(int fd, void *data), void *data) {
    if (!host_state.initialized) {
        return -1;
    }
    
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, data);
    
    if (kevent(host_state.kqueue_fd, &kev, 1, NULL, 0, NULL) == -1) {
        perror("kevent add");
        return -1;
    }
    
    return 0;
}

// Remove file descriptor from monitoring
int host_interface_unmonitor_fd(int fd) {
    if (!host_state.initialized) {
        return -1;
    }
    
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    
    if (kevent(host_state.kqueue_fd, &kev, 1, NULL, 0, NULL) == -1) {
        perror("kevent delete");
        return -1;
    }
    
    return 0;
}

// Create timer
int host_interface_create_timer(uint64_t interval_ms, bool periodic, 
                              void (*callback)(void *data), void *data) {
    if (!host_state.initialized) {
        return -1;
    }
    
    struct kevent kev;
    
    // Use kqueue timer instead of timerfd (macOS compatible)
    EV_SET(&kev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, interval_ms, data);
    
    if (kevent(host_state.kqueue_fd, &kev, 1, NULL, 0, NULL) == -1) {
        perror("kevent timer");
        return -1;
    }
    
    return 1; // Return timer ID
}

// Event handlers
static void handle_read_event(struct kevent *event) {
    int fd = event->ident;
    void *data = event->udata;
    
    // Call the registered callback if available
    if (data) {
        // In a real implementation, you'd have a callback registry
        printf("Read event on fd %d\n", fd);
    }
}

static void handle_write_event(struct kevent *event) {
    int fd = event->ident;
    printf("Write event on fd %d\n", fd);
}

static void handle_timer_event(struct kevent *event) {
    int timer_id = event->ident;
    uint64_t count = event->data;
    
    printf("Timer %d triggered %llu times\n", timer_id, (unsigned long long)count);
}
