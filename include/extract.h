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

#ifndef DEBPKG_EXTRACT_H
#define DEBPKG_EXTRACT_H

#include <stddef.h>

// 提取 .deb 包的控制信息到指定目录
int extract_control(const char *deb_path, const char *extract_dir);

// 提取 .deb 包的数据部分到指定目录
int extract_data(const char *deb_path, const char *extract_dir);

// 从 control 文件中解析包名
int parse_package_name(const char *control_path, char *pkg_name, size_t size);

#endif // DEBPKG_EXTRACT_H
