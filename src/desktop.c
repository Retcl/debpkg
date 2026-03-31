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
#include "../include/desktop.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int create_desktop_file(const char *pkg_name, const char *home_dir) {
    char desktop_dir[MAX_PATH_LEN];
    char desktop_file[MAX_PATH_LEN];
    char exec_path[MAX_PATH_LEN];
    FILE *fp;
    
    // 创建 applications 目录
    snprintf(desktop_dir, sizeof(desktop_dir), "%s/.local/share/applications", home_dir);
    
    if (!is_directory(desktop_dir)) {
        if (create_directories(desktop_dir) != 0) {
            print_error("Failed to create desktop directory");
            return -1;
        }
    }
    
    // 生成 desktop 文件路径
    snprintf(desktop_file, sizeof(desktop_file), "%s/%s.desktop", desktop_dir, pkg_name);
    
    // 检查是否已存在 desktop 文件
    if (file_exists(desktop_file)) {
        print_warning("Desktop file already exists, skipping creation");
        return 0;
    }
    
    // 打开文件准备写入
    fp = fopen(desktop_file, "w");
    if (fp == NULL) {
        print_error("Failed to create desktop file");
        return -1;
    }
    
    // 写入 Desktop Entry 内容
    fprintf(fp, "[Desktop Entry]\n");
    fprintf(fp, "Version=1.0\n");
    fprintf(fp, "Type=Application\n");
    fprintf(fp, "Name=%s\n", pkg_name);
    fprintf(fp, "Comment=Application installed by debpkg\n");
    fprintf(fp, "Exec=%s/.local/bin/%s\n", home_dir, pkg_name);
    fprintf(fp, "Icon=application-x-executable\n");
    fprintf(fp, "Path=\n");
    fprintf(fp, "Terminal=false\n");
    fprintf(fp, "Categories=Application;\n");
    fprintf(fp, "StartupNotify=true\n");
    
    fclose(fp);
    
    // 设置文件权限
    chmod(desktop_file, 0644);
    
    printf(COLOR_BLUE "[INFO] " COLOR_RESET "Desktop file created: %s\n", desktop_file);
    
    return 0;
}
