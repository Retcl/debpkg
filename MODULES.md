# debpkg 模块化架构说明

## 项目结构

**新的目录组织**：

```
debpkg/
├── include/              # 头文件目录（公共接口）
│   ├── cli.h
│   ├── desktop.h        # Desktop 文件创建模块 ✨
│   ├── depends.h        # 依赖管理模块
│   ├── env_config.h     # 环境变量配置模块 ✨
│   ├── extract.h        # DEB 包提取模块
│   ├── install_system.h # 系统级安装模块
│   ├── install_user.h   # 用户目录安装模块
│   ├── uninstall.h      # 卸载模块 ✨ NEW!
│   ├── user_deps.h      # 用户依赖记录模块 ✨
│   └── utils.h
│
├── src/                  # 源代码目录（具体实现）
│   ├── main.c
│   ├── cli.c
│   ├── desktop.c        # Desktop 文件创建实现 ✨
│   ├── depends.c        # 依赖管理实现
│   ├── env_config.c     # 环境变量配置实现 ✨
│   ├── extract.c        # DEB 包提取实现
│   ├── install_system.c # 系统级安装实现
│   ├── install_user.c   # 用户目录安装实现
│   ├── uninstall.c      # 卸载实现 ✨ NEW!
│   ├── user_deps.c      # 用户依赖记录实现 ✨
│   └── utils.c
│
├── obj/                  # 编译产物（自动生成）
│   ├── cli.o
│   ├── extract.o
│   ├── install_system.o
│   ├── install_user.o
│   ├── main.o
│   └── utils.o
│
├── Makefile              # 编译配置
└── README.md             # 项目说明
```

**旧的结构**（已重构）:
```
debpkg/
├── main.c
├── cli.c/h
├── utils.c/h
├── extract.c/h
├── install_system.c/h
├── install_user.c/h
├── Makefile
└── README.md
```

现在所有头文件都在 `include/` 目录，源文件都在 `src/` 目录。

## 模块划分

### 1. utils 模块 (utils.c/h)

**功能**: 提供通用的底层工具函数

**主要函数**:
- `print_info()` - 打印信息消息（蓝色）
- `print_success()` - 打印成功消息（绿色）
- `print_warning()` - 打印警告消息（黄色）
- `print_error()` - 打印错误消息（红色）
- `get_home_dir()` - 获取用户主目录路径
- `file_exists()` - 检查文件是否存在
- `is_directory()` - 检查是否为目录
- `create_directories()` - 递归创建目录
- `execute_command()` - 执行 shell 命令

**依赖**: 无（基础模块）

**使用示例**:
```c
#include "utils.h"

if (file_exists("/path/to/file")) {
    print_info("File found");
}
```

---

### 2. extract 模块 (extract.c/h)

**功能**: 处理 DEB 包的提取和解析

**主要函数**:
- `extract_control()` - 提取 DEB 包的控制信息
- `extract_data()` - 提取 DEB 包的数据文件
- `parse_package_name()` - 从 control 文件解析包名

**依赖**: 
- utils 模块
- 系统命令：`dpkg-deb`

**使用示例**:
```c
#include "extract.h"

char pkg_name[256];
extract_control("package.deb", "/tmp/control");
parse_package_name("/tmp/control/control", pkg_name, sizeof(pkg_name));
extract_data("package.deb", "/tmp/install");
```

---

### 3. install_system 模块 (install_system.c/h)

**功能**: 实现系统级安装（使用 dpkg）

**主要函数**:
- `install_system()` - 使用 dpkg 进行系统级安装

**依赖**: 
- utils 模块
- 系统命令：`dpkg`, `sudo`

**特点**:
- 需要 root 权限
- 自动处理依赖关系
- 安装到系统目录（/usr, /etc 等）

**使用示例**:
```c
#include "install_system.h"

install_system("package.deb");  // 系统级安装
```

---

### 4. install_user 模块 (install_user.c/h)

**功能**: 实现用户目录安装（到 ~/.local）

**主要函数**:
- `install_user()` - 将包安装到用户目录

**依赖**: 
- utils 模块
- extract 模块
- 系统命令：`dpkg-deb`, `find`, `ln`

**特点**:
- 无需 root 权限
- 安装到 ~/.local/opt/<package-name>
- 自动创建符号链接到 ~/.local/bin
- 不处理依赖关系

**安装流程**:
1. 获取用户主目录
2. 创建 ~/.local 目录结构
3. 创建临时目录
4. 提取控制信息并解析包名
5. 提取数据文件到 ~/.local/opt/<package-name>
6. 为可执行文件创建符号链接
7. 清理临时目录

**使用示例**:
```c
#include "install_user.h"

install_user("package.deb");  // 用户目录安装
```

---

### 5. cli 模块 (cli.c/h)

**功能**: 处理命令行参数和用户交互

**主要函数**:
- `show_help()` - 显示帮助信息
- `show_version()` - 显示版本信息
- `interactive_install()` - 交互式选择安装方式

**依赖**: 
- utils 模块
- install_system 模块
- install_user 模块

**支持的参数**:
- `-h, --help` - 显示帮助
- `-v, --version` - 显示版本
- `-s, --system` - 系统级安装模式
- `-u, --user` - 用户目录安装模式

**使用示例**:
```c
#include "cli.h"

show_help("debpkg");
interactive_install("package.deb");
```

---

### 6. main 模块 (main.c)

**功能**: 程序入口，协调各模块工作

**工作流程**:
1. 解析命令行参数
2. 验证输入文件
3. 根据选择的模式调用相应的安装函数
4. 返回执行状态

**依赖**: 
- cli 模块
- install_system 模块
- install_user 模块
- utils 模块

---

### 7. desktop 模块 (desktop.c/h) ✨ **新增**

**功能**: 为已安装的包创建 XDG 标准的 desktop 文件

**主要函数**:
- `create_desktop_file()` - 创建 desktop entry 文件到 `~/.local/share/applications/`

**依赖**: 
- `utils.h` - 用于目录创建和文件操作

**使用示例**:
```c
#include "desktop.h"
#include "utils.h"

char home_dir[MAX_PATH_LEN];
get_home_dir(home_dir, sizeof(home_dir));
create_desktop_file("myapp", home_dir);
```

**生成的 Desktop 文件内容**:
``ini
[Desktop Entry]
Version=1.0
Type=Application
Name=myapp
Comment=Application installed by debpkg
Exec=/home/user/.local/bin/myapp
Icon=application-x-executable
Terminal=false
Categories=Application;
StartupNotify=true
```

**特点**:
- 遵循 XDG Desktop Entry 规范
- 自动检测文件是否已存在，避免覆盖用户配置
- 仅在用户目录安装模式下启用
- 只在检测到可执行文件时创建

---

### 8. user_deps 模块 (user_deps.c/h) ✨ **新增**

**功能**: 记录和管理用户目录下安装的 DEB 包及其依赖关系，支持版本敏感的依赖检测

**主要函数**:
- `init_user_deps_db()` - 初始化用户依赖数据库
- `save_package_dependencies()` - 保存包的依赖关系到数据库
- `check_user_dependency_version()` - 检查特定版本的依赖是否已安装
- `get_installed_dep_version()` - 获取已安装依赖的版本号
- `load_package_dependencies()` - 加载包的依赖记录
- `list_user_installed_packages()` - 列出所有用户目录下安装的包
- `remove_package_from_deps_db()` - 从数据库中移除包记录（卸载时使用）

**数据结构**:
```c
typedef struct {
    char pkg_name[256];      // 包名
    char pkg_version[64];    // 包版本
    Dependency *deps;        // 依赖列表
    int dep_count;           // 依赖数量
} UserPackageRecord;
```

**依赖**: 
- `depends.h` - 依赖管理基础接口
- `utils.h` - 文件操作和目录管理

**数据库位置**:
```
~/.local/share/debpkg/user_deps.db
```

**数据库格式**:
``bash
# PKG_NAME|PKG_VERSION|DEP_NAME1:DEP_VER1,DEP_NAME2:DEP_VER2
mypackage|1.2.3|libfoo:>=1.0.0,libbar:=2.1.0,libbaz:*
```

**使用示例**:
```c
#include "user_deps.h"

char home_dir[MAX_PATH_LEN];
get_home_dir(home_dir, sizeof(home_dir));

// 保存依赖关系
Dependency deps[3] = {...};
save_package_dependencies("myapp", "1.2.3", deps, 3, home_dir);

// 检查版本
if (check_user_dependency_version("libfoo", ">=1.0.0", home_dir)) {
    printf("Dependency satisfied!\n");
}

// 获取版本号
char version[64];
get_installed_dep_version("libfoo", version, sizeof(version), home_dir);
printf("Installed version: %s\n", version);
```

**特性优势**:
- ✅ **版本敏感** - 精确匹配版本要求
- ✅ **避免冗余** - 同版本依赖不重复下载
- ✅ **智能检测** - 自动识别版本冲突
- ✅ **持久化存储** - 数据库记录所有依赖关系
- ✅ **易于查询** - 提供完整的 API 用于查询和管理

---

### 9. env_config 模块 (env_config.c/h) ✨ **新增**

**功能**: 智能检测和配置用户目录安装所需的环境变量，确保安装的应用和库能被系统正确识别

**主要函数**:
- `check_and_prompt_env_config()` - 检查并提示环境变量配置
- `generate_env_script()` - 生成统一的环境配置脚本
- `show_env_config_guide()` - 显示详细的配置指南
- `detect_env_issues()` - 自动检测环境问题

**检测的环境变量**:
- **PATH** - 可执行文件搜索路径（必需）
- **LD_LIBRARY_PATH** - 共享库搜索路径（必需）
- **C_INCLUDE_PATH** - C/C++ 头文件搜索路径（可选，开发用）
- **MANPATH** - 手册页搜索路径（可选）

**使用示例**:
```c
#include "env_config.h"

char home_dir[MAX_PATH_LEN];
get_home_dir(home_dir, sizeof(home_dir));

// 安装完成后检查和提示
check_and_prompt_env_config(home_dir, pkg_name);

// 或单独使用各个功能
int issues = detect_env_issues(home_dir);
if (issues > 0) {
    generate_env_script(home_dir, "~/.local/debpkg_env.sh");
    show_env_config_guide(home_dir);
}
```

**生成的配置脚本**:
```
#!/bin/bash
# ~/.local/debpkg_env.sh
export PATH="$HOME/.local/bin:$PATH"
export LD_LIBRARY_PATH="$HOME/.local/lib:$LD_LIBRARY_PATH"
export C_INCLUDE_PATH="$HOME/.local/include:$C_INCLUDE_PATH"
export MANPATH="$HOME/.local/share/man:$MANPATH"
```

**特性优势**:
- ✅ **自动检测** - 智能发现未配置的环境变量
- ✅ **交互友好** - 提供清晰的配置建议
- ✅ **一键配置** - 生成统一的配置脚本
- ✅ **灵活选择** - 支持临时和永久两种配置方式
- ✅ **全局生效** - 一次配置，所有包都可用

---

### 10. uninstall 模块 (uninstall.c/h) ✨ **新增**

**功能**: 实现用户目录下 DEB 包的智能卸载，具备依赖关系检查和保护机制

**主要函数**:
- `uninstall_package_smart()` - 智能卸载主函数
- `check_dependency_usage()` - 检查依赖是否被其他包使用
- `get_package_manifest()` - 获取包的安装清单
- `remove_package_files()` - 执行文件删除操作
- `cleanup_unused_dependencies()` - 清理未使用的依赖
- `show_uninstall_info()` - 显示卸载信息

**数据结构**:
``c
// 卸载选项
typedef struct {
    int force;              // 强制卸载
    int dry_run;           // 模拟卸载
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

**工作流程**:
1. 检查包是否存在
2. 显示卸载信息并等待用户确认
3. 读取 INSTALL_MANIFEST
4. 删除文件和目录
5. 删除桌面文件
6. 检查每个依赖的使用情况
7. 只删除未被使用的依赖
8. 从数据库移除记录
9. 显示统计信息

**特性优势**:
- ✅ **智能保护** - 自动保护正在使用的依赖
- ✅ **安全可靠** - 用户确认后才执行
- ✅ **完整清理** - 删除所有相关文件
- ✅ **详细反馈** - 清晰的统计信息
- ✅ **模块化** - 独立的卸载模块

**使用示例**:
```bash
# 卸载包
./debpkg -r package-name
./debpkg --remove package-name
./debpkg --uninstall package-name

# 输出示例
[INFO] Uninstalling package: myapp
...
Files removed: 15
Directories removed: 3
Dependencies kept: 2
Dependencies removed: 1
```

---

## 维护指南

### 添加新功能
1. 确定功能属于哪个模块
2. 在对应的 .h 文件中声明函数
3. 在对应的 .c 文件中实现函数
4. 更新 Makefile 的依赖关系（如有需要）
5. 重新编译测试

### 调试技巧
- 查看编译警告：`make 2>&1 | grep warning`
- 清理重编译：`make clean && make`
- 单步调试：使用 gdb 调试特定模块

### 模块测试
可以单独测试每个模块的功能：
```c
// test_module.c
#include "utils.h"
#include <stdio.h>

int main() {
    print_info("Testing utils module");
    return 0;
}
```

---

## 优势分析

### 可读性
✅ 每个模块职责单一，易于理解  
✅ 函数命名清晰，一看就懂  
✅ 代码结构层次分明  

### 可维护性
✅ 修改某个功能只需改动对应模块  
✅ 模块间接口明确，降低耦合  
✅ 便于代码审查和问题定位  

### 可扩展性
✅ 容易添加新的安装模式  
✅ 可以独立优化某个模块  
✅ 支持单元测试  

### 复用性
✅ utils 模块可在其他项目中复用  
✅ extract 模块可独立使用  
✅ 模块化的设计便于代码移植  

---

## 总结

通过将 debpkg 分成 **8 个模块**，我们实现了：
- **高内聚**: 每个模块专注于一个功能领域
- **低耦合**: 模块间通过明确的接口通信
- **易维护**: 代码结构清晰，便于理解和修改
- **易扩展**: 新增功能不会影响现有代码

### 模块统计

| 模块 | 头文件 | 源文件 | 行数 | 说明 |
|------|--------|--------|------|------|
| utils | utils.h | utils.c | ~200 | 基础工具函数 |
| cli | cli.h | cli.c | ~150 | 命令行交互 |
| extract | extract.h | extract.c | ~150 | DEB 包提取 |
| depends | depends.h | depends.c | ~280 | 依赖管理 |
| install_system | install_system.h | install_system.c | ~100 | 系统级安装 |
| install_user | install_user.h | install_user.c | ~430 | 用户目录安装 |
| desktop | desktop.h | desktop.c | ~90 | Desktop 文件创建 |
| user_deps | user_deps.h | user_deps.c | ~350 | 用户依赖记录 |
| env_config | env_config.h | env_config.c | ~230 | 环境变量配置 |
| uninstall | uninstall.h | uninstall.c | ~380 | 智能卸载 |
| **总计** | **10** | **10** | **~2360** | **模块化架构** |

这种模块化设计使得项目更易于长期维护和迭代开发.
