# debpkg 目录结构说明

## 📂 重新组织后的项目结构

为了提高项目的可维护性和专业性，我们已经将代码按照标准的 C 语言项目结构进行了重组。

## 标准目录布局

```
debpkg/
│
├── include/              # 头文件目录（公共接口声明）
│   ├── auto_deps.h      # 自动依赖处理模块接口
│   ├── cli.h            # 命令行交互模块接口
│   ├── depends.h        # 依赖检测模块接口
│   ├── desktop.h        # 桌面入口模块接口
│   ├── env_config.h     # 环境变量配置模块接口
│   ├── extract.h        # DEB 包提取模块接口
│   ├── install_system.h # 系统级安装模块接口
│   ├── install_user.h   # 用户目录安装模块接口
│   ├── uninstall.h      # 卸载功能模块接口
│   ├── user_deps.h      # 用户依赖管理模块接口
│   └── utils.h          # 通用工具函数模块接口
│
├── src/                 # 源代码目录（具体实现）
│   ├── main.c          # 主程序入口
│   ├── auto_deps.c     # 自动依赖处理实现
│   ├── cli.c           # 命令行交互实现
│   ├── depends.c       # 依赖检测实现
│   ├── desktop.c       # 桌面入口实现
│   ├── env_config.c    # 环境变量配置实现
│   ├── extract.c       # DEB 包提取实现
│   ├── install_system.c # 系统级安装实现
│   ├── install_user.c   # 用户目录安装实现
│   ├── uninstall.c      # 卸载功能实现
│   ├── user_deps.c     # 用户依赖管理实现
│   └── utils.c         # 工具函数实现
│
├── obj/                 # 编译目标文件（自动生成）
│   ├── auto_deps.o
│   ├── cli.o
│   ├── depends.o
│   ├── desktop.o
│   ├── env_config.o
│   ├── extract.o
│   ├── install_system.o
│   ├── install_user.o
│   ├── main.o
│   ├── uninstall.o
│   ├── user_deps.o
│   └── utils.o
│
├── 文档目录（根目录）
│   ├── README.md              # 项目主文档（英文）
│   ├── README_CN.md           # 项目主文档（中文）
│   ├── INSTALL_GUIDE.md       # 安装脚本使用指南
│   ├── MODULES.md             # 架构设计文档
│   ├── QUICKSTART.md          # 快速上手指南
│   ├── DIRECTORY_STRUCTURE.md # 目录结构说明
│   └── ...                    # 其他功能文档
│
├── 脚本文件
│   ├── Makefile          # GNU Make 编译脚本
│   ├── install.sh        # 自动化安装脚本 ⭐ NEW!
│   ├── test.sh           # 主测试脚本
│   ├── test_desktop.sh   # 桌面功能测试
│   ├── test_env_config.sh # 环境配置测试
│   ├── test_uninstall.sh # 卸载功能测试
│   ├── test_user_deps.sh # 用户依赖测试
│   └── clean.sh          # 清理脚本
│
├── .gitignore          # Git 忽略配置
└── debpkg              # 编译生成的可执行文件
```

## 目录分类原则

### 1. include/ - 头文件目录

**存放内容**：所有 `.h` 头文件

**作用**：
- 声明模块的公共接口
- 定义数据结构和宏
- 提供函数原型

**文件列表**：
| 文件 | 功能 | 行数 |
|------|------|------|
| `cli.h` | 命令行交互接口 | ~20 |
| `extract.h` | DEB 包提取接口 | ~15 |
| `install_system.h` | 系统安装接口 | ~10 |
| `install_user.h` | 用户安装接口 | ~10 |
| `utils.h` | 工具函数接口 | ~30 |

**命名规范**：
- 使用小写字母和下划线
- 添加头文件保护宏
- 包含必要的标准库头文件

**示例**：
```c
#ifndef DEBPKG_UTILS_H
#define DEBPKG_UTILS_H

#include <stddef.h>

int file_exists(const char *path);
void print_info(const char *msg);

#endif
```

---

### 2. src/ - 源代码目录

**存放内容**：所有 `.c` 源文件

**作用**：
- 实现头文件中声明的函数
- 包含具体的业务逻辑
- 通过 `#include` 引用头文件

**文件列表**：
| 文件 | 功能 | 行数 | 依赖头文件 |
|------|------|------|-----------|
| `main.c` | 程序入口 | ~80 | cli.h, install_*.h, utils.h |
| `cli.c` | 命令行处理 | ~70 | cli.h, install_*.h, utils.h |
| `utils.c` | 工具函数 | ~100 | utils.h |
| `extract.c` | DEB 提取 | ~80 | extract.h, utils.h |
| `install_system.c` | 系统安装 | ~40 | install_system.h, utils.h |
| `install_user.c` | 用户安装 | ~120 | install_user.h, extract.h, utils.h |

**引用头文件的方式**：
```c
// 引用项目内头文件（使用相对路径）
#include "../include/utils.h"

// 引用系统头文件
#include <stdio.h>
#include <stdlib.h>
```

---

### 3. obj/ - 编译产物目录

**存放内容**：编译生成的 `.o` 目标文件

**特点**：
- 由 Makefile 自动创建
- 执行 `make clean` 时删除
- 不需要提交到版本控制

**生成规则**：
```bash
src/main.c → obj/main.o
src/cli.c  → obj/cli.o
...
```

---

### 4. doc/ - 文档目录（可选）

**存放内容**：项目文档

**当前文档都在根目录**，未来可以移入此目录：
- `README.md` - 使用说明
- `MODULES.md` - 架构文档
- `QUICKSTART.md` - 快速指南
- `CHANGES.md` - 变更记录

---

## Makefile 配置说明

### 关键变量

```makefile
# 目录配置
SRCDIR = src
INCDIR = include
OBJDIR = obj

# 自动查找源文件
SRCS = $(wildcard $(SRCDIR)/*.c)

# 转换源文件路径为目标文件路径
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
```

### 编译规则

```makefile
# 编译单个目标文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

# 解释：
# -I$(INCDIR)  : 指定头文件搜索路径为 include/
# -c           : 只编译不链接
# -o $@        : 输出到目标文件
# $<           : 第一个依赖（源文件）
```

### 使用方法

```bash
make          # 编译所有文件
make clean    # 清理编译产物
make install  # 安装到系统
```

---

## 目录结构的优势

### ✅ 符合 C 语言项目惯例

遵循标准的 C 语言项目组织方式：
- `include/` - 公共头文件
- `src/` - 私有实现
- `obj/` - 编译产物

### ✅ 提高可维护性

**清晰的分离**：
- 接口（.h）与实现（.c）分离
- 源代码与编译产物分离
- 代码与文档分离

**易于导航**：
- 找接口 → `include/`
- 找实现 → `src/`
- 找文档 → 根目录或 `doc/`

### ✅ 便于扩展

**添加新模块**：
1. 在 `include/` 创建 `new_module.h`
2. 在 `src/` 创建 `new_module.c`
3. Makefile 自动识别并编译

**无需修改**：
- Makefile 使用通配符自动发现源文件
- 不需要手动添加编译规则

### ✅ 支持大型项目

这种结构可以轻松扩展到更复杂的项目：

```
include/
├── core/      # 核心模块
├── utils/     # 工具模块
└── gui/       # 图形界面模块

src/
├── core/
├── utils/
└── gui/

lib/           # 第三方库
tests/         # 单元测试
examples/      # 示例代码
```

---

## 迁移指南

### 从旧结构到新结构

**旧的布局**（已废弃）：
```
debpkg/
├── main.c
├── cli.c
├── cli.h
├── utils.c
├── utils.h
...
```

**新的布局**：
```
debpkg/
├── include/
│   ├── cli.h
│   └── utils.h
├── src/
│   ├── main.c
│   ├── cli.c
│   └── utils.c
```

### 迁移步骤

1. 创建目录：
   ```bash
   mkdir -p include src obj
   ```

2. 移动头文件：
   ```bash
   mv *.h include/
   ```

3. 移动源文件：
   ```bash
   mv *.c src/
   ```

4. 更新源文件中的 `#include` 路径：
   ```c
   // 从 #include "utils.h"
   // 改为 #include "../include/utils.h"
   ```

5. 更新 Makefile 的编译路径

6. 清理重编译：
   ```bash
   make clean && make
   ```

---

## 最佳实践

### 1. 头文件管理

✅ **推荐**：
- 所有头文件放在 `include/`
- 使用宏保护防止重复包含
- 保持头文件简洁，只包含必要声明

❌ **避免**：
- 在头文件中包含实现代码
- 循环依赖（A.h 包含 B.h，B.h 又包含 A.h）
- 过度复杂的目录嵌套

### 2. 源文件组织

✅ **推荐**：
- 每个源文件对应一个模块
- 通过头文件引用其他模块
- 保持源文件独立性

❌ **避免**：
- 跨目录引用源文件
- 直接 `#include "*.c"`
- 硬编码路径

### 3. 编译配置

✅ **推荐**：
- 使用变量管理目录路径
- 自动发现源文件
- 分离编译和链接

❌ **避免**：
- 硬编码文件路径
- 手动列出所有目标文件
- 混合编译不同目录的文件

---

## 总结

通过将项目重新组织为标准的目录结构：

- ✅ **include/** - 清晰的接口声明
- ✅ **src/** - 独立的代码实现
- ✅ **obj/** - 隔离的编译产物
- ✅ **文档齐全** - 易于理解和维护

这种结构使 debpkg 更加专业、可维护，并且为未来的扩展打下了坚实的基础！

---

*文档版本：1.0 | 最后更新：2024 年 3 月*
