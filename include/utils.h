#ifndef DEBPKG_UTILS_H
#define DEBPKG_UTILS_H

#include <stddef.h>

// 路径和命令长度常量
#define MAX_PATH_LEN 4096
#define MAX_CMD_LEN 8192

// 颜色输出宏
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

// 打印信息函数
void print_info(const char *msg);
void print_success(const char *msg);
void print_warning(const char *msg);
void print_error(const char *msg);

// 获取用户主目录，成功返回 0，失败返回 -1
int get_home_dir(char *buffer, size_t size);

// 检查文件是否存在，存在返回 1，否则返回 0
int file_exists(const char *path);

// 检查是否为目录，是返回 1，否则返回 0
int is_directory(const char *path);

// 递归创建目录，成功返回 0，失败返回 -1
int create_directories(const char *path);

// 执行 shell 命令，返回命令退出状态码
int execute_command(const char *cmd);

#endif // DEBPKG_UTILS_H
