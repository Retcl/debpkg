# debpkg 模块化架构说明

## 项目结构

**新的目录组织**：

```
debpkg/
├── include/              # 头文件目录（公共接口）
│   ├── cli.h
│   ├── extract.h
│   ├── install_system.h
│   ├── install_user.h
│   └── utils.h
│
├── src/                  # 源代码目录（具体实现）
│   ├── main.c
│   ├── cli.c
│   ├── utils.c
│   ├── extract.c
│   ├── install_system.c
│   └── install_user.c
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

## 模块依赖关系图

```
                    main.c
                   /  |  \
                  /   |   \
                 /    |    \
           cli.c  install_system.c  install_user.c
             |         |               /      \
             |         |              /        \
          utils.c <---+-------------/          /
             ^                               /
             |                              /
             +----------------------------/
                        extract.c
                           |
                        utils.c
```

---

## 编译说明

### 编译命令
```bash
make
```

### 编译过程
Makefile 会自动编译所有 .c 文件并链接成可执行文件：
1. 编译每个源文件为目标文件 (.o)
2. 链接所有目标文件生成 debpkg

### 依赖管理
Makefile 中明确定义了每个目标文件的依赖关系，确保头文件修改后正确重新编译。

---

## 代码规范

### 命名约定
- **头文件**: 使用 `.h` 扩展名，采用大写宏保护
- **源文件**: 使用 `.c` 扩展名
- **函数命名**: 小写字母 + 下划线，如 `print_info()`
- **宏命名**: 大写字母 + 下划线，如 `MAX_PATH_LEN`
- **变量命名**: 小写字母 + 下划线，如 `home_dir`

### 头文件保护
``c
#ifndef DEBPKG_MODULE_H
#define DEBPKG_MODULE_H

//  declarations

#endif // DEBPKG_MODULE_H
```

### 注释规范
- 每个模块文件顶部说明功能
- 函数上方注释说明用途
- 复杂逻辑添加行内注释

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

通过将 debpkg 分成 6 个模块，我们实现了：
- **高内聚**: 每个模块专注于一个功能领域
- **低耦合**: 模块间通过明确的接口通信
- **易维护**: 代码结构清晰，便于理解和修改
- **易扩展**: 新增功能不会影响现有代码

这种模块化设计使得项目更易于长期维护和迭代开发。
