# 用户目录安装故障排除指南

## 常见问题诊断

### 问题 1：文件没有正确复制

**症状：**
- 安装过程显示成功，但找不到可执行文件
- `~/.local/bin/` 目录为空
- 程序无法运行

**可能原因：**
1. DEB 包中的文件不在标准位置（如 `/usr/bin`, `/usr/lib`）
2. 文件复制命令失败但没有报错
3. 源目录为空

**诊断方法：**

使用调试脚本查看 DEB 包内容：
```bash
chmod +x debug_install.sh
./debug_install.sh package.deb
```

或手动检查：
```bash
# 查看 DEB 包内容结构
dpkg-deb -c package.deb

# 提取到临时目录检查
mkdir /tmp/test_extract
dpkg-deb -x package.deb /tmp/test_extract
find /tmp/test_extract -type f | head -20
```

**解决方案：**

修改后的代码现在会：
- ✅ 检查每个源目录是否有文件
- ✅ 显示找到的文件数量
- ✅ 使用 `-v` 参数显示详细的复制过程
- ✅ 对空目录给出明确提示

### 问题 2：权限问题

**症状：**
```
cp: cannot create regular file '~/.local/bin/program': Permission denied
```

**原因：**
- `~/.local` 目录不存在或权限不正确

**解决方案：**
```bash
# 创建目录结构
mkdir -p ~/.local/{bin,lib,share,include,etc}

# 设置正确权限
chmod 755 ~/.local
chmod 755 ~/.local/{bin,lib,share,include,etc}
```

### 问题 3：依赖未满足导致安装失败

**症状：**
```
[WARNING] Dependency not found: libfoo
[INFO] Please install the dependency manually
```

**解决方案：**
参考 [THIRD_PARTY_DEPS_QUICKREF.md](THIRD_PARTY_DEPS_QUICKREF.md)

### 问题 4：安装后程序无法运行

**症状：**
```bash
$ myapp
bash: myapp: command not found
```

**原因：**
- `~/.local/bin` 不在 PATH 环境变量中

**解决方案：**
```bash
# 添加到 ~/.bashrc
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

# 验证
echo $PATH
which myapp
```

### 问题 5：共享库找不到

**症状：**
```
error while loading shared libraries: libfoo.so: cannot open shared object file
```

**原因：**
- `~/.local/lib` 不在 LD_LIBRARY_PATH 中

**解决方案：**
```bash
# 添加到 ~/.bashrc
echo 'export LD_LIBRARY_PATH="$HOME/.local/lib:$LD_LIBRARY_PATH"' >> ~/.bashrc
source ~/.bashrc

# 或者更新 ld 配置
echo "$HOME/.local/lib" | sudo tee /etc/ld.so.conf.d/user-local.conf
sudo ldconfig
```

## 改进的安装输出

### 之前的输出

```
[INFO] Installing binaries to ~/.local/bin/[INFO] Installing libraries to ~/.local/lib/
[INFO] Installing shared data to ~/.local/share/app/
```

问题：
- ❌ 没有换行，难以阅读
- ❌ 不知道是否找到文件
- ❌ 不知道复制了哪些文件

### 现在的输出

```
[INFO] Installing binaries to ~/.local/bin/
  Found 3 files to install
  'usr/bin/app' -> '/home/user/.local/bin/app'
  'usr/bin/tool' -> '/home/user/.local/bin/tool'
  'usr/bin/util' -> '/home/user/.local/bin/util'

[INFO] Installing libraries to ~/.local/lib/
  Found 2 files to install
  'usr/lib/libfoo.so' -> '/home/user/.local/lib/libfoo.so'
  'usr/lib/libbar.so' -> '/home/user/.local/lib/libbar.so'

[INFO] Installing shared data to ~/.local/share/app/
  Found 5 files to install
  ...
```

改进：
- ✅ 每个操作都有换行和缩进
- ✅ 显示找到的文件数量
- ✅ 详细显示复制的每个文件
- ✅ 对空目录给出友好提示

## 手动调试步骤

### 步骤 1：检查 DEB 包结构

```bash
# 查看包信息
dpkg-deb -f package.deb

# 查看内容列表
dpkg-deb -c package.deb

# 提取 control 信息
dpkg-deb --control package.deb /tmp/control_test
cat /tmp/control_test/control

# 提取数据
dpkg-deb -x package.deb /tmp/data_test
find /tmp/data_test -type f | head -20
```

### 步骤 2：手动模拟安装过程

```bash
# 1. 创建目标目录
mkdir -p ~/.local/{bin,lib,share/app,include/app,etc/app}

# 2. 提取 DEB 包
TEMP_DIR=$(mktemp -d)
dpkg-deb -x package.deb "$TEMP_DIR"

# 3. 手动复制文件
cp -rfv "$TEMP_DIR/usr/bin/"* ~/.local/bin/
cp -rfv "$TEMP_DIR/usr/lib/"* ~/.local/lib/
cp -rfv "$TEMP_DIR/usr/share/"* ~/.local/share/app/

# 4. 设置权限
chmod +x ~/.local/bin/*

# 5. 清理
rm -rf "$TEMP_DIR"
```

### 步骤 3：验证安装结果

```bash
# 检查二进制文件
ls -lh ~/.local/bin/

# 检查库文件
ls -lh ~/.local/lib/

# 检查清单文件
cat ~/.local/share/<package-name>/INSTALL_MANIFEST

# 尝试运行程序
~/.local/bin/app --help
```

## 常见 DEB 包结构问题

### 情况 1：非标准目录结构

有些包可能使用非标准结构：

```
# 例如：直接放在根目录
./opt/myapp/bin/app
./opt/myapp/lib/libfoo.so

# 或者使用其他前缀
./usr/local/bin/app
./usr/local/lib/libfoo.so
```

**解决方案：**
需要修改 `install_user.c` 中的路径映射逻辑，或者手动解压并调整目录结构。

### 情况 2：符号链接问题

某些包包含符号链接：

```
usr/lib/libfoo.so -> libfoo.so.1
usr/lib/libfoo.so.1 -> libfoo.so.1.2.3
```

**当前处理：**
`cp -rf` 会保留符号链接，但可能导致链接指向不存在的位置。

**改进方案：**
后续可以考虑在复制后修复符号链接。

### 情况 3：架构相关文件

```
usr/lib/x86_64-linux-gnu/libfoo.so
usr/lib/i386-linux-gnu/libfoo.so
```

**当前处理：**
会全部复制到 `~/.local/lib/`，可能导致冲突。

**建议：**
只复制与系统架构匹配的库文件。

## 使用调试工具

### 1. 调试脚本

```bash
./debug_install.sh package.deb
```

功能：
- ✅ 自动分析 DEB 包结构
- ✅ 检查标准目录
- ✅ 显示预期安装位置
- ✅ 检测是否已安装
- ✅ 自动编译并安装
- ✅ 提供环境变量配置指引

### 2. 详细日志

安装时会生成 `/tmp/debpkg_install.log`，包含完整的安装过程输出。

### 3. 手动启用调试模式

修改代码添加更多调试输出：

```c
printf("[DEBUG] Source: %s\n", src_bin);
printf("[DEBUG] Destination: %s\n", dst_bin);
printf("[DEBUG] Files found: %d\n", file_count);
```

## 性能优化建议

### 1. 避免重复复制

安装前检查文件是否已存在且版本相同。

### 2. 并行复制

对于大量小文件，可以考虑并行复制提高速度。

### 3. 增量安装

只复制变化的文件，跳过未变化的文件。

## 相关文档

- [USER_DIR_INDEPENDENT_INSTALL.md](USER_DIR_INDEPENDENT_INSTALL.md) - 用户目录独立安装策略
- [THIRD_PARTY_DEPS_GUIDE.md](THIRD_PARTY_DEPS_GUIDE.md) - 第三方依赖处理指南
- [XDG_DIRECTORIES.md](XDG_DIRECTORIES.md) - XDG 目录规范说明

## 获取帮助

如果问题仍未解决，请提供以下信息寻求帮助：

1. DEB 包名称和版本
2. 完整的错误输出
3. `./debug_install.sh package.deb` 的输出
4. `/tmp/debpkg_install.log` 的内容
5. 系统信息（`uname -a`, `lsb_release -a`）
