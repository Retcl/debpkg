# 智能卸载功能 - 实现总结

## 🎯 功能概述

成功实现了用户目录下 DEB 包的智能卸载功能，具备依赖关系检查和保护机制。在卸载包时会自动分析依赖关系，只有当依赖不被其他包使用时才会删除，确保系统的稳定性和依赖完整性。

## 📦 实现内容

### 一、新增模块

#### `uninstall.h` / `uninstall.c` (15 KB)

**核心职责**: 智能卸载和依赖保护

**数据结构**:

```c
// 卸载选项
typedef struct {
    int force;              // 强制卸载（忽略依赖检查）
    int dry_run;           // 仅模拟，不实际删除
    int verbose;           // 详细输出
} UninstallOptions;

// 卸载结果统计
typedef struct {
    int files_removed;      // 删除的文件数
    int dirs_removed;       // 删除的目录数
    int deps_kept;          // 保留的依赖数
    int deps_removed;       // 删除的依赖数
} UninstallResult;
```

**主要函数**:

1. **`uninstall_package_smart()`** - 主卸载函数 (~80 行)
   - 检查包是否存在
   - 显示卸载信息
   - 用户确认
   - 协调各个清理步骤

2. **`check_dependency_usage()`** - 依赖使用检查 (~60 行)
   - 扫描 user_deps.db
   - 排除当前卸载的包
   - 查找依赖引用
   - 返回使用者数量

3. **`get_package_manifest()`** - 获取安装清单 (~70 行)
   - 读取 INSTALL_MANIFEST
   - 解析文件格式（Type: path）
   - 动态构建文件列表
   - 处理路径展开

4. **`remove_package_files()`** - 执行文件删除 (~90 行)
   - 遍历文件列表
   - 区分文件和目录
   - 执行删除操作
   - 统计删除数量

5. **`cleanup_unused_dependencies()`** - 清理未用依赖 (~40 行)
   - 加载依赖记录
   - 逐个检查依赖
   - 调用 usage 检查
   - 删除或保留决策

6. **`show_uninstall_info()`** - 显示卸载信息 (~20 行)
   - 包名称
   - 影响范围
   - 注意事项

### 二、集成修改

#### `main.c`
**修改内容**:
1. 添加 MODE_UNINSTALL 枚举值
2. 解析 `-r/--remove/--uninstall` 参数
3. 支持包名作为参数（而非 .deb 文件）
4. 调用 uninstall 函数

```c
// 新增卸载模式支持
else if (strcmp(argv[i], "-r") == 0 || ...) {
    mode = MODE_UNINSTALL;
} else if (argv[i][0] != '-') {
    if (mode == MODE_UNINSTALL) {
        pkg_name = argv[i];  // 包名
    } else {
        deb_file = argv[i];  // DEB 文件
    }
}

// 根据模式执行
if (mode == MODE_UNINSTALL) {
    uninstall_package_smart(pkg_name, home_dir, &options, &result);
}
```

#### `cli.c`
**修改内容**:
- 更新帮助信息
- 添加卸载命令示例
- 更新用法说明

```c
void show_help(const char *prog_name) {
    printf("Usage: %s [OPTIONS] <package.deb|package-name>\n", prog_name);
    ...
    printf("  -r, --remove, --uninstall\n");
    printf("                    Uninstall a package from user directory\n");
    ...
    printf("  %s -r package-name             # Uninstall\n", prog_name);
}
```

## 🔧 技术细节

### 依赖保护算法

```c
int check_dependency_usage(dep_name, exclude_pkg, home_dir) {
    打开 user_deps.db
    
    usage_count = 0
    逐行读取数据库 {
        跳过注释和空行
        
        解析：PKG_NAME|PKG_VERSION|DEPS
        
        if (PKG_NAME == exclude_pkg) continue
        
        解析依赖列表 {
            提取依赖名（去掉版本约束）
            
            if (依赖名 == dep_name) {
                usage_count++
                break
            }
        }
    }
    
    return usage_count
}
```

**时间复杂度**: O(n × m)
- n = 已安装包数量
- m = 平均每个包的依赖数量

**优化策略**:
- 找到第一个匹配就返回（短路）
- 逐行读取，不一次性加载
- 只解析必要字段

### 卸载流程

```
开始卸载
    ↓
检查包是否存在
    ├─→ 否 → 报错退出
    └─→ 是 → 继续
    ↓
显示卸载信息
    ↓
用户确认
    ├─→ 取消 → 退出
    └─→ 确认 → 继续
    ↓
读取 INSTALL_MANIFEST
    ↓
删除文件（按清单）
    ↓
删除目录
    ↓
删除桌面文件
    ↓
加载依赖记录
    ↓
遍历每个依赖
    ├─→ 检查使用情况
    │   ├─→ 被使用 → 保留
    │   └─→ 未使用 → 删除
    └─→ 下一个依赖
    ↓
从数据库移除记录
    ↓
显示统计信息
    ↓
完成
```

### 文件删除策略

**基于清单删除**:
```c
// 从 INSTALL_MANIFEST 读取
Binary: ~/.local/bin/myapp
Library: ~/.local/lib/libmyapp.so
Header: ~/.local/include/myapp/myapp.h
Data: ~/.local/share/myapp/data.dat

// 逐个删除
for each file in manifest {
    expand_path(file)
    if (is_directory) {
        rm -rf
    } else {
        unlink
    }
}
```

**额外清理**:
```c
// 包数据目录
rm -rf ~/.local/share/<pkg>/

// 配置目录
rm -rf ~/.local/etc/<pkg>/

// 桌面文件
unlink ~/.local/share/applications/<pkg>.desktop
```

## 📊 代码统计

| 类型 | 数量 | 说明 |
|------|------|------|
| 新增头文件 | 1 | uninstall.h (2.3 KB) |
| 新增源文件 | 1 | uninstall.c (13 KB) |
| 修改源文件 | 2 | main.c, cli.c |
| 新增文档 | 1 | UNINSTALL_FEATURE.md |
| 测试脚本 | 1 | test_uninstall.sh |
| 总新增代码 | ~380 行 | 不含注释 |

## ✅ 功能验证

### 编译测试
```bash
make clean && make
# 结果：✅ 编译成功，无错误
```

### 命令行测试
```bash
./debpkg --help
# 结果：✅ 显示正确的帮助信息
```

### 功能测试
```bash
./test_uninstall.sh
# 结果：✅ 所有检查通过
```

## 🎯 实现的优势

### 1. **智能保护** 🛡️
- 自动检测依赖使用情况
- 保护正在使用的依赖
- 避免破坏其他包

### 2. **安全可靠** ✅
- 用户确认后才执行
- 基于清单精确删除
- 不删除无关文件

### 3. **完整清理** 🧹
- 删除所有相关文件
- 清理桌面入口
- 移除依赖记录

### 4. **详细反馈** 📊
- 显示删除统计
- 报告依赖状态
- 清晰的提示信息

### 5. **模块化设计** 🧩
- 独立的 uninstall 模块
- 清晰的接口定义
- 易于维护和扩展

## ⚠️ 注意事项

### 推荐做法

1. **按顺序卸载**
   - 先卸载上层应用
   - 最后卸载基础库
   - 避免依赖冲突

2. **检查依赖关系**
   - 注意"used by X packages"提示
   - 如果要删除共享依赖，先卸载使用者

3. **备份配置**
   - 配置文件不会被完全删除
   - 重要数据手动备份

### 避免的问题

1. **不要强制卸载**
   - 除非确定没有依赖
   - 否则可能导致问题

2. **不要手动删除**
   - 使用卸载命令
   - 确保依赖正确处理

3. **不要忽略警告**
   - 注意依赖使用提示
   - 评估影响范围

## 🔮 未来改进

### 短期计划
- [ ] 添加 `--dry-run` 选项
- [ ] 添加 `--force` 选项
- [ ] 添加 `--verbose` 选项

### 中期计划
- [ ] 递归卸载依赖
- [ ] 依赖树可视化
- [ ] 卸载前影响分析报告

### 长期愿景
- [ ] 支持系统级卸载
- [ ] 依赖版本回滚
- [ ] 卸载历史记录

## 📚 相关资源

### 相关文档
- [UNINSTALL_FEATURE.md](UNINSTALL_FEATURE.md) - 详细功能说明
- [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md) - 依赖管理功能
- [MODULES.md](MODULES.md) - 模块架构文档

### 外部资源
- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/)
- [Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/)

## 🎉 总结

智能卸载功能的实现为 debpkg 补全了生命周期管理的最后一环：

✨ **智能保护** - 自动保护正在使用的依赖  
🛡️ **安全可靠** - 用户确认 + 清单删除  
🧹 **完整清理** - 删除所有相关文件  
📊 **详细反馈** - 清晰的统计信息  
🧩 **模块化** - 独立的 uninstall 模块  

这使得 debpkg 成为一个功能完整的包管理工具：
- ✅ **安装** - 用户目录安装、依赖管理、环境变量配置、桌面集成
- ✅ **卸载** - 智能依赖检查、安全删除、完整清理

项目现在具备了完整的包管理能力！🎊

---

*最后更新：2024 | debpkg v1.0.0*
