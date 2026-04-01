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
#include "../include/install_user.h"
#include "../include/extract.h"
#include "../include/utils.h"
#include "../include/depends.h"
#include "../include/desktop.h"
#include "../include/user_deps.h"
#include "../include/env_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int install_user(const char *deb_path) {
    char home_dir[MAX_PATH_LEN];
    char local_dir[MAX_PATH_LEN];
    char extract_dir[MAX_PATH_LEN];
    char control_dir[MAX_PATH_LEN];
    char pkg_name[256];
    char cmd[MAX_CMD_LEN];
    
    if (!file_exists(deb_path)) {
        print_error("DEB file does not exist");
        return -1;
    }
    
    // 获取用户主目录
    if (get_home_dir(home_dir, sizeof(home_dir)) != 0) {
        print_error("Cannot get home directory");
        return -1;
    }
    
    // 创建 ~/.local 目录结构
    snprintf(local_dir, sizeof(local_dir), "%s/.local", home_dir);
    if (!is_directory(local_dir)) {
        print_info("Creating ~/.local directory structure...");
        if (create_directories(local_dir) != 0) {
            print_error("Failed to create ~/.local directory");
            return -1;
        }
    }
    
    // 创建临时提取目录
    snprintf(extract_dir, sizeof(extract_dir), "%s/.local/tmp_debpkg_XXXXXX", home_dir);
    if (mkdtemp(extract_dir) == NULL) {
        print_error("Failed to create temporary directory");
        return -1;
    }
    
    snprintf(control_dir, sizeof(control_dir), "%s/control", extract_dir);
    
    print_info("Installing package to user directory...");
    
    // 提取控制信息
    if (extract_control(deb_path, control_dir) != 0) {
        goto cleanup;
    }
    
    // 解析包名
    char control_file[MAX_PATH_LEN];
    snprintf(control_file, sizeof(control_file), "%s/control", control_dir);
    if (parse_package_name(control_file, pkg_name, sizeof(pkg_name)) != 0) {
        goto cleanup;
    }
    
    // 解析包版本
    char pkg_version[64];
    if (parse_package_version(control_file, pkg_version, sizeof(pkg_version)) != 0) {
        print_warning("Failed to parse package version");
        pkg_version[0] = '\0';  // 使用空字符串作为默认值
    }
    
    // 检查并处理依赖
    Dependency *deps = NULL;
    int dep_count = 0;
    
    print_info("Checking dependencies...");
    if (parse_dependencies(control_file, &deps, &dep_count) == 0 && dep_count > 0) {
        // 立即保存依赖关系，防止后续操作破坏内存
        print_info("Saving dependency record immediately after parsing...");
        save_package_dependencies(pkg_name, pkg_version, deps, dep_count, home_dir);
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Found %d dependencies:\n", dep_count);
        
        int missing_deps = 0;
        int version_mismatch = 0;
        
        for (int i = 0; i < dep_count; i++) {
            // 检查系统级安装
            if (check_system_dependency(deps[i].name)) {
                printf("  ✓ %-30s (system)\n", deps[i].name);
                deps[i].installed = 1;
            }
            // 检查用户目录安装（带版本检测）
            else if (check_user_dependency_with_version(deps[i].name, deps[i].version, home_dir)) {
                char ver_str[64];
                if (deps[i].version[0]) {
                    snprintf(ver_str, sizeof(ver_str), " (user, v%s)", deps[i].version);
                } else {
                    snprintf(ver_str, sizeof(ver_str), " (user)");
                }
                printf("  ✓ %-30s%s\n", deps[i].name, ver_str);
                deps[i].installed = 1;
            }
            // 检查是否存在但版本不匹配
            else if (check_user_dependency(deps[i].name, home_dir)) {
                char installed_ver[64];
                if (get_installed_dep_version(deps[i].name, installed_ver, sizeof(installed_ver), home_dir) == 0) {
                    printf(COLOR_YELLOW "  ⚠ %-30s (user, v%s installed, v%s required)" COLOR_RESET "\n", 
                           deps[i].name, installed_ver, deps[i].version);
                    deps[i].installed = 0;
                    version_mismatch++;
                } else {
                    printf("  ✗ %-30s (missing)\n", deps[i].name);
                    deps[i].installed = 0;
                    missing_deps++;
                }
            }
            // 依赖缺失
            else {
                printf("  ✗ %-30s (missing)\n", deps[i].name);
                deps[i].installed = 0;
                missing_deps++;
            }
        }
        
        if (missing_deps > 0 || version_mismatch > 0) {
            if (version_mismatch > 0) {
                printf(COLOR_YELLOW "[WARNING] " COLOR_RESET "%d version mismatch(es):\n", version_mismatch);
            }
            if (missing_deps > 0) {
                printf(COLOR_YELLOW "[WARNING] " COLOR_RESET "%d missing dependencies:\n", missing_deps);
            }
            
            for (int i = 0; i < dep_count; i++) {
                if (!deps[i].installed) {
                    printf("  - %s%s\n", deps[i].name, deps[i].version[0] ? deps[i].version : "");
                }
            }
            
            print_info("Dependency handling options:");
            printf("  1. Auto-download and install dependencies (RECOMMENDED)\n");
            printf("  2. Install missing dependencies manually:\n");
            printf("     System: sudo apt-get install <package-name>\n");
            printf("     User:   ./debpkg -u <package-name>.deb\n");
            printf("  3. Continue installation anyway (may not work properly)\n");
            printf("  4. Cancel installation\n");
            printf("\nEnter your choice [1-4]: ");
            
            char choice;
            if (scanf(" %c", &choice) != 1) {
                print_error("Invalid input");
                goto cleanup;
            }
            
            switch (choice) {
                case '1':
                    // 自动下载安装依赖
                    print_info("Starting automatic dependency installation...");
                    
                    // 调用自动安装函数
                    extern int auto_install_dependencies(Dependency *deps, int count, const char *home_dir);
                    if (auto_install_dependencies(deps, dep_count, home_dir) == 0) {
                        print_success("All dependencies installed successfully!");
                        // 重新检查依赖状态
                        for (int i = 0; i < dep_count; i++) {
                            if (!deps[i].installed) {
                                if (check_system_dependency(deps[i].name)) {
                                    deps[i].installed = 1;
                                } else if (check_user_dependency_with_version(deps[i].name, deps[i].version, home_dir)) {
                                    deps[i].installed = 1;
                                }
                            }
                        }
                    } else {
                        print_warning("Some dependencies failed to install. Continue anyway? (y/n): ");
                        char cont;
                        if (scanf(" %c", &cont) != 1 || cont != 'y' && cont != 'Y') {
                            print_info("Installation cancelled.");
                            goto cleanup;
                        }
                    }
                    break;
                    
                case '2':
                    print_info("Please install the missing dependencies and try again.");
                    goto cleanup;
                    
                case '3':
                    print_warning("Continuing with missing dependencies...");
                    break;
                    
                case '4':
                    print_info("Installation cancelled.");
                    goto cleanup;
                    
                default:
                    print_error("Invalid choice, cancelling installation.");
                    goto cleanup;
            }
        } else {
            print_success("All dependencies satisfied!");
        }
    } else {
        print_info("No dependencies found or unable to parse dependencies.");
    }
    
    free_dependencies(deps, dep_count);
    
    print_info("Package name: ");
    printf(COLOR_GREEN "%s" COLOR_RESET "\n", pkg_name);
    
    // 创建包专用目录（用于存放元数据和配置文件）
    char pkg_data_dir[MAX_PATH_LEN];
    snprintf(pkg_data_dir, sizeof(pkg_data_dir), "%s/.local/share/%s", home_dir, pkg_name);
    
    if (create_directories(pkg_data_dir) != 0) {
        print_error("Failed to create package data directory");
        goto cleanup;
    }
    
    // 提取到临时目录
    char temp_extract_dir[MAX_PATH_LEN];
    snprintf(temp_extract_dir, sizeof(temp_extract_dir), "%s/temp_extract", extract_dir);
    
    if (extract_data(deb_path, temp_extract_dir) != 0) {
        goto cleanup;
    }
    
    print_info("Installing files to standard directories...");
    
    // 用于记录找到的可执行文件路径
    char main_exec_path[MAX_PATH_LEN] = {0};
    
    // 按照标准目录结构分发文件
    
    // 1. 安装可执行文件到 ~/.local/bin/
    char src_bin[MAX_PATH_LEN];
    char dst_bin[MAX_PATH_LEN];
    snprintf(src_bin, sizeof(src_bin), "%s/usr/bin", temp_extract_dir);
    snprintf(dst_bin, sizeof(dst_bin), "%s/.local/bin", home_dir);
    
    if (is_directory(src_bin)) {
        if (!is_directory(dst_bin)) {
            mkdir(dst_bin, 0755);
        }
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Installing binaries to ~/.local/bin/\n");
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f -o -type l | wc -l", src_bin);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_bin, dst_bin);
            execute_command(cmd);
            
            // 设置可执行权限
            snprintf(cmd, sizeof(cmd), 
                "chmod +x \"%s\"/* 2>/dev/null", dst_bin);
            execute_command(cmd);
            
            // 记录主可执行文件路径（假设第一个文件是主程序）
            snprintf(main_exec_path, sizeof(main_exec_path), "%s/.local/bin/%s", home_dir, pkg_name);
        } else {
            printf("  No binaries found in usr/bin/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No usr/bin/ directory in package\n");
        
        // 尝试在其他常见位置查找可执行文件
        // 检查 usr/share/<pkg-name>/bin/ 或 usr/share/<pkg-name>/
        char alt_bin_dirs[][MAX_PATH_LEN] = {
            {0}, // 会被填充
            {0},
            {0}
        };
        snprintf(alt_bin_dirs[0], sizeof(alt_bin_dirs[0]), "%s/usr/share/%s/bin", temp_extract_dir, pkg_name);
        snprintf(alt_bin_dirs[1], sizeof(alt_bin_dirs[1]), "%s/usr/share/%s", temp_extract_dir, pkg_name);
        snprintf(alt_bin_dirs[2], sizeof(alt_bin_dirs[2]), "%s/usr/lib/%s/bin", temp_extract_dir, pkg_name);
        
        for (int i = 0; i < 3; i++) {
            if (is_directory(alt_bin_dirs[i])) {
                printf(COLOR_BLUE "[INFO] " COLOR_RESET "Found binaries in %s/\n", alt_bin_dirs[i]);
                
                // 复制整个目录到用户共享目录
                char src_share[MAX_PATH_LEN];
                char dst_share[MAX_PATH_LEN];
                snprintf(src_share, sizeof(src_share), "%s/usr/share/%s", temp_extract_dir, pkg_name);
                snprintf(dst_share, sizeof(dst_share), "%s/.local/share/%s", home_dir, pkg_name);
                
                if (create_directories(dst_share) != 0) {
                    print_error("Failed to create share directory");
                } else {
                    FILE *fp;
                    snprintf(cmd, sizeof(cmd), 
                        "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_share, dst_share);
                    execute_command(cmd);
                    
                    // 查找可执行文件
                    char find_exec_cmd[MAX_CMD_LEN];
                    snprintf(find_exec_cmd, sizeof(find_exec_cmd), 
                        "find \"%s/bin\" -type f -executable 2>/dev/null | head -1", dst_share);
                    fp = popen(find_exec_cmd, "r");
                    if (fp) {
                        if (fgets(main_exec_path, sizeof(main_exec_path), fp) != NULL) {
                            // 移除末尾换行符
                            size_t len = strlen(main_exec_path);
                            if (len > 0 && main_exec_path[len-1] == '\n') {
                                main_exec_path[len-1] = '\0';
                            }
                        }
                        pclose(fp);
                    }
                    
                    // 创建符号链接到 ~/.local/bin/
                    if (main_exec_path[0] != '\0') {
                        char link_path[MAX_PATH_LEN];
                        snprintf(link_path, sizeof(link_path), "%s/.local/bin/%s", home_dir, pkg_name);
                        
                        if (!is_directory(dst_bin)) {
                            mkdir(dst_bin, 0755);
                        }
                        
                        snprintf(cmd, sizeof(cmd), 
                            "ln -sfv \"%s\" \"%s\" 2>&1", main_exec_path, link_path);
                        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Creating symlink: %s -> %s\n", 
                            link_path, main_exec_path);
                        execute_command(cmd);
                        
                        printf(COLOR_GREEN "[SUCCESS] " COLOR_RESET "Symlink created, you can run '%s' directly\n", pkg_name);
                    }
                }
                break;
            }
        }
    }
    
    // 2. 安装库文件到 ~/.local/lib/
    char src_lib[MAX_PATH_LEN];
    char dst_lib[MAX_PATH_LEN];
    snprintf(src_lib, sizeof(src_lib), "%s/usr/lib", temp_extract_dir);
    snprintf(dst_lib, sizeof(dst_lib), "%s/.local/lib", home_dir);
    
    if (is_directory(src_lib)) {
        if (!is_directory(dst_lib)) {
            mkdir(dst_lib, 0755);
        }
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Installing libraries to ~/.local/lib/\n");
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f | wc -l", src_lib);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_lib, dst_lib);
            execute_command(cmd);
        } else {
            printf("  No libraries found in usr/lib/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No usr/lib/ directory in package\n");
    }
    
    // 3. 安装共享数据到 ~/.local/share/<package-name>/
    char src_share[MAX_PATH_LEN];
    char dst_share[MAX_PATH_LEN];
    snprintf(src_share, sizeof(src_share), "%s/usr/share", temp_extract_dir);
    snprintf(dst_share, sizeof(dst_share), "%s/.local/share/%s", home_dir, pkg_name);
    
    if (is_directory(src_share)) {
        if (!is_directory(dst_share)) {
            create_directories(dst_share);
        }
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Installing shared data to ~/.local/share/%s/\n", pkg_name);
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f | wc -l", src_share);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_share, dst_share);
            execute_command(cmd);
        } else {
            printf("  No shared data found in usr/share/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No usr/share/ directory in package\n");
    }
    
    // 4. 安装头文件到 ~/.local/include/<package-name>/
    char src_include[MAX_PATH_LEN];
    char dst_include[MAX_PATH_LEN];
    snprintf(src_include, sizeof(src_include), "%s/usr/include", temp_extract_dir);
    snprintf(dst_include, sizeof(dst_include), "%s/.local/include/%s", home_dir, pkg_name);
    
    if (is_directory(src_include)) {
        if (!is_directory(dst_include)) {
            create_directories(dst_include);
        }
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Installing headers to ~/.local/include/%s/\n", pkg_name);
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f | wc -l", src_include);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_include, dst_include);
            execute_command(cmd);
        } else {
            printf("  No headers found in usr/include/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No usr/include/ directory in package\n");
    }
    
    // 5. 安装配置文件到 ~/.local/etc/<package-name>/
    char src_etc[MAX_PATH_LEN];
    char dst_etc[MAX_PATH_LEN];
    snprintf(src_etc, sizeof(src_etc), "%s/etc", temp_extract_dir);
    snprintf(dst_etc, sizeof(dst_etc), "%s/.local/etc/%s", home_dir, pkg_name);
    
    if (is_directory(src_etc)) {
        if (!is_directory(dst_etc)) {
            create_directories(dst_etc);
        }
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Installing config files to ~/.local/etc/%s/\n", pkg_name);
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f | wc -l", src_etc);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_etc, dst_etc);
            execute_command(cmd);
        } else {
            printf("  No config files found in etc/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No etc/ directory in package\n");
    }
    
    // 6. 安装 man 文档到 ~/.local/share/man/
    char src_man[MAX_PATH_LEN];
    char dst_man[MAX_PATH_LEN];
    snprintf(src_man, sizeof(src_man), "%s/usr/share/man", temp_extract_dir);
    snprintf(dst_man, sizeof(dst_man), "%s/.local/share/man", home_dir);
    
    if (is_directory(src_man)) {
        if (!is_directory(dst_man)) {
            create_directories(dst_man);
        }
        
        print_info("Installing man pages to ~/.local/share/man/");
        
        // 检查源目录是否有文件
        char check_cmd[MAX_CMD_LEN];
        snprintf(check_cmd, sizeof(check_cmd), "find \"%s\" -type f | wc -l", src_man);
        FILE *fp = popen(check_cmd, "r");
        int file_count = 0;
        if (fp) {
            if (fscanf(fp, "%d", &file_count) != 1) file_count = 0;
            pclose(fp);
        }
        
        if (file_count > 0) {
            printf("  Found %d files to install\n", file_count);
            snprintf(cmd, sizeof(cmd), 
                "cp -rfv \"%s\"/* \"%s/\" 2>&1 | sed 's/^/  /'", src_man, dst_man);
            execute_command(cmd);
        } else {
            printf("  No man pages found in usr/share/man/\n");
        }
    } else {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "No usr/share/man/ directory in package\n");
    }
    
    // 保存安装清单
    char manifest_file[MAX_PATH_LEN];
    snprintf(manifest_file, sizeof(manifest_file), "%s/INSTALL_MANIFEST", pkg_data_dir);
    
    FILE *manifest = fopen(manifest_file, "w");
    if (manifest) {
        fprintf(manifest, "# Installation Manifest for %s\n", pkg_name);
        fprintf(manifest, "# Installed at: %s\n", pkg_data_dir);
        fprintf(manifest, "\n");
        if (is_directory(src_bin)) fprintf(manifest, "Binaries: ~/.local/bin/\n");
        if (is_directory(src_lib)) fprintf(manifest, "Libraries: ~/.local/lib/\n");
        if (is_directory(src_share)) fprintf(manifest, "Shared data: ~/.local/share/%s/\n", pkg_name);
        if (is_directory(src_include)) fprintf(manifest, "Headers: ~/.local/include/%s/\n", pkg_name);
        if (is_directory(src_etc)) fprintf(manifest, "Config files: ~/.local/etc/%s/\n", pkg_name);
        if (is_directory(src_man)) fprintf(manifest, "Man pages: ~/.local/share/man/\n");
        fclose(manifest);
    }
    
    // 创建 desktop 文件
    print_info("Creating desktop file...");
    if (main_exec_path[0] != '\0') {
        // 使用找到的实际可执行文件路径
        if (create_desktop_file_with_exec(pkg_name, home_dir, main_exec_path) == 0) {
            print_success("Desktop entry created successfully!");
            printf(COLOR_BLUE "[INFO] " COLOR_RESET "Exec: %s\n", main_exec_path);
        } else {
            print_warning("Failed to create desktop file");
        }
    } else if (is_directory(dst_bin)) {
        // 使用默认路径 ~/.local/bin/<pkg-name>
        if (create_desktop_file(pkg_name, home_dir) == 0) {
            print_success("Desktop entry created successfully!");
        } else {
            print_warning("Failed to create desktop file");
        }
    } else {
        print_warning("No executable found, skipping desktop file creation");
    }
    
    // 保存依赖关系到数据库（已移至解析后立即保存）
    print_info("Dependency record already saved during parsing");
    
    print_success("Package installed successfully!");
    print_info("Installation summary:");
    printf("  Executables:   ~/.local/bin/\n");
    printf("  Libraries:     ~/.local/lib/\n");
    printf("  Shared data:   ~/.local/share/%s/\n", pkg_name);
    if (is_directory(src_include)) {
        printf("  Headers:       ~/.local/include/%s/\n", pkg_name);
    }
    if (is_directory(src_etc)) {
        printf("  Config files:  ~/.local/etc/%s/\n", pkg_name);
    }
    if (is_directory(src_man)) {
        printf("  Documentation: ~/.local/share/man/\n");
    }
    printf("  Desktop file:  ~/.local/share/applications/%s.desktop\n", pkg_name);
    if (dep_count > 0) {
        printf("  Dependencies:  Recorded in database\n");
    }
    
    // 检查并提示环境变量配置
    print_info("Checking environment configuration...");
    if (check_and_prompt_env_config(home_dir, pkg_name) != 0) {
        print_warning("Failed to check environment configuration");
    }
    
cleanup:
    // 清理临时目录
    snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", extract_dir);
    execute_command(cmd);
    
    return 0;
}
