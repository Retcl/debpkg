# 依赖管理功能实现总结

## ✅ 功能完成

已成功为 debpkg 添加依赖检查和管理功能，在用户目录安装模式下自动检测并提示缺失的依赖。

## 📦 新增模块

### depends.c/h - 依赖管理模块

**位置**: 
- `include/depends.h` (头文件)
- `src/depends.c` (实现文件)

**主要函数**:

| 函数 | 功能 | 返回值 |
|------|------|--------|
| `check_system_dependency()` | 检查系统级依赖是否安装 | 1=已安装，0=未安装 |
| `check_user_dependency()` | 检查用户目录依赖是否安装 | 1=已安装，0=未安装 |
| `extract_dependencies()` | 从 DEB 包提取依赖列表 | 0=成功，-1=失败 |
| `parse_dependencies()` | 解析 control 文件获取依赖 | 0=成功，-1=失败 |
| `install_dependency_to_user()` | 安装单个依赖到用户目录 | 0=成功，-1=失败 |
| `free_dependencies()` | 释放依赖列表内存 | void |

**数据结构**:
```c
typedef struct {
    char name[256];      // 依赖包名
    char version[64];    // 版本号（如 ">= 1.0.0"）
    int installed;       // 是否已安装
} Dependency;
```

## 🔧 集成到 install_user.c

### 修改内容

在 [install_user.c](file:///home/retcl/Projects/c/debpkg/src/install_user.c) 中添加了依赖检查流程：

1. **提取依赖列表** (第 68-73 行)
   ```c
   Dependency *deps = NULL;
   int dep_count = 0;
   parse_dependencies(control_file, &deps, &dep_count);
   ```

2. **检查依赖状态** (第 75-92 行)
   - 检查系统级安装
   - 检查用户目录安装
   - 标记缺失依赖

3. **交互式处理** (第 93-128 行)
   - 显示缺失依赖列表
   - 提供三个处理选项
   - 根据用户选择执行相应操作

### 工作流程

```
开始安装
  ↓
提取 DEB 包信息
  ↓
解析依赖列表
  ↓
检查每个依赖:
  ├─ 系统级已安装？→ ✓ (system)
  ├─ 用户目录已安装？→ ✓ (user)
  └─ 都未安装？→ ✗ (missing)
  ↓
统计缺失依赖数量
  ↓
有缺失？
  ├─ 是 → 显示交互菜单
  │      ├─ 选项 1: 手动安装依赖
  │      ├─ 选项 2: 继续安装（警告）
  │      └─ 选项 3: 取消安装
  └─ 否 → 继续安装流程
  ↓
完成安装
```

## 📊 文件变更统计

### 新增文件
- `include/depends.h` - 95 行
- `src/depends.c` - 226 行

### 修改文件
- `src/install_user.c` - 增加约 60 行依赖检查代码
- `README.md` - 更新特性说明

### 新增文档
- `DEPENDENCY_MANAGEMENT.md` - 详细的使用文档
- `DEPENDENCY_FEATURE_SUMMARY.md` - 本文档

## 🎯 功能特性

### 1. 智能依赖检查

**双重检查机制**:
```
系统级检查 (dpkg -l)
  ↓
用户目录检查 (~/.local/share/<pkg>/INSTALL_MANIFEST)
  ↓
综合判断依赖状态
```

**检查顺序**:
1. 优先检查系统级安装（更高效）
2. 其次检查用户目录安装
3. 两者都不是则标记为缺失

### 2. 清晰的依赖状态显示

```
Found 5 dependencies:
  ✓ libc6                          (system)
  ✓ libssl1.1                      (system)
  ✗ libspeciallib >= 1.0.0         (missing)
  ✗ libnewfeature                  (missing)
  ✓ libcommon                      (user)
```

**符号说明**:
- ✓ - 已安装
- ✗ - 缺失
- (system) - 系统级安装
- (user) - 用户目录安装
- (missing) - 未安装

### 3. 交互式依赖处理

当检测到缺失依赖时，提供三个选项：

**选项 1: 手动安装依赖**
```
Enter your choice [1-3]: 1
[INFO] Please install the missing dependencies and try again.
```
- 暂停当前安装
- 用户手动安装依赖
- 重新运行安装命令

**选项 2: 继续安装**
```
Enter your choice [1-3]: 2
[WARNING] Continuing with missing dependencies...
```
- 忽略缺失依赖
- 继续安装主程序
- 可能无法正常工作

**选项 3: 取消安装**
```
Enter your choice [1-3]: 3
[INFO] Installation cancelled.
```
- 终止安装过程
- 清理临时文件
- 返回错误码

## 💡 使用场景

### 场景 1: 所有依赖已满足

```bash
$ ./debpkg -u myapp.deb
[INFO] Checking dependencies...
[INFO] Found 3 dependencies:
  ✓ libc6                          (system)
  ✓ libssl1.1                      (system)
  ✓ libcurl4                       (user)
[SUCCESS] All dependencies satisfied!
[INFO] Installing files...
```
**结果**: 直接安装，无需额外操作

### 场景 2: 有缺失依赖

```bash
$ ./debpkg -u myapp.deb
[INFO] Found 2 dependencies:
  ✓ libc6                          (system)
  ✗ libspeciallib                  (missing)

Dependency handling options:
  1. Install missing dependencies manually
  2. Continue installation anyway
  3. Cancel installation

Enter your choice [1-3]: 1
[INFO] Please install the missing dependencies and try again.
```

**后续步骤**:
```bash
# 安装依赖
sudo apt-get install libspeciallib

# 或用户目录安装
./debpkg -u libspeciallib.deb

# 重新安装主程序
./debpkg -u myapp.deb
```

### 场景 3: 强制安装（测试用）

```bash
$ ./debpkg -u myapp.deb
...
Enter your choice [1-3]: 2
[WARNING] Continuing with missing dependencies...
[INFO] Installing files...
[SUCCESS] Package installed successfully!
[WARNING] Note: Some dependencies are missing, the application may not work properly.
```

## ⚠️ 当前限制

### 1. 不自动下载安装
当前版本**不会自动下载**依赖包，需要用户手动安装。

**解决方案**:
- 系统依赖：使用 `apt-get install`
- 用户依赖：使用 `./debpkg -u <dep>.deb`

### 2. 版本检查不严格
只检查包名，不严格验证版本号。

**影响**:
- 如果程序需要特定版本的库，可能出错
- 需要用户自行确认版本兼容性

### 3. 虚拟包识别有限
某些依赖是虚拟包（由多个实际包提供）。

**示例**:
- `libc-dev` 可能由 `glibc-dev` 提供
- 需要用户判断是否已安装

### 4. 仅检查 Depends 字段
不检查 Recommends、Suggests 等可选依赖。

## 🔮 未来改进计划

### 短期（v1.1）
- [ ] 自动下载依赖（从 APT 仓库）
- [ ] 严格的版本检查
- [ ] 虚拟包映射表

### 中期（v1.2）
- [ ] 依赖树可视化
- [ ] 批量安装优化
- [ ] 依赖缓存机制

### 长期（v2.0）
- [ ] 自动解决循环依赖
- [ ] 支持更多包格式
- [ ] 图形界面依赖管理

## 📝 最佳实践

### 1. 优先使用系统依赖

对于常用库，建议先用 apt 安装：
```bash
sudo apt-get install libssl libcurl zlib1g
./debpkg -u myapp.deb
```

**优势**:
- ✅ 自动处理版本冲突
- ✅ 所有程序共享
- ✅ 系统统一管理

### 2. 用户目录安装顺序

按依赖层级从底向上安装：
```bash
# 基础库
./debpkg -u libbase.deb

# 中间层
./debpkg -u libmiddle.deb

# 应用程序
./debpkg -u myapp.deb
```

### 3. 记录依赖信息

创建简单的依赖清单：
```markdown
# myapp 依赖

## 已安装（系统）
- libc6
- libssl1.1

## 需安装（用户）
- libspecial.deb
- libfeature.deb
```

### 4. 安装后测试

立即测试程序是否正常运行：
```bash
~/.local/bin/myapp --version
ldd ~/.local/lib/*.so | grep "not found"
```

## 🎓 技术要点

### 依赖解析算法

```c
// 伪代码
int parse_dependencies(const char *control_path, ...) {
    fopen(control_path);
    
    while (fgets(line)) {
        if (strncmp(line, "Depends:", 8) == 0) {
            // 解析依赖列表
            parse_depends_line(line);
            
            // 处理多行情况
            while (next_line starts with space) {
                parse_continuation_line();
            }
        }
    }
}
```

### 依赖检查实现

```c
int check_system_dependency(const char *pkg_name) {
    // 执行：dpkg -l "pkg_name" | grep -q "^ii"
    snprintf(cmd, "dpkg -l \"%s\" 2>/dev/null | grep -q \"^ii\"", pkg_name);
    return (system(cmd) == 0);
}

int check_user_dependency(const char *pkg_name, const char *home_dir) {
    // 检查 INSTALL_MANIFEST 是否存在
    snprintf(path, "%s/.local/share/%s/INSTALL_MANIFEST", home_dir, pkg_name);
    return file_exists(path);
}
```

### 交互式菜单

```c
if (missing_deps > 0) {
    printf("Dependency handling options:\n");
    printf("  1. Install missing dependencies manually\n");
    printf("  2. Continue installation anyway\n");
    printf("  3. Cancel installation\n");
    
    scanf(" %c", &choice);
    
    switch (choice) {
        case '1': handle_manual_install(); break;
        case '2': handle_continue_anyway(); break;
        case '3': handle_cancel(); break;
    }
}
```

## 📚 相关文档

- [DEPENDENCY_MANAGEMENT.md](DEPENDENCY_MANAGEMENT.md) - 详细使用指南
- [README.md](README.md) - 项目使用说明
- [USER_DIRECTORY_INSTALL.md](USER_DIRECTORY_INSTALL.md) - 用户目录安装指南
- [XDG_DIRECTORIES.md](XDG_DIRECTORIES.md) - XDG 目录规范

## 🎉 总结

通过添加依赖管理功能，debpkg 现在可以：

✅ **自动检测依赖** - 从 DEB 包解析依赖信息  
✅ **智能状态检查** - 同时检查系统和用户目录  
✅ **清晰状态报告** - 显示每个依赖的安装状态  
✅ **交互式处理** - 提供多种依赖处理选项  
✅ **符合用户习惯** - 与主流包管理器行为一致  

这使得用户目录安装更加可靠和专业！🚀

---

*版本：1.0 | 完成时间：2024 年 3 月*
