#!/bin/bash

# ========================================
# debpkg 清理脚本
# ========================================
# 功能：清理所有编译产物、临时文件和测试文件

echo "========================================"
echo "  debpkg 项目清理"
echo "========================================"
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 计数器
REMOVED_FILES=0
REMOVED_DIRS=0

# 清理函数
clean_item() {
    local item=$1
    local type=$2
    
    if [ -e "$item" ]; then
        if [ "$type" == "file" ]; then
            rm -f "$item"
            echo -e "${YELLOW}[REMOVE]${NC} $item"
            ((REMOVED_FILES++))
        elif [ "$type" == "dir" ]; then
            rm -rf "$item"
            echo -e "${YELLOW}[REMOVE]${NC} $item/"
            ((REMOVED_DIRS++))
        fi
    fi
}

echo -e "${BLUE}正在清理编译产物...${NC}"
echo "----------------------------------------"

# 清理可执行文件
clean_item "debpkg" "file"

# 清理目标文件
for obj in *.o obj/*.o; do
    if [ -f "$obj" ]; then
        clean_item "$obj" "file"
    fi
done

# 清理 obj 目录
clean_item "obj" "dir"

# 清理其他编译产物
clean_item "*.out" "file"
clean_item "*.so" "file"
clean_item "*.a" "file"
clean_item "build" "dir"
clean_item "bin" "dir"

echo ""
echo -e "${BLUE}正在清理临时文件...${NC}"
echo "----------------------------------------"

# 清理临时文件
for tmp in *.tmp *~ .*\.swp .*\.swo *.bak *.old; do
    for file in $tmp; do
        if [ -f "$file" ]; then
            clean_item "$file" "file"
        fi
    done
done

# 清理编辑器文件
clean_item ".vscode" "dir"
clean_item ".idea" "dir"

# 清理 OS 文件
clean_item ".DS_Store" "file"
clean_item "Thumbs.db" "file"
clean_item "desktop.ini" "file"

echo ""
echo -e "${BLUE}正在清理测试文件...${NC}"
echo "----------------------------------------"

# 清理测试 DEB 包
for deb in test_*.deb *.deb.bak; do
    if [ -f "$deb" ]; then
        clean_item "$deb" "file"
    fi
done

# 清理日志
for log in *.log; do
    if [ -f "$log" ]; then
        clean_item "$log" "file"
    fi
done

echo ""
echo "========================================"
echo -e "${GREEN}✅ 清理完成！${NC}"
echo "========================================"
echo ""
echo "统计信息:"
echo "  - 删除文件数：$REMOVED_FILES"
echo "  - 删除目录数：$REMOVED_DIRS"
echo ""

# 显示剩余的文件
echo -e "${BLUE}当前项目文件:${NC}"
echo "----------------------------------------"
find . -maxdepth 3 -type f \( \
    -name "*.c" -o \
    -name "*.h" -o \
    -name "*.md" -o \
    -name "Makefile" -o \
    -name "*.sh" -o \
    -name ".gitignore" \
\) ! -path "./.git/*" | sort | while read file; do
    size=$(wc -c < "$file")
    printf "  %-35s %8s bytes\n" "$(basename $file)" "$size"
done

echo ""
echo -e "${GREEN}项目已整理干净，可以随时开始新的编译！${NC}"
echo ""
