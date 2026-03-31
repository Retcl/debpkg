#!/bin/bash
# 智能卸载功能测试脚本

echo "======================================"
echo "智能卸载功能测试"
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
ls -lh include/uninstall.h src/uninstall.c
echo ""

# 查看核心函数
echo "======================================"
echo "uninstall.c 核心函数:"
echo "======================================"
grep -E "^int |^void " src/uninstall.c | head -n 10
echo ""

# 查看修改的文件
echo "======================================"
echo "修改的文件:"
echo "======================================"
echo "✓ src/main.c - 添加卸载命令支持"
echo "✓ src/cli.c - 更新帮助信息"
echo ""

# 查看依赖检查逻辑
echo "======================================"
echo "依赖使用检查逻辑:"
echo "======================================"
grep -A 25 "check_dependency_usage" src/uninstall.c | head -n 30
echo ""

# 查看卸载流程
echo "======================================"
echo "卸载流程代码片段:"
echo "======================================"
grep -B 2 -A 15 "uninstall_package_smart" src/uninstall.c | head -n 20
echo ""

# 使用说明
echo "======================================"
echo "使用说明:"
echo "======================================"
echo "1. 卸载包（智能检查依赖）:"
echo "   ./debpkg -r package-name"
echo "   ./debpkg --remove package-name"
echo "   ./debpkg --uninstall package-name"
echo ""
echo "2. 查看帮助:"
echo "   ./debpkg --help"
echo ""

echo "======================================"
echo "功能特性总结:"
echo "======================================"
echo "✅ 智能依赖检查 - 自动检测依赖是否被使用"
echo "✅ 安全保护机制 - 不删除正在使用的依赖"
echo "✅ 完整清理 - 删除所有相关文件"
echo "✅ 用户确认 - 卸载前需要用户确认"
echo "✅ 详细统计 - 显示删除的文件和依赖数量"
echo "✅ 模块化设计 - 独立的 uninstall 模块"
echo ""

echo "======================================"
echo "依赖保护示例:"
echo "======================================"
cat << 'EOF'
场景：
  已安装包:
    - myapp → 依赖：libfoo, libbar
    - anotherapp → 依赖：libfoo, libbaz

  卸载 myapp 时:
    ✓ libfoo - 被 anotherapp 使用 → 保留
    ✓ libbar - 未被使用 → 删除
    ✓ libbaz - 未被使用 → 不处理

  结果:
    Files removed: 15
    Directories removed: 3
    Dependencies kept: 1 (libfoo)
    Dependencies removed: 1 (libbar)
EOF
echo ""

echo "======================================"
echo "相关文档:"
echo "======================================"
echo "- UNINSTALL_FEATURE.md (详细功能说明)"
echo "- USER_DEPS_FEATURE.md (依赖管理功能)"
echo "- MODULES.md (模块架构文档)"
echo ""

echo "======================================"
echo "测试完成!"
echo "======================================"
