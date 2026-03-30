# 依赖管理功能说明

## 📋 概述

debpkg 现已集成依赖检查功能，可以在安装 DEB 包时自动检测并提示缺失的依赖。

## ✨ 功能特性

### 1. 依赖检查
- ✅ **自动检测**: 从 DEB 包的 control 文件中解析依赖信息
- ✅ **双重检查**: 同时检查系统级和用户目录的安装状态
- ✅ **详细报告**: 显示每个依赖的安装状态（系统/用户/缺失）

### 2. 依赖状态识别
```
✓ package-name (system)  - 系统级已安装
✓ package-name (user)    - 用户目录已安装
✗ package-name (missing) - 依赖缺失
```

### 3. 交互式处理
当检测到缺失的依赖时，提供三个选项：
1. **手动安装依赖** - 暂停当前安装，先安装依赖
2. **继续安装** - 忽略缺失的依赖（可能无法正常工作）
3. **取消安装** - 终止安装过程

## 🔧 工作原理

### 依赖解析流程

```
1. 提取 DEB 包的 control 文件
   ↓
2. 解析 Depends 字段获取依赖列表
   ↓
3. 对每个依赖进行检查:
   a. 检查系统级是否安装 (dpkg -l)
   b. 检查用户目录是否安装 (~/.local/share/<pkg>/INSTALL_MANIFEST)
   c. 标记未安装的依赖
   ↓
4. 生成依赖状态报告
   ↓
5. 如有缺失依赖，提供交互选项
```

### 依赖存储位置

#### 系统级依赖
使用 `dpkg -l` 命令检查：
```bash
dpkg -l <package-name> | grep "^ii"
```

#### 用户目录依赖
检查 INSTALL_MANIFEST 文件：
```
~/.local/share/<package-name>/INSTALL_MANIFEST
```

## 📊 使用示例

### 示例 1: 无缺失依赖

```bash
./debpkg -u myapp.deb
```

**输出**:
```
[INFO] Installing package to user directory...
[INFO] Package name: myapp
[INFO] Checking dependencies...
[INFO] Found 3 dependencies:
  ✓ libc6                          (system)
  ✓ libssl1.1                      (system)
  ✓ libcurl4                       (user)
[SUCCESS] All dependencies satisfied!
[INFO] Installing files to standard directories...
...
[SUCCESS] Package installed successfully!
```

### 示例 2: 有缺失依赖

```bash
./debpkg -u myapp.deb
```

**输出**:
```
[INFO] Installing package to user directory...
[INFO] Package name: myapp
[INFO] Checking dependencies...
[INFO] Found 5 dependencies:
  ✓ libc6                          (system)
  ✓ libssl1.1                      (system)
  ✗ libspeciallib                  (missing)
  ✗ libnewfeature                  (missing)
  ✓ libcommon                      (user)
[WARNING] Missing 2 dependencies:
  - libspeciallib >= 1.0.0
  - libnewfeature

Dependency handling options:
  1. Install missing dependencies manually
     System: sudo apt-get install <package-name>
     User:   ./debpkg -u <package-name>.deb
  2. Continue installation anyway (may not work properly)
  3. Cancel installation

Enter your choice [1-3]: _
```

### 选项说明

**选择 1** - 手动安装依赖:
```
Enter your choice [1-3]: 1
[INFO] Please install the missing dependencies and try again.
```
程序退出，等待用户安装依赖后重新运行。

**选择 2** - 继续安装:
```
Enter your choice [1-3]: 2
[WARNING] Continuing with missing dependencies...
[INFO] Installing files to standard directories...
...
[SUCCESS] Package installed successfully!
[WARNING] Note: Some dependencies are missing, the application may not work properly.
```

**选择 3** - 取消安装:
```
Enter your choice [1-3]: 3
[INFO] Installation cancelled.
```

## 🎯 依赖处理策略

### 推荐方案

#### 方案 A: 系统级依赖（推荐）
适用于常用库和系统组件：
```bash
sudo apt-get install libspeciallib libnewfeature
./debpkg -u myapp.deb
```

**优势**:
- ✅ 自动处理依赖关系
- ✅ 所有程序共享库文件
- ✅ 系统统一管理

#### 方案 B: 用户级依赖
适用于个人使用或无 root 权限：
```bash
# 先安装依赖
./debpkg -u libspeciallib.deb
./debpkg -u libnewfeature.deb

# 再安装主程序
./debpkg -u myapp.deb
```

**优势**:
- ✅ 无需 root 权限
- ✅ 隔离在用户目录
- ✅ 不影响系统

### 依赖链处理

对于有依赖链的情况（A 依赖 B，B 依赖 C），建议：

1. **从底向上安装**:
   ```bash
   # 1. 先安装最底层依赖
   ./debpkg -u libbase.deb
   
   # 2. 安装中间层依赖
   ./debpkg -u libmiddle.deb
   
   # 3. 最后安装应用程序
   ./debpkg -u myapp.deb
   ```

2. **使用系统包管理器**:
   ```bash
   sudo apt-get install libbase libmiddle
   ./debpkg -u myapp.deb
   ```

## 📚 技术细节

### 依赖解析算法

```c
// 伪代码示例
for each dependency in DEB:
    if system_installed(dependency):
        mark_as_installed(SYSTEM)
    else if user_installed(dependency):
        mark_as_installed(USER)
    else:
        mark_as_missing()

if has_missing():
    prompt_user_for_action()
```

### 检查函数

#### check_system_dependency()
```c
int check_system_dependency(const char *pkg_name) {
    // 执行：dpkg -l "pkg_name" | grep -q "^ii"
    return (system(cmd) == 0);
}
```

#### check_user_dependency()
```c
int check_user_dependency(const char *pkg_name, const char *home_dir) {
    // 检查：~/.local/share/<pkg>/INSTALL_MANIFEST
    return file_exists(check_path);
}
```

### 依赖数据结构

```c
typedef struct {
    char name[256];      // 包名
    char version[64];    // 版本要求（如 ">= 1.0.0"）
    int installed;       // 是否已安装
} Dependency;
```

## ⚠️ 注意事项

### 1. 版本兼容性
当前实现**不严格检查版本**，只检查包名。如果程序需要特定版本：
- 系统级：由 apt/dpkg 处理版本冲突
- 用户级：需要手动确认版本兼容

### 2. 虚拟包
某些依赖可能是虚拟包（virtual package），实际由其他包提供。例如：
- `libc-dev` 可能由 `glibc-dev` 提供
- 这种情况需要手动判断

### 3. 可选依赖
DEB 包可能包含可选依赖（Recommends/Suggests），当前只检查必需依赖（Depends）。

### 4. 循环依赖
如果 A 依赖 B，B 又依赖 A：
- 先安装任意一个（选择"继续安装"）
- 再安装另一个（依赖会自动满足）

## 🔮 未来改进

### 计划功能

1. **自动下载并安装依赖**
   ```bash
   ./debpkg -u --auto-deps myapp.deb
   ```
   自动从仓库下载并安装缺失的依赖到用户目录。

2. **依赖树显示**
   ```bash
   ./debpkg --show-deps myapp.deb
   ```
   显示完整的依赖关系树。

3. **版本检查**
   严格检查依赖版本是否满足要求。

4. **依赖缓存**
   缓存已检查的依赖信息，加速重复安装。

5. **批量安装**
   一次性安装多个 DEB 包及其所有依赖。

## 📝 最佳实践

### 1. 优先使用系统依赖
```bash
# 对于常用库，先用 apt 安装
sudo apt-get install libssl libcurl

# 再用 debpkg 安装应用
./debpkg -u myapp.deb
```

### 2. 用户目录安装顺序
```bash
# 基础库 → 高级库 → 应用程序
./debpkg -u libbase.deb
./debpkg -u libadvanced.deb
./debpkg -u myapp.deb
```

### 3. 记录依赖信息
维护一个简单的文档：
```markdown
# myapp 依赖

## 系统级
- libc6
- libssl1.1

## 用户级（如无 root）
- libspecial.deb
- libfeature.deb
```

### 4. 测试依赖
安装完成后立即测试程序：
```bash
~/.local/bin/myapp --version
```

## 🎓 相关模块

依赖管理涉及以下模块：

- **depends.c/h** - 依赖管理核心模块
- **install_user.c** - 集成依赖检查的用户安装
- **extract.c** - 提取 DEB 包控制信息

## 📖 参考资料

- [Debian Control File Format](https://www.debian.org/doc/debian-policy/ch-controlfields.html)
- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/)
- [dpkg documentation](https://manpages.debian.org/dpkg)

---

*版本：1.0 | 最后更新：2024 年 3 月*
