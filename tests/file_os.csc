constant File = system.file
constant Path = system.path

function gen_file()
    var outfs = iostream.fstream("./file_os/test/test.txt", iostream.openmode.out)
    outfs.println("hello world")
    outfs.flush()
end

if Path.mkdir_p("./file_os/test")
	system.out.println("mkdir succeed")
else
    system.out.println("mkdir failed")
    system.exit(1)
end

if Path.is_directory("./file_os/test")
	system.out.println("is_directory succeed")
end

gen_file()

if Path.chmod("./file_os/test/test.txt", "rwxr-xr-x")
	system.out.println("chmod 1 succeed")
end

if File.can_read("./file_os/test/test.txt")
	system.out.println("can_read 1 succeed")
end

if File.can_write("./file_os/test/test.txt")
	system.out.println("can_write 1 succeed")
end

if File.can_execute("./file_os/test/test.txt")
	system.out.println("can_execute 1 succeed")
end

if Path.chmod("./file_os/test/test.txt", "0000")
	system.out.println("chmod 2 succeed")
end

if !File.can_read("./file_os/test/test.txt")
	system.out.println("can_read 2 succeed")
end

if !File.can_write("./file_os/test/test.txt")
	system.out.println("can_write 2 succeed")
end

if !File.can_execute("./file_os/test/test.txt")
	system.out.println("can_execute 2 succeed")
end

if File.remove("./file_os/test/test.txt")
	system.out.println("remove succeed")
end

if File.remove("./file_os/test")
	system.out.println("remove succeed")
end

if File.remove("./file_os")
	system.out.println("remove succeed")
end




