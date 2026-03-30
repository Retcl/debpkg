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

#ifndef DEBPKG_DEPENDS_H
#define DEBPKG_DEPENDS_H

#include <stddef.h>

// 依赖信息结构体
typedef struct {
    char name[256];      // 依赖包名
    char version[64];    // 版本号
    int installed;       // 是否已安装
} Dependency;

// 检查系统级依赖是否已安装
int check_system_dependency(const char *pkg_name);

// 检查用户目录依赖是否已安装
int check_user_dependency(const char *pkg_name, const char *home_dir);

// 从 DEB 包提取依赖列表
int extract_dependencies(const char *deb_path, Dependency **deps, int *count);

// 安装依赖到用户目录
int install_dependency_to_user(const char *dep_name, const char *home_dir);

// 解析 control 文件获取依赖信息
int parse_dependencies(const char *control_path, Dependency **deps, int *count);

// 释放依赖列表内存
void free_dependencies(Dependency *deps, int count);

#endif // DEBPKG_DEPENDS_H
