#!/bin/bash

# debpkg 功能测试脚本

echo "======================================"
echo "debpkg 功能测试"
echo "======================================"
echo ""

# 检查 debpkg 是否存在
if [ ! -f "./debpkg" ]; then
    echo "错误：未找到 debpkg 可执行文件"
    echo "请先运行：make"
    exit 1
fi

echo "✓ debpkg 可执行文件存在"
echo ""

# 测试帮助信息
echo "测试 1: 帮助信息"
echo "--------------------------------------"
./debpkg --help
echo ""

# 测试版本信息
echo "测试 2: 版本信息"
echo "--------------------------------------"
./debpkg --version
echo ""

# 测试无参数情况
echo "测试 3: 无参数（应显示错误）"
echo "--------------------------------------"
./debpkg 2>&1 | head -5
echo ""

# 测试不存在的文件
echo "测试 4: 不存在的 DEB 文件"
echo "--------------------------------------"
./debpkg -u nonexistent.deb 2>&1 | head -3
echo ""

# 检查是否安装了 dpkg-deb
echo "测试 5: 检查系统依赖"
echo "--------------------------------------"
if command -v dpkg-deb &> /dev/null; then
    echo "✓ dpkg-deb 已安装"
else
    echo "✗ dpkg-deb 未安装"
fi

if command -v dpkg &> /dev/null; then
    echo "✓ dpkg 已安装"
else
    echo "✗ dpkg 未安装"
fi
echo ""

echo "======================================"
echo "基本功能测试完成！"
echo "======================================"
echo ""
echo "使用说明："
echo "  系统级安装：./debpkg -s package.deb"
echo "  用户目录安装：./debpkg -u package.deb"
echo "  交互式安装：./debpkg package.deb"
echo ""
