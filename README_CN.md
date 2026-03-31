# debpkg - DEB 包安装工具（模块化版本）

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
├── README.md         # 英文文档
├── README_CN.md      # 中文文档（本文件）
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

### 快速安装（推荐）

使用自动化安装脚本：

```bash
./install.sh
```

该脚本将自动：
- 检查系统依赖（gcc, make）
- 清理旧的编译产物
- 编译项目
- 提供交互式安装选项

#### 安装模式

```bash
# 交互模式（默认）
./install.sh

# 系统级安装到 /usr/local/bin（需要 sudo 权限）
./install.sh --system

# 用户级安装到 ~/.local/bin（无需 sudo）
./install.sh --user

# 仅编译不安装
./install.sh --build

# 仅清理编译产物
./install.sh --clean

# 显示帮助信息
./install.sh --help
```

### 手动编译

#### 基础编译

```bash
make
```

这将编译所有模块并生成 `debpkg` 可执行文件。

#### 其他编译选项

```bash
make clean      # 清理编译文件
make install    # 安装到 /usr/local/bin
make uninstall  # 从 /usr/local/bin 移除
```

### 编译要求

- GCC 编译器（支持 C99 标准）
- Make 构建工具
- Linux/Debian 系统

### 创建 Debian 包

使用自动化打包脚本：

```bash
./package.sh
```

这将编译项目并生成可安装的 .deb 包。

#### 打包模式

```bash
# 完整流程：编译 + 打包
./package.sh

# 清理构建目录和旧包
./package.sh --clean

# 仅编译
./package.sh --build

# 仅打包（假设已编译）
./package.sh --package

# 验证现有包
./package.sh --verify

# 显示帮助
./package.sh --help
```

**输出文件：** `debpkg_<VERSION>_amd64.deb`

详细的打包文档请查看 [PACKAGE_GUIDE.md](PACKAGE_GUIDE.md)

## 📖 使用方法

### 基本语法

```bash
./debpkg <COMMAND> [OPTIONS] <ARGUMENT>
```

### 命令

| 命令 | 功能 | 说明 |
|------|------|------|
| `install <package.deb>` | 安装 DEB 包 | 见下方安装选项 |
| `uninstall <package-name>` | 从用户目录卸载 | 删除包并清理 |

### 安装选项

| 选项 | 功能 | 说明 |
|------|------|------|
| `-s, --system` | 系统级安装 | 使用 dpkg，需要 sudo |
| `-u, --user` | 用户目录安装 | 安装到 ~/.local，无需 root |
| `-h, --help` | 显示帮助 | 查看使用说明 |

### 卸载选项

| 选项 | 功能 | 说明 |
|------|------|------|
| `-h, --help` | 显示帮助 | 查看使用说明 |

### 全局选项

| 选项 | 功能 | 说明 |
|------|------|------|
| `-h, --help` | 显示全局帮助 | 查看总体帮助 |
| `-v, --version` | 显示版本 | 查看版本信息 |

### 使用示例

#### 1. 系统级安装

```bash
sudo ./debpkg install -s package.deb
```

需要 sudo 权限，安装到 `/usr/bin`、`/usr/lib` 等系统目录。

#### 2. 用户目录安装（推荐）

```bash
./debpkg install -u package.deb
```

无需 root 权限，安装到 `~/.local/bin`、`~/.local/lib` 等用户目录。

#### 3. 交互式安装

```bash
./debpkg install package.deb
```

提示您选择安装方式。

#### 4. 卸载包

```bash
./debpkg uninstall package-name
# 或
./debpkg remove package-name
```

智能检查依赖关系后删除。

#### 5. 查看帮助

```bash
# 全局帮助
./debpkg --help

# 安装命令帮助
./debpkg install --help

# 卸载命令帮助
./debpkg uninstall --help
```

## 🎯 高级功能

### 自动依赖安装

在用户目录安装时，自动检测并安装缺失的依赖：

```bash
$ ./debpkg install -u myapp.deb

[INFO] 正在检查依赖...
[WARNING] 检测到缺失的依赖：
  - libfoo>=1.0.0
  - libbar>=2.0.0

[INFO] 依赖处理选项：
  1. 自动下载并安装（推荐）
  2. 手动安装
  3. 继续安装
  4. 取消安装

请输入您的选择 [1-4]: 1

[INFO] 开始自动安装依赖...
[SUCCESS] 所有依赖安装成功！
```

### 智能卸载与依赖保护

卸载时智能保护仍被使用的依赖：

```bash
$ ./debpkg uninstall myapp

[INFO] 正在卸载包：myapp
[INFO] 正在检查依赖...
  保留 libfoo（被 1 个其他包使用）
  删除未使用的依赖：libbar

[SUCCESS] 卸载完成！
保留的依赖：1
删除的依赖：1
```

### Desktop 文件集成

为 GUI 应用自动创建桌面入口：

```bash
./debpkg install -u gui-app.deb

# 创建：~/.local/share/applications/gui-app.desktop
```

### 环境变量配置

自动提示配置环境变量：

```bash
[INFO] 正在检查环境变量配置...
[WARNING] 环境变量可能需要配置！

是否要生成环境配置脚本？(y/n): y

[SUCCESS] 环境脚本已创建：~/.local/debpkg_env.sh

激活方法：source ~/.local/debpkg_env.sh
```

## 📊 安装位置

### 系统级安装 (`-s`)

| 类型 | 位置 | 需要 |
|------|------|------|
| 可执行文件 | `/usr/bin/` | sudo |
| 库文件 | `/usr/lib/` | sudo |
| 数据文件 | `/usr/share/<pkg>/` | sudo |
| 配置文件 | `/etc/<pkg>/` | sudo |
| 手册页 | `/usr/share/man/` | sudo |

### 用户目录安装 (`-u`)

| 类型 | 位置 | 需要 |
|------|------|------|
| 可执行文件 | `~/.local/bin/` | 无 |
| 库文件 | `~/.local/lib/` | 无 |
| 数据文件 | `~/.local/share/<pkg>/` | 无 |
| 配置文件 | `~/.local/etc/<pkg>/` | 无 |
| 手册页 | `~/.local/share/man/` | 无 |
| Desktop 文件 | `~/.local/share/applications/` | 无 |
| 依赖记录 | 数据库记录 | 无 |

## 🍁 文档说明

本项目提供双语文档：

- **🇬🇧 English**: [README.md](README.md) (**主版本**)
- **🇨🇳 中文**: [README_CN.md](README_CN.md)（本文件）

### 更多文档

完整文档列表请查看 [DOCS.md](DOCS.md)。

**功能文档**:
- [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md) - 依赖管理
- [UNINSTALL_FEATURE.md](UNINSTALL_FEATURE.md) - 智能卸载
- [ENV_CONFIG_FEATURE.md](ENV_CONFIG_FEATURE.md) - 环境配置
- [DESKTOP_FILE_FEATURE.md](DESKTOP_FILE_FEATURE.md) - Desktop 集成

**技术文档**:
- [MODULES.md](MODULES.md) - 模块架构
- [CLI_REFACTOR.md](CLI_REFACTOR.md) - CLI 设计
- [CHANGES.md](CHANGES.md) - 变更历史
- [QUICKSTART.md](QUICKSTART.md) - 快速入门指南

## ⚠️ 注意事项

### 用户目录安装

- 无需 root 权限
- 适合开发测试
- 不影响系统包
- 支持多版本共存

### 系统级安装

- 需要 sudo 权限
- 适合生产环境
- 可能与系统包冲突
- 谨慎使用

## 🔒 安全建议

1. **验证包来源**
   - 仅安装来自可信源的 DEB 包
   - 可能时验证包签名

2. **安装前审查**
   - 安装前检查包内容
   - 了解将安装哪些文件

3. **优先使用用户目录**
   - 测试时优先使用 `-u` 选项
   - 限制恶意包的潜在损害

## 🐛 故障排除

### 问题："Permission denied"

**解决**：使用用户目录安装
```bash
./debpkg install -u package.deb
```

### 问题："Dependency missing"

**解决**：使用自动依赖安装
```bash
# 提示时选择选项 1
```

### 问题：安装后 "Command not found"

**解决**：添加 `~/.local/bin` 到 PATH
```bash
export PATH=$HOME/.local/bin:$PATH
```

## 🤝 贡献

欢迎贡献！请：

1. Fork 仓库
2. 创建功能分支
3. 进行更改
4. 提交 Pull Request

## 📄 许可证

本项目采用 GPL v2 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 🙏 致谢

感谢所有贡献者和开源社区！

---

**版本**: 1.0.0  
**语言**: 中文  
**最后更新**: 2024
