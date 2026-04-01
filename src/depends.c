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
#include "../include/depends.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 检查系统级依赖是否已安装
int check_system_dependency(const char *pkg_name) {
    char cmd[MAX_CMD_LEN];
    
    // 使用 dpkg -l 检查包是否安装
    snprintf(cmd, sizeof(cmd), "dpkg -l \"%s\" 2>/dev/null | grep -q \"^ii\"", pkg_name);
    
    return (system(cmd) == 0) ? 1 : 0;
}

// 检查用户目录依赖是否已安装
int check_user_dependency(const char *pkg_name, const char *home_dir) {
    char check_path[MAX_PATH_LEN];
    
    // 检查 ~/.local/share/<pkg-name>/INSTALL_MANIFEST 是否存在
    snprintf(check_path, sizeof(check_path), 
        "%s/.local/share/%s/INSTALL_MANIFEST", home_dir, pkg_name);
    
    return file_exists(check_path);
}

// 从 DEB 包提取依赖列表
int extract_dependencies(const char *deb_path, Dependency **deps, int *count) {
    char temp_control[MAX_PATH_LEN];
    char cmd[MAX_CMD_LEN];
    int ret = 0;
    
    // 创建临时目录提取 control 信息
    snprintf(temp_control, sizeof(temp_control), "/tmp/debpkg_control_XXXXXX");
    
    if (mkdtemp(temp_control) == NULL) {
        print_error("Failed to create temporary directory");
        return -1;
    }
    
    // 提取 control 信息
    snprintf(cmd, sizeof(cmd), "dpkg-deb --control \"%s\" \"%s\"", deb_path, temp_control);
    if (execute_command(cmd) != 0) {
        ret = -1;
        goto cleanup;
    }
    
    // 解析依赖
    char control_file[MAX_PATH_LEN];
    snprintf(control_file, sizeof(control_file), "%s/control", temp_control);
    ret = parse_dependencies(control_file, deps, count);
    
cleanup:
    // 清理临时目录
    snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", temp_control);
    execute_command(cmd);
    
    return ret;
}

// 解析 control 文件获取依赖信息
int parse_dependencies(const char *control_path, Dependency **deps, int *count) {
    FILE *fp = fopen(control_path, "r");
    if (fp == NULL) {
        print_error("Cannot open control file");
        return -1;
    }
    
    // 初始化
    *deps = NULL;
    *count = 0;
    
    char line[4096];  // 增大缓冲区以支持长依赖列表（VS Code 的 Depends 字段超过 1KB）
    int in_depends = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        // 检查是否是 Depends 字段
        if (strncmp(line, "Depends:", 8) == 0) {
            in_depends = 1;
            
            // 跳过 "Depends: " 前缀
            char *dep_line = line + 8;
            while (*dep_line == ' ' || *dep_line == '\t') dep_line++;
            
            // 移除换行符
            char *newline = strchr(dep_line, '\n');
            if (newline) *newline = '\0';
            
            // 解析依赖列表（逗号分隔）
            char *token = strtok(dep_line, ",");
            while (token != NULL) {
                // 跳过空格
                while (*token == ' ') token++;
                
                // 分配内存
                Dependency *new_deps = realloc(*deps, (*count + 1) * sizeof(Dependency));
                if (new_deps == NULL) {
                    free_dependencies(*deps, *count);
                    fclose(fp);
                    return -1;
                }
                
                *deps = new_deps;
                
                // 解析包名和版本
                Dependency *dep = &(*deps)[*count];
                memset(dep, 0, sizeof(Dependency));
                
                // 提取包名（去除版本信息）
                // 首先跳过 token 的前导空格
                char *trimmed_token = token;
                while (*trimmed_token == ' ' || *trimmed_token == '\t') trimmed_token++;
                
                // 查找第一个空格，分离包名和版本部分
                char *space_sep = strchr(trimmed_token, ' ');
                if (space_sep) {
                    // 有空格分隔，包名在空格前
                    size_t name_len = space_sep - trimmed_token;
                    strncpy(dep->name, trimmed_token, name_len);
                    dep->name[name_len] = '\0';
                    
                    // 版本信息在空格后（可能包含括号）
                    char *version_start = space_sep + 1;
                    while (*version_start == ' ' || *version_start == '\t') version_start++;
                    strncpy(dep->version, version_start, sizeof(dep->version) - 1);
                    dep->version[sizeof(dep->version) - 1] = '\0';
                } else {
                    // 没有空格，检查是否有版本约束符号
                    char *version_sep = strpbrk(trimmed_token, "<>=");
                    if (version_sep) {
                        // 有版本约束符号（无空格的情况）
                        size_t name_len = version_sep - trimmed_token;
                        strncpy(dep->name, trimmed_token, name_len);
                        dep->name[name_len] = '\0';
                        
                        strncpy(dep->version, version_sep, sizeof(dep->version) - 1);
                        dep->version[sizeof(dep->version) - 1] = '\0';
                    } else {
                        // 没有版本信息，只有包名
                        strncpy(dep->name, trimmed_token, sizeof(dep->name) - 1);
                        dep->name[sizeof(dep->name) - 1] = '\0';
                    }
                }
                
                dep->installed = 0;  // 初始化为未安装
                
                (*count)++;
                
                // 下一个依赖
                token = strtok(NULL, ",");
            }
        } else if (in_depends) {
            // 继续处理多行的依赖（以空格或制表符开头的行）
            if (line[0] == ' ' || line[0] == '\t') {
                char *dep_line = line;
                while (*dep_line == ' ' || *dep_line == '\t') dep_line++;
                
                // 移除换行符
                char *newline = strchr(dep_line, '\n');
                if (newline) *newline = '\0';
                
                // 解析依赖
                char *token = strtok(dep_line, ",");
                while (token != NULL) {
                    while (*token == ' ') token++;
                    
                    Dependency *new_deps = realloc(*deps, (*count + 1) * sizeof(Dependency));
                    if (new_deps == NULL) {
                        free_dependencies(*deps, *count);
                        fclose(fp);
                        return -1;
                    }
                    
                    *deps = new_deps;
                    Dependency *dep = &(*deps)[*count];
                    memset(dep, 0, sizeof(Dependency));
                    
                    // 首先跳过 token 的前导空格
                    char *trimmed_token = token;
                    while (*trimmed_token == ' ' || *trimmed_token == '\t') trimmed_token++;
                    
                    // 查找第一个空格，分离包名和版本部分
                    char *space_sep = strchr(trimmed_token, ' ');
                    if (space_sep) {
                        // 有空格分隔，包名在空格前
                        size_t name_len = space_sep - trimmed_token;
                        strncpy(dep->name, trimmed_token, name_len);
                        dep->name[name_len] = '\0';
                        
                        // 版本信息在空格后（可能包含括号）
                        char *version_start = space_sep + 1;
                        while (*version_start == ' ' || *version_start == '\t') version_start++;
                        strncpy(dep->version, version_start, sizeof(dep->version) - 1);
                        dep->version[sizeof(dep->version) - 1] = '\0';
                    } else {
                        // 没有空格，检查是否有版本约束符号
                        char *version_sep = strpbrk(trimmed_token, "<>=");
                        if (version_sep) {
                            // 有版本约束符号（无空格的情况）
                            size_t name_len = version_sep - trimmed_token;
                            strncpy(dep->name, trimmed_token, name_len);
                            dep->name[name_len] = '\0';
                            
                            strncpy(dep->version, version_sep, sizeof(dep->version) - 1);
                            dep->version[sizeof(dep->version) - 1] = '\0';
                        } else {
                            // 没有版本信息，只有包名
                            strncpy(dep->name, trimmed_token, sizeof(dep->name) - 1);
                            dep->name[sizeof(dep->name) - 1] = '\0';
                        }
                    }
                    
                    dep->installed = 0;
                    (*count)++;
                    
                    token = strtok(NULL, ",");
                }
            } else {
                in_depends = 0;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

// 安装依赖到用户目录
int install_dependency_to_user(const char *dep_name, const char *home_dir) {
    print_info("Checking dependency: ");
    printf("%s\n", dep_name);
    
    // 首先检查系统级是否已安装
    if (check_system_dependency(dep_name)) {
        printf("  ✓ Already installed (system)\n");
        return 0;
    }
    
    // 检查用户目录是否已安装
    if (check_user_dependency(dep_name, home_dir)) {
        printf("  ✓ Already installed (user)\n");
        return 0;
    }
    
    print_warning("Dependency not found: ");
    printf("%s\n", dep_name);
    print_info("Note: Automatic dependency installation is not yet supported.");
    print_info("Please install the dependency manually:");
    printf("  System: sudo apt-get install %s\n", dep_name);
    printf("  User:   ./debpkg -u %s.deb\n", dep_name);
    
    return -1;  // 返回 -1 表示依赖未安装，但不阻止主程序安装
}

// 释放依赖列表内存
void free_dependencies(Dependency *deps, int count) {
    if (deps != NULL) {
        free(deps);
    }
}

// 检查用户目录依赖是否已安装（带版本检测）
int check_user_dependency_with_version(const char *pkg_name, const char *version, const char *home_dir) {
    // 首先检查包是否存在
    if (!check_user_dependency(pkg_name, home_dir)) {
        return 0;  // 包未安装
    }
    
    // 如果不需要检查版本，直接返回已安装
    if (version == NULL || version[0] == '\0' || strcmp(version, "*") == 0) {
        return 1;  // 已安装且不要求版本
    }
    
    // 检查版本是否匹配
    return check_user_dependency_version(pkg_name, version, home_dir);
}
