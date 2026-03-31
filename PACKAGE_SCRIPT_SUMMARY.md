# Debian 打包脚本特性总结

## 📦 新增文件

### 核心文件
- **[`package.sh`](package.sh)** - Debian 自动化打包脚本（主脚本）
- **[`PACKAGE_GUIDE.md`](PACKAGE_GUIDE.md)** - 详细的打包指南文档

### 更新文件
- **[`README.md`](README.md)** - 添加打包脚本使用说明
- **[`README_CN.md`](README_CN.md)** - 添加打包脚本使用说明（中文版）
- **[`CHANGES.md`](CHANGES.md)** - 更新 v1.2.0 变更记录

## ✨ 主要功能

### 1. 多种打包模式

| 模式 | 参数 | 说明 | 适用场景 |
|------|------|------|----------|
| **完整打包** | 无参数 | 编译 + 打包全流程 | 发布新版本 |
| **仅清理** | `-c` / `--clean` | 清理构建产物 | 释放空间/重新打包 |
| **仅编译** | `-b` / `--build` | 只编译不打包 | 开发调试 |
| **仅打包** | `-p` / `--package` | 假设已编译 | 修改配置后快速打包 |
| **仅验证** | `-v` / `--verify` | 验证包的完整性 | 质量检查 |

### 2. 自动化流程

```
检测依赖 → 准备目录 → 编译 → 安装文件 → 创建配置 → 打包 → 验证 → 清理
```

每个步骤都有清晰的彩色输出提示。

### 3. 智能特性

- ✅ **工具检测** - 自动检查 dpkg-dev、fakeroot 是否安装
- ✅ **错误处理** - 遇到问题时提供明确的解决方案
- ✅ **无需 root** - 使用 fakeroot 安全打包
- ✅ **文档集成** - 自动包含 README、QUICKSTART 等文档
- ✅ **维护脚本** - 自动生成 postinst 和 prerm 脚本
- ✅ **版权信息** - 包含完整的 copyright 和 license
- ✅ **包验证** - 自动验证生成的 .deb 包完整性
- ✅ **自动清理** - 打包完成后清理临时文件

## 🎨 用户体验

### 彩色输出

```bash
[INFO]    - 蓝色   # 普通信息
[SUCCESS] - 绿色   # 成功提示
[WARNING] - 黄色   # 警告信息
[ERROR]   - 红色   # 错误提示
```

### 详细进度

```
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
...
[SUCCESS] Debian 软件包创建成功：debpkg_1.1.0_amd64.deb (52K)
[SUCCESS] 软件包验证通过
========================================
[SUCCESS] 打包过程完成！
========================================
```

## 📋 生成的包结构

```
debpkg_1.1.0_amd64.deb (52KB)
│
├── DEBIAN/
│   ├── control        # 包元数据（名称、版本、依赖等）
│   ├── postinst       # 安装后配置脚本
│   ├── prerm          # 卸载前清理脚本
│   └── copyright      # GPL-2+ 版权信息
│
└── usr/local/
    ├── bin/
    │   └── debpkg           # 主程序（可执行文件）
    └── share/doc/debpkg/
        ├── README.md        # 使用说明
        ├── QUICKSTART.md    # 快速入门
        ├── INSTALL_GUIDE.md # 安装指南
        └── LICENSE          # GPL v2 许可证
```

## 🔍 包配置信息

### 默认配置

```bash
Package: debpkg
Version: 1.1.0
Architecture: amd64
Maintainer: Your Name <your.email@example.com>
Installed-Size: ~128 KB
Depends: dpkg, wget | curl
Section: utils
Priority: optional
Description: A modular DEB package installer written in C
```

### 可配置项

在 `package.sh` 中可自定义：
- `VERSION` - 版本号
- `MAINTAINER` - 维护者信息
- `DEPENDS` - 依赖关系
- `ARCHITECTURE` - 目标架构
- `SECTION` - 包分类
- `PRIORITY` - 优先级

## 🛠️ 技术实现

### Shell 脚本最佳实践

- ✅ **严格模式** - `set -e` 确保错误立即退出
- ✅ **函数化** - 模块化组织代码，便于维护
- ✅ **错误处理** - 完善的错误检测和提示
- ✅ **变量引用** - 所有变量加引号防止空格问题
- ✅ **注释清晰** - 关键步骤都有注释说明
- ✅ **安全性** - 使用 fakeroot 避免权限问题

### 关键函数列表

```bash
check_dependencies()      # 检测系统打包工具
prepare_build_dir()       # 准备临时构建目录
build_project()           # 编译源代码
install_files()           # 安装文件到打包目录
create_control_file()     # 创建 Debian control 文件
create_postinst()         # 创建安装后脚本 (post-installation)
create_prerm()            # 创建卸载前脚本 (pre-removal)
create_copyright()        # 创建版权和许可证文件
package_deb()             # 使用 dpkg-deb 打包成 .deb
verify_package()          # 验证包的完整性
cleanup()                 # 清理临时构建文件
show_help()               # 显示帮助信息
main()                    # 主函数，协调各步骤
```

## 🌟 优势对比

### vs 传统 Makefile install

| 特性 | package.sh | make install |
|------|-----------|--------------|
| 产物类型 | .deb 安装包 | 直接安装到系统 |
| 分发能力 | ✅ 强（可共享） | ❌ 弱（单机） |
| 版本管理 | ✅ 支持多版本 | ❌ 困难 |
| 批量部署 | ✅ 适合 | ❌ 不适合 |
| 依赖管理 | ✅ 自动声明 | ❌ 手动处理 |
| 卸载支持 | ✅ 完整 | ⚠️ 基础 |
| 离线安装 | ✅ 支持 | ❌ 不支持 |

### vs 手动打包

| 步骤 | package.sh | 手动操作 |
|------|-----------|----------|
| 创建目录结构 | 1 步自动 | 5+ 条 mkdir 命令 |
| 编译 | 自动 | make clean && make |
| 复制文件 | 自动 | 多条 cp 命令 |
| Control 文件 | 自动生成 | 手动编写格式 |
| 维护脚本 | 自动生成 | 手动编写 |
| Copyright | 自动生成 | 手动查找模板 |
| 设置权限 | 自动 | chmod/chown |
| 打包 | 1 条命令 | fakeroot dpkg-deb |
| 验证 | 自动 | 手动 dpkg-deb -I |
| 清理 | 自动 | rm -rf |

**总结：从 15+ 步简化到 1 步！** ⚡

## 📊 使用场景

### 开发者工作流程

```bash
# 1. 日常开发调试
./package.sh --build
./debpkg --help

# 2. 测试打包
./package.sh --package

# 3. 验证包
./package.sh --verify

# 4. 本地安装测试
sudo dpkg -i debpkg_*.deb

# 5. 清理
./package.sh --clean
```

### 发布新版本流程

```bash
# 1. 更新版本号（编辑 package.sh）
sed -i 's/VERSION=".*"/VERSION="1.2.0"/' package.sh

# 2. 完整打包
./package.sh

# 3. 测试包
dpkg-deb -I debpkg_1.2.0_amd64.deb
dpkg-deb -c debpkg_1.2.0_amd64.deb

# 4. 安装测试
sudo dpkg -i debpkg_1.2.0_amd64.deb
debpkg --version

# 5. 发布
# - GitHub Release
# - PPA 上传
# - 社区分享
```

### CI/CD 集成

```yaml
# .github/workflows/build.yml
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Install dependencies
        run: sudo apt install dpkg-dev fakeroot
      
      - name: Build package
        run: ./package.sh
      
      - name: Upload artifact
        uses: actions/upload-artifact@v2
        with:
          name: debpkg-package
          path: debpkg_*.deb
```

## 🔒 安全性保障

### Fakeroot 机制
- ✅ **模拟 root** - 创建虚拟的 root 环境
- ✅ **隔离系统** - 不会影响真实系统文件
- ✅ **保留权限** - 正确记录文件所有者和权限
- ✅ **无需 sudo** - 普通用户即可打包

### 安装脚本安全
- ✅ **postinst** - 安装后自动配置，确保可用性
- ✅ **prerm** - 卸载前清理，避免残留
- ✅ **错误处理** - `set -e` 确保失败时回滚
- ✅ **幂等性** - 可重复安装/卸载

### 许可证合规
- ✅ **GPL-2+** - 明确的开源许可证声明
- ✅ **Copyright** - 完整的版权信息
- ✅ **透明** - 所有源码可见可审查

## 🎯 故障排除

### 常见问题速查

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| dpkg-deb: command not found | 缺少工具 | `sudo apt install dpkg-dev` |
| fakeroot: command not found | 缺少工具 | `sudo apt install fakeroot` |
| 打包时权限错误 | 未用 fakeroot | 检查脚本是否完整执行 |
| 包太大 (>1MB) | 未优化 | 压缩文档，strip 调试符号 |
| 依赖冲突 | 版本不兼容 | 检查 DEPENDS 配置 |
| 安装失败 | 文件冲突 | 检查 postinst 脚本 |

### 调试技巧

```bash
# 查看详细打包过程
bash -x ./package.sh

# 保留构建目录查看内容
# 注释掉 cleanup() 调用
ls -R build/deb-pkg/

# 提取包内容分析
dpkg-deb -x debpkg_*.deb /tmp/extract
tree /tmp/extract
```

## 💡 最佳实践

### 1. 版本管理
```bash
# 使用语义化版本号
VERSION="1.2.0"  # Major.Minor.Patch

# 每次发布更新 CHANGES.md
git tag -a "v$VERSION" -m "Release $VERSION"
```

### 2. 依赖声明
```bash
# 明确指定最小版本
DEPENDS="dpkg (>= 1.15), wget | curl, libc6 (>= 2.15)"
```

### 3. 包优化
```bash
# 压缩文档
find build/install/usr/local/share/doc -type f -exec gzip -9 {} \;

# 剥离调试符号
strip build/install/usr/local/bin/debpkg
```

### 4. 兼容性测试
```bash
# 在多个 Ubuntu 版本测试
# - Ubuntu 18.04 LTS
# - Ubuntu 20.04 LTS
# - Ubuntu 22.04 LTS
```

## 📈 进阶主题

### 交叉编译打包

```bash
# 为 ARM64 架构打包
export ARCHITECTURE="arm64"
export CC=aarch64-linux-gnu-gcc
export CXX=aarch64-linux-gnu-g++

./package.sh --build
./package.sh --package
```

### 发布到 PPA

```bash
# 安装工具
sudo apt install devscripts dput

# 签名 changes 文件
debsign debpkg_1.2.0_amd64.changes

# 上传到 PPA
dput ppa:your-ppa/debpkg debpkg_1.2.0_amd64.changes
```

### 创建仓库

```bash
# 创建本地 APT 仓库
apt-ftparchive packages . > Packages
gzip -c Packages > Packages.gz

# 添加到 sources.list
echo "deb [trusted=yes] file:///path/to/repo ./" | \
  sudo tee /etc/apt/sources.list.d/local-debpkg.list
```

## 🔗 相关资源

- [Debian 打包新手指南](https://www.debian.org/doc/manuals/maint-guide/)
- [dpkg-deb 手册](https://manpages.debian.org/dpkg-dev/dpkg-deb.1.en.html)
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Ubuntu Packaging Guide](https://packaging.ubuntu.com/html/)

## 🎉 总结

`package.sh` 提供了：

### 功能完整性
- ✅ 一键式完整打包流程
- ✅ 多种打包模式可选
- ✅ 自动化依赖检测
- ✅ 智能错误处理

### 标准化规范
- ✅ 遵循 Debian Policy
- ✅ 标准包结构
- ✅ 完整的维护脚本
- ✅ 合规的版权信息

### 用户友好性
- ✅ 清晰的彩色输出
- ✅ 详细的进度提示
- ✅ 友好的错误信息
- ✅ 完善的配套文档

### 开发效率
- ⚡ 从 15+ 步简化到 1 步
- 🔧 灵活的配置选项
- 🛡️ 安全的打包环境
- 📦 易于分发和部署

这使得 debpkg 项目的发布和部署变得前所未有的简单！🚀

---

*文档版本：1.0 | 创建时间：2024 年 3 月*
