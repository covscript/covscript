# Contributing to Covariant Script

Thank you for your interest in contributing to Covariant Script! We welcome contributions from the community.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
  - [Using CovScript](#using-covscript)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Code Contributions](#code-contributions)
- [Development Setup](#development-setup)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Contributor Rights](#contributor-rights)
- [Sponsoring](#sponsoring)
- [License](#license)

## Code of Conduct

This project and everyone participating in it is expected to uphold a respectful and harassment-free environment. Please be considerate and constructive in your communications with others.

## How Can I Contribute?

### Using CovScript

The simplest way to support us is to use CovScript!
- Use CovScript in your projects
- Report issues when you encounter problems (this is a contribution too!)
- Share your experience, such as writing blog posts or sharing code examples

**Community Discussion:**
- Feel free to ask questions and discuss in [GitHub Discussions](https://github.com/covscript/covscript/discussions)
- WeChat Group: Email [mikecovlee@163.com](mailto:mikecovlee@163.com) to join
- Offline Events: We regularly organize offline technical meetups in Chengdu, China

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When creating a bug report, include as many details as possible:

- **Use a clear and descriptive title**
- **Describe the exact steps to reproduce the problem**
- **Provide specific examples** (code snippets, test cases)
- **Describe the behavior you observed** and what you expected to see
- **Include details about your environment:**
  - OS version (Windows, macOS, Linux, Android)
  - Covariant Script version
  - Compiler version (if building from source)
  - Architecture (x86, ARM, MIPS, LoongArch)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion:

- **Use a clear and descriptive title**
- **Provide a detailed description** of the proposed feature
- **Explain why this enhancement would be useful** to most Covariant Script users
- **Provide examples** of how the feature would be used
- **List any similar features** in other languages if applicable

### Code Contributions

Unsure where to begin? You can start by looking through issues labeled as:
- `good first issue` - issues suitable for newcomers
- `help wanted` - issues that need assistance

## Development Setup

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git

### Building from Source

**Recommended: Use build scripts in csbuild/** (optimized for Release builds)

#### Linux / macOS / Unix

```bash
git clone https://github.com/covscript/covscript.git
cd covscript/csbuild
./make.sh
```

#### Windows

Use the provided batch files in `csbuild/`:

```cmd
git clone https://github.com/covscript/covscript.git
cd covscript\csbuild
```

Then run the appropriate script for your target platform:
- `make_msvc_win64.bat` - 64-bit Release build (recommended)
- `make_msvc_win32.bat` - 32-bit Release build
- `make_msvc_arm64.bat` - ARM64 Release build

---

**Manual build method** (for development and debugging):

#### Linux / macOS / Unix

```bash
git clone https://github.com/covscript/covscript.git
cd covscript
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Windows (Visual Studio)

```cmd
git clone https://github.com/covscript/covscript.git
cd covscript
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### Running Tests

After building, run the test suite:

#### Linux / macOS / Unix
```bash
cd tests
./auto_test.sh
```

#### Windows
```cmd
cd tests
auto_test.bat
```

## Pull Request Process

1. **Fork the repository** and create your branch from `master`
   ```bash
   git checkout -b feature/my-new-feature
   ```

2. **Make your changes** following the coding standards

3. **Test your changes** thoroughly:
   - Add new tests for new features
   - Ensure all existing tests pass
   - Test on multiple platforms if possible

4. **Commit your changes** with clear, descriptive commit messages:
   ```bash
   git commit -m "Add feature: description of the feature"
   ```

5. **Push to your fork**:
   ```bash
   git push origin feature/my-new-feature
   ```

6. **Open a Pull Request** with:
   - A clear title and description
   - Reference to any related issues
   - Description of changes and rationale
   - Screenshots or examples if applicable

7. **Address review feedback** promptly and professionally

8. **Ensure CI/CD checks pass** (Build & Test, CodeQL)

## Coding Standards

### C++ Code Style

- Use **consistent indentation** (4 spaces or tabs as used in the project)
- Follow the existing code style in the project
- Use meaningful variable and function names
- Add comments for complex logic
- Prefer modern C++17 features where appropriate

### Formatting

Use the provided formatting scripts before committing:

**Linux/macOS:**
```bash
cd csbuild
./format.sh
```

**Windows:**
```cmd
cd csbuild
format.bat
```

### Best Practices

- **Keep commits focused**: One logical change per commit
- **Write clear commit messages**: Explain what and why, not just what
- **Update documentation**: If you change behavior, update relevant docs
- **Add tests**: New features should include tests
- **Maintain backwards compatibility**: Avoid breaking existing code when possible

### File Organization

- **Header files**: Place in `include/covscript/`
- **Source files**: Place in `sources/`
- **Tests**: Add test cases in `tests/`
- **Documentation**: Update `docs/` if needed

## Contributor Rights

When you contribute to Covariant Script:
1. **Your contributions are always attributed to you** - Your name/GitHub handle is preserved in Git history
2. **You retain copyright to your contributions** - Licensed under Apache 2.0. Major contributors will be added to the software copyright registration as co-authors during major version updates
3. **Recognition** - Contributors are listed in project history and may be acknowledged in release notes
4. **Community** - Become part of an open source project with thousands of developers

We believe in recognizing the value that community contributions bring to Covariant Script.

## Sponsoring

Interested in financially supporting Covariant Script? Learn more on our [Sponsoring page](SPONSORING.md)!

**Key Points:**
- Sponsorship funds support both project operations and contributor bonuses
- Currently accepting sponsorships from mainland China (legal requirement)
- Active contributors receive annual bonus distributions based on contribution metrics
- Financial reports are published transparently

For details, see [SPONSORING.md](SPONSORING.md).

## License

By contributing to Covariant Script, you agree that your contributions will be licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

---

## Questions?

Feel free to:
- Open an issue for discussion
- Contact the maintainers
- Join our community discussions

Thank you for contributing to Covariant Script!
