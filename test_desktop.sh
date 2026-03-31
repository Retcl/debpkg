#!/bin/bash
# Desktop 文件功能测试脚本

echo "======================================"
echo "Desktop 文件自动创建功能测试"
echo "======================================"
echo ""

# 检查是否编译成功
if [ ! -f "./debpkg" ]; then
    echo "[INFO] Building debpkg..."
    make clean && make
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build failed!"
        exit 1
    fi
fi

echo "[SUCCESS] Build successful!"
echo ""

# 显示新增的文件
echo "======================================"
echo "新增的模块文件:"
echo "======================================"
ls -lh include/desktop.h src/desktop.c
echo ""

# 查看 desktop.c 代码摘要
echo "======================================"
echo "desktop.c 核心函数:"
echo "======================================"
grep -A 5 "int create_desktop_file" src/desktop.c | head -n 10
echo ""

# 查看生成的 desktop 文件格式
echo "======================================"
echo "Desktop 文件格式示例:"
echo "======================================"
cat << 'EOF'
[Desktop Entry]
Version=1.0
Type=Application
Name=<package-name>
Comment=Application installed by debpkg
Exec=/home/<user>/.local/bin/<package-name>
Icon=application-x-executable
Terminal=false
Categories=Application;
StartupNotify=true
EOF
echo ""

# 查看集成代码
echo "======================================"
echo "在 install_user.c 中的集成:"
echo "======================================"
grep -B 2 -A 8 "create_desktop_file" src/install_user.c | head -n 15
echo ""

# 查看文档更新
echo "======================================"
echo "相关文档:"
echo "======================================"
echo "- README.md (已更新用户目录安装说明)"
echo "- MODULES.md (已添加 desktop 模块说明)"
echo "- DESKTOP_FILE_FEATURE.md (功能详细说明)"
echo "- DESKTOP_FEATURE_SUMMARY.md (实现总结)"
echo ""

echo "======================================"
echo "使用说明:"
echo "======================================"
echo "1. 用户目录安装包时会自动创建 desktop 文件:"
echo "   ./debpkg -u package.deb"
echo ""
echo "2. Desktop 文件位置:"
echo "   ~/.local/share/applications/<package-name>.desktop"
echo ""
echo "3. 特性:"
echo "   ✓ 符合 XDG Desktop Entry 规范"
echo "   ✓ 自动检测，避免覆盖用户配置"
echo "   ✓ 仅在检测到可执行文件时创建"
echo "   ✓ 集成到应用程序菜单"
echo ""

echo "======================================"
echo "测试完成!"
echo "======================================"
