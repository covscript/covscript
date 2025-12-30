# 支持 Covariant Script

感谢您对 Covariant Script 的支持！我们欢迎来自社区任何形式的贡献。

## 目录

- [行为准则](#行为准则)
- [如何支持](#如何支持)
  - [使用 CovScript](#使用-covscript)
  - [报告 Bug](#报告-bug)
  - [提出功能建议](#提出功能建议)
  - [代码贡献](#代码贡献)
  - [帮助宣传](#帮助宣传)
- [开发环境配置](#开发环境配置)
- [Pull Request 流程](#pull-request-流程)
- [编码规范](#编码规范)
- [贡献者的那些事](#贡献者的那些事)
- [赞助项目](#赞助项目)
- [许可证](#许可证)

## 行为准则

本项目及其所有参与者都应维护一个相互尊重、无骚扰的环境。请在与他人交流时保持体贴和建设性。

## 如何支持

### 使用 CovScript

最简单的支持方式就是使用 CovScript！
- 在你的项目中使用 CovScript
- 遇到问题时反馈给我们（这本身也是贡献）
- 分享你的使用经验，比如写博客、分享代码例子

**社区讨论：**
- 在 [GitHub Discussions](https://github.com/covscript/covscript/discussions) 中随时提问和讨论
- 微信群：请发邮件至 [mikecovlee@163.com](mailto:mikecovlee@163.com) 申请加入
- 线下活动：我们在成都定期组织线下技术交流活动

### 报告 Bug

在创建 Bug 报告之前，请检查现有的 issue 以避免重复。创建 Bug 报告时，请尽可能包含详细信息：

- **用清晰的标题** 说明问题
- **描述重现步骤** 让我们能复现问题
- **提供具体例子** 代码片段、测试用例等
- **说明预期和实际表现** 你期望什么，实际发生了什么
- **环境信息**：
  - 操作系统版本（Windows、macOS、Linux、Android）
  - Covariant Script 版本
  - 编译器版本（如果从源码构建）
  - 架构（x86、ARM、MIPS、LoongArch）

### 提出功能建议

功能建议通过 GitHub issue 进行跟踪。创建功能建议时：

- **用清晰的标题** 说明建议的功能
- **详细描述** 你想要什么功能
- **解释意义** 为什么这个功能对大多数用户有用
- **提供例子** 功能应该如何使用
- **对标功能**（可选）列出其他语言中的类似功能

### 代码贡献

不确定从哪里开始？你可以从以下标签的 issue 开始：
- `good first issue` - 适合新手的问题
- `help wanted` - 需要帮助的问题

### 帮助宣传

你也可以通过以下方式帮助传播 CovScript 的声音：
- 在 GitHub 上给我们 star
- 推荐给感兴趣的朋友
- 写文章或教程介绍 CovScript
- 在社区或会议上分享你的经验
- 制作视频教程或演示

## 开发环境配置

### 前置要求

- CMake 3.10 或更高版本
- 兼容 C++17 的编译器（GCC 7+、Clang 5+、MSVC 2017+）
- Git

### 从源码构建

**推荐方式：使用 csbuild/ 中的构建脚本**（已针对 Release 构建优化）

#### Linux / macOS / Unix

```bash
git clone https://github.com/covscript/covscript.git
cd covscript/csbuild
./make.sh
```

#### Windows

使用 `csbuild/` 目录下提供的批处理文件：

```cmd
git clone https://github.com/covscript/covscript.git
cd covscript\csbuild
```

然后根据目标平台运行相应脚本：
- `make_msvc_win64.bat` - 64 位 Release 构建（推荐）
- `make_msvc_win32.bat` - 32 位 Release 构建
- `make_msvc_arm64.bat` - ARM64 Release 构建

---

**手动构建方式**（适用于开发和调试）：

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

### 运行测试

构建完成后，运行测试套件：

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

## Pull Request 流程

1. **Fork 仓库**并从 `master` 创建您的分支
   ```bash
   git checkout -b feature/my-new-feature
   ```

2. **进行更改**，遵循编码规范

3. **彻底测试您的更改**：
   - 为新功能添加新测试
   - 确保所有现有测试通过
   - 如果可能，在多个平台上测试

4. **提交您的更改**，使用清晰、描述性的提交信息：
   ```bash
   git commit -m "添加功能：功能描述"
   ```

5. **推送到您的 Fork**：
   ```bash
   git push origin feature/my-new-feature
   ```

6. **开启 Pull Request**，包含：
   - 清晰的标题和描述
   - 引用相关的 issue
   - 更改的描述和理由
   - 截图或示例（如果适用）

7. **及时、专业地回应审查反馈**

8. **确保 CI/CD 检查通过**（构建与测试、CodeQL）

## 编码规范

### C++ 代码风格

- 使用**一致的缩进**（项目中使用的 4 个空格或制表符）
- 遵循项目中现有的代码风格
- 使用有意义的变量和函数名称
- 为复杂逻辑添加注释
- 在适当的地方优先使用现代 C++17 特性

### 格式化

在提交之前使用提供的格式化脚本：

**Linux/macOS：**
```bash
cd csbuild
./format.sh
```

**Windows：**
```cmd
cd csbuild
format.bat
```

### 最佳实践

- **专注提交**：每次只提交一个逻辑更改
- **清晰的提交信息**：说清楚改了什么、为什么改，不只是改了什么
- **更新文档**：改了行为就记得更新相关文档
- **加测试**：新功能应该有测试
- **保持兼容**：尽量不要破坏现有代码

### 文件组织

- **头文件**：放置在 `include/covscript/`
- **源文件**：放置在 `sources/`
- **测试**：在 `tests/` 中添加测试用例
- **文档**：如果需要，更新 `docs/`

## 贡献者的那些事

向 Covariant Script 贡献代码时，你可以得到：
1. **永久署名** - 你的名字/GitHub 账号会被保留在 Git 历史中
2. **著作权归你** - 按 Apache License 2.0 授权，贡献较多者将在大版本更新时增加到软件著作权作者中
3. **应有的认可** - 贡献者列在项目历史里，重要贡献可能在发布说明中被提到
4. **加入社区** - 成为数千名开发者共同维护的项目的一部分

我们真诚地感谢每一个为 Covariant Script 贡献力量的人！

## 赞助项目

有想法赞助 Covariant Script 吗？请来看看我们的[赞助页面](SPONSORING-zh.md)！

**核心信息：**
- 赞助资金用于项目运营和贡献者分红
- 目前仅接受来自中国大陆的赞助（法律要求）
- 活跃贡献者将按年度根据贡献度获得分红
- 财务情况完全透明公开

详情见 [SPONSORING-zh.md](SPONSORING-zh.md)。

## 许可证

通过为 Covariant Script 贡献代码，你同意你的贡献将在 Apache License 2.0 下授权。详见 [LICENSE](LICENSE) 文件。

---

## 有问题？

欢迎：
- 开启 issue 进行讨论
- 联系维护者
- 加入我们的社区讨论

感谢您对 Covariant Script 的支持！
