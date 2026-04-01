#!/bin/bash
# 用户目录安装调试脚本

echo "======================================"
echo "用户目录安装问题诊断工具"
echo "======================================"
echo ""

if [ -z "$1" ]; then
    echo "用法：$0 <package.deb>"
    echo ""
    echo "示例:"
    echo "  $0 myapp.deb"
    echo "  $0 /path/to/package.deb"
    exit 1
fi

DEB_FILE="$1"

if [ ! -f "$DEB_FILE" ]; then
    echo "[ERROR] 文件不存在：$DEB_FILE"
    exit 1
fi

echo "[INFO] 分析 DEB 包：$DEB_FILE"
echo ""

# 获取包名
PKG_NAME=$(dpkg-deb -f "$DEB_FILE" Package 2>/dev/null)
if [ -z "$PKG_NAME" ]; then
    echo "[ERROR] 无法读取包信息"
    exit 1
fi

echo "[INFO] 包名：$PKG_NAME"
echo ""

# 查看包内容结构
echo "======================================"
echo "DEB 包内容结构:"
echo "======================================"
dpkg-deb -c "$DEB_FILE" | head -30
echo ""

# 检查是否有 usr/bin, usr/lib 等目录
echo "======================================"
echo "检查标准目录结构:"
echo "======================================"
echo "提取到临时目录进行检查..."

TEMP_DIR=$(mktemp -d)
dpkg-deb -x "$DEB_FILE" "$TEMP_DIR"

echo ""
echo "usr/bin/ 目录:"
if [ -d "$TEMP_DIR/usr/bin" ]; then
    FILE_COUNT=$(find "$TEMP_DIR/usr/bin" -type f | wc -l)
    echo "  ✓ 存在，包含 $FILE_COUNT 个文件"
    find "$TEMP_DIR/usr/bin" -type f | sed 's|^|    |'
else
    echo "  ✗ 不存在"
fi

echo ""
echo "usr/lib/ 目录:"
if [ -d "$TEMP_DIR/usr/lib" ]; then
    FILE_COUNT=$(find "$TEMP_DIR/usr/lib" -type f | wc -l)
    echo "  ✓ 存在，包含 $FILE_COUNT 个文件"
    find "$TEMP_DIR/usr/lib" -type f | head -10 | sed 's|^|    |'
    if [ $FILE_COUNT -gt 10 ]; then
        echo "    ... 还有 $((FILE_COUNT - 10)) 个文件"
    fi
else
    echo "  ✗ 不存在"
fi

echo ""
echo "usr/share/ 目录:"
if [ -d "$TEMP_DIR/usr/share" ]; then
    FILE_COUNT=$(find "$TEMP_DIR/usr/share" -type f | wc -l)
    echo "  ✓ 存在，包含 $FILE_COUNT 个文件"
    find "$TEMP_DIR/usr/share" -type f | head -10 | sed 's|^|    |'
    if [ $FILE_COUNT -gt 10 ]; then
        echo "    ... 还有 $((FILE_COUNT - 10)) 个文件"
    fi
else
    echo "  ✗ 不存在"
fi

echo ""
echo "etc/ 目录:"
if [ -d "$TEMP_DIR/etc" ]; then
    FILE_COUNT=$(find "$TEMP_DIR/etc" -type f | wc -l)
    echo "  ✓ 存在，包含 $FILE_COUNT 个文件"
    find "$TEMP_DIR/etc" -type f | sed 's|^|    |'
else
    echo "  ✗ 不存在"
fi

# 清理临时目录
rm -rf "$TEMP_DIR"

echo ""
echo "======================================"
echo "用户目录安装位置:"
echo "======================================"
HOME_DIR=$(eval echo ~$USER)
echo "主目录：$HOME_DIR"
echo ""
echo "安装后的文件位置:"
echo "  可执行文件：$HOME_DIR/.local/bin/"
echo "  库文件：  $HOME_DIR/.local/lib/"
echo "  共享数据：$HOME_DIR/.local/share/$PKG_NAME/"
echo "  头文件：  $HOME_DIR/.local/include/$PKG_NAME/"
echo "  配置文件：$HOME_DIR/.local/etc/$PKG_NAME/"
echo "  元数据：  $HOME_DIR/.local/share/$PKG_NAME/INSTALL_MANIFEST"
echo ""

# 检查是否已安装
MANIFEST_FILE="$HOME_DIR/.local/share/$PKG_NAME/INSTALL_MANIFEST"
if [ -f "$MANIFEST_FILE" ]; then
    echo "⚠️  检测到该包已安装!"
    echo ""
    echo "已安装的文件:"
    cat "$MANIFEST_FILE" | sed 's/^/  /'
    echo ""
    read -p "是否继续安装？(y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 0
    fi
fi

echo ""
echo "======================================"
echo "准备安装"
echo "======================================"
echo ""

# 检查 debpkg 是否已编译
if [ ! -f "./debpkg" ]; then
    echo "[INFO] 未找到 debpkg 可执行文件，正在编译..."
    make clean && make
    if [ $? -ne 0 ]; then
        echo "[ERROR] 编译失败!"
        exit 1
    fi
fi

echo "[INFO] 开始安装到用户目录..."
echo ""

# 使用详细模式运行安装
./debpkg -u "$DEB_FILE" 2>&1 | tee /tmp/debpkg_install.log

echo ""
echo "======================================"
echo "安装完成检查"
echo "======================================"
echo ""

# 检查安装结果
if [ -f "$MANIFEST_FILE" ]; then
    echo "✅ 安装成功！"
    echo ""
    echo "新安装的文件:"
    cat "$MANIFEST_FILE" | sed 's/^/  /'
else
    echo "❌ 安装可能失败或未创建清单文件"
fi

echo ""
echo "查看详细日志：/tmp/debpkg_install.log"
echo ""

# 提示环境变量
echo "======================================"
echo "环境变量配置提示:"
echo "======================================"
echo ""
echo "要将这些文件添加到 PATH 和库路径，请运行:"
echo ""
echo "cat >> ~/.bashrc << 'EOF'"
echo "# Add $PKG_NAME to PATH and library path"
echo "export PATH=\"$HOME_DIR/.local/bin:\$PATH\""
echo "export LD_LIBRARY_PATH=\"$HOME_DIR/.local/lib:\$LD_LIBRARY_PATH\""
echo "export XDG_DATA_DIRS=\"$HOME_DIR/.local/share:\$XDG_DATA_DIRS\""
echo "EOF"
echo ""
echo "然后执行：source ~/.bashrc"
echo ""

echo "======================================"
echo "诊断完成!"
echo "======================================"
