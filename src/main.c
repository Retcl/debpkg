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
#include "../include/uninstall.h"
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

// 命令类型枚举
typedef enum {
    CMD_NONE,       // 未指定命令
    CMD_INSTALL,    // 安装命令
    CMD_UNINSTALL   // 卸载命令
} CommandType;

int main(int argc, char *argv[]) {
    CommandType cmd = CMD_NONE;
    InstallMode mode = MODE_NONE;
    const char *deb_file = NULL;
    const char *pkg_name = NULL;
    int i;
    
    // 检查是否有命令
    if (argc < 2) {
        print_error("No command specified");
        show_help(argv[0]);
        return 1;
    }
    
    // 解析第一个参数（命令）
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        show_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        show_version();
        return 0;
    } else if (strcmp(argv[1], "install") == 0) {
        cmd = CMD_INSTALL;
    } else if (strcmp(argv[1], "uninstall") == 0 || 
               strcmp(argv[1], "remove") == 0) {
        cmd = CMD_UNINSTALL;
    } else {
        print_error("Unknown command: ");
        fprintf(stderr, "%s\n", argv[1]);
        printf("Use '%s --help' for usage information.\n", argv[0]);
        return 1;
    }
    
    // 根据命令类型解析后续参数
    if (cmd == CMD_INSTALL) {
        // 解析安装命令的参数
        for (i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--system") == 0) {
                mode = MODE_SYSTEM;
            } else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--user") == 0) {
                mode = MODE_USER;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printf("Usage: %s install [OPTIONS] <package.deb>\n", argv[0]);
                printf("\nInstall a DEB package.\n");
                printf("\nOptions:\n");
                printf("  -s, --system      Install system-wide using dpkg (requires sudo)\n");
                printf("  -u, --user        Install to user directory (~/.local)\n");
                printf("  -h, --help        Show this help message\n");
                printf("\n");
                return 0;
            } else if (argv[i][0] != '-') {
                if (deb_file == NULL) {
                    deb_file = argv[i];
                } else {
                    print_error("Multiple DEB files specified");
                    return 1;
                }
            } else {
                print_error("Unknown option: ");
                fprintf(stderr, "%s\n", argv[i]);
                printf("Use '%s install --help' for usage information.\n", argv[0]);
                return 1;
            }
        }
        
        // 检查是否指定了 DEB 文件
        if (deb_file == NULL) {
            print_error("No DEB file specified");
            printf("Usage: %s install [OPTIONS] <package.deb>\n", argv[0]);
            printf("Use '%s install --help' for usage information.\n", argv[0]);
            return 1;
        }
        
        // 检查文件是否存在
        if (!file_exists(deb_file)) {
            print_error("DEB file not found: ");
            fprintf(stderr, "%s\n", deb_file);
            return 1;
        }
        
        // 根据模式执行安装
        if (mode == MODE_SYSTEM) {
            if (install_system(deb_file) != 0) {
                print_error("System installation failed");
                return 1;
            }
        } else if (mode == MODE_USER) {
            if (install_user(deb_file) != 0) {
                print_error("User installation failed");
                return 1;
            }
        } else {
            // 交互式选择
            if (interactive_install(deb_file) != 0) {
                print_error("Installation failed");
                return 1;
            }
        }
        
    } else if (cmd == CMD_UNINSTALL) {
        // 解析卸载命令的参数
        for (i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printf("Usage: %s uninstall [OPTIONS] <package-name>\n", argv[0]);
                printf("\nUninstall a package from user directory.\n");
                printf("\nOptions:\n");
                printf("  -h, --help        Show this help message\n");
                printf("\n");
                return 0;
            } else if (argv[i][0] != '-') {
                if (pkg_name == NULL) {
                    pkg_name = argv[i];
                } else {
                    print_error("Multiple package names specified");
                    return 1;
                }
            } else {
                print_error("Unknown option: ");
                fprintf(stderr, "%s\n", argv[i]);
                printf("Use '%s uninstall --help' for usage information.\n", argv[0]);
                return 1;
            }
        }
        
        // 检查是否指定了包名
        if (pkg_name == NULL) {
            print_error("No package name specified");
            printf("Usage: %s uninstall <package-name>\n", argv[0]);
            printf("Use '%s uninstall --help' for usage information.\n", argv[0]);
            return 1;
        }
        
        char home_dir[MAX_PATH_LEN];
        if (get_home_dir(home_dir, sizeof(home_dir)) != 0) {
            print_error("Cannot get home directory");
            return 1;
        }
        
        UninstallOptions options = {0};
        UninstallResult result;
        
        printf("\n");
        print_info("Uninstalling package: ");
        printf(COLOR_GREEN "%s\n" COLOR_RESET, pkg_name);
        
        if (uninstall_package_smart(pkg_name, home_dir, &options, &result) != 0) {
            print_error("Failed to uninstall package");
            return 1;
        }
    }
    
    return 0;
}
