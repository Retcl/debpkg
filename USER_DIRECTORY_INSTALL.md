# 用户目录安装 - XDG 标准目录结构

## ✅ 更新完成

debpkg 的用户目录安装功能已更新，现在遵循 **XDG Base Directory** 规范，将 DEB 包的文件分发到 `~/.local` 下的标准位置。

## 🔄 主要变更

### 旧方案（已过时）
```
~/.local/opt/<package-name>/
├── bin/
├── lib/
├── include/
└── share/
```

**问题**:
- ❌ 每个包都创建完整的目录树，造成冗余
- ❌ PATH 需要添加多个目录
- ❌ 不符合 Linux 标准惯例
- ❌ 文件分散，难以管理

### 新方案（XDG 标准）✅
```
~/.local/
├── bin/              # 所有包的可执行文件集中存放
├── lib/              # 所有包的库文件集中存放
├── include/<pkg>/    # 按包名组织头文件
├── share/<pkg>/      # 包专用共享数据
├── etc/<pkg>/        # 包专用配置文件
└── share/man/        # 手册页集中存放
```

**优势**:
- ✅ 符合 XDG Base Directory 规范
- ✅ 与系统包管理器目录结构一致
- ✅ 只需添加一个 PATH 路径
- ✅ 文件类型清晰，易于查找
- ✅ 多包共享资源不重复

## 📊 文件映射表

| DEB 包内路径 | 安装到用户目录 | 用途 |
|-------------|---------------|------|
| `/usr/bin/*` | `~/.local/bin/` | 可执行文件 |
| `/usr/lib/*` | `~/.local/lib/` | 库文件 |
| `/usr/include/*` | `~/.local/include/<pkg>/` | 头文件 |
| `/usr/share/*` | `~/.local/share/<pkg>/` | 共享数据 |
| `/etc/*` | `~/.local/etc/<pkg>/` | 配置文件 |
| `/usr/share/man/*` | `~/.local/share/man/` | 手册页 |

## 🔧 环境变量配置

### 必需配置
```bash
export PATH=$HOME/.local/bin:$PATH
```

### 推荐完整配置
```bash
# ~/.bashrc 或 ~/.zshrc
export PATH=$HOME/.local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
export CPATH=$HOME/.local/include:$CPATH
export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH
export MANPATH=$HOME/.local/share/man:$MANPATH
```

## 📝 新增功能

### 1. 智能目录创建
自动创建必要的目录结构：
- `~/.local/bin/`
- `~/.local/lib/`
- `~/.local/include/<pkg>/`
- `~/.local/share/<pkg>/`
- `~/.local/etc/<pkg>/`
- `~/.local/share/man/`

### 2. 安装清单生成
为每个包生成 `INSTALL_MANIFEST` 文件：
```
~/.local/share/<package-name>/INSTALL_MANIFEST
```

记录安装的所有文件和位置，便于：
- 查看安装了什么
- 故障排查
- 手动卸载参考

### 3. 文件分发逻辑
根据文件类型自动分发到对应目录：
- 可执行文件 → `bin/`
- 库文件 → `lib/`
- 头文件 → `include/<pkg>/`
- 共享数据 → `share/<pkg>/`
- 配置文件 → `etc/<pkg>/`
- 手册页 → `share/man/`

### 4. 权限设置
自动设置可执行文件的权限：
```bash
chmod +x ~/.local/bin/*
```

## 💡 使用示例

### 安装应用程序
```bash
./debpkg -u myapp.deb
```

**输出示例**:
```
[INFO] Installing package to user directory...
[INFO] Package name: myapp
[INFO] Installing files to standard directories...
[INFO] Installing binaries to ~/.local/bin/
[INFO] Installing libraries to ~/.local/lib/
[INFO] Installing shared data to ~/.local/share/myapp/
[SUCCESS] Package installed successfully!

Installation summary:
  Executables:   ~/.local/bin/
  Libraries:     ~/.local/lib/
  Shared data:   ~/.local/share/myapp/
  Headers:       ~/.local/include/myapp/
  Config files:  ~/.local/etc/myapp/

To use the installed package:
  export PATH=$HOME/.local/bin:$PATH
  export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
  export MANPATH=$HOME/.local/share/man:$MANPATH
```

### 验证安装
```bash
# 查看安装的可执行文件
ls -la ~/.local/bin/

# 查看特定包的安装信息
cat ~/.local/share/myapp/INSTALL_MANIFEST

# 运行程序
myapp
```

## 🗑️ 卸载方法

### 手动卸载
```bash
# 1. 查看安装清单
cat ~/.local/share/<pkg>/INSTALL_MANIFEST

# 2. 删除文件
rm -rf ~/.local/bin/<executables>
rm -rf ~/.local/lib/<libraries>
rm -rf ~/.local/include/<pkg>
rm -rf ~/.local/share/<pkg>
rm -rf ~/.local/etc/<pkg>
```

### 未来计划
添加自动卸载命令：
```bash
./debpkg --uninstall <package-name>
```

## 📚 相关文档

- [XDG_DIRECTORIES.md](XDG_DIRECTORIES.md) - 详细的 XDG 目录结构说明
- [README.md](README.md) - 项目使用说明
- [MODULES.md](MODULES.md) - 模块架构文档

## 🎯 兼容性

### 支持的系统
- ✅ Debian/Ubuntu（使用 dpkg）
- ✅ 任何遵循 XDG 规范的 Linux 发行版
- ✅ 支持 FHS（Filesystem Hierarchy Standard）

### 环境要求
- 需要 `dpkg-deb` 命令提取 DEB 包
- 需要标准的 shell 环境（bash）
- 需要基本的 Unix 工具（cp, chmod, mkdir）

## ⚠️ 注意事项

### 1. 依赖处理
用户目录安装**不会自动处理依赖**，需要手动安装：
```bash
# 检查依赖
ldd ~/.local/lib/*.so

# 手动安装缺失的依赖
sudo apt install libxxx
```

### 2. 环境变量
必须正确设置环境变量才能使用安装的软件：
```bash
# 临时设置（当前会话）
export PATH=$HOME/.local/bin:$PATH

# 永久设置（添加到 ~/.bashrc）
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

### 3. 库文件路径
如果运行时提示找不到库文件：
```bash
# 方法 1: 设置 LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH

# 方法 2: 使用 ldconfig（需要 root）
echo "$HOME/.local/lib" | sudo tee /etc/ld.so.conf.d/local.conf
sudo ldconfig
```

### 4. 配置文件冲突
某些程序可能会在 `~/.config/` 或 `~/.local/share/` 创建同名目录，注意区分。

## 📈 性能影响

- **安装速度**: 与旧方案相当
- **运行时性能**: 无影响
- **磁盘空间**: 更节省（减少目录冗余）
- **维护成本**: 显著降低

## 🔮 未来改进

1. **自动卸载**: `./debpkg --uninstall <pkg>`
2. **依赖检查**: 安装前检查依赖关系
3. **版本管理**: 支持多版本共存
4. **升级功能**: `./debpkg --upgrade <pkg>`
5. **查询功能**: `./debpkg --list-installed`

---

*更新时间：2024 年 3 月 | 版本：1.0*
