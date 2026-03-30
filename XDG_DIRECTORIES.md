# XDG 标准目录结构说明

## 📋 概述

debpkg 的用户目录安装模式现在遵循 **XDG Base Directory** 规范，将文件安装到 `~/.local` 下的标准位置，确保与系统包管理器的目录结构保持一致。

## 🏛️ XDG Base Directory 规范

XDG (X Desktop Group) 规范定义了一套标准的目录结构，用于组织用户级别的文件：

- `~/.local/bin/` - 可执行文件
- `~/.local/lib/` - 库文件
- `~/.local/include/` - 头文件
- `~/.local/share/` - 共享数据
- `~/.local/etc/` - 配置文件
- `~/.local/share/man/` - 文档和手册页

## 📁 debpkg 安装的目录结构

### 完整示例

```
~/.local/
├── bin/                           # 可执行文件
│   ├── myapp                      # 主程序
│   ├── myapp-cli                  # 命令行工具
│   └── myapp-gui                  # GUI 程序
│
├── lib/                           # 库文件
│   ├── libmyapp.so               # 共享库
│   ├── libmyapp.a                # 静态库
│   └── libmyapp.so.1.0           # 版本化库
│
├── include/                       # 头文件（按包名组织）
│   └── myapp/
│       ├── foo.h
│       ├── bar.h
│       └── version.h
│
├── share/                         # 共享数据
│   ├── myapp/                     # 包专用数据目录
│   │   ├── icons/                # 图标文件
│   │   │   ├── 16x16/
│   │   │   ├── 32x32/
│   │   │   └── scalable/
│   │   ├── themes/               # 主题文件
│   │   ├── data/                 # 应用程序数据
│   │   │   ├── database.db
│   │   │   └── templates/
│   │   ├── translations/         # 翻译文件
│   │   └── INSTALL_MANIFEST      # 安装清单
│   │
│   └── man/                       # 手册页
│       ├── man1/                 # 用户命令
│       │   └── myapp.1
│       └── man3/                 # 库函数
│           └── myapp_init.3
│
├── etc/                           # 配置文件（按包名组织）
│   └── myapp/
│       ├── config.conf           # 主配置文件
│       ├── settings.ini          # 设置文件
│       └── plugins/              # 插件配置
│           └── plugin.conf
│
└── share/myapp/                   # 包元数据
    └── INSTALL_MANIFEST           # 安装清单文件
```

## 📊 文件映射关系

| DEB 包内路径 | 用户目录安装位置 | 说明 |
|-------------|-----------------|------|
| `/usr/bin/*` | `~/.local/bin/` | 可执行文件 |
| `/usr/sbin/*` | `~/.local/bin/` | 系统管理命令 |
| `/usr/lib/*` | `~/.local/lib/` | 库文件 |
| `/usr/libexec/*` | `~/.local/libexec/` | 内部执行程序 |
| `/usr/include/*` | `~/.local/include/<pkg>/` | C/C++ 头文件 |
| `/usr/share/*` | `~/.local/share/<pkg>/` | 架构无关的共享数据 |
| `/etc/*` | `~/.local/etc/<pkg>/` | 配置文件 |
| `/usr/share/man/*` | `~/.local/share/man/` | Unix 手册页 |
| `/usr/share/doc/*` | `~/.local/share/<pkg>/doc/` | 文档文件 |
| `/usr/share/icons/*` | `~/.local/share/<pkg>/icons/` | 图标文件 |
| `/usr/share/locale/*` | `~/.local/share/<pkg>/locale/` | 国际化文件 |

## 🔧 环境变量配置

为了使用安装的文件，需要设置以下环境变量：

### 基础配置（必需）

```bash
# 添加到 ~/.bashrc 或 ~/.zshrc
export PATH=$HOME/.local/bin:$PATH
```

### 完整配置（推荐）

```bash
# 可执行文件路径
export PATH=$HOME/.local/bin:$PATH

# 库文件路径
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH

# 头文件路径（编译时使用）
export CPATH=$HOME/.local/include:$CPATH

#  pkg-config 路径
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH

# 手册页路径
export MANPATH=$HOME/.local/share/man:$MANPATH

# XDG 数据路径
export XDG_DATA_DIRS=$HOME/.local/share:$XDG_DATA_DIRS
```

### 永久配置

```bash
# 自动检测并添加到 ~/.bashrc
cat >> ~/.bashrc << 'EOF'

# debpkg installed packages
export PATH=$HOME/.local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
export CPATH=$HOME/.local/include:$CPATH
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH
export MANPATH=$HOME/.local/share/man:$MANPATH
EOF

source ~/.bashrc
```

## 📝 安装清单文件

每个包安装后都会生成 `INSTALL_MANIFEST` 文件，记录安装的文件和位置：

**位置**: `~/.local/share/<package-name>/INSTALL_MANIFEST`

**内容示例**:
```
# Installation Manifest for myapp
# Installed at: /home/user/.local/share/myapp

Binaries: ~/.local/bin/
Libraries: ~/.local/lib/
Shared data: ~/.local/share/myapp/
Headers: ~/.local/include/myapp/
Config files: ~/.local/etc/myapp/
Man pages: ~/.local/share/man/
```

**用途**:
- 查看包安装了哪些文件
- 卸载时参考
- 故障排查

## 🎯 优势对比

### vs 旧方案（opt 子目录）

**旧方案**:
```
~/.local/opt/myapp/
├── bin/
├── lib/
├── include/
└── share/
```

**新方案（XDG 标准）**:
```
~/.local/
├── bin/           ← 所有包的二进制文件集中存放
├── lib/           ← 所有包的库文件集中存放
├── include/myapp/ ← 按包名组织头文件
├── share/myapp/   ← 包专用数据
└── etc/myapp/     ← 包专用配置
```

### 优势

✅ **统一管理**: 所有包的可执行文件都在 `bin/`，便于管理  
✅ **避免重复**: 不需要为每个包创建完整的目录树  
✅ **符合规范**: 遵循 XDG 标准，与其他工具兼容  
✅ **易于发现**: 文件类型清晰，便于查找  
✅ **简化 PATH**: 只需添加 `~/.local/bin` 到 PATH  

## 🗑️ 卸载方法

### 手动卸载

1. 查看安装清单：
   ```bash
   cat ~/.local/share/<package-name>/INSTALL_MANIFEST
   ```

2. 删除安装的文件：
   ```bash
   rm -rf ~/.local/bin/<executables>
   rm -rf ~/.local/lib/<libraries>
   rm -rf ~/.local/include/<package-name>
   rm -rf ~/.local/share/<package-name>
   rm -rf ~/.local/etc/<package-name>
   ```

### 未来功能

计划添加卸载命令：
```bash
./debpkg --uninstall <package-name>
```

## 📦 实际示例

### 安装 Firefox

```bash
./debpkg -u firefox.deb
```

**安装结果**:
```
~/.local/
├── bin/firefox                    # 可执行文件
├── lib/
│   └── firefox/                   # 库文件和资源
├── share/firefox/
│   ├── icons/                     # 图标
│   ├── browser/                   # 浏览器资源
│   └── INSTALL_MANIFEST
└── share/man/man1/firefox.1       # 手册页
```

### 安装开发库（libcurl）

```bash
./debpkg -u libcurl.deb
```

**安装结果**:
```
~/.local/
├── bin/curl                       # 命令行工具
├── lib/
│   ├── libcurl.so                 # 共享库
│   └── libcurl.a                  # 静态库
├── include/curl/                  # 头文件
│   ├── curl.h
│   ├── easy.h
│   └── ...
└── share/man/
    ├── man1/curl.1
    └── man3/
        └── curl_easy_init.3
```

## 🔍 查看已安装的包

```bash
# 列出所有已安装的包
ls -d ~/.local/share/*/ | grep -v man

# 查看特定包的安装信息
cat ~/.local/share/<package-name>/INSTALL_MANIFEST

# 查看安装的可执行文件
ls ~/.local/bin/

# 查看安装的库文件
ls ~/.local/lib/
```

## ⚙️ 高级配置

### 自定义安装前缀

未来可能支持：
```bash
./debpkg -u --prefix=~/.custom package.deb
```

### 多版本共存

```bash
~/.local/
├── lib/
│   ├── python3.8/
│   └── python3.9/
└── share/
    └── ...
```

## 📚 参考资料

- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/)
- [Filesystem Hierarchy Standard](https://refspecs.linuxfoundation.org/FHS_3.0/fhs/index.html)
- [GNU Makefile 最佳实践](https://www.gnu.org/software/make/manual/)

---

*最后更新：2024 年 3 月 | 版本：1.0*
