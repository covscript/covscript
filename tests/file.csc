var infs=iostream.filestream("./file.csc",iostream.openmode.in)
if !infs.good()
  runtime.error("file is not exsist.")
end
var outfs=iostream.filestream("./test.txt",iostream.openmode.out)
outfs.println("#Clone of file.csc")
while !infs.eof()
  var str=infs.getline()
  if str!=""
    system.out.println(str)
    outfs.println(str)
  end
end
