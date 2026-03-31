# debpkg 安装脚本使用指南

## 📖 概述

`install.sh` 是一个自动化安装脚本，用于简化 debpkg 工具的编译和安装过程。它提供了多种安装模式以适应不同的使用场景。

## ✨ 功能特性

- ✅ **自动依赖检测** - 检查系统是否安装了必要的编译工具（gcc, make）
- ✅ **智能清理** - 自动清理旧的编译产物
- ✅ **灵活安装** - 支持系统级和用户级两种安装方式
- ✅ **交互友好** - 提供彩色输出和交互式选择
- ✅ **桌面集成** - 可选创建桌面入口文件
- ✅ **安装验证** - 自动验证安装结果

## 🚀 快速开始

### 基本用法

```bash
./install.sh
```

默认情况下会进入交互模式，您可以根据提示选择安装方式。

## 📋 安装模式

### 1. 交互模式（默认）

```bash
./install.sh
```

脚本会依次执行以下步骤：
1. 检查系统依赖
2. 清理旧文件
3. 编译项目
4. 显示菜单让您选择安装方式

**菜单选项：**
- **1) 系统级安装** - 安装到 `/usr/local/bin`（需要 sudo 权限）
- **2) 用户级安装** - 安装到 `~/.local/bin`（无需 sudo）
- **3) 仅编译不安装** - 只生成可执行文件
- **4) 取消安装** - 退出安装流程

### 2. 系统级安装

```bash
./install.sh --system
```

或直接使用：

```bash
./install.sh -s
```

**特点：**
- ✅ 全局可用，所有用户都能使用
- ✅ 无需配置 PATH 环境变量
- ⚠️ 需要 sudo 权限
- ⚠️ 安装位置：`/usr/local/bin/debpkg`

**适用场景：** 为系统所有用户提供 debpkg 工具

### 3. 用户级安装

```bash
./install.sh --user
```

或直接使用：

```bash
./install.sh -u
```

**特点：**
- ✅ 无需 sudo 权限
- ✅ 仅当前用户可用
- ✅ 自动创建桌面入口（可选）
- ⚠️ 可能需要配置 PATH 环境变量
- 📍 安装位置：`$HOME/.local/bin/debpkg`

**适用场景：** 
- 没有系统管理员权限
- 只想为当前用户安装
- 测试或开发环境

### 4. 仅编译

```bash
./install.sh --build
```

或直接使用：

```bash
./install.sh -b
```

**特点：**
- ✅ 只编译生成可执行文件
- ✅ 不进行安装
- 📍 输出位置：`./debpkg`（项目根目录）

**适用场景：** 
- 开发者调试
- 想手动指定安装位置

### 5. 仅清理

```bash
./install.sh --clean
```

或直接使用：

```bash
./install.sh -c
```

**特点：**
- ✅ 清理编译产物
- ✅ 不执行编译

**适用场景：** 
- 清理项目
- 重新编译前清理

## 🔍 安装流程示例

### 完整的用户级安装流程

```bash
$ ./install.sh --user

========================================
  debpkg 安装脚本
========================================

[INFO] 检查系统依赖...
[SUCCESS] 系统依赖检查通过
[INFO] 清理旧的编译产物...
[SUCCESS] 清理完成
[INFO] 编译项目...
[编译输出...]
[SUCCESS] 编译成功
[INFO] 执行用户级安装到 ~/.local/bin...
[SUCCESS] 用户级安装完成
[INFO] debpkg 已安装到 /home/username/.local/bin/debpkg
[INFO] 检查是否需要创建桌面入口...
是否创建桌面入口文件？(y/N): y
[SUCCESS] 桌面入口已创建：/home/username/.local/share/applications/debpkg.desktop
[INFO] 验证安装...
[SUCCESS] 安装验证成功
  可执行文件位置：/home/username/.local/bin/debpkg

========================================
[SUCCESS] 安装过程完成！
========================================
```

## 🛠️ 故障排除

### 问题 1: "缺少系统依赖"

**错误信息：**
```
[ERROR] 缺少以下依赖：
  - gcc
  - make
```

**解决方案：**
根据您的 Linux 发行版安装编译工具：

```bash
# Ubuntu/Debian
sudo apt install build-essential

# Fedora/RHEL
sudo dnf install gcc make

# Arch Linux
sudo pacman -S gcc make
```

### 问题 2: "debpkg 不在 PATH 中"

**现象：** 安装完成后无法直接运行 `debpkg` 命令

**原因：** `~/.local/bin` 未添加到 PATH 环境变量

**解决方案：**

1. 临时生效（当前终端）：
```bash
export PATH="$HOME/.local/bin:$PATH"
```

2. 永久生效（添加到配置文件）：
```bash
# 对于 bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

# 对于 zsh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### 问题 3: "Permission denied"（系统安装时）

**错误信息：**
```
cp: cannot create regular file '/usr/local/bin/debpkg': Permission denied
```

**原因：** 没有使用 sudo 权限进行系统级安装

**解决方案：**
- 使用 `--system` 选项会自动请求 sudo
- 或者改用用户级安装：`./install.sh --user`

### 问题 4: 编译失败

**可能原因：**
- 源代码有语法错误
- 缺少头文件
- 编译器版本过低

**排查步骤：**
1. 查看具体编译错误信息
2. 确认 GCC 支持 C99 标准：`gcc --version`
3. 尝试手动编译查看详细错误：`make clean && make`

## 📊 安装模式对比

| 特性 | 系统级 (`--system`) | 用户级 (`--user`) |
|------|-------------------|-----------------|
| 需要 sudo | ✅ 是 | ❌ 否 |
| 安装位置 | `/usr/local/bin` | `~/.local/bin` |
| 适用范围 | 所有用户 | 当前用户 |
| PATH 配置 | 无需 | 可能需要 |
| 桌面集成 | 可选 | 可选 |
| 推荐场景 | 生产环境 | 开发/测试环境 |

## 🔒 安全说明

- 系统级安装需要 sudo 权限，请确保您信任此软件
- 建议在安装前查看 `install.sh` 脚本内容
- 用户级安装更加安全，不需要系统权限

## 📝 卸载方法

### 系统级安装卸载

```bash
sudo rm /usr/local/bin/debpkg
```

或使用 Makefile：

```bash
make uninstall
```

### 用户级安装卸载

```bash
rm ~/.local/bin/debpkg
rm ~/.local/share/applications/debpkg.desktop  # 如果创建了桌面入口
```

## 💡 最佳实践

1. **开发环境**：使用 `--user` 模式，避免影响系统
2. **生产环境**：使用 `--system` 模式，确保所有用户可用
3. **持续集成**：使用 `--build` 模式，手动控制安装流程
4. **日常清理**：使用 `--clean` 模式快速清理编译产物

## 🎯 相关文档

- [README.md](README.md) - 项目主文档
- [QUICKSTART.md](QUICKSTART.md) - 快速入门指南
- [MODULES.md](MODULES.md) - 模块架构说明

## 🤝 贡献

如果您在使用过程中遇到问题或有改进建议，欢迎提交 Issue 或 Pull Request。
