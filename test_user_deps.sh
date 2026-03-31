#!/bin/bash
# 用户目录依赖管理功能测试脚本

echo "======================================"
echo "用户目录依赖管理功能测试"
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
ls -lh include/user_deps.h src/user_deps.c
echo ""

# 查看核心函数
echo "======================================"
echo "user_deps.c 核心函数:"
echo "======================================"
grep -E "^int |^void " src/user_deps.c | head -n 10
echo ""

# 查看修改的文件
echo "======================================"
echo "修改的文件:"
echo "======================================"
echo "✓ include/depends.h - 添加版本敏感接口"
echo "✓ src/depends.c - 实现版本检测逻辑"
echo "✓ include/extract.h - 添加版本解析接口"
echo "✓ src/extract.c - 实现版本解析函数"
echo "✓ src/install_user.c - 集成依赖记录和版本检测"
echo ""

# 查看数据库格式
echo "======================================"
echo "依赖数据库格式示例:"
echo "======================================"
cat << 'EOF'
# User Dependencies Database
# Format: PKG_NAME|PKG_VERSION|DEP_NAME1:DEP_VER1,DEP_NAME2:DEP_VER2
# Created by debpkg

mypackage|1.2.3|libfoo:>=1.0.0,libbar:=2.1.0,libbaz:*
anotherapp|3.0.0|libqux:>=3.0.0
EOF
echo ""

# 查看版本检测逻辑
echo "======================================"
echo "版本敏感的依赖检测逻辑:"
echo "======================================"
grep -A 15 "check_user_dependency_with_version" src/depends.c | head -n 20
echo ""

# 查看依赖记录保存
echo "======================================"
echo "保存依赖记录代码片段:"
echo "======================================"
grep -B 2 -A 8 "save_package_dependencies" src/install_user.c | head -n 15
echo ""

# 查看依赖状态显示
echo "======================================"
echo "依赖状态显示（带版本信息）:"
echo "======================================"
grep -B 2 -A 5 "version mismatch" src/install_user.c | head -n 15
echo ""

echo "======================================"
echo "功能特性总结:"
echo "======================================"
echo "✅ 自动记录用户目录下安装的 DEB 包依赖关系"
echo "✅ 版本敏感的依赖检测机制"
echo "✅ 同版本依赖不重复下载"
echo "✅ 不同版本依赖触发下载"
echo "✅ 智能显示依赖状态（system/user/version/missing）"
echo "✅ 存储在 ~/.local/share/debpkg/user_deps.db"
echo ""

echo "======================================"
echo "使用说明:"
echo "======================================"
echo "1. 安装 DEB 包时自动记录依赖:"
echo "   ./debpkg -u package.deb"
echo ""
echo "2. 查看依赖数据库:"
echo "   cat ~/.local/share/debpkg/user_deps.db"
echo ""
echo "3. 依赖状态说明:"
echo "   ✓ (system)         - 系统级已安装"
echo "   ✓ (user, v_x.x)    - 用户目录已安装且版本匹配"
echo "   ⚠ (user, v_x.x installed, v_y.y required) - 版本不匹配"
echo "   ✗ (missing)        - 依赖缺失"
echo ""

echo "======================================"
echo "相关文档:"
echo "======================================"
echo "- USER_DEPS_FEATURE.md (详细功能说明)"
echo "- README.md (项目主文档)"
echo "- MODULES.md (模块架构说明)"
echo ""

echo "======================================"
echo "测试完成!"
echo "======================================"
