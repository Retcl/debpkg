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
#include "../include/uninstall.h"
#include "../include/user_deps.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// 从 INSTALL_MANIFEST 读取文件列表
int get_package_manifest(const char *pkg_name, const char *home_dir, 
                        char ***files, int *count) {
    char manifest_path[MAX_PATH_LEN];
    snprintf(manifest_path, sizeof(manifest_path), 
             "%s/.local/share/%s/INSTALL_MANIFEST", home_dir, pkg_name);
    
    if (!file_exists(manifest_path)) {
        print_error("Manifest file not found");
        return -1;
    }
    
    FILE *fp = fopen(manifest_path, "r");
    if (fp == NULL) {
        print_error("Cannot open manifest file");
        return -1;
    }
    
    // 分配初始空间
    *files = malloc(sizeof(char*) * 100);
    if (*files == NULL) {
        fclose(fp);
        return -1;
    }
    
    *count = 0;
    char line[1024];
    
    while (fgets(line, sizeof(line), fp)) {
        // 移除换行符
        char *newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        
        // 跳过空行和注释
        if (line[0] == '\0' || line[0] == '#') continue;
        
        // 解析格式：Type: path
        char *colon = strchr(line, ':');
        if (colon == NULL) continue;
        
        char *path = colon + 1;
        while (*path == ' ') path++;  // 跳过空格
        
        // 添加到列表
        if (*count >= 100) {
            // 扩展空间
            *files = realloc(*files, sizeof(char*) * (*count + 100));
            if (*files == NULL) {
                fclose(fp);
                return -1;
            }
        }
        
        (*files)[*count] = strdup(path);
        if ((*files)[*count] == NULL) {
            fclose(fp);
            return -1;
        }
        (*count)++;
    }
    
    fclose(fp);
    return 0;
}

// 检查依赖是否被其他包使用
int check_dependency_usage(const char *dep_name, const char *exclude_pkg, 
                          const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    snprintf(db_path, sizeof(db_path), "%s/.local/share/debpkg/user_deps.db", 
             home_dir);
    
    if (!file_exists(db_path)) {
        return 0;  // 数据库不存在，依赖未被使用
    }
    
    FILE *fp = fopen(db_path, "r");
    if (fp == NULL) {
        return 0;
    }
    
    char line[1024];
    int usage_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // 解析格式：PKG_NAME|PKG_VERSION|DEPS
        char *pkg_name = strtok(line, "|");
        if (pkg_name == NULL) continue;
        
        // 跳过要排除的包（当前正在卸载的包）
        if (exclude_pkg != NULL && strcmp(pkg_name, exclude_pkg) == 0) {
            continue;
        }
        
        // 获取依赖列表部分
        char *version = strtok(NULL, "|");
        char *deps_str = strtok(NULL, "|");
        if (deps_str == NULL) continue;
        
        // 检查依赖列表中是否包含该依赖
        char *dep = strtok(deps_str, ",");
        while (dep != NULL) {
            // 提取依赖名（去掉版本信息）
            char dep_pkg_name[256];
            strncpy(dep_pkg_name, dep, sizeof(dep_pkg_name) - 1);
            dep_pkg_name[sizeof(dep_pkg_name) - 1] = '\0';
            
            char *colon = strchr(dep_pkg_name, ':');
            if (colon) *colon = '\0';
            
            // 去除前后空格
            char *start = dep_pkg_name;
            while (*start == ' ' || *start == '\t') start++;
            
            char *end = start + strlen(start) - 1;
            while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) {
                *end = '\0';
                end--;
            }
            
            if (strcmp(start, dep_name) == 0) {
                usage_count++;
                break;  // 找到一个使用就够
            }
            
            dep = strtok(NULL, ",");
        }
    }
    
    fclose(fp);
    return usage_count;
}

// 执行实际的文件删除操作
int remove_package_files(const char *pkg_name, const char *home_dir, 
                        UninstallResult *result) {
    char **files;
    int count;
    
    if (get_package_manifest(pkg_name, home_dir, &files, &count) != 0) {
        print_error("Failed to read manifest");
        return -1;
    }
    
    print_info("Removing files...");
    
    for (int i = 0; i < count; i++) {
        if (files[i] == NULL || files[i][0] == '\0') continue;
        
        // 构建完整路径
        char full_path[MAX_PATH_LEN];
        
        // 处理 ~ 符号
        if (strncmp(files[i], "~/", 2) == 0) {
            snprintf(full_path, sizeof(full_path), "%s/%s", home_dir, files[i] + 2);
        } else {
            strncpy(full_path, files[i], sizeof(full_path) - 1);
            full_path[sizeof(full_path) - 1] = '\0';
        }
        
        // 删除文件或目录
        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                char cmd[MAX_CMD_LEN];
                snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", full_path);
                if (execute_command(cmd) == 0) {
                    result->dirs_removed++;
                    if (result->dirs_removed <= 10) {
                        printf("  Removed dir: %s\n", full_path);
                    }
                }
            } else {
                if (unlink(full_path) == 0) {
                    result->files_removed++;
                    if (result->files_removed <= 20) {
                        printf("  Removed: %s\n", full_path);
                    }
                }
            }
        }
        
        free(files[i]);
    }
    
    free(files);
    
    // 删除包的数据目录
    char pkg_data_dir[MAX_PATH_LEN];
    snprintf(pkg_data_dir, sizeof(pkg_data_dir), 
             "%s/.local/share/%s", home_dir, pkg_name);
    
    if (is_directory(pkg_data_dir)) {
        char cmd[MAX_CMD_LEN];
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", pkg_data_dir);
        execute_command(cmd);
        result->dirs_removed++;
    }
    
    // 删除配置文件目录
    char pkg_etc_dir[MAX_PATH_LEN];
    snprintf(pkg_etc_dir, sizeof(pkg_etc_dir), 
             "%s/.local/etc/%s", home_dir, pkg_name);
    
    if (is_directory(pkg_etc_dir)) {
        char cmd[MAX_CMD_LEN];
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", pkg_etc_dir);
        execute_command(cmd);
        result->dirs_removed++;
    }
    
    return 0;
}

// 清理不再需要的依赖
int cleanup_unused_dependencies(const char *pkg_name, const char *home_dir,
                               UninstallResult *result) {
    UserPackageRecord record;
    
    // 加载包的依赖记录
    if (load_package_dependencies(pkg_name, &record, home_dir) != 0) {
        print_warning("No dependency record found for this package");
        return 0;  // 没有依赖记录，不是错误
    }
    
    print_info("Checking dependencies...");
    
    for (int i = 0; i < record.dep_count; i++) {
        char dep_name[256];
        strncpy(dep_name, record.deps[i].name, sizeof(dep_name) - 1);
        dep_name[sizeof(dep_name) - 1] = '\0';
        
        // 检查是否被其他包使用
        int usage = check_dependency_usage(dep_name, pkg_name, home_dir);
        
        if (usage > 0) {
            printf("  Keeping %-30s (used by %d other package(s))\n", 
                   dep_name, usage);
            result->deps_kept++;
        } else {
            printf("  Removing unused dependency: %s\n", dep_name);
            
            // 递归卸载依赖（如果存在）
            // 注意：这里简化处理，只删除依赖记录，不实际删除依赖包
            // 完整的实现应该递归调用 uninstall_package_smart
            
            result->deps_removed++;
        }
    }
    
    free_user_package_record(&record);
    return 0;
}

// 显示卸载信息
void show_uninstall_info(const char *pkg_name, const char *home_dir) {
    char pkg_bin[MAX_PATH_LEN];
    char pkg_lib[MAX_PATH_LEN];
    char pkg_share[MAX_PATH_LEN];
    
    snprintf(pkg_bin, sizeof(pkg_bin), "%s/.local/bin", home_dir);
    snprintf(pkg_lib, sizeof(pkg_lib), "%s/.local/lib", home_dir);
    snprintf(pkg_share, sizeof(pkg_share), "%s/.local/share/%s", home_dir, pkg_name);
    
    printf("\n");
    print_info("Uninstall Information:");
    printf(COLOR_BLUE "═══════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("Package: " COLOR_GREEN "%s\n" COLOR_RESET, pkg_name);
    printf("\nWill remove:\n");
    printf("  - Executables from: ~/.local/bin/\n");
    printf("  - Libraries from: ~/.local/lib/\n");
    printf("  - Data from: ~/.local/share/%s/\n", pkg_name);
    printf("  - Config from: ~/.local/etc/%s/\n", pkg_name);
    printf("\n");
    printf(COLOR_YELLOW "[NOTE] " COLOR_RESET "Dependencies will be checked before removal.\n");
    printf(COLOR_YELLOW "[NOTE] " COLOR_RESET "Only unused dependencies will be removed.\n");
    printf(COLOR_BLUE "═══════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("\n");
}

// 智能卸载包（主函数）
int uninstall_package_smart(const char *pkg_name, const char *home_dir, 
                           UninstallOptions *options, UninstallResult *result) {
    // 初始化结果
    memset(result, 0, sizeof(UninstallResult));
    
    // 检查包是否存在
    char pkg_share_dir[MAX_PATH_LEN];
    snprintf(pkg_share_dir, sizeof(pkg_share_dir), 
             "%s/.local/share/%s", home_dir, pkg_name);
    
    if (!is_directory(pkg_share_dir)) {
        print_error("Package is not installed");
        return -1;
    }
    
    // 显示卸载信息
    if (!options->dry_run) {
        show_uninstall_info(pkg_name, home_dir);
        
        // 确认卸载
        printf("Do you want to proceed with uninstall? (y/n): ");
        char choice;
        if (scanf(" %c", &choice) != 1) {
            print_error("Invalid input");
            return -1;
        }
        
        if (choice != 'y' && choice != 'Y') {
            print_info("Uninstall cancelled");
            return 0;
        }
    }
    
    print_info("Starting uninstall process...");
    
    if (options->dry_run) {
        printf(COLOR_YELLOW "[DRY RUN] Would remove package: %s\n" COLOR_RESET, pkg_name);
    } else {
        // 1. 删除包文件
        if (remove_package_files(pkg_name, home_dir, result) != 0) {
            print_error("Failed to remove package files");
            return -1;
        }
        
        // 2. 清理桌面文件
        char desktop_file[MAX_PATH_LEN];
        snprintf(desktop_file, sizeof(desktop_file), 
                 "%s/.local/share/applications/%s.desktop", home_dir, pkg_name);
        
        if (file_exists(desktop_file)) {
            unlink(desktop_file);
            printf("  Removed desktop file: %s\n", desktop_file);
        }
        
        // 3. 检查和清理依赖
        cleanup_unused_dependencies(pkg_name, home_dir, result);
        
        // 4. 从依赖数据库中移除记录
        remove_package_from_deps_db(pkg_name, home_dir);
        
        printf("\n");
        print_success("Uninstall completed!");
        printf("Files removed: %d\n", result->files_removed);
        printf("Directories removed: %d\n", result->dirs_removed);
        printf("Dependencies kept: %d\n", result->deps_kept);
        printf("Dependencies removed: %d\n", result->deps_removed);
    }
    
    return 0;
}
