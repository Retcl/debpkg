# Documentation Structure / 文档结构

🌍 **This project provides bilingual documentation. English is the primary version.**

本项目提供双语文档。**英文为主版本**。

---

## 📚 Core Documentation / 核心文档

### Main Guides / 主指南

| Document | English 🇬🇧 | Chinese 🇨🇳 | Status |
|----------|--------------|--------------|--------|
| **README** | [README.md](README.md) | [README_CN.md](README_CN.md) | ✅ Complete |
| **Quick Start** | [QUICKSTART.md](QUICKSTART.md) | _Coming soon_ | 🟡 Partial |
| **Modules** | [MODULES.md](MODULES.md) | _Coming soon_ | 🟡 Partial |

### Feature Documentation / 功能文档

| Feature | English 🇬🇧 | Chinese 🇨🇳 | Status |
|---------|--------------|--------------|--------|
| **Dependency Management** | [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md) | _Coming soon_ | 🟡 Partial |
| **Uninstall System** | [UNINSTALL_FEATURE.md](UNINSTALL_FEATURE.md) | _Coming soon_ | 🟡 Partial |
| **Environment Config** | [ENV_CONFIG_FEATURE.md](ENV_CONFIG_FEATURE.md) | _Coming soon_ | 🟡 Partial |
| **Desktop Integration** | [DESKTOP_FILE_FEATURE.md](DESKTOP_FILE_FEATURE.md) | _Coming soon_ | 🟡 Partial |

### Technical Documentation / 技术文档

| Topic | English 🇬🇧 | Chinese 🇨🇳 | Status |
|-------|--------------|--------------|--------|
| **CLI Design** | [CLI_REFACTOR.md](CLI_REFACTOR.md) | _Coming soon_ | 🟡 Partial |
| **Changes** | [CHANGES.md](CHANGES.md) | _Coming soon_ | 🟡 Partial |

---

## 📋 File Organization / 文件组织

```
debpkg/
│
├── 📖 README.md              # Main English documentation (PRIMARY VERSION)
├── 📖 README_CN.md           # Chinese translation (参考翻译)
├── 📑 DOCS.md                # This file - Documentation index
│
├── 🚀 QUICKSTART.md          # Quick start guide (EN)
├── 🏗️  MODULES.md             # Module architecture (EN)
│
├── 🔗 USER_DEPS_FEATURE.md   # Dependency management (EN)
├── 🗑️  UNINSTALL_FEATURE.md   # Uninstall system (EN)
├── ⚙️  ENV_CONFIG_FEATURE.md  # Environment config (EN)
├── 🖥️  DESKTOP_FILE_FEATURE.md # Desktop integration (EN)
│
├── 💻 CLI_REFACTOR.md        # CLI design (EN)
├── 📝 CHANGES.md             # Change history (EN)
│
└── [... more CN translations coming soon ...]
```

---

## 🎯 Translation Status / 翻译状态 Legend

### Status Indicators / 状态标识

- ✅ **Complete / 已完成** - Both EN and CN available
- 🟡 **Partial / 部分完成** - EN available, CN in progress
- ⏳ **Coming soon / 即将推出** - Planned for translation
- 🔴 **Critical / 关键** - Must-have documentation
- 🟠 **High / 重要** - Should-have documentation
- 🟢 **Low / 低优** - Nice-to-have documentation

---

## ℹ️ Usage Guidelines / 使用指南

### Which Version to Use? / 使用哪个版本？

**English Version (Primary)**:
- ✅ Always up-to-date
- ✅ Authoritative reference
- ✅ Recommended for accuracy

**Chinese Version (Translation)**:
- 🔄 For convenience
- 🔄 May lag behind updates
- 🔄 Reference only

**Recommendation**: Use English version when possible for technical accuracy.

**建议**：可能时使用英文版本以确保技术准确性。

### Cross-Reference Format / 交叉引用格式

When citing in issues or PRs:
```markdown
See: [README.md#usage](README.md#usage) (English)
或见：[README_CN.md#使用方法](README_CN.md#使用方法)（中文）
```

---

## 🔧 For Contributors / 贡献者指南

### Adding New Features / 添加新功能

1. Create English documentation first: `FEATURE_NAME.md`
2. Update `DOCS.md` index with new feature
3. Mark Chinese translation as "Coming soon"

### Contributing Translations / 贡献翻译

We welcome help with Chinese translations!

我们欢迎帮助中文翻译！

**Steps / 步骤**:

1. Check if `[FILENAME]_CN.md` exists
2. If not, create it based on latest English version
3. Add translation status at bottom:
   ```markdown
   ---
   **Translation Status**: ✅ Complete / 翻译完成  
   **Last Updated**: 2024-XX-XX  
   **Source Version**: [FILENAME].md (English)
   **Translator**: Your Name / GitHub ID
   ```
4. Update `DOCS.md` to reflect new translation
5. Submit pull request

**Translation Guidelines / 翻译指南**:

- Keep technical terms in English with Chinese explanation
  - e.g., "Desktop File (.desktop 文件)", "Dependency (依赖)"
- Do NOT translate code blocks or commands
  - ✅ `./debpkg install package.deb`
  - ❌ `./debpkg 安装 包.deb`
- Maintain same document structure
- Mark clearly if translation may be outdated

---

## 📅 Translation Roadmap / 翻译路线图

### Phase 1: Core Docs (Completed ✅)
- [x] README.md → README_CN.md

### Phase 2: Feature Docs (In Progress 🟡)
- [ ] QUICKSTART_CN.md
- [ ] USER_DEPS_FEATURE_CN.md
- [ ] UNINSTALL_FEATURE_CN.md

### Phase 3: Technical Docs (Planned 🟢)
- [ ] MODULES_CN.md
- [ ] ENV_CONFIG_FEATURE_CN.md
- [ ] DESKTOP_FILE_FEATURE_CN.md
- [ ] CLI_REFACTOR_CN.md

---

## 🔗 External Links / 外部链接

- [debpkg GitHub Repository](https://github.com/yourusername/debpkg)
- [Contributing Guide](CONTRIBUTING.md) (Coming soon)
- [Issue Tracker](https://github.com/yourusername/debpkg/issues)

---

## 📞 Questions / 问题

For questions about documentation or translations:

- Open an issue on GitHub
- Use English or Chinese in your issue
- Tag with `documentation` or `translation`

关于文档或翻译的问题：

- 在 GitHub 上提交 issue
- 使用英文或中文描述问题
- 添加 `documentation` 或 `translation` 标签

---

**Last Updated**: 2024  
**Primary Language**: English (主语言：英文)  
**Documentation Maintainer**: debpkg Authors  
**Translation Coordinator**: Community-driven

---

<div align="center">

**🌍 Bilingual Documentation for debpkg**

English | 中文

[Back to README.md](README.md) • [返回 README_CN.md](README_CN.md)

</div>
