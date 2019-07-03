var info = extension.get_extension_info("../../csbuild/mac_tools/build/imports/darwin.cse")

system.out.println(type(info))
system.out.println("name: " + info.name())
system.out.println("version: " + info.version())
system.out.println("author: " + info.author())
system.out.println("major: " + info.major())
system.out.println("minor: " + info.minor())
system.out.println("patch: " + info.patch())
system.out.println("build: " + info.build())

