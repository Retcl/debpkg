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
