define infs=file.open("./test_file.cs",file.read_method)
if(!file.is_open(infs))
  runtime.error("file is not exsist.")
end
define outfs=file.open("./test.txt",file.write_method)
file.write(outfs,"#Clone of test_file.cs","\n")
while(!file.eof(infs))
  define str=file.getline(infs)
  system.println(str)
  file.write(outfs,str,"\n")
end
