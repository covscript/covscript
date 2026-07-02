# System Environment Variables

This document describes the environment variables used by Covariant Script to locate runtime files, modules, and development tools.

## Runtime Path

**Environment Variable**: `COVSCRIPT_HOME`

The runtime path is where Covariant Script stores user-specific data and standard library modules.

**Default Locations**:

+ **Windows**: `%USERPROFILE%\Documents\CovScript` (typically `C:\Users\<username>\Documents\CovScript`)
+ **Linux & macOS**: `~/.covscript` (in the user's home directory)

**Priority**: If the `COVSCRIPT_HOME` preprocessor macro is defined at compile time, it takes precedence over the environment variable — the runtime `getenv` call is skipped entirely and the compile-time path is used. This is common in distribution packages that hardcode the install prefix.

## Import Path

**Environment Variable**: `CS_IMPORT_PATH`

The import path is a list of directories where Covariant Script searches for module files when resolving `import` statements.

Multiple paths are separated by the platform delimiter: **Windows** uses `;` (semicolon), **Linux / macOS** uses `:` (colon). This follows the same convention as the system `PATH` variable.

**Search Order** (processed sequentially, earlier = higher priority):

1. The directory containing the executing script file (prepended at runtime)
2. `.` (the current working directory; the initial default value of `import_path`)
3. `-i` / `--import-path` command-line paths (single use only; separate multiple paths with the platform delimiter)
4. `$CS_IMPORT_PATH` (user-defined import paths)
5. `$COVSCRIPT_HOME/imports` (user module directory)
6. `$COVSCRIPT_PLATFORM_HOME/imports` (system module directory)

Command-line usage:

```bash
cs -i /path/to/modules main.csc
cs -i "/path/a;/path/b" main.csc    # Windows: separate multiple paths with semicolons
cs -i "/path/a:/path/b" main.csc    # Linux / macOS: separate multiple paths with colons
```

`-i` can only be used once; to specify multiple paths, use the platform delimiter within a single value. Both `-i` paths and `$CS_IMPORT_PATH` are included in the final import path, with `-i` paths taking higher priority than `$CS_IMPORT_PATH`.

**Platform-specific Default Locations**:

+ **Windows**: `C:\Program Files (x86)\CovScript`
+ **macOS**: `/Applications/CovScript.app/Contents/MacOS/covscript`
+ **Linux**: `/usr/share/covscript`

## SDK Path (Development Path)

**Environment Variable**: `CS_DEV_PATH`

The SDK path is a CMake-level convention used by third-party extension projects to locate the CovScript SDK during their own build process. Extension projects typically reference it to find `csbuild.cmake`, headers, and other SDK resources:

```cmake
include($ENV{CS_DEV_PATH}/csbuild.cmake)
```

**Note**: This variable is **not** consumed by the CovScript interpreter or runtime. It is read by external extension build systems only.
