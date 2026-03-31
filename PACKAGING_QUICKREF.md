# debpkg 打包快速参考

## 🚀 一键打包

```bash
./package.sh
```

## 📋 常用命令速查

### 完整流程
```bash
./package.sh                    # 编译 + 打包全流程
```

### 单独操作
```bash
./package.sh --build            # 仅编译
./package.sh --package          # 仅打包（假设已编译）
./package.sh --verify           # 验证包
./package.sh --clean            # 清理
```

### 帮助信息
```bash
./package.sh --help             # 显示帮助
```

## 📦 输出文件

```
debpkg_1.1.0_amd64.deb    # 生成的 Debian 包
```

## 🔧 安装生成的包

```bash
# 直接安装
sudo dpkg -i debpkg_*.deb

# 或自动解决依赖
sudo apt install ./debpkg_*.deb

# 查看信息
dpkg-deb -I debpkg_*.deb

# 列出内容
dpkg-deb -c debpkg_*.deb

# 卸载
sudo dpkg -r debpkg
```

## ⚙️ 自定义配置

编辑 `package.sh`：

```bash
VERSION="1.2.0"               # 版本号
MAINTAINER="Name <email>"     # 维护者
DEPENDS="dpkg, wget | curl"   # 依赖
ARCHITECTURE="amd64"          # 架构
```

## 🛠️ 依赖工具

```bash
# 检查是否安装
dpkg-deb --version
fakeroot --version

# 安装缺失的工具
sudo apt install dpkg-dev fakeroot
```

## 📊 打包流程

```
检测依赖 → 准备目录 → 编译 → 安装文件 
→ 创建配置 → 打包 → 验证 → 清理
```

**时间：** ~5-10 秒

## 🎯 典型工作流

### 开发调试
```bash
./package.sh --build
./debpkg --help
```

### 发布版本
```bash
# 1. 更新版本号
sed -i 's/VERSION=".*"/VERSION="1.2.0"/' package.sh

# 2. 打包
./package.sh

# 3. 测试
sudo dpkg -i debpkg_1.2.0_amd64.deb
debpkg --version
```

### 清理
```bash
./package.sh --clean
```

## 📁 包结构

```
debpkg_*.deb
├── DEBIAN/
│   ├── control      # 包信息
│   ├── postinst     # 安装脚本
│   └── prerm        # 卸载脚本
└── usr/local/
    ├── bin/debpkg
    └── share/doc/debpkg/
        └── *.md
```

## ⚡ 快速故障排除

| 问题 | 解决方案 |
|------|----------|
| 缺少 dpkg-deb | `sudo apt install dpkg-dev` |
| 缺少 fakeroot | `sudo apt install fakeroot` |
| 包太大 | 压缩文档，strip 符号 |
| 权限错误 | 确保使用 fakeroot |

## 📖 详细文档

- [PACKAGE_GUIDE.md](PACKAGE_GUIDE.md) - 完整指南
- [PACKAGE_SCRIPT_SUMMARY.md](PACKAGE_SCRIPT_SUMMARY.md) - 特性总结

---

*快速参考 | 更新时间：2024 年 3 月*
