/*
 * Mach Error String Implementation
 */

#include <stdio.h>
#include <string.h>
#include "mach/mach_error.h"
#include "mach/mach.h"

static const struct {
    kern_return_t code;
    const char *name;
    const char *description;
} mach_errors[] = {
    {KERN_SUCCESS, "KERN_SUCCESS", "Operation succeeded"},
    {KERN_INVALID_ADDRESS, "KERN_INVALID_ADDRESS", "Specified address is not currently valid"},
    {KERN_PROTECTION_FAILURE, "KERN_PROTECTION_FAILURE", "Specified memory is valid, but does not permit the required forms of access"},
    {KERN_NO_SPACE, "KERN_NO_SPACE", "The address range specified is already in use, or no address range of the size specified could be found"},
    {KERN_INVALID_ARGUMENT, "KERN_INVALID_ARGUMENT", "The argument specified is not in range or is invalid"},
    {KERN_FAILURE, "KERN_FAILURE", "The operation could not be completed"},
    {KERN_RESOURCE_SHORTAGE, "KERN_RESOURCE_SHORTAGE", "A system resource could not be allocated to complete this operation"},
    {KERN_NOT_RECEIVER, "KERN_NOT_RECEIVER", "The attempt to locate the port failed"},
    {KERN_NO_ACCESS, "KERN_NO_ACCESS", "The target port is not a send rights port for this task"},
    {KERN_MEMORY_FAILURE, "KERN_MEMORY_FAILURE", "A memory error occurred"},
    {KERN_MEMORY_ERROR, "KERN_MEMORY_ERROR", "Another memory error occurred"},
    {KERN_ALREADY_IN_SET, "KERN_ALREADY_IN_SET", "The receive right is already a member of a port set"},
    {KERN_NOT_IN_SET, "KERN_NOT_IN_SET", "The receive right is not a member of a port set"},
    {KERN_NAME_EXISTS, "KERN_NAME_EXISTS", "The name already denotes a right in the task"},
    {KERN_ABORTED, "KERN_ABORTED", "The operation was aborted"},
    {KERN_INVALID_NAME, "KERN_INVALID_NAME", "The name doesn't denote a right in the task"},
    {KERN_INVALID_TASK, "KERN_INVALID_TASK", "The task specified is invalid"},
    {KERN_INVALID_RIGHT, "KERN_INVALID_RIGHT", "The right specified is invalid"},
    {KERN_INVALID_VALUE, "KERN_INVALID_VALUE", "Illegal value supplied by caller"},
    {KERN_UREFS, "KERN_UREFS", "Operation on object with UNIX-restricted fields"},
    {KERN_INVALID_CAPABILITY, "KERN_INVALID_CAPABILITY", "The capability is invalid"},
    {KERN_RIGHT_EXISTS, "KERN_RIGHT_EXISTS", "The right is already present in the task"},
    {KERN_INVALID_HOST, "KERN_INVALID_HOST", "Host specified is invalid"},
    {KERN_MEMORY_PRESENT, "KERN_MEMORY_PRESENT", "Memory object present"},
    {KERN_MEMORY_DATA_MOVED, "KERN_MEMORY_DATA_MOVED", "Memory object data was moved"},
    {KERN_MEMORY_RESTART_COPY, "KERN_MEMORY_RESTART_COPY", "Memory object data was restarted"},
    {KERN_INVALID_PROCESSOR_SET, "KERN_INVALID_PROCESSOR_SET", "Processor set is invalid"},
    {KERN_POLICY_LIMIT, "KERN_POLICY_LIMIT", "Exceeded policy limit"},
    {KERN_INVALID_POLICY, "KERN_INVALID_POLICY", "Invalid policy"},
    {KERN_INVALID_OBJECT, "KERN_INVALID_OBJECT", "Invalid object"},
    {KERN_ALREADY_WAITING, "KERN_ALREADY_WAITING", "Already waiting on an event"},
    {KERN_ALREADY_SET, "KERN_ALREADY_SET", "Already set"},
    {KERN_NOT_SET, "KERN_NOT_SET", "Not set"},
    {KERN_NOT_LOCKED, "KERN_NOT_LOCKED", "Not locked"},
    {KERN_NOT_UNLOCKED, "KERN_NOT_UNLOCKED", "Not unlocked"},
    {KERN_EXISTS, "KERN_EXISTS", "Already exists"},
    {KERN_TERMINATED, "KERN_TERMINATED", "Object was terminated"},
    {KERN_SET_ATTRIBUTES, "KERN_SET_ATTRIBUTES", "Unable to set object attributes"},
    {KERN_NOT_SUPPORTED, "KERN_NOT_SUPPORTED", "Operation not supported"},
    {KERN_NO_DATA, "KERN_NO_DATA", "No data available"},
    {KERN_INVALID_RIGHT, "KERN_INVALID_RIGHT", "Invalid right"},
    {KERN_INVALID_CAPABILITY, "KERN_INVALID_CAPABILITY", "Invalid capability"},
    {KERN_POLICY_STATIC, "KERN_POLICY_STATIC", "Policy is static"},
};

const char *mach_error_string(kern_return_t error_code) {
    for (size_t i = 0; i < sizeof(mach_errors) / sizeof(mach_errors[0]); i++) {
        if (mach_errors[i].code == error_code) {
            return mach_errors[i].description;
        }
    }
    
    // Fallback for unknown errors
    static char unknown_error[64];
    snprintf(unknown_error, sizeof(unknown_error), "Unknown Mach error: %d", error_code);
    return unknown_error;
}
