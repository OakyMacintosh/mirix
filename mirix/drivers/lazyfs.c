#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>
#include <strings.h> // For strcasecmp
#include <math.h> // For ceil

#include "lazyfs.h"

// Constants
#define LAZYFS_SUPERBLOCK_BLOCK 0
#define LAZYFS_MAGIC_NUMBER     0xDEADC0DE // Same as in lazyfs.h

// --- Forward Declarations for Internal Helpers ---
// Block I/O
__attribute__((unused)) static int lazyfs_read_block_internal(uint32_t block_num, void *buf);
__attribute__((unused)) static int lazyfs_write_block_internal(uint32_t block_num, const void *buf);

// Bitmap operations
__attribute__((unused)) static int lazyfs_set_bit(uint32_t bitmap_block_num, uint32_t bit_index, bool value);
__attribute__((unused)) static bool lazyfs_get_bit(uint32_t bitmap_block_num, uint32_t bit_index);

// Inode operations
__attribute__((unused)) static uint32_t lazyfs_alloc_inode_internal(lazyfs_node_type_t type, mode_t mode, uid_t uid, gid_t gid);
__attribute__((unused)) static void lazyfs_free_inode_internal(uint32_t inode_num);
__attribute__((unused)) static lazyfs_inode_t* lazyfs_get_inode_internal(uint32_t inode_num);
__attribute__((unused)) static int lazyfs_put_inode_internal(uint32_t inode_num, lazyfs_inode_t *inode);

// Block allocation
__attribute__((unused)) static uint32_t lazyfs_alloc_block_internal(void);
__attribute__((unused)) static void lazyfs_free_block_internal(uint32_t block_num);

// Directory operations
__attribute__((unused)) static int lazyfs_add_dirent(uint32_t parent_inode_num, uint32_t child_inode_num, const char *name);
__attribute__((unused)) static int lazyfs_remove_dirent(uint32_t parent_inode_num, const char *name);
__attribute__((unused)) static int lazyfs_find_dirent(uint32_t parent_inode_num, const char *name, uint32_t *found_inode_num);

// Path resolution
__attribute__((unused)) static uint32_t lazyfs_walk_path(const char *path, uint32_t *parent_inode_num, char *last_component);
__attribute__((unused)) static lazyfs_inode_t* lazyfs_namei_internal(const char *path);

// LazyFS - Case-sensitive filesystem with symbolic link support for MIRIX

// LazyFS superblock
static lazyfs_superblock_t fs_superblock = {
    .magic = 0,
    .version = 0,
    .block_size = 0,
    .total_blocks = 0,
    .free_blocks = 0,
    .inode_count = 0,
    .free_inodes = 0,
    .inode_bitmap_block = 0,
    .block_bitmap_block = 0,
    .inode_table_block = 0,
    .root_inode_num = 0,
    .case_sensitive = false,
    .backing_file_fd = -1,
    .backing_file_path = {0} // Initialize fixed-size array
};

// Helper to save superblock
static int lazyfs_save_superblock(void) {
    if (fs_superblock.backing_file_fd == -1) {
        return -1; // No backing file
    }
    lseek(fs_superblock.backing_file_fd, 0, SEEK_SET);
    if (write(fs_superblock.backing_file_fd, &fs_superblock, sizeof(lazyfs_superblock_t)) != sizeof(lazyfs_superblock_t)) {
        perror("lazyfs_save_superblock: write failed");
        return -1;
    }
    return 0;
}

// --- Internal Helper Function Implementations ---

// Block I/O
__attribute__((unused)) static int lazyfs_read_block_internal(uint32_t block_num, void *buf) {
    UNUSED(block_num); UNUSED(buf);
    printf("lazyfs_read_block_internal: Placeholder for reading block %u\n", block_num);
    return -1; // Not yet implemented
}

__attribute__((unused)) static int lazyfs_write_block_internal(uint32_t block_num, const void *buf) {
    UNUSED(block_num); UNUSED(buf);
    printf("lazyfs_write_block_internal: Placeholder for writing block %u\n", block_num);
    return -1; // Not yet implemented
}

// Bitmap operations
__attribute__((unused)) static int lazyfs_set_bit(uint32_t bitmap_block_num, uint32_t bit_index, bool value) {
    UNUSED(bitmap_block_num); UNUSED(bit_index); UNUSED(value);
    printf("lazyfs_set_bit: Placeholder for setting bit %u in bitmap block %u to %d\n", bit_index, bitmap_block_num, value);
    return -1; // Not yet implemented
}

__attribute__((unused)) static bool lazyfs_get_bit(uint32_t bitmap_block_num, uint32_t bit_index) {
    UNUSED(bitmap_block_num); UNUSED(bit_index);
    printf("lazyfs_get_bit: Placeholder for getting bit %u in bitmap block %u\n", bit_index, bitmap_block_num);
    return false; // Not yet implemented
}

// Inode operations
__attribute__((unused)) static uint32_t lazyfs_alloc_inode_internal(lazyfs_node_type_t type, mode_t mode, uid_t uid, gid_t gid) {
    UNUSED(type); UNUSED(mode); UNUSED(uid); UNUSED(gid);
    printf("lazyfs_alloc_inode_internal: Placeholder for allocating inode (type %d, mode %o)\n", type, mode);
    return 0; // Not yet implemented
}

__attribute__((unused)) static void lazyfs_free_inode_internal(uint32_t inode_num) {
    UNUSED(inode_num);
    printf("lazyfs_free_inode_internal: Placeholder for freeing inode %u\n", inode_num);
    // Not yet implemented
}

__attribute__((unused)) static lazyfs_inode_t* lazyfs_get_inode_internal(uint32_t inode_num) {
    UNUSED(inode_num);
    printf("lazyfs_get_inode_internal: Placeholder for getting inode %u\n", inode_num);
    return NULL; // Not yet implemented
}

__attribute__((unused)) static int lazyfs_put_inode_internal(uint32_t inode_num, lazyfs_inode_t *inode) {
    UNUSED(inode_num); UNUSED(inode);
    printf("lazyfs_put_inode_internal: Placeholder for putting inode %u\n", inode_num);
    return -1; // Not yet implemented
}

// Block allocation
__attribute__((unused)) static uint32_t lazyfs_alloc_block_internal(void) {
    printf("lazyfs_alloc_block_internal: Placeholder for allocating block\n");
    return 0; // Not yet implemented
}

__attribute__((unused)) static void lazyfs_free_block_internal(uint32_t block_num) {
    UNUSED(block_num);
    printf("lazyfs_free_block_internal: Placeholder for freeing block %u\n", block_num);
    // Not yet implemented
}

// Directory operations
__attribute__((unused)) static int lazyfs_add_dirent(uint32_t parent_inode_num, uint32_t child_inode_num, const char *name) {
    UNUSED(parent_inode_num); UNUSED(child_inode_num); UNUSED(name);
    return -1; // Not yet implemented
}
__attribute__((unused)) static int lazyfs_remove_dirent(uint32_t parent_inode_num, const char *name) {
    UNUSED(parent_inode_num); UNUSED(name);
    return -1; // Not yet implemented
}
__attribute__((unused)) static int lazyfs_find_dirent(uint32_t parent_inode_num, const char *name, uint32_t *found_inode_num) {
    UNUSED(parent_inode_num); UNUSED(name); UNUSED(found_inode_num);
    return -1; // Not yet implemented
}

// Path resolution
__attribute__((unused)) static uint32_t lazyfs_walk_path(const char *path, uint32_t *parent_inode_num, char *last_component) {
    UNUSED(path); UNUSED(parent_inode_num); UNUSED(last_component);
    return 0; // Not yet implemented
}
__attribute__((unused)) static lazyfs_inode_t* lazyfs_namei_internal(const char *path) {
    UNUSED(path);
    return NULL; // Not yet implemented
}

// Initialize LazyFS
int lazyfs_init(const char *backing_file_path, bool case_sensitive) {
    // Initialize superblock (clear it first)
            memset(&fs_superblock, 0, sizeof(fs_superblock));
            fs_superblock.case_sensitive = case_sensitive;
            fs_superblock.inode_count = 1024; // Maximum 1024 nodes
            fs_superblock.free_inodes = 1024; // All inodes are free initially
            fs_superblock.backing_file_fd = -1; // Initialize fd to invalid
    if (backing_file_path == NULL) {
        // In-memory only mode (for testing/simple cases)
        printf("lazyfs_init: Initializing in-memory mode.\n");
        // Create root directory
        // For now, root remains NULL as node allocation is not implemented
        return 0;
    }

    strncpy(fs_superblock.backing_file_path, backing_file_path, LAZYFS_MAX_PATH_LEN);
    fs_superblock.backing_file_path[LAZYFS_MAX_PATH_LEN] = '\0'; // Ensure null-termination

    if (fs_superblock.backing_file_path[0] == '\0' && backing_file_path != NULL && backing_file_path[0] != '\0') { // Check if strncpy failed to copy
        perror("lazyfs_init: strncpy failed"); // More accurate error message
        return -1;
    }

    // Open or create the backing file
    fs_superblock.backing_file_fd = open(backing_file_path, O_RDWR | O_CREAT, 0644);
            if (fs_superblock.backing_file_fd == -1) {
                perror("lazyfs_init: failed to open backing file");
                // The backing_file_path is a fixed-size array, so we clear it instead of freeing/NULLing
                fs_superblock.backing_file_path[0] = '\0';
                return -1;
            }
    // Try to read existing superblock
    if (read(fs_superblock.backing_file_fd, &fs_superblock, sizeof(lazyfs_superblock_t)) == sizeof(lazyfs_superblock_t)) {
        printf("lazyfs_init: Loaded existing filesystem from '%s'.\n", backing_file_path);
        // Root node needs to be re-pointed correctly if serialized
        // For now, assume it's valid if read succeeds, will need more complex deserialization
            } else {
                // New filesystem, or failed to read. Initialize for new.
                printf("lazyfs_init: Creating new filesystem in '%s'.\n", backing_file_path);
                fs_superblock.inode_count = 0; // Initialize inode count for new FS
                fs_superblock.free_inodes = 0; // Initialize free inode count
                fs_superblock.root_inode_num = 0; // No root inode yet, will be created by lazyfs_mkdir("/")
                        if (lazyfs_save_superblock() == -1) {
                            close(fs_superblock.backing_file_fd);
                            fs_superblock.backing_file_path[0] = '\0'; // Clear the path
                            fs_superblock.backing_file_fd = -1;
                            return -1;
                        }    }
    
    printf("lazyfs_init: Filesystem initialized. Case-sensitive: %d\n", case_sensitive);
    return 0;
}

// Cleanup LazyFS
void lazyfs_cleanup(void) {
    if (fs_superblock.backing_file_fd != -1) {
        lazyfs_save_superblock(); // Save before closing
        close(fs_superblock.backing_file_fd);
        fs_superblock.backing_file_fd = -1;
    }
    
    if (fs_superblock.backing_file_path[0] != '\0') {
        // No need to free as it's not dynamically allocated in the updated code
        fs_superblock.backing_file_path[0] = '\0'; // Clear the path
    }

    if (fs_superblock.root_inode_num != 0) { // Check if root inode is set
        // In a real implementation, would recursively free all nodes
        fs_superblock.root_inode_num = 0; // Clear root inode number
    }
    
    memset(&fs_superblock, 0, sizeof(fs_superblock));
    printf("lazyfs_cleanup: Filesystem cleaned up.\n");
}

// Case-sensitive string comparison
__attribute__((unused)) static int lazyfs_strcmp(const char *s1, const char *s2) {
    if (fs_superblock.case_sensitive) {
        return strcmp(s1, s2);
    } else {
        return strcasecmp(s1, s2);
    }
}

// Find node by path
__attribute__((unused)) static lazyfs_inode_t* lazyfs_find_node(const char *path) {
    if (!path || fs_superblock.root_inode_num == 0) return NULL; // Use root_inode_num

    printf("lazyfs_find_node: looking for '%s'\n", path);    
    // Handle absolute path
    if (path[0] == '/') {
                if (strcmp(path, "/") == 0) {
                    return lazyfs_get_inode_internal(fs_superblock.root_inode_num); // Fetch the root inode
                }
        
                // For now, return NULL for other paths
                return NULL;    }
    
    return NULL;
}

// LazyFS operations
int lazyfs_create(const char *path, mode_t mode) {
    printf("lazyfs_create: creating '%s' with mode 0%o\n", path, mode);
    return 0; // Placeholder
}

int lazyfs_mkdir(const char *path, mode_t mode) {
    printf("lazyfs_mkdir: creating directory '%s' with mode 0%o\n", path, mode);
    return 0; // Placeholder
}

int lazyfs_symlink(const char *target, const char *linkpath) {
    printf("lazyfs_symlink: creating symlink '%s' -> '%s'\n", linkpath, target);
    return 0; // Placeholder
}

ssize_t lazyfs_readlink(const char *path, char *buf, size_t bufsiz) {
    UNUSED(buf); UNUSED(bufsiz);
    printf("lazyfs_readlink: reading symlink '%s'\n", path);
    return 0; // Placeholder
}

ssize_t lazyfs_read(const char *path, void *buf, size_t count, off_t offset) {
    UNUSED(buf);
    printf("lazyfs_read: reading '%s' (count=%zu, offset=%lld)\n", 
           path, count, (long long)offset);
    return 0; // Placeholder
}

ssize_t lazyfs_write(const char *path, const void *buf, size_t count, off_t offset) {
    UNUSED(buf);
    printf("lazyfs_write: writing '%s' (count=%zu, offset=%lld)\n", 
           path, count, (long long)offset);
    return count; // Placeholder
}

int lazyfs_stat(const char *path, struct stat *st) {
    UNUSED(st);
    printf("lazyfs_stat: getting status of '%s'\n", path);
    return -1; // Placeholder
}
