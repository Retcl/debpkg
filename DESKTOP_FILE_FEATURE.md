# Desktop 文件自动创建功能

## 功能概述

当使用 `debpkg -u`（用户目录安装模式）安装包时，工具会自动为安装的包创建 XDG 标准的 desktop 文件。

## Desktop 文件位置

创建的 desktop 文件位于：
```
~/.local/share/applications/<package-name>.desktop
```

这符合 XDG Base Directory 规范，桌面环境会自动识别这些文件并在应用程序菜单中显示。

## Desktop Entry 内容示例

生成的 desktop 文件包含以下标准字段：

```ini
[Desktop Entry]
Version=1.0
Type=Application
Name=<package-name>
Comment=Application installed by debpkg
Exec=/home/username/.local/bin/<package-name>
Icon=application-x-executable
Path=
Terminal=false
Categories=Application;
StartupNotify=true
```

## 字段说明

- **Name**: 应用程序名称（从包名获取）
- **Exec**: 可执行文件路径（指向 `~/.local/bin/<package-name>`）
- **Icon**: 默认图标（`application-x-executable`）
- **Terminal**: 是否以终端模式运行（默认为 false）
- **Categories**: 应用程序分类
- **StartupNotify**: 启动时是否显示通知

## 注意事项

1. **仅用户目录安装支持**：此功能仅在 `-u`（用户目录安装）模式下启用
2. **需要可执行文件**：只有当包中包含 `usr/bin/` 目录时才会创建 desktop 文件
3. **文件已存在处理**：如果 desktop 文件已存在，会跳过创建以避免覆盖用户自定义配置
4. **图标自定义**：如需自定义图标，可以手动编辑生成的 desktop 文件

## 手动编辑 Desktop 文件

如果需要自定义图标或其他设置，可以编辑：
```bash
nano ~/.local/share/applications/<package-name>.desktop
```

常用自定义项：
- `Icon=/path/to/custom/icon.png` - 自定义图标路径
- `Terminal=true` - 以终端模式运行
- `Comment=Your custom description` - 自定义描述

## 刷新 Desktop 数据库

某些桌面环境可能需要刷新才能显示新安装的应用：

```bash
# GNOME/KDE
update-desktop-database ~/.local/share/applications/

# 或者重新登录/重启桌面环境
```

## 卸载时的清理

目前卸载时不会自动删除 desktop 文件，可以手动删除：
```bash
rm ~/.local/share/applications/<package-name>.desktop
```

## 技术实现

- 新增 `desktop.c` 和 `desktop.h` 模块
- 在 `install_user()` 函数中集成 desktop 文件创建
- 遵循 XDG Desktop Entry 规范
- 自动检测并避免覆盖已存在的文件
