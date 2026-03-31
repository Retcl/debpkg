# 用户目录依赖管理 - 快速参考

## 📦 核心功能

### 1. 依赖关系记录
- ✅ 自动记录安装到用户目录的 DEB 包依赖
- ✅ 保存包名、版本号和完整依赖列表
- ✅ 存储在 `~/.local/share/debpkg/user_deps.db`

### 2. 版本敏感检测
- ✅ **同版本不下载** - 已存在相同版本时跳过
- ✅ **不同版本才下载** - 版本不匹配时触发下载
- ✅ **支持复杂约束** - `>=`, `<=`, `=` 等

### 3. 智能状态显示
```
✓ libfoo                         (system)           # 系统级已安装
✓ libbar                         (user, v2.1.0)     # 用户目录版本匹配
⚠ libbaz                         (user, v1.0.0 installed, v1.2.0 required)  # 版本不符
✗ libqux                         (missing)          # 缺失
```

## 🔧 数据库格式

**位置**: `~/.local/share/debpkg/user_deps.db`

**格式**:
```bash
PKG_NAME|PKG_VERSION|DEP1:VER1,DEP2:VER2,DEP3:*
```

**示例**:
```bash
myapp|1.2.3|libfoo:>=1.0.0,libbar:=2.1.0,libbaz:*
anotherapp|3.0.0|libqux:>=3.0.0
```

## 💻 使用示例

### 安装包（自动记录依赖）
```bash
./debpkg -u package.deb
```

### 查看依赖数据库
```bash
cat ~/.local/share/debpkg/user_deps.db
```

### 列出所有已安装包
```bash
grep -v "^#" ~/.local/share/debpkg/user_deps.db | cut -d'|' -f1 | sort -u
```

### 查询特定包的依赖
```bash
grep "^pkgname|" ~/.local/share/debpkg/user_deps.db
```

## 📊 API 参考

### C 语言 API

```c
#include "user_deps.h"

// 初始化数据库
init_user_deps_db(home_dir);

// 保存依赖记录
save_package_dependencies(pkg_name, pkg_version, deps, count, home_dir);

// 检查版本
check_user_dependency_version(dep_name, version, home_dir);

// 获取版本号
get_installed_dep_version(dep_name, version_buffer, buffer_size, home_dir);

// 加载记录
load_package_dependencies(pkg_name, &record, home_dir);

// 列出所有包
list_user_installed_packages(&packages, &count, home_dir);

// 删除记录
remove_package_from_deps_db(pkg_name, home_dir);
```

### Shell 命令

```bash
# 统计已安装包数量
grep -v "^#" ~/.local/share/debpkg/user_deps.db | grep -v "^$" | wc -l

# 查找包含特定依赖的包
grep "libfoo" ~/.local/share/debpkg/user_deps.db

# 导出包列表
grep -v "^#" ~/.local/share/debpkg/user_deps.db | cut -d'|' -f1 > packages.txt
```

## 🎯 版本策略

### 严格匹配
```
libfoo:=1.2.3    # 必须完全匹配 v1.2.3
```

### 最小版本
```
libbar:>=1.0.0   # v1.0.0 或更高
```

### 任意版本
```
libbaz:*         # 任何版本都可以
```

## ⚠️ 注意事项

### ✅ 推荐做法
- 定期备份数据库
- 使用提供的 API 操作数据
- 保持数据库格式正确

### ❌ 避免做法
- 手动编辑数据库文件
- 破坏字段分隔符 `|`
- 删除必要的字段

## 🔍 故障排查

### 数据库不存在
```bash
# 首次安装时会自动创建
# 如需手动初始化，运行一次安装即可
```

### 格式错误
```bash
# 检查格式
cat ~/.local/share/debpkg/user_deps.db

# 修复：删除损坏的行，保持格式正确
```

### 版本检测失败
```bash
# 检查依赖是否真的安装
ls -la ~/.local/share/<dep-name>/INSTALL_MANIFEST

# 验证版本号
grep "^<dep-name>|" ~/.local/share/debpkg/user_deps.db
```

## 📈 性能优化

### 查询优化
```bash
# 使用 grep 快速搜索
grep "^pkgname|" ~/.local/share/debpkg/user_deps.db

# 使用 cut 提取特定字段
grep "^pkgname|" ~/.local/share/debpkg/user_deps.db | cut -d'|' -f2
```

### 空间优化
```bash
# 定期清理不再使用的记录
# （需要手动实现或使用未来版本的卸载功能）
```

## 🎨 输出示例

### 完整安装流程
```
$ ./debpkg -u myapp_1.2.3_amd64.deb

[INFO] Checking dependencies...
[INFO] Found 3 dependencies:
  ✓ libfoo                         (system)
  ✓ libbar                         (user, v2.1.0)
  ⚠ libbaz                         (user, v1.0.0 installed, v1.2.0 required)
  ✗ libqux                         (missing)

[WARNING] 1 version mismatch(es):
  - libbaz>=1.2.0
[WARNING] 1 missing dependencies:
  - libqux>=3.0.0

[INFO] Dependency handling options:
  1. Auto-download and install dependencies (RECOMMENDED)
  2. Install missing dependencies manually
  3. Continue installation anyway
  4. Cancel installation

Enter your choice [1-4]: 1

[INFO] Starting automatic dependency installation...
[SUCCESS] All dependencies installed successfully!

[INFO] Saving dependency record...
[SUCCESS] Dependency record saved successfully!

[SUCCESS] Package installed successfully!
  Executables:   ~/.local/bin/
  Libraries:     ~/.local/lib/
  Shared data:   ~/.local/share/myapp/
  Desktop file:  ~/.local/share/applications/myapp.desktop
  Dependencies:  Recorded in database
```

### 数据库内容
```
$ cat ~/.local/share/debpkg/user_deps.db

# User Dependencies Database
# Format: PKG_NAME|PKG_VERSION|DEP_NAME1:DEP_VER1,DEP_NAME2:DEP_VER2
# Created by debpkg

myapp|1.2.3|libfoo:*,libbar:>=2.0.0,libbaz:>=1.2.0,libqux:>=3.0.0
```

## 📚 相关资源

- **详细文档**: [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md)
- **实现总结**: [USER_DEPS_SUMMARY.md](USER_DEPS_SUMMARY.md)
- **模块说明**: [MODULES.md](MODULES.md)
- **项目主页**: [README.md](README.md)

---

*最后更新：2024 | debpkg v1.0.0*
