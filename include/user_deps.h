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

#ifndef DEBPKG_USERDEPS_H
#define DEBPKG_USERDEPS_H

#include "depends.h"

// 用户依赖记录结构体
typedef struct {
    char pkg_name[256];      // 包名
    char pkg_version[64];    // 包版本
    Dependency *deps;        // 依赖列表
    int dep_count;           // 依赖数量
} UserPackageRecord;

// 初始化用户依赖数据库
int init_user_deps_db(const char *home_dir);

// 保存包的依赖关系到数据库
int save_package_dependencies(const char *pkg_name, const char *pkg_version, 
                              Dependency *deps, int count, const char *home_dir);

// 检查特定版本的依赖是否已安装
int check_user_dependency_version(const char *pkg_name, const char *version, const char *home_dir);

// 获取已安装依赖的版本号
int get_installed_dep_version(const char *pkg_name, char *version, size_t version_size, const char *home_dir);

// 加载包的依赖记录
int load_package_dependencies(const char *pkg_name, UserPackageRecord *record, const char *home_dir);

// 列出所有用户目录下安装的包
int list_user_installed_packages(char ***packages, int *count, const char *home_dir);

// 清理用户依赖数据库（卸载时使用）
int remove_package_from_deps_db(const char *pkg_name, const char *home_dir);

// 释放 UserPackageRecord 内存
void free_user_package_record(UserPackageRecord *record);

#endif // DEBPKG_USERDEPS_H
