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
 * with this program; if not, write to the File System Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef DEBPKG_UNINSTALL_H
#define DEBPKG_UNINSTALL_H

// 卸载选项
typedef struct {
    int force;              // 强制卸载（忽略依赖检查）
    int dry_run;           // 仅模拟，不实际删除
    int verbose;           // 详细输出
} UninstallOptions;

// 卸载结果统计
typedef struct {
    int files_removed;      // 删除的文件数
    int dirs_removed;       // 删除的目录数
    int deps_kept;          // 保留的依赖数
    int deps_removed;       // 删除的依赖数
} UninstallResult;

// 智能卸载包（检查依赖关系）
int uninstall_package_smart(const char *pkg_name, const char *home_dir, 
                            UninstallOptions *options, UninstallResult *result);

// 检查包的依赖是否被其他包使用
int check_dependency_usage(const char *dep_name, const char *exclude_pkg, 
                          const char *home_dir);

// 获取包的安装清单
int get_package_manifest(const char *pkg_name, const char *home_dir, 
                        char ***files, int *count);

// 执行实际的文件删除操作
int remove_package_files(const char *pkg_name, const char *home_dir, 
                        UninstallResult *result);

// 清理不再需要的依赖
int cleanup_unused_dependencies(const char *pkg_name, const char *home_dir,
                               UninstallResult *result);

// 显示卸载信息
void show_uninstall_info(const char *pkg_name, const char *home_dir);

#endif // DEBPKG_UNINSTALL_H
