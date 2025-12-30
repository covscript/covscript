# System Environment Variables

This document describes the environment variables used by Covariant Script to locate runtime files, modules, and development tools.

## Runtime Path

The runtime path is where Covariant Script stores user-specific data and standard library modules.

**Environment Variable**: `COVSCRIPT_HOME`

**Default Locations**:

+ **Windows**: `%USERPROFILE%\Documents\CovScript` (typically `C:\Users\<username>\Documents\CovScript`)
+ **Linux & macOS**: `~/.covscript` (in the user's home directory)

## Import Path

The import path is a list of directories where Covariant Script searches for module files when resolving `import` and `using` statements.

**Environment Variable**: `CS_IMPORT_PATH`

**Search Order** (processed sequentially):

1. `.` (current working directory)
2. `$CS_IMPORT_PATH` (user-defined import paths)
3. `$COVSCRIPT_HOME/imports` (user module directory)
4. `$COVSCRIPT_PLATFORM_HOME/imports` (system module directory)

**Platform-specific Default Locations**:

+ **Windows**: `C:\Program Files (x86)\CovScript`
+ **macOS**: `/Applications/CovScript.app/Contents/MacOS/covscript`
+ **Linux**: `/usr/share/covscript`

## SDK Path (Development Path)

The SDK path is used for Covariant Script development and extension compilation.

**Environment Variable**: `CS_DEV_PATH`

This path is used to locate SDK resources, headers, and build tools required for developing native extensions in C/C++.
