var infs=file.open("./test_file.cs",file.read_method)
if(!file.is_open(infs))
  runtime.error("file is not exsist.")
end
var outfs=file.open("./test.txt",file.write_method)
file.write(outfs,"#Clone of test_file.cs","\n")
while(!file.eof(infs))
  var str=file.getline(infs)
  system.println(str)
  file.write(outfs,str,"\n")
end
