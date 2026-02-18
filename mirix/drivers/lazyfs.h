#ifndef LAZYFS_H
#define LAZYFS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h> // For uint32_t, etc.

#define UNUSED(x) (void)(x)

// LazyFS - Case-sensitive filesystem with symbolic link support for MIRIX

// Constants for filesystem structure
#define LAZYFS_MAGIC           0xDEADC0DE // Magic number for lazyfs
#define LAZYFS_VERSION         1          // Current filesystem version
#define LAZYFS_BLOCK_SIZE      4096       // 4KB blocks
#define LAZYFS_MAX_NAME_LEN    255        // Max length for a file/directory name
#define LAZYFS_MAX_PATH_LEN    1024       // Max length for a full path
#define LAZYFS_MAX_DIRECT_BLOCKS 12       // Number of direct data blocks in inode
#define LAZYFS_INODES_PER_BLOCK (LAZYFS_BLOCK_SIZE / sizeof(struct lazyfs_inode))
#define LAZYFS_DIRENTS_PER_BLOCK (LAZYFS_BLOCK_SIZE / sizeof(lazyfs_dirent_t))

// File system node types
typedef enum {
    LAZYFS_UNUSED = 0, // Inode is unused
    LAZYFS_FILE,
    LAZYFS_DIRECTORY,
    LAZYFS_SYMLINK
} lazyfs_node_type_t;

// Directory entry structure
typedef struct {
    uint32_t        inode_num;                 // Inode number of the entry
    char            name[LAZYFS_MAX_NAME_LEN + 1]; // Name of the file/directory
} lazyfs_dirent_t;

// Inode structure (replaces lazyfs_node)
typedef struct lazyfs_inode {
    lazyfs_node_type_t type;        // Type of node (file, directory, symlink)
    mode_t             mode;        // Permissions and file type
    uid_t              uid;         // User ID of owner
    gid_t              gid;         // Group ID of owner
    uint32_t           link_count;  // Number of hard links to this inode
    size_t             size;        // Size of the file in bytes
    time_t             atime;       // Last access time
    time_t             mtime;       // Last modification time
    time_t             ctime;       // Creation time

    uint32_t           data_blocks[LAZYFS_MAX_DIRECT_BLOCKS]; // Direct data block numbers
    // Future: indirect blocks, double indirect, etc.
} lazyfs_inode_t;

// LazyFS superblock
typedef struct lazyfs_superblock {
    uint32_t    magic;              // Magic number to identify lazyfs
    uint32_t    version;            // Filesystem version
    uint32_t    block_size;         // Size of a data block
    uint32_t    total_blocks;       // Total number of blocks in the filesystem
    uint32_t    free_blocks;        // Number of currently free blocks
    uint32_t    inode_count;        // Total number of inodes
    uint32_t    free_inodes;        // Number of currently free inodes
    uint32_t    inode_bitmap_block; // Block number where the inode bitmap starts
    uint32_t    block_bitmap_block; // Block number where the data block bitmap starts
    uint32_t    inode_table_block;  // Block number where the inode table starts
    uint32_t    root_inode_num;     // The inode number of the root directory

    bool        case_sensitive;     // Is the filesystem case-sensitive?
    int         backing_file_fd;    // File descriptor for the backing file
    char        backing_file_path[LAZYFS_MAX_PATH_LEN + 1]; // Path to the backing file
} lazyfs_superblock_t;

// LazyFS operations (inode-based, no longer path-based for internal)
int lazyfs_init(const char *backing_file_path, bool case_sensitive);
void lazyfs_cleanup(void);

// Internal helper functions (will be implemented in lazyfs.c)
lazyfs_inode_t* lazyfs_get_inode(uint32_t inode_num);
int lazyfs_put_inode(uint32_t inode_num, lazyfs_inode_t *inode);
uint32_t lazyfs_alloc_inode(lazyfs_node_type_t type, mode_t mode, uid_t uid, gid_t gid);
void lazyfs_free_inode(uint32_t inode_num);
uint32_t lazyfs_alloc_block(void);
void lazyfs_free_block(uint32_t block_num);
int lazyfs_read_block(uint32_t block_num, void *buf);
int lazyfs_write_block(uint32_t block_num, const void *buf);
lazyfs_inode_t* lazyfs_namei(const char *path); // Name to inode (path resolution)

// File operations (path-based for external interface, will use lazyfs_namei internally)
int lazyfs_create(const char *path, mode_t mode);
int lazyfs_mkdir(const char *path, mode_t mode);
int lazyfs_symlink(const char *target, const char *linkpath);
ssize_t lazyfs_readlink(const char *path, char *buf, size_t bufsiz);
ssize_t lazyfs_read(const char *path, void *buf, size_t count, off_t offset);
ssize_t lazyfs_write(const char *path, const void *buf, size_t count, off_t offset);
int lazyfs_stat(const char *path, struct stat *st);
int lazyfs_unlink(const char *path);
int lazyfs_rmdir(const char *path);
int lazyfs_rename(const char *oldpath, const char *newpath);

#endif // LAZYFS_H
