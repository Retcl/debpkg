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

#ifndef DEBPKG_ENV_CONFIG_H
#define DEBPKG_ENV_CONFIG_H

// 环境变量配置结构体
typedef struct {
    int has_bin;         // 是否有可执行文件
    int has_lib;         // 是否有库文件
    int has_include;     // 是否有头文件
    int has_man;         // 是否有手册页
    char bin_path[512];  // bin 目录路径
    char lib_path[512];  // lib 目录路径
    char include_path[512]; // include 目录路径
    char man_path[512];  // man 目录路径
} EnvConfig;

// 检查并提示环境变量配置
int check_and_prompt_env_config(const char *home_dir, const char *pkg_name);

// 生成环境变量配置脚本
int generate_env_script(const char *home_dir, const char *output_path);

// 显示环境变量配置说明
void show_env_config_guide(const char *home_dir);

// 自动检测是否需要配置环境变量
int detect_env_issues(const char *home_dir);

#endif // DEBPKG_ENV_CONFIG_H
