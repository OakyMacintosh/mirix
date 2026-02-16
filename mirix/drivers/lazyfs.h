#ifndef LAZYFS_H
#define LAZYFS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>

// LazyFS - Case-sensitive filesystem with symbolic link support for MIRIX

// File system node types
typedef enum {
    LAZYFS_FILE,
    LAZYFS_DIRECTORY,
    LAZYFS_SYMLINK
} lazyfs_node_type_t;

// File system node structure
typedef struct lazyfs_node {
    char *name;
    lazyfs_node_type_t type;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    size_t size;
    time_t mtime;
    time_t ctime;
    
    union {
        struct {
            char *data;
            size_t capacity;
        } file_data;
        
        struct {
            struct lazyfs_node **children;
            size_t child_count;
            size_t child_capacity;
        } dir_data;
        
        struct {
            char *target;
        } symlink_data;
    };
    
    struct lazyfs_node *parent;
} lazyfs_node_t;

// LazyFS superblock
typedef struct lazyfs_superblock {
    lazyfs_node_t *root;
    size_t total_nodes;
    size_t max_nodes;
    bool case_sensitive;
} lazyfs_superblock_t;

// LazyFS operations
int lazyfs_init(bool case_sensitive);
void lazyfs_cleanup(void);

// File operations
int lazyfs_create(const char *path, mode_t mode);
int lazyfs_mkdir(const char *path, mode_t mode);
int lazyfs_symlink(const char *target, const char *linkpath);
ssize_t lazyfs_readlink(const char *path, char *buf, size_t bufsiz);
ssize_t lazyfs_read(const char *path, void *buf, size_t count, off_t offset);
ssize_t lazyfs_write(const char *path, const void *buf, size_t count, off_t offset);
int lazyfs_stat(const char *path, struct stat *st);

#endif // LAZYFS_H
