#ifndef DEBPKG_INSTALL_USER_H
#define DEBPKG_INSTALL_USER_H

// 用户目录安装函数，将包安装到 ~/.local 目录
int install_user(const char *deb_path);

#endif // DEBPKG_INSTALL_USER_H
