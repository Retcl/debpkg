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
#include "../include/auto_deps.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// 检查是否有 root 权限
int has_root_privileges(void) {
    return (geteuid() == 0);
}

// 从 apt-cache 输出解析下载 URL
int get_package_download_url(const char *pkg_name, char *url, size_t url_size) {
    char cmd[MAX_CMD_LEN];
    FILE *fp;
    
    // 首先尝试直接使用 apt-cache show
    snprintf(cmd, sizeof(cmd), "apt-cache show %s 2>/dev/null | grep -E '^Filename:' | head -1 | awk '{print $2}'", pkg_name);
    
    fp = popen(cmd, "r");
    if (fp == NULL) {
        return -1;
    }
    
    if (fgets(url, url_size, fp) == NULL) {
        pclose(fp);
        
        // 如果直接查找失败，尝试搜索相似的包名
        printf(COLOR_YELLOW "[WARNING] " COLOR_RESET "Package '%s' not found in APT, trying to find similar package...\n", pkg_name);
        
        // 提取基础包名（去掉版本号和变体）
        char base_pkg[256];
        strncpy(base_pkg, pkg_name, sizeof(base_pkg) - 1);
        base_pkg[sizeof(base_pkg) - 1] = '\0';
        
        // 去掉版本号后缀（如 >= 1.0.17）
        char *space = strchr(base_pkg, ' ');
        if (space) *space = '\0';
        
        // 使用 apt-cache search 搜索
        snprintf(cmd, sizeof(cmd), "apt-cache search ^%s$ 2>/dev/null | head -1 | awk '{print $1}'", base_pkg);
        fp = popen(cmd, "r");
        if (fp == NULL) {
            return -1;
        }
        
        char alt_pkg[256];
        if (fgets(alt_pkg, sizeof(alt_pkg), fp) == NULL) {
            pclose(fp);
            return -1;
        }
        
        // 移除换行符
        char *newline = strchr(alt_pkg, '\n');
        if (newline) *newline = '\0';
        
        pclose(fp);
        
        if (strlen(alt_pkg) > 0 && strcmp(alt_pkg, base_pkg) != 0) {
            printf(COLOR_BLUE "[INFO] " COLOR_RESET "Found alternative package: %s\n", alt_pkg);
            // 递归调用获取替代包的 URL
            return get_package_download_url(alt_pkg, url, url_size);
        }
        
        return -1;
    }
    
    // 移除换行符
    char *newline = strchr(url, '\n');
    if (newline) *newline = '\0';
    
    pclose(fp);
    
    // 如果没有找到 URL，返回错误
    if (strlen(url) == 0 || strstr(url, ".deb") == NULL) {
        return -1;
    }
    
    return 0;
}

// 从 APT 仓库下载 DEB 包
int download_package_from_apt(const char *pkg_name, const char *output_path) {
    char cmd[MAX_CMD_LEN];
    char url[512];
    char temp_dir[MAX_PATH_LEN];
    char deb_file[MAX_PATH_LEN];
    
    print_info("Getting package info for: ");
    printf("%s\n", pkg_name);
    
    // 获取下载 URL
    if (get_package_download_url(pkg_name, url, sizeof(url)) != 0) {
        print_error("Failed to get package download URL");
        print_info("Package might not be available in APT repositories");
        return -1;
    }
    
    printf("  URL: %s\n", url);
    
    // 构建完整的下载 URL
    char full_url[1024];
    if (strncmp(url, "http", 4) != 0) {
        snprintf(full_url, sizeof(full_url), "http://mirrors.aliyun.com/%s", url);
    } else {
        snprintf(full_url, sizeof(full_url), "%s", url);
    }
    
    // 创建临时目录
    snprintf(temp_dir, sizeof(temp_dir), "/tmp/debpkg_dl_XXXXXX");
    if (mkdtemp(temp_dir) == NULL) {
        print_error("Failed to create temporary directory");
        return -1;
    }
    
    // 构建本地文件路径
    snprintf(deb_file, sizeof(deb_file), "%s/%s.deb", temp_dir, pkg_name);
    
    print_info("Downloading package...");
    
    // 使用 wget 或 curl 下载
    snprintf(cmd, sizeof(cmd), 
        "wget -q --timeout=30 --tries=3 -O \"%s\" \"%s\" 2>/dev/null || "
        "curl -sL --connect-timeout 30 --max-time 60 -o \"%s\" \"%s\" 2>/dev/null",
        deb_file, full_url, deb_file, full_url);
    
    int ret = system(cmd);
    if (ret != 0 || !file_exists(deb_file)) {
        print_error("Download failed");
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", temp_dir);
        execute_command(cmd);
        return -1;
    }
    
    // 验证下载的文件
    struct stat st;
    if (stat(deb_file, &st) != 0 || st.st_size == 0) {
        print_error("Downloaded file is invalid");
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", temp_dir);
        execute_command(cmd);
        return -1;
    }
    
    printf("  Downloaded: %ld bytes\n", st.st_size);
    
    // 复制到目标位置
    snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\"", deb_file, output_path);
    if (execute_command(cmd) != 0) {
        print_error("Failed to copy downloaded package");
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", temp_dir);
        execute_command(cmd);
        return -1;
    }
    
    // 清理临时文件
    snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", temp_dir);
    execute_command(cmd);
    
    print_success("Package downloaded successfully");
    return 0;
}

// 自动下载并安装所有缺失的依赖
int auto_install_dependencies(Dependency *deps, int count, const char *home_dir) {
    int installed_count = 0;
    int failed_count = 0;
    int skipped_count = 0;
    char temp_deb_path[MAX_PATH_LEN];
    
    printf("\n");
    print_info("Starting automatic dependency installation...");
    printf("\n");
    
    for (int i = 0; i < count; i++) {
        if (deps[i].installed) {
            continue;
        }
        
        // 提取包名（去掉版本约束）
        char pkg_name[256];
        strncpy(pkg_name, deps[i].name, sizeof(pkg_name) - 1);
        pkg_name[sizeof(pkg_name) - 1] = '\0';
        
        // 去掉版本号后缀
        char *space = strchr(pkg_name, ' ');
        if (space) *space = '\0';
        
        printf(COLOR_BLUE "[INFO] " COLOR_RESET "Downloading: %s\n", pkg_name);
        
        // 创建临时文件路径
        snprintf(temp_deb_path, sizeof(temp_deb_path), "/tmp/%s_auto.deb", pkg_name);
        
        // 下载包
        if (download_package_from_apt(pkg_name, temp_deb_path) != 0) {
            print_warning("Failed to download: ");
            printf("%s\n", pkg_name);
            
            failed_count++;
            continue;
        }
        
        // 使用 debpkg 递归安装（用户目录模式）
        char cmd[MAX_CMD_LEN];
        snprintf(cmd, sizeof(cmd), "echo \"3\" | ./debpkg install -u \"%s\" 2>/dev/null", temp_deb_path);
        
        int ret = system(cmd);
        
        // 清理临时文件
        snprintf(cmd, sizeof(cmd), "rm -f \"%s\"", temp_deb_path);
        execute_command(cmd);
        
        if (ret == 0) {
            print_success("Dependency installed: ");
            printf("%s\n", pkg_name);
            deps[i].installed = 1;
            installed_count++;
        } else {
            print_error("Failed to install: ");
            printf("%s\n", pkg_name);
            failed_count++;
        }
        
        printf("\n");
    }
    
    // 显示统计信息
    printf("\n");
    print_info("Installation summary:");
    printf("  Successfully installed: %d\n", installed_count);
    printf("  Skipped (already installed): %d\n", skipped_count);
    printf("  Failed: %d\n", failed_count);
    printf("\n");
    
    if (failed_count > 0) {
        printf(COLOR_YELLOW "[WARNING] " COLOR_RESET "%d dependencies failed to install.\n", failed_count);
        printf("You can continue installation anyway or install missing dependencies manually.\n\n");
    }
    
    return (failed_count == 0) ? 0 : -1;
}
