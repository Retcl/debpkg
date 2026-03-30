# 自动依赖安装功能

## 🚀 概述

debpkg 现已支持**自动下载并安装依赖**，无需用户手动操作。当检测到缺失的依赖时，程序会自动从 APT 仓库下载 DEB 包并安装到用户目录。

## ✨ 核心特性

### 1. 全自动依赖管理
- ✅ **自动检测** - 从 DEB 包解析依赖列表
- ✅ **智能下载** - 从 APT 仓库自动下载缺失的依赖
- ✅ **递归安装** - 自动处理依赖的依赖
- ✅ **用户目录** - 所有依赖安装到 `~/.local`

### 2. 多种镜像源支持
- 默认使用阿里云镜像（速度快）
- 支持其他 Debian/Ubuntu 镜像源
- 自动选择可用镜像

### 3. 下载工具兼容
- 优先使用 `wget` 下载
- 自动回退到 `curl`
- 支持断点续传（如果工具支持）

## 💡 使用流程

### 基本用法

```bash
./debpkg -u myapp.deb
```

### 完整交互示例

```bash
$ ./debpkg -u myapp.deb

[INFO] Installing package to user directory...
[INFO] Package name: myapp
[INFO] Checking dependencies...
[INFO] Found 4 dependencies:
  ✓ libc6                          (system)
  ✓ libssl1.1                      (system)
  ✗ libspeciallib >= 1.0.0         (missing)
  ✗ libnewfeature                  (missing)

[WARNING] Missing 2 dependencies:
  - libspeciallib >= 1.0.0
  - libnewfeature

Dependency handling options:
  1. Auto-download and install dependencies (RECOMMENDED)
  2. Install missing dependencies manually
     System: sudo apt-get install <package-name>
     User:   ./debpkg -u <package-name>.deb
  3. Continue installation anyway (may not work properly)
  4. Cancel installation

Enter your choice [1-4]: 1

[INFO] Starting automatic dependency installation...

[INFO] Processing: libspeciallib >= 1.0.0
[INFO] Getting package info for: libspeciallib
  URL: pool/main/libs/libspecial/libspecial_1.2.3_amd64.deb
[INFO] Downloading package...
  Downloaded: 245760 bytes
[SUCCESS] Package downloaded successfully
[INFO] Installing downloaded package to user directory...
  ... (递归安装 libspeciallib)
[SUCCESS] Dependency installed: libspeciallib

[INFO] Processing: libnewfeature
[INFO] Getting package info for: libnewfeature
  URL: pool/main/libn/libnewfeature/libnewfeature_2.0.0_amd64.deb
[INFO] Downloading package...
  Downloaded: 128456 bytes
[SUCCESS] Package downloaded successfully
[INFO] Installing downloaded package to user directory...
  ... (递归安装 libnewfeature)
[SUCCESS] Dependency installed: libnewfeature

[INFO] Installation summary:
  Successfully installed: 2
  Failed: 0

[SUCCESS] All dependencies installed successfully!
[INFO] Installing files to standard directories...
...
[SUCCESS] Package installed successfully!
```

## 🔧 工作原理

### 自动依赖安装流程

```
检测到缺失依赖
  ↓
显示交互菜单
  ↓
用户选择"自动安装" (选项 1)
  ↓
遍历每个缺失的依赖:
  ├─ 1. 查询 APT 仓库获取下载 URL
  │    (apt-cache show <pkg>)
  ├─ 2. 构建完整的镜像源 URL
  ├─ 3. 使用 wget/curl 下载 DEB 包
  ├─ 4. 验证下载的文件
  ├─ 5. 递归调用 debpkg -u 安装
  └─ 6. 清理临时文件
  ↓
统计安装结果
  ↓
成功 → 继续主程序安装
失败 → 提示用户处理
```

### 依赖解析层次

```
myapp.deb
├── libc6 (系统已安装) ✓
├── libssl1.1 (系统已安装) ✓
└── libspeciallib (缺失) ✗
    └── libcrypto (libspeciallib 的依赖)
        └── zlib1g (系统已安装) ✓

自动安装顺序:
1. zlib1g (跳过，已安装)
2. libcrypto (下载并安装)
3. libspeciallib (下载并安装)
4. myapp (最后安装主程序)
```

## 📊 技术细节

### 1. 下载 URL 获取

使用 `apt-cache show` 命令：
```bash
apt-cache show <package-name> | grep '^Filename:'
```

输出示例：
```
Filename: pool/main/libs/libspecial/libspecial_1.2.3_amd64.deb
```

### 2. 镜像源配置

默认使用阿里云镜像：
```c
http://mirrors.aliyun.com/<filename-from-apt>
```

支持的镜像源：
- 阿里云：`mirrors.aliyun.com`
- 清华大学：`mirrors.tuna.tsinghua.edu.cn`
- 中国科技大学：`mirrors.ustc.edu.cn`
- 网易：`mirrors.163.com`

### 3. 下载实现

使用 `wget` 或 `curl`：
```bash
# 优先使用 wget
wget -q --timeout=30 --tries=3 -O <output> <url>

# 如果 wget 失败，使用 curl
curl -sL --connect-timeout 30 --max-time 60 -o <output> <url>
```

### 4. 递归安装

通过外部调用实现：
```bash
./debpkg -u /tmp/<package>_auto.deb < /dev/null
```

使用 `< /dev/null` 防止递归时的交互式输入。

## ⚙️ 环境要求

### 必需工具

1. **APT 工具**
   ```bash
   apt-cache      # 查询包信息
   dpkg-deb       # 提取 DEB 包
   ```

2. **下载工具**（至少一个）
   ```bash
   wget           # 推荐
   curl           # 备用
   ```

3. **网络访问**
   - 需要能够访问镜像源服务器
   - 建议配置国内镜像源以获得更好速度

### 可选配置

#### 1. 修改镜像源

编辑 `/etc/apt/sources.list`：
```bash
# 阿里云（推荐）
deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse

# 清华大学
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
```

#### 2. 更新 APT 缓存

```bash
sudo apt-get update
```

## 🎯 优势对比

### vs 手动安装

| 特性 | 手动安装 | 自动安装 |
|------|---------|---------|
| **操作步骤** | 多个命令 | 一键完成 |
| **依赖链处理** | 需要手动追踪 | 自动递归 |
| **错误处理** | 用户负责 | 自动重试 |
| **时间成本** | 高 | 低 |
| **适用场景** | 离线环境 | 在线环境 |

### vs 系统级安装

| 特性 | 系统级 (`apt`) | 用户级 (debpkg) |
|------|---------------|----------------|
| **权限要求** | 需要 root | ❌ 无需 root |
| **影响范围** | 所有用户 | 仅当前用户 |
| **安全性** | 影响系统 | ✅ 隔离安全 |
| **灵活性** | 受系统限制 | 更灵活 |

## ⚠️ 注意事项

### 1. 网络要求

**必需联网** - 自动下载需要网络连接

解决方案：
```bash
# 离线环境使用手动安装
sudo apt-get download <package-name>
./debpkg -u <package-name>.deb
```

### 2. 镜像源配置

确保 APT 缓存是最新的：
```bash
sudo apt-get update
```

### 3. 磁盘空间

下载的包会临时存储：
- 临时目录：`/tmp/debpkg_dl_*`
- 安装后自动清理
- 建议保留至少 500MB 空间

### 4. 防火墙设置

如果需要配置代理：
```bash
export http_proxy=http://proxy.example.com:8080
export https_proxy=http://proxy.example.com:8080
```

## 🔍 故障排查

### 问题 1: 无法下载包

**症状**:
```
[ERROR] Failed to get package download URL
```

**原因**:
- 包不在 APT 仓库中
- APT 缓存过期

**解决**:
```bash
sudo apt-get update
apt-cache show <package-name>  # 检查包是否存在
```

### 问题 2: 下载速度慢

**症状**:
```
[INFO] Downloading package...
(长时间无响应)
```

**解决**:
```bash
# 更换镜像源
sudo sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list
sudo apt-get update
```

### 问题 3: 下载失败

**症状**:
```
[ERROR] Download failed
```

**解决**:
```bash
# 检查网络连接
ping mirrors.aliyun.com

# 手动测试下载
wget http://mirrors.aliyun.com/pool/main/libx/libxxx/libxxx_1.0_amd64.deb

# 使用代理（如果需要）
export http_proxy=http://proxy:port
```

### 问题 4: 依赖冲突

**症状**:
```
[ERROR] Failed to install: <package>
```

**解决**:
```bash
# 查看详细信息
apt-cache depends <package>

# 手动安装冲突的依赖
sudo apt-get install <package>

# 然后继续
./debpkg -u myapp.deb
```

## 📝 最佳实践

### 1. 预先更新 APT 缓存

```bash
sudo apt-get update
./debpkg -u myapp.deb
```

### 2. 使用稳定的镜像源

推荐配置（`/etc/apt/sources.list`）：
```bash
# 阿里云（快速稳定）
deb http://mirrors.aliyun.com/ubuntu/ $(lsb_release -cs) main restricted universe multiverse
```

### 3. 监控磁盘空间

```bash
# 检查可用空间
df -h /tmp

# 清理临时文件
rm -rf /tmp/debpkg_*
```

### 4. 记录安装的依赖

```bash
# 查看安装清单
cat ~/.local/share/<package>/INSTALL_MANIFEST

# 列出所有用户目录安装的包
ls -d ~/.local/share/*/
```

## 🔮 未来改进

### v1.2 计划
- [ ] 多镜像源自动切换
- [ ] 并行下载依赖
- [ ] 下载进度显示
- [ ] 断点续传优化

### v1.3 计划
- [ ] 依赖冲突检测
- [ ] 版本兼容性检查
- [ ] 离线模式支持
- [ ] 依赖树可视化

## 📚 相关模块

- **auto_deps.c/h** - 自动依赖安装核心模块
- **depends.c/h** - 依赖管理基础模块
- **install_user.c** - 用户目录安装集成

## 📖 参考资料

- [APT Repository Format](https://wiki.debian.org/DebianRepository/Format)
- [Debian Package Management](https://www.debian.org/doc/manuals/debian-handbook/)
- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/)

---

*版本：1.0 | 最后更新：2024 年 3 月*
