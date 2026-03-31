# debpkg - DEB Package Installer (Modular Version)

A modular DEB package installer written in C, supporting both system-wide and user directory installations.

## ✨ Features

- ✅ **Modular Design** - Clean code structure, easy to maintain
- ✅ **System-wide Installation** - Install packages to system directories using `dpkg`
- ✅ **User Directory Installation** - Install to `~/.local` (no root required)
- ✅ **Smart Dependency Check** - Automatically detect and prompt for missing dependencies
- ✅ **Automatic Dependency Installation** - Auto-download and install missing dependencies from APT repositories (NEW! 🎉)
- ✅ **Interactive Dependency Handling** - Multiple dependency handling options
- ✅ **Colored Output** - Clear terminal color prompts
- ✅ **Interactive Selection** - Support for interactive installation mode selection

## 📁 Project Structure

```
debpkg/
├── include/            # Header files directory
│   ├── cli.h          # Command-line interaction module
│   ├── extract.h      # DEB package extraction module
│   ├── install_system.h  # System-wide installation module
│   ├── install_user.h    # User directory installation module
│   └── utils.h        # Utility functions module
│
├── src/              # Source code directory
│   ├── main.c        # Main program entry
│   ├── cli.c         # Command-line interaction implementation
│   ├── utils.c       # Utility functions implementation
│   ├── extract.c     # DEB package extraction implementation
│   ├── install_system.c  # System-wide installation implementation
│   └── install_user.c    # User directory installation implementation
│
├── obj/              # Compiled object files (auto-generated)
│   ├── cli.o
│   ├── extract.o
│   ├── install_system.o
│   ├── install_user.o
│   ├── main.o
│   └── utils.o
│
├── Makefile          # Build configuration
├── README.md         # This file (English)
├── README_CN.md      # Chinese documentation
├── MODULES.md        # Detailed architecture documentation
├── QUICKSTART.md     # Quick start guide
├── CHANGES.md        # Change log
└── test.sh           # Test script
```

**Directory Description**:
- **include/** - All header files (.h), containing public interface declarations for modules
- **src/** - All source files (.c), containing specific implementations of modules
- **obj/** - Compiled object files, automatically created by Makefile

For detailed module documentation, see [MODULES.md](MODULES.md)

## 🔧 Building

### Quick Installation (Recommended)

Use the automated installation script:

```bash
./install.sh
```

This script will:
- Check system dependencies (gcc, make)
- Clean old build artifacts
- Compile the project
- Provide interactive installation options

#### Installation Modes

```bash
# Interactive mode (default)
./install.sh

# System-wide installation to /usr/local/bin (requires sudo)
./install.sh --system

# User-local installation to ~/.local/bin (no sudo required)
./install.sh --user

# Build only without installation
./install.sh --build

# Clean build artifacts only
./install.sh --clean

# Show help
./install.sh --help
```

### Manual Build

#### Basic Build

```bash
make
```

This will compile all modules and generate the `debpkg` executable.

#### Other Build Options

```bash
make clean      # Clean build files
make install    # Install to /usr/local/bin
make uninstall  # Remove from /usr/local/bin
```

### Build Requirements

- GCC compiler (C99 standard support)
- Make build tool
- Linux/Debian system

### Creating Debian Package

Use the automated packaging script:

```bash
./package.sh
```

This will compile the project and create a `.deb` package ready for installation.

#### Packaging Modes

```bash
# Full process: compile + package
./package.sh

# Clean build directory and old packages
./package.sh --clean

# Compile only
./package.sh --build

# Package only (assumes already compiled)
./package.sh --package

# Verify existing package
./package.sh --verify

# Show help
./package.sh --help
```

**Output:** `debpkg_<VERSION>_amd64.deb`

For detailed packaging documentation, see [PACKAGE_GUIDE.md](PACKAGE_GUIDE.md)

## 📖 Usage

### Basic Syntax

```bash
./debpkg <COMMAND> [OPTIONS] <ARGUMENT>
```

### Commands

| Command | Description | Notes |
|---------|-------------|-------|
| `install <package.deb>` | Install a DEB package | See install options below |
| `uninstall <package-name>` | Uninstall from user directory | Remove package and cleanup |

### Install Options

| Option | Function | Description |
|--------|----------|-------------|
| `-s, --system` | System-wide install | Uses dpkg, requires sudo |
| `-u, --user` | User directory install | Install to ~/.local, no root |
| `-h, --help` | Show help | View usage information |

### Uninstall Options

| Option | Function | Description |
|--------|----------|-------------|
| `-h, --help` | Show help | View usage information |

### Global Options

| Option | Function | Description |
|--------|----------|-------------|
| `-h, --help` | Show global help | View overall help |
| `-v, --version` | Show version | View version info |

### Examples

#### 1. System-wide Installation

```bash
sudo ./debpkg install -s package.deb
```

Requires sudo privileges, installs to `/usr/bin`, `/usr/lib`, etc.

#### 2. User Directory Installation (Recommended)

```bash
./debpkg install -u package.deb
```

No root required, installs to `~/.local/bin`, `~/.local/lib`, etc.

#### 3. Interactive Installation

```bash
./debpkg install package.deb
```

Prompts you to choose installation method interactively.

#### 4. Uninstall Package

```bash
./debpkg uninstall package-name
# or
./debpkg remove package-name
```

Intelligently checks dependencies before removal.

#### 5. View Help

```bash
# Global help
./debpkg --help

# Install command help
./debpkg install --help

# Uninstall command help
./debpkg uninstall --help
```

## 🎯 Advanced Features

### Automatic Dependency Installation

When installing to user directory, automatically detects and installs missing dependencies:

```bash
$ ./debpkg install -u myapp.deb

[INFO] Checking dependencies...
[WARNING] Missing dependencies detected:
  - libfoo>=1.0.0
  - libbar>=2.0.0

[INFO] Dependency handling options:
  1. Auto-download and install (RECOMMENDED)
  2. Install manually
  3. Continue anyway
  4. Cancel

Enter your choice [1-4]: 1

[INFO] Starting automatic dependency installation...
[SUCCESS] All dependencies installed successfully!
```

### Smart Uninstall with Dependency Protection

When uninstalling, intelligently protects dependencies still in use:

```bash
$ ./debpkg uninstall myapp

[INFO] Uninstalling package: myapp
[INFO] Checking dependencies...
  Keeping libfoo (used by 1 other package(s))
  Removing unused dependency: libbar

[SUCCESS] Uninstall completed!
Dependencies kept: 1
Dependencies removed: 1
```

### Desktop File Integration

Automatically creates desktop entries for GUI applications:

```bash
./debpkg install -u gui-app.deb

# Creates: ~/.local/share/applications/gui-app.desktop
```

### Environment Variable Configuration

Automatically prompts to configure environment variables:

```bash
[INFO] Checking environment configuration...
[WARNING] Environment variables may need configuration!

Do you want to generate an environment configuration script? (y/n): y

[SUCCESS] Environment script created: ~/.local/debpkg_env.sh

To activate: source ~/.local/debpkg_env.sh
```

## 📊 Installation Locations

### System-wide Installation (`-s`)

| Type | Location | Requires |
|------|----------|----------|
| Executables | `/usr/bin/` | sudo |
| Libraries | `/usr/lib/` | sudo |
| Data | `/usr/share/<pkg>/` | sudo |
| Config | `/etc/<pkg>/` | sudo |
| Man pages | `/usr/share/man/` | sudo |

### User Directory Installation (`-u`)

| Type | Location | Requires |
|------|----------|----------|
| Executables | `~/.local/bin/` | None |
| Libraries | `~/.local/lib/` | None |
| Data | `~/.local/share/<pkg>/` | None |
| Config | `~/.local/etc/<pkg>/` | None |
| Man pages | `~/.local/share/man/` | None |
| Desktop file | `~/.local/share/applications/` | None |
| Dependencies | Recorded in database | None |

## 🌍 Documentation

This project provides bilingual documentation:

- **🇬🇧 English**: [README.md](README.md) (this file - **Primary Version**)
- **🇨🇳 中文**: [README_CN.md](README_CN.md)

### Additional Documentation / 更多文档

For complete documentation list, see [DOCS.md](DOCS.md).

**Features / 功能文档**:
- [USER_DEPS_FEATURE.md](USER_DEPS_FEATURE.md) - Dependency management
- [UNINSTALL_FEATURE.md](UNINSTALL_FEATURE.md) - Smart uninstall
- [ENV_CONFIG_FEATURE.md](ENV_CONFIG_FEATURE.md) - Environment configuration
- [DESKTOP_FILE_FEATURE.md](DESKTOP_FILE_FEATURE.md) - Desktop integration

**Technical / 技术文档**:
- [MODULES.md](MODULES.md) - Module architecture
- [CLI_REFACTOR.md](CLI_REFACTOR.md) - CLI design
- [CHANGES.md](CHANGES.md) - Change history
- [QUICKSTART.md](QUICKSTART.md) - Quick start guide

## ⚠️ Notes

### User Directory Installation

- No root privileges required
- Ideal for development testing
- Does not affect system packages
- Supports multiple versions coexistence

### System-wide Installation

- Requires sudo privileges
- Suitable for production environments
- May conflict with system packages
- Use with caution

## 🔒 Security Recommendations

1. **Verify Package Sources**
   - Only install DEB packages from trusted sources
   - Verify package signatures when possible

2. **Review Before Installing**
   - Check package contents before installation
   - Understand what files will be installed

3. **Use User Directory When Possible**
   - Prefer `-u` option for testing
   - Limits potential damage from malicious packages

## 🐛 Troubleshooting

### Issue: "Permission denied"

**Solution**: Use user directory installation
```bash
./debpkg install -u package.deb
```

### Issue: "Dependency missing"

**Solution**: Use automatic dependency installation
```bash
# Choose option 1 when prompted
```

### Issue: "Command not found" after install

**Solution**: Add `~/.local/bin` to PATH
```bash
export PATH=$HOME/.local/bin:$PATH
```

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## 📄 License

This project is licensed under the GPL v2 License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

Thanks to all contributors and the open-source community!

---

**Version**: 1.0.0  
**Language**: English  
**Last Updated**: 2024
