# debpkg 打包指南

## 📦 概述

`package.sh` 是一个自动化脚本，用于将 debpkg 项目编译并打包成标准的 Debian (.deb) 安装包。生成的包可以在任何 Debian/Ubuntu 系统上安装。

## ✨ 功能特性

- ✅ **自动依赖检测** - 检查必要的打包工具（dpkg-dev, fakeroot）
- ✅ **完整构建流程** - 从源代码到 .deb 包的一键打包
- ✅ **标准包结构** - 遵循 Debian 打包规范
- ✅ **无需 root 权限** - 使用 fakeroot 进行安全打包
- ✅ **包含文档** - 自动集成 README、QUICKSTART 等文档
- ✅ **维护脚本** - 包含 postinst 和 prerm 安装/卸载脚本
- ✅ **版权信息** - 包含完整的 copyright 和 license 文件
- ✅ **包验证** - 自动验证生成的包完整性

## 🔧 打包模式

| 模式 | 参数 | 说明 |
|------|------|------|
| **完整打包** | （无参数） | 编译 + 打包全流程 |
| **仅清理** | `-c` / `--clean` | 清理构建目录和旧的 deb 包 |
| **仅编译** | `-b` / `--build` | 只编译生成可执行文件 |
| **仅打包** | `-p` / `--package` | 假设已编译，直接打包 |
| **仅验证** | `-v` / `--verify` | 验证已有的 deb 包 |

## 🚀 快速开始

### 完整打包流程

```bash
./package.sh
```

这将执行以下步骤：
1. 检查打包工具依赖
2. 准备临时构建目录
3. 编译源代码
4. 安装文件到打包结构
5. 创建 Debian control 文件
6. 创建安装/卸载脚本
7. 打包成 .deb 文件
8. 验证包的完整性
9. 清理临时文件

### 输出示例

```bash
$ ./package.sh

========================================
  debpkg Debian 打包脚本
  Version: 1.1.0
========================================

[INFO] 检查打包工具...
[SUCCESS] 打包工具检查通过
[INFO] 准备构建目录...
[SUCCESS] 构建目录准备完成
[INFO] 编译项目...
[编译输出...]
[SUCCESS] 项目编译成功
[INFO] 安装文件到打包目录...
[SUCCESS] 文件安装完成
[INFO] 创建 Debian control 文件...
[SUCCESS] Control 文件创建完成
[INFO] 创建 postinst 脚本...
[SUCCESS] Postinst 脚本创建完成
[INFO] 创建 prerm 脚本...
[SUCCESS] Prerm 脚本创建完成
[INFO] 创建 copyright 文件...
[SUCCESS] Copyright 文件创建完成
[INFO] 打包 Debian 软件包...
[SUCCESS] Debian 软件包创建成功：debpkg_1.1.0_amd64.deb (52K)

软件包信息:
 Package: debpkg
 Version: 1.1.0
 Architecture: amd64
 Maintainer: Your Name <your.email@example.com>
 Installed-Size: 128 KB
 Depends: dpkg, wget | curl
 Section: utils
 Priority: optional
 Description: A modular DEB package installer written in C

[SUCCESS] 软件包验证通过

========================================
[SUCCESS] 打包过程完成！
========================================
```

## 📋 详细用法

### 1. 完整打包（推荐）

```bash
./package.sh
```

**适用场景：** 
- 首次打包
- 发布新版本
- 完整测试流程

**输出文件：** `debpkg_<VERSION>_amd64.deb`

### 2. 清理构建产物

```bash
./package.sh --clean
```

或直接清理：

```bash
rm -rf build/ debpkg_*.deb
```

**适用场景：**
- 释放磁盘空间
- 重新打包前清理

### 3. 仅编译

```bash
./package.sh --build
```

**适用场景：**
- 调试代码
- 测试编译
- 手动控制打包流程

**输出文件：** `./debpkg`（可执行文件）

### 4. 仅打包

```bash
./package.sh --package
```

**前提条件：** 已经编译好可执行文件

**适用场景：**
- 修改了包配置但未改代码
- 快速重新打包

### 5. 验证包

```bash
./package.sh --verify
```

**适用场景：**
- 检查已有包的完整性
- 查看包内容

## 📦 生成的包结构

```
debpkg_<VERSION>_amd64.deb
├── DEBIAN/
│   ├── control      # 包元数据
│   ├── postinst     # 安装后脚本
│   ├── prerm        # 卸载前脚本
│   └── copyright    # 版权信息
│
└── usr/local/
    ├── bin/
    │   └── debpkg          # 主程序
    └── share/doc/debpkg/
        ├── README.md       # 使用说明
        ├── QUICKSTART.md   # 快速入门
        ├── INSTALL_GUIDE.md # 安装指南
        └── LICENSE         # 许可证
```

## 🔍 包配置说明

### 默认配置

在 `package.sh` 中定义的配置：

```bash
PACKAGE_NAME="debpkg"           # 包名
VERSION="1.1.0"                 # 版本号
ARCHITECTURE="amd64"            # 架构
MAINTAINER="Your Name <your.email@example.com>"  # 维护者
DESCRIPTION="A modular DEB package installer..."  # 描述
SECTION="utils"                 # 分类
PRIORITY="optional"             # 优先级
DEPENDS="dpkg, wget | curl"     # 依赖
```

### 自定义配置

编辑 `package.sh` 修改配置：

```bash
# 修改版本号
VERSION="1.2.0"

# 修改维护者信息
MAINTAINER="John Doe <john@example.com>"

# 修改依赖
DEPENDS="dpkg, wget, curl"

# 修改架构（交叉编译时）
ARCHITECTURE="arm64"
```

## 🛠️ 包管理操作

### 安装生成的包

```bash
sudo dpkg -i debpkg_1.1.0_amd64.deb
```

或（自动解决依赖）：

```bash
sudo apt install ./debpkg_1.1.0_amd64.deb
```

### 查看包信息

```bash
dpkg-deb -I debpkg_1.1.0_amd64.deb
```

### 列出包内容

```bash
dpkg-deb -c debpkg_1.1.0_amd64.deb
```

### 提取包文件

```bash
dpkg-deb -x debpkg_1.1.0_amd64.deb /tmp/extract
```

### 卸载包

```bash
sudo dpkg -r debpkg
```

## 📊 打包流程对比

### 传统手动打包

```bash
# 1. 创建目录结构
mkdir -p debian/DEBIAN
mkdir -p debian/usr/local/bin

# 2. 编译
make clean && make

# 3. 复制文件
cp debpkg debian/usr/local/bin/

# 4. 创建 control 文件
vim debian/DEBIAN/control

# 5. 创建维护脚本
vim debian/DEBIAN/postinst
vim debian/DEBIAN/prerm

# 6. 设置权限
chmod 755 debian/DEBIAN/postinst
chmod 755 debian/DEBIAN/prerm

# 7. 打包
fakeroot dpkg-deb --build debian debpkg_1.1.0_amd64.deb

# 8. 验证
dpkg-deb -I debpkg_1.1.0_amd64.deb
```

**步骤数：8 步**

### 使用 package.sh

```bash
./package.sh
```

**步骤数：1 步** ⚡

## 🔒 安全性

### Fakeroot 机制

- ✅ **无需 root 权限** - 模拟 root 环境进行打包
- ✅ **文件隔离** - 不会影响系统文件
- ✅ **权限保留** - 正确保留文件权限和所有者信息

### 安装脚本安全

- ✅ **postinst** - 安装后配置，确保正确设置
- ✅ **prerm** - 卸载前清理，避免残留文件
- ✅ **错误处理** - 使用 `set -e` 确保错误时立即退出

## 🎯 最佳实践

### 1. 版本管理

```bash
# 每次发布新版本时更新 version
VERSION="1.1.0"  # Major.Minor.Patch
```

### 2. 依赖声明

明确声明所有运行时依赖：

```bash
DEPENDS="dpkg (>= 1.15), wget | curl, libc6 (>= 2.15)"
```

### 3. 包大小优化

- 压缩文档：`gzip`
- 剥离调试符号：`strip`
- 移除不必要的文件

### 4. 兼容性

支持的系统：
- ✅ Debian 10+ (Buster, Bullseye, Bookworm)
- ✅ Ubuntu 18.04+ (Bionic, Focal, Jammy)
- ✅ Linux Mint 19+
- ✅ 其他基于 Debian 的发行版

## 🐛 故障排除

### 问题 1: "dpkg-deb: command not found"

**解决方案：**
```bash
sudo apt install dpkg-dev
```

### 问题 2: "fakeroot: command not found"

**解决方案：**
```bash
sudo apt install fakeroot
```

### 问题 3: 打包时权限错误

**原因：** 试图修改系统目录

**解决方案：**
- 确保使用 `fakeroot`
- 检查构建目录路径是否正确

### 问题 4: 包太大

**排查步骤：**
```bash
# 查看包内容大小分布
dpkg-deb -c debpkg_*.deb | sort -k3 -n

# 压缩文档
find build/install/usr/local/share/doc -type f -exec gzip -9 {} \;
```

### 问题 5: 依赖冲突

**解决方案：**
```bash
# 检查依赖
apt-cache depends debpkg

# 测试安装
sudo apt install ./debpkg_*.deb
```

## 📝 相关文件

| 文件 | 说明 |
|------|------|
| `package.sh` | 主打包脚本 |
| `Makefile` | 编译配置 |
| `README.md` | 项目文档 |
| `LICENSE` | 许可证文件 |
| `CHANGELOG.md` | 变更记录 |

## 🎓 进阶主题

### 交叉编译打包

```bash
# 为 ARM64 架构打包
ARCHITECTURE="arm64"
CC=aarch64-linux-gnu-gcc
CXX=aarch64-linux-gnu-g++

./package.sh --build
./package.sh --package
```

### 添加 PPA 源

```bash
# 添加到个人 PPA
sudo apt install devscripts
dput ppa:your-ppa/debpkg debpkg_1.1.0_amd64.changes
```

### 签名包

```bash
# GPG 签名
debsign debpkg_1.1.0_amd64.changes
```

## 💡 提示与技巧

### 快速测试安装

```bash
# 在 chroot 或容器中测试
sudo pbuilder create
sudo pbuilder install debpkg_*.deb
```

### 批量打包多个版本

```bash
for version in 1.0.0 1.1.0 1.2.0; do
    sed -i "s/VERSION=\".*\"/VERSION=\"$version\"/" package.sh
    ./package.sh --clean
    ./package.sh
done
```

### 自动化发布流程

```bash
#!/bin/bash
# release.sh

VERSION=$1
sed -i "s/VERSION=\".*\"/VERSION=\"$VERSION\"/" package.sh
./package.sh
git tag -a "v$VERSION" -m "Release $VERSION"
git push origin v$VERSION
```

## 🔗 相关资源

- [Debian 打包新手指南](https://www.debian.org/doc/manuals/maint-guide/)
- [dpkg-deb 手册](https://manpages.debian.org/dpkg-dev/dpkg-deb.1.en.html)
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)

## 🎉 总结

`package.sh` 提供了：

- ✅ **一键打包** - 从源码到 .deb 包的完整流程
- ✅ **标准化** - 遵循 Debian 打包最佳实践
- ✅ **灵活性** - 多种打包模式可选
- ✅ **安全性** - fakeroot 隔离，无需 root
- ✅ **文档齐全** - 完整的配套文档

让 debpkg 的发布和部署变得简单高效！

---

*文档版本：1.0 | 创建时间：2024 年 3 月*
