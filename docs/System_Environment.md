# Environment Variables

## Runtime Path

Runtime Path=`$COVSCRIPT_HOME`

Default:

+ Windows: `$USERPROFILE/Documents/CovScript`
+ Linux & macOS: `$HOME/.covscript`

## Import Path

Import Path=`.;$CS_IMPORT_PATH;$COVSCRIPT_HOME/imports;$COVSCRIPT_PLATFORM_HOME/imports`

COVSCRIPT_PLATFORM_HOME:

+ Windows: `C:\Program Files (x86)\CovScript`
+ macOS: `/Applications/CovScript.app/Contents/MacOS/covscript`
+ Linux: `/usr/share/covscript`

## SDK Path

SDK Path=`$CS_DEV_PATH`
