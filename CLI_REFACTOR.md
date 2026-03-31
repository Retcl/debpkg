# 命令行接口重构 - 子命令格式

## 🎯 变更概述

将命令行接口从传统的选项优先格式改为现代的子命令格式，类似于 `git`、`apt`、`docker` 等工具的使用方式。

## 📊 新旧对比

### ❌ 旧格式（已废弃）
```bash
./debpkg -u package.deb          # 安装到用户目录
./debpkg -s package.deb          # 安装到系统
./debpkg -r package-name         # 卸载包
```

### ✅ 新格式（推荐）
```bash
./debpkg install -u package.deb      # 安装到用户目录
./debpkg install -s package.deb      # 安装到系统
./debpkg uninstall package-name      # 卸载包
```

## 🔧 新的命令结构

### 基本语法
```bash
./debpkg <COMMAND> [OPTIONS] <ARGUMENT>
```

### 可用命令

#### 1. `install` - 安装包
```bash
./debpkg install [OPTIONS] <package.deb>
```

**选项**:
- `-s, --system` - 安装到系统目录（需要 sudo）
- `-u, --user` - 安装到用户目录（~/.local）
- `-h, --help` - 显示安装帮助

**示例**:
```bash
# 交互式安装（询问安装位置）
./debpkg install package.deb

# 直接安装到用户目录
./debpkg install -u package.deb

# 直接安装到系统
./debpkg install -s package.deb

# 查看安装帮助
./debpkg install --help
```

#### 2. `uninstall` (或 `remove`) - 卸载包
```bash
./debpkg uninstall [OPTIONS] <package-name>
```

**选项**:
- `-h, --help` - 显示卸载帮助

**示例**:
```bash
# 卸载包
./debpkg uninstall myapp

# 使用 remove 别名
./debpkg remove myapp

# 查看卸载帮助
./debpkg uninstall --help
```

#### 3. 全局选项
```bash
./debpkg --help        # 显示总体帮助
./debpkg --version     # 显示版本信息
```

## 📋 使用示例

### 安装场景

```bash
# 场景 1: 安装到用户目录（推荐）
./debpkg install -u google-chrome.deb

# 场景 2: 安装到系统（需要 root）
sudo ./debpkg install -s vscode.deb

# 场景 3: 交互式选择
./debpkg install package.deb
# 然后选择 [1] System-wide 或 [2] User directory

# 场景 4: 批量安装脚本
for pkg in *.deb; do
    ./debpkg install -u "$pkg"
done
```

### 卸载场景

```bash
# 场景 1: 卸载单个包
./debpkg uninstall google-chrome

# 场景 2: 使用 remove 别名
./debpkg remove vscode

# 场景 3: 批量卸载
./debpkg uninstall app1 app2 app3

# 场景 4: 查看已安装包后卸载
ls ~/.local/share/ | grep -E "^[a-z]"  # 查看已安装
./debpkg uninstall package-name        # 卸载
```

## 🎨 错误处理

### 无命令
```bash
$ ./debpkg

[ERROR] No command specified
Usage: ./debpkg <COMMAND> [OPTIONS] <ARGUMENT>

Commands:
  install <package.deb>     Install a DEB package
  uninstall <package-name>  Uninstall a package from user directory
...
```

### 无效命令
```bash
$ ./debpkg upgrade

[ERROR] Unknown command: upgrade
Use './debpkg --help' for usage information.
```

### 缺少参数
```bash
$ ./debpkg install

[ERROR] No DEB file specified
Usage: ./debpkg install [OPTIONS] <package.deb>
Use './debpkg install --help' for usage information.
```

### 文件不存在
```bash
$ ./debpkg install nonexistent.deb

[ERROR] DEB file not found: nonexistent.deb
```

### 包未安装
```bash
$ ./debpkg uninstall nonexistent

[ERROR] Package is not installed
[ERROR] Failed to uninstall package
```

## 🔍 技术实现

### 命令解析流程

```c
// 1. 解析第一个参数（命令）
if (strcmp(argv[1], "install") == 0) {
    cmd = CMD_INSTALL;
} else if (strcmp(argv[1], "uninstall") == 0) {
    cmd = CMD_UNINSTALL;
}

// 2. 根据命令类型解析后续参数
if (cmd == CMD_INSTALL) {
    // 解析 -s, -u 等选项
    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) mode = MODE_SYSTEM;
        else if (strcmp(argv[i], "-u") == 0) mode = MODE_USER;
        else deb_file = argv[i];
    }
} else if (cmd == CMD_UNINSTALL) {
    // 解析包名
    for (i = 2; i < argc; i++) {
        pkg_name = argv[i];
    }
}

// 3. 执行对应操作
if (cmd == CMD_INSTALL) {
    install_xxx(deb_file);
} else if (cmd == CMD_UNINSTALL) {
    uninstall_package_smart(pkg_name, ...);
}
```

### 枚举定义

```c
// 命令类型枚举
typedef enum {
    CMD_NONE,       // 未指定命令
    CMD_INSTALL,    // 安装命令
    CMD_UNINSTALL   // 卸载命令
} CommandType;

// 安装模式枚举
typedef enum {
    MODE_NONE,      // 未指定模式（交互式）
    MODE_SYSTEM,    // 系统级安装
    MODE_USER       // 用户目录安装
} InstallMode;
```

## 📈 优势分析

### ✅ 优点

1. **更直观**
   - 命令动词明确表达意图
   - 符合现代 CLI 工具习惯

2. **易扩展**
   - 可以轻松添加新命令（如 `list`, `search`, `update`）
   - 每个命令可以有独立的选项

3. **自文档化**
   - `install` 比 `-u` 更易理解
   - 子命令帮助更精准

4. **避免冲突**
   - 不同命令可以有相同选项名
   - 选项语义依赖于命令

5. **符合直觉**
   ```bash
   git commit -m "msg"
   apt install package
   docker run image
   # 现在 debpkg 也一样
   debpkg install package.deb
   ```

### ⚠️ 注意事项

1. **向后不兼容**
   - 旧的 `-u package.deb` 格式不再支持
   - 需要更新使用习惯和脚本

2. **学习成本**
   - 老用户需要适应新格式
   - 文档需要同步更新

## 🔮 未来扩展

基于新的子命令架构，可以轻松添加更多功能：

```bash
# 列出已安装的包
./debpkg list [--user|--system]

# 搜索包
./debpkg search keyword

# 查看包信息
./debpkg info package.deb

# 清理缓存
./debpkg clean

# 验证包
./debpkg verify package.deb

# 下载依赖
./debpkg download-deps package.deb
```

## 📚 相关资源

### 帮助命令
```bash
# 总体帮助
./debpkg --help

# 子命令帮助
./debpkg install --help
./debpkg uninstall --help
```

### 文档
- [README.md](README.md) - 项目主文档
- [MODULES.md](MODULES.md) - 模块架构说明
- [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md) - 依赖管理功能
- [UNINSTALL_FEATURE.md](UNINSTALL_FEATURE.md) - 卸载功能说明

## 🎉 总结

新的子命令格式让 debpkg 的命令行接口更加现代化和易用：

✅ **清晰明确** - 命令动词直接表达操作  
✅ **易于扩展** - 可以方便地添加新功能  
✅ **符合习惯** - 与主流工具保持一致  
✅ **自包含帮助** - 每个命令有独立的帮助信息  

这使得 debpkg 从一个简单的安装工具进化为一个完整的包管理平台！🚀

---

*最后更新：2024 | debpkg v1.0.0*
