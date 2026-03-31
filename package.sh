#!/bin/bash

# debpkg 打包脚本
# 将项目打包成 Debian (.deb) 安装包

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印信息函数
info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 包配置
PACKAGE_NAME="debpkg"
VERSION="1.1.0"
ARCHITECTURE="amd64"
MAINTAINER="Your Name <your.email@example.com>"
DESCRIPTION="A modular DEB package installer written in C"
SECTION="utils"
PRIORITY="optional"
DEPENDS="dpkg, wget | curl"

# 临时目录
BUILD_DIR="build/deb-pkg"
DEBIAN_DIR="$BUILD_DIR/DEBIAN"
INSTALL_DIR="$BUILD_DIR/install"

# 清理函数
cleanup() {
    info "清理临时文件..."
    rm -rf "$BUILD_DIR"
}

# 检查依赖
check_dependencies() {
    info "检查打包工具..."
    
    local missing_tools=()
    
    if ! command -v dpkg-deb &> /dev/null; then
        missing_tools+=("dpkg-dev")
    fi
    
    if ! command -v fakeroot &> /dev/null; then
        missing_tools+=("fakeroot")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        error "缺少以下打包工具："
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        echo ""
        info "请使用以下命令安装:"
        echo "  sudo apt install dpkg-dev fakeroot"
        exit 1
    fi
    
    success "打包工具检查通过"
}

# 准备构建目录
prepare_build_dir() {
    info "准备构建目录..."
    
    # 清理旧的构建文件
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    mkdir -p "$DEBIAN_DIR"
    mkdir -p "$INSTALL_DIR/usr/local/bin"
    mkdir -p "$INSTALL_DIR/usr/local/share/doc/$PACKAGE_NAME"
    
    success "构建目录准备完成"
}

# 编译项目
build_project() {
    info "编译项目..."
    
    # 先清理再编译
    make clean > /dev/null 2>&1 || true
    
    if ! make; then
        error "编译失败"
        exit 1
    fi
    
    if [ ! -f "$PACKAGE_NAME" ]; then
        error "未找到可执行文件 $PACKAGE_NAME"
        exit 1
    fi
    
    success "项目编译成功"
}

# 安装文件到临时目录
install_files() {
    info "安装文件到打包目录..."
    
    # 复制主程序
    cp "$PACKAGE_NAME" "$INSTALL_DIR/usr/local/bin/"
    chmod 755 "$INSTALL_DIR/usr/local/bin/$PACKAGE_NAME"
    
    # 复制文档
    cp README.md "$INSTALL_DIR/usr/local/share/doc/$PACKAGE_NAME/" 2>/dev/null || true
    cp QUICKSTART.md "$INSTALL_DIR/usr/local/share/doc/$PACKAGE_NAME/" 2>/dev/null || true
    cp INSTALL_GUIDE.md "$INSTALL_DIR/usr/local/share/doc/$PACKAGE_NAME/" 2>/dev/null || true
    cp LICENSE "$INSTALL_DIR/usr/local/share/doc/$PACKAGE_NAME/" 2>/dev/null || true
    
    # 复制 man page（如果存在）
    if [ -f "*.1" ] || [ -f "debpkg.1" ]; then
        mkdir -p "$INSTALL_DIR/usr/local/share/man/man1"
        cp *.1 "$INSTALL_DIR/usr/local/share/man/man1/" 2>/dev/null || true
        gzip -n "$INSTALL_DIR/usr/local/share/man/man1/"*.1 2>/dev/null || true
    fi
    
    success "文件安装完成"
}

# 创建 control 文件
create_control_file() {
    info "创建 Debian control 文件..."
    
    cat > "$DEBIAN_DIR/control" << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Architecture: $ARCHITECTURE
Maintainer: $MAINTAINER
Installed-Size: $(du -sk "$INSTALL_DIR" | cut -f1)
Depends: $DEPENDS
Section: $SECTION
Priority: $PRIORITY
Description: $DESCRIPTION
 A modular DEB package installer supporting:
  - System-wide installation via dpkg
  - User directory installation (no root required)
  - Smart dependency detection
  - Automatic dependency installation
  - Interactive mode selection
EOF
    
    success "Control 文件创建完成"
}

# 创建 postinst 脚本
create_postinst() {
    info "创建 postinst 脚本..."
    
    cat > "$DEBIAN_DIR/postinst" << 'EOF'
#!/bin/bash
# postinst script for debpkg

set -e

case "$1" in
    configure)
        echo "Setting up debpkg..."
        
        # 确保可执行权限
        chmod +x /usr/local/bin/debpkg 2>/dev/null || true
        
        # 创建符号链接到标准路径（可选）
        if [ ! -e /usr/bin/debpkg ]; then
            ln -sf /usr/local/bin/debpkg /usr/bin/debpkg 2>/dev/null || true
        fi
        
        echo "debpkg has been successfully installed!"
        echo "Run 'debpkg --help' for usage information."
        ;;
esac

exit 0
EOF
    
    chmod 755 "$DEBIAN_DIR/postinst"
    
    success "Postinst 脚本创建完成"
}

# 创建 prerm 脚本
create_prerm() {
    info "创建 prerm 脚本..."
    
    cat > "$DEBIAN_DIR/prerm" << 'EOF'
#!/bin/bash
# prerm script for debpkg

set -e

case "$1" in
    remove)
        echo "Removing debpkg..."
        
        # 删除符号链接
        rm -f /usr/bin/debpkg 2>/dev/null || true
        
        echo "debpkg has been removed."
        ;;
esac

exit 0
EOF
    
    chmod 755 "$DEBIAN_DIR/prerm"
    
    success "Prerm 脚本创建完成"
}

# 创建 copyright 文件
create_copyright() {
    info "创建 copyright 文件..."
    
    cat > "$DEBIAN_DIR/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: debpkg
Upstream-Contact: $MAINTAINER
Source: https://github.com/yourusername/debpkg

Files: *
Copyright: (c) $(date +%Y) Your Name
License: GPL-2+
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 or later
 as published by the Free Software Foundation.
 .
 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.
EOF
    
    success "Copyright 文件创建完成"
}

# 打包 deb
package_deb() {
    info "打包 Debian 软件包..."
    
    local output_file="${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"
    
    # 使用 fakeroot 打包（避免需要 root 权限）
    fakeroot dpkg-deb --build "$BUILD_DIR" "$output_file"
    
    if [ ! -f "$output_file" ]; then
        error "打包失败"
        exit 1
    fi
    
    # 显示包信息
    local file_size=$(ls -lh "$output_file" | awk '{print $5}')
    success "Debian 软件包创建成功：$output_file ($file_size)"
    
    # 显示包的详细信息
    echo ""
    info "软件包信息:"
    dpkg-deb -I "$output_file"
}

# 验证 deb 包
verify_package() {
    info "验证软件包..."
    
    local deb_file="${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"
    
    if ! dpkg-deb -I "$deb_file" &> /dev/null; then
        error "软件包验证失败"
        exit 1
    fi
    
    # 列出包内容
    echo ""
    info "软件包内容:"
    dpkg-deb -c "$deb_file" | head -20
    
    success "软件包验证通过"
}

# 显示帮助
show_help() {
    echo "用法：$0 [选项]"
    echo ""
    echo "选项:"
    echo "  -c, --clean      仅清理构建目录"
    echo "  -b, --build      仅编译不打包"
    echo "  -p, --package    仅打包不编译（假设已编译）"
    echo "  -v, --verify     仅验证已有的 deb 包"
    echo "  -h, --help       显示此帮助信息"
    echo ""
    echo "如果不指定选项，将执行完整的编译打包流程"
    echo ""
    echo "示例:"
    echo "  $0              # 完整流程：编译 + 打包"
    echo "  $0 --clean      # 清理构建目录"
    echo "  $0 --build      # 只编译"
    echo "  $0 --package    # 只打包"
}

# 主函数
main() {
    echo "========================================"
    echo "  debpkg Debian 打包脚本"
    echo "  Version: $VERSION"
    echo "========================================"
    echo ""
    
    # 解析命令行参数
    ACTION="full"
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--clean)
                ACTION="clean"
                shift
                ;;
            -b|--build)
                ACTION="build"
                shift
                ;;
            -p|--package)
                ACTION="package"
                shift
                ;;
            -v|--verify)
                ACTION="verify"
                shift
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            *)
                error "未知选项：$1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # 确保在项目根目录执行
    if [ ! -f "Makefile" ]; then
        error "请在项目根目录执行此脚本（需要包含 Makefile）"
        exit 1
    fi
    
    case $ACTION in
        clean)
            cleanup
            rm -f ${PACKAGE_NAME}_*.deb
            success "清理完成"
            ;;
        build)
            check_dependencies
            build_project
            success "编译完成，可执行文件位于：./$PACKAGE_NAME"
            ;;
        package)
            check_dependencies
            prepare_build_dir
            install_files
            create_control_file
            create_postinst
            create_prerm
            create_copyright
            package_deb
            verify_package
            ;;
        verify)
            check_dependencies
            verify_package
            ;;
        full)
            check_dependencies
            prepare_build_dir
            build_project
            install_files
            create_control_file
            create_postinst
            create_prerm
            create_copyright
            package_deb
            verify_package
            cleanup
            ;;
    esac
    
    echo ""
    echo "========================================"
    success "打包过程完成！"
    echo "========================================"
}

# 执行主函数
main "$@"
