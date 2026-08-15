using system
# system environment/platform introspection (non-interactive, deterministic)
system.out.println(system.os_name != "")
system.out.println(system.arch_name != "")
system.out.println(system.compiler_name != "")
system.out.println(system.getenv("PATH") != "")
system.out.println(system.is_platform_linux() || system.is_platform_windows() || system.is_platform_darwin())
