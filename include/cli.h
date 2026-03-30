#ifndef DEBPKG_CLI_H
#define DEBPKG_CLI_H

// 显示帮助信息
void show_help(const char *prog_name);

// 显示版本信息
void show_version(void);

// 交互式选择安装方式
int interactive_install(const char *deb_path);

#endif // DEBPKG_CLI_H
