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
