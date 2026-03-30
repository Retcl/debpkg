/*
 * debpkg - A simple DEB package installer
 * Copyright (C) 2024  debpkg Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
