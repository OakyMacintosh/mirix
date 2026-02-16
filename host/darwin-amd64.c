#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LEN 4096

static int run_and_capture(const char *cmd, char *output, size_t out_size) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;

    if (!fgets(output, (int)out_size, fp)) {
        pclose(fp);
        return -1;
    }

    pclose(fp);

    // Strip newline
    output[strcspn(output, "\n")] = 0;
    return 0;
}

static int find_tool(const char *tool, char *path, size_t path_size) {
    char cmd[256];

    // First try xcrun (the canonical macOS way)
    snprintf(cmd, sizeof(cmd), "xcrun --find %s 2>/dev/null", tool);
    if (run_and_capture(cmd, path, path_size) == 0 && access(path, X_OK) == 0) {
        return 0;
    }

    // Fallback: search in PATH manually
    const char *env_path = getenv("PATH");
    if (!env_path) return -1;

    char *paths = strdup(env_path);
    if (!paths) return -1;

    char *dir = strtok(paths, ":");
    while (dir) {
        snprintf(path, path_size, "%s/%s", dir, tool);
        if (access(path, X_OK) == 0) {
            free(paths);
            return 0;
        }
        dir = strtok(NULL, ":");
    }

    free(paths);
    return -1;
}

int find_compiler_and_linker(char *cc_path, size_t cc_size,
                             char *ld_path, size_t ld_size) {
    if (find_tool("clang", cc_path, cc_size) != 0)
        return -1;

    if (find_tool("ld", ld_path, ld_size) != 0)
        return -1;

    return 0;
}

