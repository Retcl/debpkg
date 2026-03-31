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
#include "../include/user_deps.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

// 获取用户依赖数据库文件路径
static int get_user_deps_db_path(char *path, size_t size, const char *home_dir) {
    snprintf(path, size, "%s/.local/share/debpkg/user_deps.db", home_dir);
    return 0;
}

// 初始化用户依赖数据库
int init_user_deps_db(const char *home_dir) {
    char db_dir[MAX_PATH_LEN];
    char db_path[MAX_PATH_LEN];
    
    // 创建数据库目录
    snprintf(db_dir, sizeof(db_dir), "%s/.local/share/debpkg", home_dir);
    
    if (!is_directory(db_dir)) {
        if (create_directories(db_dir) != 0) {
            print_error("Failed to create debpkg database directory");
            return -1;
        }
    }
    
    // 检查数据库文件是否存在
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    if (!file_exists(db_path)) {
        // 创建空的数据库文件
        FILE *fp = fopen(db_path, "w");
        if (fp == NULL) {
            print_error("Failed to create user dependencies database");
            return -1;
        }
        fprintf(fp, "# User Dependencies Database\n");
        fprintf(fp, "# Format: PKG_NAME|PKG_VERSION|DEP_NAME1:DEP_VER1,DEP_NAME2:DEP_VER2\n");
        fprintf(fp, "# Created by debpkg\n\n");
        fclose(fp);
        print_info("Created user dependencies database");
    }
    
    return 0;
}

// 保存包的依赖关系到数据库
int save_package_dependencies(const char *pkg_name, const char *pkg_version, 
                              Dependency *deps, int count, const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    FILE *fp;
    
    // 确保数据库已初始化
    if (init_user_deps_db(home_dir) != 0) {
        return -1;
    }
    
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    // 以追加方式打开文件
    fp = fopen(db_path, "a");
    if (fp == NULL) {
        print_error("Failed to open dependencies database");
        return -1;
    }
    
    // 写入包名和版本
    fprintf(fp, "%s|%s|", pkg_name, pkg_version);
    
    // 写入依赖列表
    for (int i = 0; i < count; i++) {
        if (i > 0) fprintf(fp, ",");
        fprintf(fp, "%s:%s", deps[i].name, deps[i].version[0] ? deps[i].version : "*");
    }
    
    fprintf(fp, "\n");
    fclose(fp);
    
    printf(COLOR_BLUE "[INFO] " COLOR_RESET "Saved dependency record for %s\n", pkg_name);
    
    return 0;
}

// 检查特定版本的依赖是否已安装
int check_user_dependency_version(const char *pkg_name, const char *version, const char *home_dir) {
    char installed_version[64];
    
    if (get_installed_dep_version(pkg_name, installed_version, sizeof(installed_version), home_dir) != 0) {
        return 0;  // 未找到
    }
    
    // 如果版本为空或为通配符，认为匹配
    if (version[0] == '\0' || strcmp(version, "*") == 0) {
        return 1;
    }
    
    // 比较版本号
    if (strcmp(installed_version, version) == 0) {
        return 1;  // 版本匹配
    }
    
    return 0;  // 版本不匹配
}

// 获取已安装依赖的版本号
int get_installed_dep_version(const char *pkg_name, char *version, size_t version_size, const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    FILE *fp;
    char line[1024];
    
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    if (!file_exists(db_path)) {
        return -1;
    }
    
    fp = fopen(db_path, "r");
    if (fp == NULL) {
        return -1;
    }
    
    // 查找包名对应的记录
    while (fgets(line, sizeof(line), fp)) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // 解析行：PKG_NAME|PKG_VERSION|DEPENDENCIES
        char *pkg_name_field = strtok(line, "|");
        char *pkg_version_field = strtok(NULL, "|");
        
        if (pkg_name_field && pkg_version_field) {
            // 移除换行符
            char *newline = strchr(pkg_version_field, '\n');
            if (newline) *newline = '\0';
            
            // 检查包名是否匹配
            if (strcmp(pkg_name_field, pkg_name) == 0) {
                strncpy(version, pkg_version_field, version_size - 1);
                fclose(fp);
                return 0;
            }
        }
    }
    
    fclose(fp);
    return -1;  // 未找到
}

// 加载包的依赖记录
int load_package_dependencies(const char *pkg_name, UserPackageRecord *record, const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    FILE *fp;
    char line[1024];
    
    memset(record, 0, sizeof(UserPackageRecord));
    
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    if (!file_exists(db_path)) {
        return -1;
    }
    
    fp = fopen(db_path, "r");
    if (fp == NULL) {
        return -1;
    }
    
    // 查找包名对应的记录
    while (fgets(line, sizeof(line), fp)) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // 解析行：PKG_NAME|PKG_VERSION|DEPENDENCIES
        char *pkg_name_field = strtok(line, "|");
        char *pkg_version_field = strtok(NULL, "|");
        char *deps_field = strtok(NULL, "|");
        
        if (pkg_name_field && pkg_version_field && deps_field) {
            // 移除换行符
            char *newline = strchr(deps_field, '\n');
            if (newline) *newline = '\0';
            
            // 检查包名是否匹配
            if (strcmp(pkg_name_field, pkg_name) == 0) {
                strncpy(record->pkg_name, pkg_name_field, sizeof(record->pkg_name) - 1);
                strncpy(record->pkg_version, pkg_version_field, sizeof(record->pkg_version) - 1);
                
                // 解析依赖列表
                record->deps = NULL;
                record->dep_count = 0;
                
                char *dep_token = strtok(deps_field, ",");
                while (dep_token != NULL) {
                    // 解析 DEP_NAME:DEP_VERSION
                    char *colon = strchr(dep_token, ':');
                    if (colon) {
                        Dependency *new_deps = realloc(record->deps, 
                            (record->dep_count + 1) * sizeof(Dependency));
                        if (new_deps == NULL) {
                            free_user_package_record(record);
                            fclose(fp);
                            return -1;
                        }
                        
                        record->deps = new_deps;
                        Dependency *dep = &record->deps[record->dep_count];
                        memset(dep, 0, sizeof(Dependency));
                        
                        size_t name_len = colon - dep_token;
                        strncpy(dep->name, dep_token, name_len);
                        strncpy(dep->version, colon + 1, sizeof(dep->version) - 1);
                        
                        record->dep_count++;
                    }
                    
                    dep_token = strtok(NULL, ",");
                }
                
                fclose(fp);
                return 0;
            }
        }
    }
    
    fclose(fp);
    return -1;  // 未找到
}

// 列出所有用户目录下安装的包
int list_user_installed_packages(char ***packages, int *count, const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    FILE *fp;
    char line[1024];
    
    *packages = NULL;
    *count = 0;
    
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    if (!file_exists(db_path)) {
        return 0;  // 数据库不存在，返回空列表
    }
    
    fp = fopen(db_path, "r");
    if (fp == NULL) {
        return -1;
    }
    
    // 统计包数量
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        (*count)++;
    }
    
    if (*count == 0) {
        fclose(fp);
        return 0;
    }
    
    // 重新分配内存
    rewind(fp);
    *packages = malloc(*count * sizeof(char *));
    if (*packages == NULL) {
        fclose(fp);
        return -1;
    }
    
    int index = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // 提取包名
        char *pkg_name = strtok(line, "|");
        if (pkg_name) {
            (*packages)[index] = strdup(pkg_name);
            index++;
        }
    }
    
    fclose(fp);
    return 0;
}

// 清理用户依赖数据库（卸载时使用）
int remove_package_from_deps_db(const char *pkg_name, const char *home_dir) {
    char db_path[MAX_PATH_LEN];
    char temp_path[MAX_PATH_LEN];
    FILE *fp_in, *fp_out;
    char line[1024];
    
    get_user_deps_db_path(db_path, sizeof(db_path), home_dir);
    
    if (!file_exists(db_path)) {
        return 0;  // 数据库不存在，无需删除
    }
    
    // 创建临时文件
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", db_path);
    
    fp_in = fopen(db_path, "r");
    if (fp_in == NULL) {
        print_error("Failed to open dependencies database");
        return -1;
    }
    
    fp_out = fopen(temp_path, "w");
    if (fp_out == NULL) {
        fclose(fp_in);
        print_error("Failed to create temporary file");
        return -1;
    }
    
    // 复制除目标包外的所有记录
    int removed = 0;
    while (fgets(line, sizeof(line), fp_in)) {
        // 保留注释和空行
        if (line[0] == '#' || line[0] == '\n') {
            fputs(line, fp_out);
            continue;
        }
        
        // 检查是否是目标包
        char line_pkg_name[256];
        strncpy(line_pkg_name, line, sizeof(line_pkg_name) - 1);
        char *sep = strchr(line_pkg_name, '|');
        if (sep) *sep = '\0';
        
        if (strcmp(line_pkg_name, pkg_name) == 0) {
            removed = 1;
            continue;  // 跳过此行（删除）
        }
        
        fputs(line, fp_out);
    }
    
    fclose(fp_in);
    fclose(fp_out);
    
    // 替换原文件
    char cmd[MAX_CMD_LEN];
    snprintf(cmd, sizeof(cmd), "mv \"%s\" \"%s\"", temp_path, db_path);
    execute_command(cmd);
    
    if (removed) {
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Removed %s from dependencies database\n", pkg_name);
    }
    
    return 0;
}

// 释放 UserPackageRecord 内存
void free_user_package_record(UserPackageRecord *record) {
    if (record != NULL) {
        if (record->deps != NULL) {
            free(record->deps);
            record->deps = NULL;
        }
        record->dep_count = 0;
    }
}
