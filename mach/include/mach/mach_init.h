#ifndef MACH_INIT_H
#define MACH_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Mach initialization functions
kern_return_t mach_init(void);
kern_return_t mach_init_with_ports(mach_port_t bootstrap_port, mach_port_t exception_port);

#ifdef __cplusplus
}
#endif

#endif /* MACH_INIT_H */
