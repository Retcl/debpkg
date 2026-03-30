# debpkg 快速使用指南

## 编译

```bash
make
```

## 使用方法

### 1. 系统级安装（需要 sudo）

```bash
./debpkg -s package.deb
```

这会使用 `dpkg` 将包安装到系统目录，需要管理员权限。

**适用场景：**
- 安装系统级软件
- 需要自动处理依赖关系
- 为所有用户安装软件

### 2. 用户目录安装（无需 root）

```bash
./debpkg -u package.deb
```

这会将包安装到您的家目录下的 `~/.local/opt/` 文件夹。

**安装位置：**
- 主程序：`~/.local/opt/<package-name>/`
- 可执行文件链接：`~/.local/bin/`
- 库文件：`~/.local/opt/<package-name>/lib/`

**环境变量配置：**

使用用户目录安装后，需要设置环境变量：

```bash
# 临时设置（当前终端会话）
export PATH=$HOME/.local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH

# 永久设置（添加到 ~/.bashrc 或 ~/.zshrc）
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

**适用场景：**
- 没有 sudo 权限
- 不想影响系统稳定性
- 仅个人使用的软件
- 测试或开发环境

### 3. 交互式安装

```bash
./debpkg package.deb
```

不指定选项时，程序会提示选择安装方式。

## 查看帮助

```bash
./debpkg --help
```

## 查看版本

```bash
./debpkg --version
```

## 卸载方法

### 系统级安装的卸载

使用 apt 或 dpkg 卸载：

```bash
sudo apt remove <package-name>
# 或
sudo dpkg -r <package-name>
```

### 用户目录安装的卸载

直接删除对应目录：

```bash
rm -rf ~/.local/opt/<package-name>
# 清理符号链接
rm -f ~/.local/bin/<executables>
```

## 常见问题

### Q: 用户目录安装后可执行文件找不到？

A: 确保已添加 `~/.local/bin` 到 PATH：

```bash
export PATH=$HOME/.local/bin:$PATH
```

### Q: 运行时提示找不到库文件？

A: 设置 LD_LIBRARY_PATH：

```bash
export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH
```

### Q: 如何查看包会安装到什么位置？

A: 可以先解压 DEB 包查看结构：

```bash
dpkg-deb -c package.deb
```

### Q: 两种安装方式有什么区别？

| 特性 | 系统级 (-s) | 用户级 (-u) |
|------|------------|------------|
| 权限 | 需要 sudo | 无需 root |
| 位置 | /usr, /etc 等 | ~/.local |
| 依赖 | 自动处理 | 手动处理 |
| 范围 | 所有用户 | 单用户 |
| 卸载 | apt/dpkg | 删除目录 |

## 示例

### 安装 VSCode

```bash
# 系统级安装
wget https://go.microsoft.com/fwlink/?LinkID=760668 -o vscode.deb
./debpkg -s vscode.deb

# 或用户目录安装
./debpkg -u vscode.deb
export PATH=$HOME/.local/bin:$PATH
```

### 安装 Google Chrome

```bash
# 下载
wget https://dl.google.com/linux/chrome/deb/pool/main/g/google-chrome-stable/google-chrome-stable_amd64.deb -o chrome.deb

# 系统级安装（推荐，因为需要系统集成）
./debpkg -s chrome.deb
```

## 注意事项

1. **系统级安装**会影响整个系统，请确保来源可信
2. **用户目录安装**不会自动处理依赖，需要手动安装依赖
3. 某些需要系统集成的软件（如浏览器）建议使用系统级安装
4. 开发工具和库可以使用用户目录安装
