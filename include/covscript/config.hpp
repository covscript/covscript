#pragma once

#if defined(__unix__) || defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#   ifndef CONFIG_CS_HOME
#       define CONFIG_CS_HOME "/usr/share/covscript"
#   endif
#   define CONFIG_PATH_SEPARATOR "/"
#elif defined(_MSC_VER)
#   ifndef CONFIG_CS_HOME
#       define CONFIG_CS_HOME "<TODO: Replace this>"
#   endif
#   define CONFIG_PATH_SEPARATOR "\\"
#endif

#define CONFIG_CS_IMPORT_PATH (CONFIG_CS_HOME CONFIG_PATH_SEPARATOR "imports")


