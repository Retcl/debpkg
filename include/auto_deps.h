#ifndef DEBPKG_AUTO_DEPS_H
#define DEBPKG_AUTO_DEPS_H

#include "depends.h"

// 自动下载并安装所有缺失的依赖
int auto_install_dependencies(Dependency *deps, int count, const char *home_dir);

// 从 APT 仓库下载 DEB 包
int download_package_from_apt(const char *pkg_name, const char *output_path);

// 解析 apt-cache 输出获取下载 URL
int get_package_download_url(const char *pkg_name, char *url, size_t url_size);

// 检查是否有 root 权限（用于系统级安装）
int has_root_privileges(void);

#endif // DEBPKG_AUTO_DEPS_H
