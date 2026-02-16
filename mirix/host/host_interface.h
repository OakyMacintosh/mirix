#ifndef MIRIX_HOST_INTERFACE_H
#define MIRIX_HOST_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

// Forward declarations
struct kevent;

// Host interface API
int host_interface_init(void);
void host_interface_cleanup(void);
int host_interface_poll_events(void);

// File descriptor monitoring
int host_interface_monitor_fd(int fd, void (*callback)(int fd, void *data), void *data);
int host_interface_unmonitor_fd(int fd);

// Timer management
int host_interface_create_timer(uint64_t interval_ms, bool periodic, 
                              void (*callback)(void *data), void *data);

// Internal event handlers
static void handle_read_event(struct kevent *event);
static void handle_write_event(struct kevent *event);
static void handle_timer_event(struct kevent *event);

#endif // MIRIX_HOST_INTERFACE_H
