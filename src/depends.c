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
    
    char line[1024];
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
                char *version_sep = strpbrk(token, "<>= ");
                if (version_sep) {
                    size_t name_len = version_sep - token;
                    strncpy(dep->name, token, name_len);
                    dep->name[name_len] = '\0';
                    
                    // 提取版本信息
                    strncpy(dep->version, version_sep, sizeof(dep->version) - 1);
                } else {
                    // 没有版本信息，只有包名
                    // 移除可能的空格和换行
                    char *end = token + strlen(token) - 1;
                    while (end > token && (*end == ' ' || *end == '\n' || *end == '\r')) {
                        *end = '\0';
                        end--;
                    }
                    strncpy(dep->name, token, sizeof(dep->name) - 1);
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
                    
                    char *version_sep = strpbrk(token, "<>= ");
                    if (version_sep) {
                        size_t name_len = version_sep - token;
                        strncpy(dep->name, token, name_len);
                        dep->name[name_len] = '\0';
                        strncpy(dep->version, version_sep, sizeof(dep->version) - 1);
                    } else {
                        char *end = token + strlen(token) - 1;
                        while (end > token && (*end == ' ' || *end == '\n' || *end == '\r')) {
                            *end = '\0';
                            end--;
                        }
                        strncpy(dep->name, token, sizeof(dep->name) - 1);
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
