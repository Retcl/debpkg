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
#include "../include/extract.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int extract_control(const char *deb_path, const char *extract_dir) {
    char cmd[MAX_CMD_LEN];
    
    snprintf(cmd, sizeof(cmd), "dpkg-deb --control \"%s\" \"%s\"", deb_path, extract_dir);
    
    print_info("Extracting control information...");
    if (execute_command(cmd) != 0) {
        print_error("Failed to extract control information");
        return -1;
    }
    
    return 0;
}

int extract_data(const char *deb_path, const char *extract_dir) {
    char cmd[MAX_CMD_LEN];
    
    snprintf(cmd, sizeof(cmd), "dpkg-deb -x \"%s\" \"%s\"", deb_path, extract_dir);
    
    print_info("Extracting data files...");
    if (execute_command(cmd) != 0) {
        print_error("Failed to extract data files");
        return -1;
    }
    
    return 0;
}

int parse_package_name(const char *control_path, char *pkg_name, size_t size) {
    FILE *fp = fopen(control_path, "r");
    if (fp == NULL) {
        print_error("Cannot open control file");
        return -1;
    }
    
    char line[1024];
    pkg_name[0] = '\0';
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Package:", 8) == 0) {
            // 跳过 "Package: " 前缀
            char *name = line + 8;
            while (*name == ' ' || *name == '\t') name++;
            
            // 移除换行符
            char *newline = strchr(name, '\n');
            if (newline) *newline = '\0';
            
            strncpy(pkg_name, name, size - 1);
            pkg_name[size - 1] = '\0';
            break;
        }
    }
    
    fclose(fp);
    
    if (pkg_name[0] == '\0') {
        print_error("Cannot find package name in control file");
        return -1;
    }
    
    return 0;
}
