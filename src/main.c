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
#include "../include/cli.h"
#include "../include/install_system.h"
#include "../include/install_user.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 安装模式枚举
typedef enum {
    MODE_NONE,      // 未指定模式（交互式）
    MODE_SYSTEM,    // 系统级安装
    MODE_USER       // 用户目录安装
} InstallMode;

int main(int argc, char *argv[]) {
    InstallMode mode = MODE_NONE;
    const char *deb_file = NULL;
    int i;
    
    // 解析命令行参数
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            show_version();
            return 0;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--system") == 0) {
            mode = MODE_SYSTEM;
        } else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--user") == 0) {
            mode = MODE_USER;
        } else if (argv[i][0] != '-') {
            if (deb_file == NULL) {
                deb_file = argv[i];
            } else {
                print_error("Multiple DEB files specified");
                return 1;
            }
        } else {
            print_error("Unknown option");
            show_help(argv[0]);
            return 1;
        }
    }
    
    // 检查是否指定了 DEB 文件
    if (deb_file == NULL) {
        print_error("No DEB file specified");
        show_help(argv[0]);
        return 1;
    }
    
    // 检查文件是否存在
    if (!file_exists(deb_file)) {
        print_error("DEB file not found: ");
        fprintf(stderr, "%s\n", deb_file);
        return 1;
    }
    
    // 根据模式执行安装
    switch (mode) {
        case MODE_SYSTEM:
            return install_system(deb_file) == 0 ? 0 : 1;
        case MODE_USER:
            return install_user(deb_file) == 0 ? 0 : 1;
        case MODE_NONE:
            // 未指定模式，使用交互式选择
            return interactive_install(deb_file) == 0 ? 0 : 1;
    }
    
    return 0;
}
