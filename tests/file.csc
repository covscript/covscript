var infs=file.open("./file.csc",file.read_method)
if(!infs.is_open())
  runtime.error("file is not exsist.")
end
var outfs=file.open("./test.txt",file.write_method)
outfs.write("#Clone of file.csc","\n")
while(!infs.eof())
  var str=infs.getline()
  system.println(str)
  outfs.write(str,"\n")
end
