#!/bin/bash

# debpkg 安装脚本
# 自动编译并安装 debpkg 工具到系统

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

# 检测系统依赖
check_dependencies() {
    info "检查系统依赖..."
    
    local missing_deps=()
    
    if ! command -v gcc &> /dev/null; then
        missing_deps+=("gcc")
    fi
    
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        error "缺少以下依赖："
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo ""
        info "请使用以下命令安装依赖:"
        echo "  Ubuntu/Debian: sudo apt install build-essential"
        echo "  Fedora/RHEL: sudo dnf install gcc make"
        echo "  Arch Linux: sudo pacman -S gcc make"
        exit 1
    fi
    
    success "系统依赖检查通过"
}

# 清理旧的编译产物
clean_build() {
    info "清理旧的编译产物..."
    if [ -f "Makefile" ]; then
        make clean 2>/dev/null || true
    fi
    rm -rf obj/ debpkg 2>/dev/null || true
    success "清理完成"
}

# 编译项目
build_project() {
    info "编译项目..."
    if ! make; then
        error "编译失败"
        exit 1
    fi
    success "编译成功"
}

# 系统级安装
install_system_wide() {
    info "执行系统级安装到 /usr/local/bin..."
    
    if [ ! -f "debpkg" ]; then
        error "未找到 debpkg 可执行文件，请先编译"
        exit 1
    fi
    
    sudo cp debpkg /usr/local/bin/
    sudo chmod +x /usr/local/bin/debpkg
    
    success "系统级安装完成"
    info "debpkg 已安装到 /usr/local/bin/debpkg"
}

# 用户级安装
install_user_local() {
    info "执行用户级安装到 ~/.local/bin..."
    
    if [ ! -f "debpkg" ]; then
        error "未找到 debpkg 可执行文件，请先编译"
        exit 1
    fi
    
    mkdir -p "$HOME/.local/bin"
    cp debpkg "$HOME/.local/bin/"
    chmod +x "$HOME/.local/bin/debpkg"
    
    success "用户级安装完成"
    
    # 检查 PATH 是否包含 ~/.local/bin
    if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
        warning "~/.local/bin 不在 PATH 中"
        echo ""
        echo "请将以下内容添加到您的 shell 配置文件 (~/.bashrc, ~/.zshrc 等):"
        echo '  export PATH="$HOME/.local/bin:$PATH"'
        echo ""
        echo "然后运行：source ~/.bashrc (或对应的配置文件)"
    else
        info "debpkg 已安装到 $HOME/.local/bin/debpkg"
    fi
}

# 创建桌面入口（如果适用）
create_desktop_entry() {
    info "检查是否需要创建桌面入口..."
    
    local desktop_file="$HOME/.local/share/applications/debpkg.desktop"
    
    if [ -f "$desktop_file" ]; then
        info "桌面入口文件已存在，跳过创建"
        return
    fi
    
    read -p "是否创建桌面入口文件？(y/N): " create_desktop
    
    if [ "$create_desktop" = "y" ] || [ "$create_desktop" = "Y" ]; then
        mkdir -p "$HOME/.local/share/applications"
        
        cat > "$desktop_file" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=debpkg
Comment=DEB Package Installer
Exec=$HOME/.local/bin/debpkg
Icon=system-software-install
Categories=System;PackageManager;
Terminal=true
EOF
        
        success "桌面入口已创建：$desktop_file"
    fi
}

# 验证安装
verify_installation() {
    info "验证安装..."
    
    local debpkg_path=""
    
    if command -v debpkg &> /dev/null; then
        debpkg_path=$(command -v debpkg)
    elif [ -x "$HOME/.local/bin/debpkg" ]; then
        debpkg_path="$HOME/.local/bin/debpkg"
    elif [ -x "/usr/local/bin/debpkg" ]; then
        debpkg_path="/usr/local/bin/debpkg"
    fi
    
    if [ -n "$debpkg_path" ]; then
        success "安装验证成功"
        echo "  可执行文件位置：$debpkg_path"
        echo ""
        info "测试运行:"
        debpkg --help | head -5
        echo ""
        return 0
    else
        warning "无法验证安装（debpkg 可能不在 PATH 中）"
        return 1
    fi
}

# 显示使用帮助
show_help() {
    echo "用法：$0 [选项]"
    echo ""
    echo "选项:"
    echo "  -s, --system     系统级安装到 /usr/local/bin (需要 sudo)"
    echo "  -u, --user       用户级安装到 ~/.local/bin (无需 sudo)"
    echo "  -c, --clean      仅清理编译产物"
    echo "  -b, --build      仅编译不安装"
    echo "  -h, --help       显示此帮助信息"
    echo ""
    echo "如果不指定选项，将进入交互式安装模式"
}

# 主函数
main() {
    echo "========================================"
    echo "  debpkg 安装脚本"
    echo "========================================"
    echo ""
    
    # 解析命令行参数
    INSTALL_MODE="interactive"
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -s|--system)
                INSTALL_MODE="system"
                shift
                ;;
            -u|--user)
                INSTALL_MODE="user"
                shift
                ;;
            -c|--clean)
                INSTALL_MODE="clean"
                shift
                ;;
            -b|--build)
                INSTALL_MODE="build"
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
    
    case $INSTALL_MODE in
        clean)
            check_dependencies
            clean_build
            success "清理完成"
            ;;
        build)
            check_dependencies
            clean_build
            build_project
            success "编译完成，可执行文件位于：./debpkg"
            ;;
        system)
            check_dependencies
            clean_build
            build_project
            install_system_wide
            verify_installation
            success "系统级安装完成！"
            ;;
        user)
            check_dependencies
            clean_build
            build_project
            install_user_local
            create_desktop_entry
            verify_installation
            success "用户级安装完成！"
            ;;
        interactive)
            check_dependencies
            clean_build
            build_project
            
            echo ""
            echo "选择安装方式:"
            echo "  1) 系统级安装 (/usr/local/bin，需要 sudo 权限)"
            echo "  2) 用户级安装 (~/.local/bin，无需 sudo)"
            echo "  3) 仅编译不安装"
            echo "  4) 取消安装"
            echo ""
            
            read -p "请选择 [1-4]: " choice
            
            case $choice in
                1)
                    install_system_wide
                    verify_installation
                    success "系统级安装完成！"
                    ;;
                2)
                    install_user_local
                    create_desktop_entry
                    verify_installation
                    success "用户级安装完成！"
                    ;;
                3)
                    success "编译完成，可执行文件位于：./debpkg"
                    ;;
                4)
                    info "取消安装"
                    exit 0
                    ;;
                *)
                    error "无效选择"
                    exit 1
                    ;;
            esac
            ;;
    esac
    
    echo ""
    echo "========================================"
    success "安装过程完成！"
    echo "========================================"
}

# 执行主函数
main "$@"
