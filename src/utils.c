#define _GNU_SOURCE
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>

void print_info(const char *msg) {
    printf(COLOR_BLUE "[INFO] " COLOR_RESET "%s\n", msg);
}

void print_success(const char *msg) {
    printf(COLOR_GREEN "[SUCCESS] " COLOR_RESET "%s\n", msg);
}

void print_warning(const char *msg) {
    printf(COLOR_YELLOW "[WARNING] " COLOR_RESET "%s\n", msg);
}

void print_error(const char *msg) {
    fprintf(stderr, COLOR_RED "[ERROR] " COLOR_RESET "%s\n", msg);
}

int get_home_dir(char *buffer, size_t size) {
    const char *home = getenv("HOME");
    if (home == NULL) {
        struct passwd *pw = getpwuid(getuid());
        if (pw == NULL) {
            return -1;
        }
        home = pw->pw_dir;
    }
    
    if (strlen(home) >= size) {
        return -1;
    }
    
    strcpy(buffer, home);
    return 0;
}

int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

int create_directories(const char *path) {
    char tmp[MAX_PATH_LEN];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    
    // 移除末尾的斜杠
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }
    
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
        return -1;
    }
    
    return 0;
}

int execute_command(const char *cmd) {
    int ret = system(cmd);
    if (ret == -1) {
        print_error("Failed to execute command");
        return -1;
    }
    return WEXITSTATUS(ret);
}
