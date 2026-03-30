# debpkg - DEB Package Installer (模块化版本)

一个用 C 语言编写的**模块化**DEB 包安装工具，支持系统级安装和用户目录安装。

## ✨ 特性

- ✅ **模块化设计** - 代码结构清晰，易于维护
- ✅ **系统级安装** - 使用 `dpkg` 将包安装到系统目录
- ✅ **用户目录安装** - 将包安装到 `~/.local`（无需 root）
- ✅ **智能依赖检查** - 自动检测并提示缺失的依赖
- ✅ **自动依赖安装** - 从 APT 仓库自动下载并安装缺失依赖（NEW! 🎉）
- ✅ **交互式依赖处理** - 提供多种依赖处理选项
- ✅ **彩色输出** - 清晰的终端彩色提示
- ✅ **交互式选择** - 支持交互式选择安装方式

## 📁 项目结构

```
debpkg/
├── include/            # 头文件目录
│   ├── cli.h          # 命令行交互模块
│   ├── extract.h      # DEB 包提取模块
│   ├── install_system.h  # 系统级安装模块
│   ├── install_user.h    # 用户目录安装模块
│   └── utils.h        # 通用工具函数模块
│
├── src/              # 源代码目录
│   ├── main.c        # 主程序入口
│   ├── cli.c         # 命令行交互实现
│   ├── utils.c       # 工具函数实现
│   ├── extract.c     # DEB 包提取实现
│   ├── install_system.c  # 系统级安装实现
│   └── install_user.c    # 用户目录安装实现
│
├── obj/              # 编译目标文件（自动生成）
│   ├── cli.o
│   ├── extract.o
│   ├── install_system.o
│   ├── install_user.o
│   ├── main.o
│   └── utils.o
│
├── Makefile          # 编译配置
├── README.md         # 项目说明
├── MODULES.md        # 详细架构文档
├── QUICKSTART.md     # 快速上手指南
├── CHANGES.md        # 变更记录
└── test.sh           # 测试脚本
```

**目录说明**：
- **include/** - 所有头文件 (.h)，存放模块的公共接口声明
- **src/** - 所有源文件 (.c)，存放模块的具体实现
- **obj/** - 编译生成的目标文件，由 Makefile 自动创建

详细的模块说明请查看 [MODULES.md](MODULES.md)

## 🔧 编译方法

### 基础编译

```bash
make
```

这将编译所有模块并生成 `debpkg` 可执行文件。

### 其他编译选项

```bash
make clean      # 清理编译文件
make install    # 安装到 /usr/local/bin
make uninstall  # 从 /usr/local/bin 移除
```

### 编译要求

- GCC 编译器（支持 C99 标准）
- Make 构建工具
- Linux/Debian 系统

## 📖 使用方法

### 基本语法

```bash
./debpkg [OPTIONS] <package.deb>
```

### 命令参数

| 参数 | 功能 | 说明 |
|------|------|------|
| `-s, --system` | 系统级安装 | 使用 dpkg，需要 sudo |
| `-u, --user` | 用户目录安装 | 安装到 ~/.local，无需 root |
| `-h, --help` | 显示帮助 | 查看使用说明 |
| `-v, --version` | 显示版本 | 查看版本信息 |

### 使用示例

#### 1. 系统级安装

```bash
./debpkg -s package.deb
```

或

```bash
sudo ./debpkg --system package.deb
```

**特点**:
- 使用 `dpkg -i` 安装包
- 自动处理依赖关系
- 安装到系统目录（/usr, /etc 等）
- 需要管理员权限

#### 2. 用户目录安装

```bash
./debpkg -u package.deb
```

这会将包安装到您的家目录下的 `~/.local` 目录，遵循 **XDG Base Directory** 规范。

**标准目录结构**：

```
~/.local/
├── bin/                    # 可执行文件
│   ├── program1
│   └── program2
├── lib/                    # 库文件
│   ├── libfoo.so
│   └── libbar.a
├── include/                # 头文件
│   └── <package-name>/
│       ├── foo.h
│       └── bar.h
├── share/                  # 共享数据
│   ├── <package-name>/     # 包专用数据
│   │   ├── icons/
│   │   ├── themes/
│   │   └── data/
│   └── man/                # 手册页
│       ├── man1/
│       └── man3/
├── etc/                    # 配置文件
│   └── <package-name>/
│       └── config.conf
└── share/<package-name>/   # 包元数据
    └── INSTALL_MANIFEST    # 安装清单
```

**安装位置说明**：

| DEB 包内路径 | 安装到用户目录 | 用途 |
|-------------|---------------|------|
| `/usr/bin/*` | `~/.local/bin/` | 可执行文件 |
| `/usr/lib/*` | `~/.local/lib/` | 库文件 |
| `/usr/include/*` | `~/.local/include/<pkg>/` | 头文件 |
| `/usr/share/*` | `~/.local/share/<pkg>/` | 共享数据 |
| `/etc/*` | `~/.local/etc/<pkg>/` | 配置文件 |
| `/usr/share/man/*` | `~/.local/share/man/` | 手册页 |

**特点**:
- 安装到 `~/.local/opt/<package-name>`
- 无需 root 权限
- 自动创建符号链接到 `~/.local/bin`
- 不影响系统

**环境变量配置**:

```bash
# 临时设置
export PATH=$HOME/.local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH

# 永久设置（添加到 ~/.bashrc 或 ~/.zshrc）
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

#### 3. 交互式安装

```bash
./debpkg package.deb
```

不指定选项时，程序会提示选择安装方式：
```
Choose installation method:
  [1] System-wide (requires sudo, uses dpkg)
  [2] User directory (~/.local, no root required)
Enter your choice [1/2]: 
```

## 🏗️ 模块架构

### 模块划分

1. **utils 模块** (`utils.c/h`)
   - 通用工具函数
   - 文件操作、目录创建
   - 彩色输出函数

2. **extract 模块** (`extract.c/h`)
   - DEB 包提取功能
   - 解析包名
   - 依赖 `dpkg-deb` 命令

3. **install_system 模块** (`install_system.c/h`)
   - 系统级安装实现
   - 使用 `dpkg` 和 `sudo`

4. **install_user 模块** (`install_user.c/h`)
   - 用户目录安装实现
   - 自动配置符号链接

5. **cli 模块** (`cli.c/h`)
   - 命令行参数解析
   - 用户交互界面

6. **main 模块** (`main.c`)
   - 程序入口
   - 协调各模块工作

### 依赖关系

```
main.c → cli.c → install_system.c, install_user.c
                     ↓                    ↓
                 utils.c ←------------ extract.c
```

详细的架构说明请查看 [MODULES.md](MODULES.md)

## 💡 工作原理

### 系统级安装流程

1. 检查 DEB 文件是否存在
2. 使用 `sudo dpkg -i` 安装包
3. 处理可能的依赖问题

### 用户目录安装流程

1. 获取用户主目录 (`$HOME`)
2. 创建 `~/.local` 目录结构
3. 创建临时目录
4. 使用 `dpkg-deb --control` 提取控制信息
5. 解析 control 文件获取包名
6. 使用 `dpkg-deb -x` 提取数据文件到 `~/.local/opt/<package-name>`
7. 为可执行文件创建符号链接到 `~/.local/bin`
8. 清理临时目录

## ⚖️ 两种安装方式对比

| 特性 | 系统级 (-s) | 用户级 (-u) |
|------|------------|-------------|
| 权限要求 | 需要 sudo | 无需 root |
| 安装位置 | /usr, /etc 等系统目录 | ~/.local/opt/ |
| 依赖管理 | ✅ 自动处理 | ⚠️ 手动处理 |
| 影响范围 | 所有用户 | 仅当前用户 |
| 安全性 | 影响系统稳定性 | ✅ 隔离安全 |
| 卸载方式 | apt/dpkg | 删除目录即可 |
| 适用场景 | 生产环境、系统服务 | 开发测试、个人工具 |

## 🔍 故障排除

### 编译错误

**问题**: `implicit declaration of function`

**解决**: 确保已定义 `_GNU_SOURCE` 宏，并且在使用前包含了正确的头文件。

### 用户目录安装后命令不可用

**问题**: 找不到安装的命令

**解决**: 
```bash
export PATH=$HOME/.local/bin:$PATH
```

### 运行时找不到库

**问题**: error while loading shared libraries

**解决**:
```bash
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
```

### 权限错误

**问题**: Permission denied（系统级安装）

**解决**: 使用 sudo
```bash
sudo ./debpkg -s package.deb
```

## 📝 开发指南

### 添加新功能

1. 确定功能所属模块
2. 在对应的 `.h` 文件中声明函数
3. 在对应的 `.c` 文件中实现
4. 更新 Makefile 依赖（如需要）
5. 重新编译测试

### 调试技巧

```bash
# 清理并重编
make clean && make

# 查看编译警告
make 2>&1 | grep warning

# 单步调试
gdb ./debpkg
```

### 代码规范

- 函数命名：小写字母 + 下划线（如 `print_info`）
- 宏命名：大写字母 + 下划线（如 `MAX_PATH_LEN`）
- 每个模块职责单一
- 头文件使用宏保护

## 📚 文档

- [README.md](README.md) - 使用说明（本文档）
- [MODULES.md](MODULES.md) - 详细架构文档
- [QUICKSTART.md](QUICKSTART.md) - 快速上手指南

## 🛠️ 测试

运行测试脚本：

```bash
./test.sh
```

测试内容包括：
- ✅ 帮助信息显示
- ✅ 版本信息显示
- ✅ 参数验证
- ✅ 文件存在性检查
- ✅ 系统依赖检查

## 📄 许可证

本项目仅供学习和研究使用。

## 🤝 贡献

欢迎提交问题和改进建议！

---

**享受模块化的力量！** 🚀
