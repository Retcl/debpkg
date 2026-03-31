# Documentation Translation Plan / 文档翻译计划

## Status / 状态

| Document | English | Chinese | Priority |
|----------|---------|---------|----------|
| README | ✅ `README.md` | ✅ `README_CN.md` | 🔴 Critical |
| Quick Start | ✅ `QUICKSTART.md` | ⏳ `QUICKSTART_CN.md` | 🟠 High |
| Modules | ✅ `MODULES.md` | ⏳ `MODULES_CN.md` | 🟡 Medium |
| User Deps Feature | ✅ `USER_DEPS_FEATURE.md` | ⏳ `USER_DEPS_FEATURE_CN.md` | 🟠 High |
| Uninstall Feature | ✅ `UNINSTALL_FEATURE.md` | ⏳ `UNINSTALL_FEATURE_CN.md` | 🟠 High |
| Env Config | ✅ `ENV_CONFIG_FEATURE.md` | ⏳ `ENV_CONFIG_FEATURE_CN.md` | 🟡 Medium |
| Desktop Feature | ✅ `DESKTOP_FILE_FEATURE.md` | ⏳ `DESKTOP_FILE_FEATURE_CN.md` | 🟡 Medium |
| CLI Refactor | ✅ `CLI_REFACTOR.md` | ⏳ `CLI_REFACTOR_CN.md` | 🟢 Low |

Legend:
- ✅ Completed / 已完成
- ⏳ Pending / 待完成
- 🔴 Critical (Must have) / 关键
- 🟠 High (Should have) / 重要
- 🟡 Medium (Nice to have) / 可选
- 🟢 Low (Optional) / 低优先级

## Naming Convention / 命名规范

- English: `[FILENAME].md`
- Chinese: `[FILENAME]_CN.md`

Example:
- `README.md` → English version (primary)
- `README_CN.md` → Chinese version (translation)

## Translation Guidelines / 翻译指南

### 1. Keep Technical Terms in English

When translating technical terms, keep the English term in parentheses:

```
Desktop File → Desktop 文件 (.desktop)
Dependency → 依赖 (Dependency) 
Package → 包 (Package)
```

### 2. Code Examples Stay in English

Do NOT translate code blocks, commands, or file paths:

```bash
# ✅ Correct
./debpkg install -u package.deb

# ❌ Wrong (don't do this)
./debpkg 安装 -u 包.deb
```

### 3. Error Messages Reference English

When discussing error messages, reference the English version:

```
[ERROR] "No command specified" will appear when...
显示错误信息 `[ERROR] No command specified` 当...
```

### 4. Maintain Structure

Keep the same document structure, headings, and organization.

### 5. Mark Translation Status

At the bottom of each translated doc:

```markdown
---
**Translation Status**: ✅ Complete / 翻译完成  
**Last Updated**: 2024-XX-XX  
**Source Version**: [FILENAME].md (English)
```

## Next Actions / 下一步

1. Create Chinese versions for high-priority documents
2. Add translation status badges
3. Create cross-references between EN/CN versions
4. Set up update workflow (when EN changes, mark CN as "Needs Update")

---

Created: 2024
