#!/usr/bin/env cs

import csbuild

constant VERSION = "1.0"
var CSBUILD_RUNNING_IN_MINGW_MSYS = false

constant echo = system.out.println
constant echon = system.out.print
constant File = system.file
constant Path = system.path
constant IOStream = iostream

constant CONFIG_FILE_NAME = "csbuild.config"
constant configFilePath = [](path) -> path + Path.separator + CONFIG_FILE_NAME
constant isPathValid = [](path) -> File.exists(configFilePath(path))
constant invoke = [](name, args) -> context.solve(context.build(name))(args...)

@begin
constant fatalPathInvalid = [](path) -> (
    echo("fatal error: " + path + " is not an extension source directory"),
    echo("note: did you forget to run `csbuild init' first?"))
@end

@begin
constant print = [](...msg) -> 
    msg.size > 1 ? 
    (system.out.print(msg.front), msg.pop_front(), echo(msg...)) :
    system.out.println(msg.front)
@end

@begin
constant TEMPLATE_CONFIG = 
"import csbuild\n" + 
"\n" + 
"function csbuild_main(config)\n" + 
"    config.name = \"<extension-name>\"\n" +
"    config.author = \"<extension-author>\"\n" +
"    config.version = \"<extension-version>\"\n" +
"    config.version_code = 1\n" +
"    @begin\n" +
"    config.target(\"<target-name>\", {\"main.cpp\"})\n" + 
"    @end\n" +
"end\n"
@end

@begin
constant TEMPLATE_CMAKE_HEAD = 
"##############################################\n" +
"##############################################\n" +
"## DO NOT EDIT csbuild auto generated file. ##\n" +
"##############################################\n" +
"##############################################\n" +
"cmake_minimum_required(VERSION 3.4)\n" +
"project(CovScriptExtension)\n" +
"\n" +
"include_directories(include)\n" +
"\n" +
"if(DEFINED ENV{CS_DEV_PATH})\n" +
"    include_directories($ENV{CS_DEV_PATH}/include)\n" +
"    link_directories($ENV{CS_DEV_PATH}/lib)\n" +
"endif()\n" +
"\n" +
"if(DEFINED ENV{CS_DEV_OUTPUT})\n" +
"    set(LIBRARY_OUTPUT_PATH $ENV{CS_DEV_OUTPUT})\n" +
"    set(EXECUTABLE_OUTPUT_PATH $ENV{CS_DEV_OUTPUT})\n" +
"endif()\n" +
"\n" +
"# Compiler Options\n" +
"set(CMAKE_CXX_STANDARD 14)\n" +
"\n" +
"if (MSVC)\n" +
"    set(CMAKE_CXX_FLAGS \"/O2 /EHsc /utf-8 /w\")\n" +
"    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)\n" +
"elseif (CMAKE_COMPILER_IS_GNUCXX)\n" +
"    if (WIN32)\n" +
"        set(CMAKE_C_FLAGS \"--static -fPIC -s -O3\")\n" +
"        set(CMAKE_CXX_FLAGS \"--static -fPIC -s -O3\")\n" +
"    else ()\n" +
"        set(CMAKE_C_FLAGS \"-fPIC -s -O3\")\n" +
"        set(CMAKE_CXX_FLAGS \"-fPIC -s -O3\")\n" +
"    endif ()\n" +
"else ()\n" +
"    set(CMAKE_C_FLAGS \"-fPIC -O3\")\n" +
"    set(CMAKE_CXX_FLAGS \"-fPIC -O3\")\n" +
"endif ()\n" +
"\n\n\n" + 
"# Extension Configuration\n"
@end

function removeRecursive(path)
    foreach i in Path.scan(path)
        var name = i.name
        if name == "." || name == ".."
            continue
        end

        if i.type == Path.type.dir
            if !removeRecursive(path + Path.separator + name)
                return false
            end
        else
            if !File.remove(path + Path.separator + name)
                return false
            end
        end
    end
    return File.remove(path)
end

function loadConfig(file)
    var config = new csbuild.config
    var ns = context.source_import(configFile)
    ns.csbuild_main(config)
    return config
end

function fixPath(path)
    if path == "."
        return runtime.get_current_dir()
    end

    if system.is_platform_windows()
        # path is not an absolute path
        if path[0] != Path.separator && (path.size < 3 || (path[1] != ':' || path[2] != '\\'))
            return runtime.get_current_dir() + Path.separator + path 
        end
    else
        # path is not an absolute path
        if path[0] != Path.separator
            return runtime.get_current_dir() + Path.separator + path
        end
    end

    if path[path.size - 1] == '/' || path[path.size - 1] == '\\'
        path = path.substr(0, path.size - 1)
    end
    return path
end

function selectPath(path)
    path = fixPath(path)
    var configFile = configFilePath(path)
    var cfg = loadConfig(configFile)
    return {path, cfg}
end

function csbuild_command_gate(command, args)
    # common options come here
    var parsing = true
    while parsing && !args.empty()
        switch args.front
            # what if the source directory is named "help"?
            # we need this '--' options
            case "--"; args.pop_front(); parsing = false; end
            case "--mingw"; args.pop_front(); CSBUILD_RUNNING_IN_MINGW_MSYS = true; end
            case "--cygwin"; args.pop_front(); CSBUILD_RUNNING_IN_MINGW_MSYS = true; end
            case "--msys"; args.pop_front(); CSBUILD_RUNNING_IN_MINGW_MSYS = true; end
            case "help"; echo("No help for command `" + command + "' currently :)"); return 0; end
            default; parsing = false; end
        end
    end

    var path = args.size > 0 ? args.front : "."
    if (args.size > 0); args.pop_front(); end

    path = fixPath(path)

    if !isPathValid(path)
        fatalPathInvalid(path)
        return 1
    end

    var (newPath, cfg) = selectPath(path)

    return invoke("csbuild_" + command, {newPath, cfg, args})
end

function csbuild_help()
    @begin
    echo("CSBuild: The CovScript build system.\n" + 
        "Usage: csbuild <command> [args...]\n" + 
        "  where commands are:\n" + 
        "    init        Initialize CovScript Extension directory\n" + 
        "    check       Check build files but don't build any extension\n" + 
        "    generate    Generate build files accroding to csbuild.config\n" + 
        "    make        Build extension\n" + 
        "    run         Build and run extension in CovScript REPL\n" +
        "    push        Publish extension to public server\n" +
        "    help        Show this text\n" + 
        "    version     Display CSBuild version\n" +
        "\n" + 
        "for detailed help of a command, type `csbuild <command> help'\n");
    @end
    return 0
end

function csbuild_version()
    echo("csbuild v" + VERSION)
    return 0
end

function csbuild_init(args)
    var path = "."
    var force = false

    while !args.empty()
        var arg = args.front
        args.pop_front()
        switch arg
            case "help"; echo("No help currently :)"); return 0; end
            case "-f"; force = true; end
            default; path = arg; end
        end
    end

    path = fixPath(path)

    if !File.exists(path)
        if !File.mkdir(path)
            echo("fatal error: " + path + " does not exist and we failed to create one")
            echo("note: checking permission will usually help")
            return 1
        end
    end

    var configFile = configFilePath(path)

    if !force && File.exists(configFile)
        echo("fatal error: " + path + " has been initialized as extension source directory")
        echo("note: use -f option to force re-initialize, which leads to a clear start")
        return 1
    end

    var configFileStream = IOStream.fstream(configFile, IOStream.openmode.out)
    configFileStream.println(TEMPLATE_CONFIG);

    echo(":: Initialized " + path + " as extension source directory.")
    echo("   Start your project by editing csbuild.config!")
    return 0
end

function csbuild_check(path, cfg, args)
    echo("  > Extension Name:      " + cfg.name)
    echo("  > Extension Author:    " + cfg.author)
    echo("  > Extension Version:   " + cfg.version)
    echo("  > Extension Targets:   ")
    foreach item in cfg.targets
        var target = item.second
        echo("    > " + target->name)
        echon("        source: ")
        foreach item in target->source
            echon(item + " ")
        end
        echo("")
        echon("        libs: ")
        foreach item in target->libs
            echon(item + " ")
        end
        echo("")
        echo("        compile options: " + target->coptions)
    end
    return 0
end

function csbuild_generate_target(stream, path, target)
    var libName = target->name

    var cmakeCode = "# Extension Target: " + libName + "\n"
    var sourcePath = ""

    # Replace windows path separator with Linux standard path separator
    # see: https://github.com/covscript/csbuild/issues/2
    # see: https://stackoverflow.com/questions/13737370/cmake-error-invalid-escape-sequence-u
    if system.is_platform_windows()
        var parts = path.split({Path.separator})
        foreach item in parts
            sourcePath += item + "/"
        end
    else
        sourcePath = path + "/"
    end

    cmakeCode += "add_library(" + libName + " SHARED "
    foreach item in target->source
        cmakeCode += sourcePath + item + " ";
    end
    cmakeCode += ")\n"

    cmakeCode += "target_link_libraries(" + libName + " covscript)\n"
    cmakeCode += "target_link_libraries(" + libName + " "
    foreach item in target->libs
        cmakeCode += item + " ";
    end
    cmakeCode += ")\n"

    cmakeCode += "target_compile_options(" + libName + " PUBLIC " + target->coptions + ")\n"

    cmakeCode += "set_target_properties(" + libName + " PROPERTIES OUTPUT_NAME " + libName + ")\n"
    cmakeCode += "set_target_properties(" + libName + " PROPERTIES PREFIX \"\")\n"
    cmakeCode += "set_target_properties(" + libName + " PROPERTIES SUFFIX \".cse\")\n"
    cmakeCode += "# Extension Target End: " + libName + "\n"
    stream.println(cmakeCode)
end

function csbuild_generate(path, cfg, args)
    var genDir = path + Path.separator + "gen"
    var cmakeFile = genDir + Path.separator + "CMakeLists.txt"

    File.mkdir(genDir)
    var stream = IOStream.fstream(cmakeFile, IOStream.openmode.out)
    stream.println(TEMPLATE_CMAKE_HEAD)

    foreach item in cfg.targets
        var target = item.second
        csbuild_generate_target(stream, path, target)
    end

    echo(":: Build files have been written to " + path)
    return 0
end

function csbuild_make_check_binaries()
    var pathArray = system.getenv("PATH").split({Path.delimiter})
    var cmakeName = system.is_platform_windows() ? "cmake.exe" : "cmake"

    foreach entry in pathArray
        var cmake = entry + Path.separator + cmakeName
        if File.can_execute(cmake)
            return true
        end
    end
    return false
end

function csbuild_make_check_envs()
    try
        system.getenv("CS_DEV_PATH")
        return true
    catch ignore
        return false
    end
end

function csbuild_make_is_mingw_or_msys()
    #if !system.is_platform_windows()
    #    return false
    #end
    #
    #try
    #    var osType = system.getenv("OSTYPE").tolower()
    #    echo(":: os type = " + osType)
    #    return osType.find("msys", 0) >= 0 || osType.find("mingw", 0) >= 0 || osType.find("cygwin", 0) >= 0
    #catch ignore
    #    return false
    #end
    return CSBUILD_RUNNING_IN_MINGW_MSYS
end

function csbuild_make(path, cfg, args)
    if !csbuild_make_check_binaries()
        echo("It seems that your system lacks cmake or make. Please install them before make.")
        echo("note: did you forget to set these binaries executable?")
        return 1
    end

    if !csbuild_make_check_envs()
        echo("It seems that you have environment variable CS_DEV_PATH unset")
        echo("CovScript Extension requires that to build.")
        echo("note: set CS_DEV_PATH to /path/to/covscript-source/csdev after building CovScript")
        if system.is_platform_linux()
            echo("warning: set csdev environment to default system path.")
        else
            return 1
        end
    end

    var genDir = path + Path.separator + "gen"
    var buildDir = genDir + Path.separator + "build"
    var cmakeFile = genDir + Path.separator + "CMakeLists.txt"
    
    if !File.exists(cmakeFile)
        echo(":: Build files not found. Generating now.")
        if csbuild_generate(path, cfg, {}) != 0
            echo("\n\n:: Failed: csbuild generate")
            return 1
        end
    end

    if !File.mkdir(buildDir)
        echo("Cannot create directory" + buildDir)
        echo("\n\n:: Failed: mkdir")
        return 1
    end

    echo(":: Building in " + buildDir)

    var cmakeFlags = ""
    var cmakeBuildFlags = ""
    if csbuild_make_is_mingw_or_msys()
        echo(":: Detected we are building in MinGW/MSYS/Cygwin")
        cmakeFlags = " -G \"MinGW Makefiles\""
    else
        if system.is_platform_windows()
            cmakeFlags = " -G \"Visual Studio 15 2017 Win64\""
            cmakeBuildFlags = " --config MinSizeRel"
        end
    end

    var cmakeCommand = "cmake -H" +  genDir + " -B" + buildDir + " " + cmakeFlags
    echo(":: Running command: " + cmakeCommand)
    if system.run(cmakeCommand) != 0
        echo("\n\n:: Failed: cmake")
        return 1
    end

    var makeCommand = "cmake --build " + buildDir + cmakeBuildFlags
    echo(":: Running command: " + makeCommand)
    if system.run(makeCommand) != 0
        echo("\n\n:: Faild: make (also cmake --build)")
        return 1
    end

    echo("\n\n:: Build successful")
    return 0
end

function csbuild_clean(path, cfg, args)
    var genDir = path + Path.separator + "gen"
    if File.exists(genDir)
        return removeRecursive(genDir) ? 0 : 1
    end
    return 0
end

function csbuild_push(path, cfg, args)
    return 0
end

function main(args)
    args.pop_front()
    while !args.empty()
        var arg = args.front
        args.pop_front()
        switch arg
            case "init"; return csbuild_init(args); end
            case "check"; return csbuild_command_gate(arg, args); end
            case "generate"; return csbuild_command_gate(arg, args); end
            case "make"; return csbuild_command_gate(arg, args); end
            case "clean"; return csbuild_command_gate(arg, args); end
            case "push"; return csbuild_command_gate(arg, args); end
            case "version"; return csbuild_version(); end
            case "help"; return csbuild_help(); end
            default
                echo("csbuild: Unrecognized command: " + arg)
                echo("Type 'csbuild help' for help")
                return 1
            end
        end
    end
    csbuild_help()
    return 1
end

system.exit(main(clone(context.cmd_args)))
