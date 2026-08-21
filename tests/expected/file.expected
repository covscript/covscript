var infs=iostream.fstream("./file.csc",iostream.openmode.in)
if !infs.good()
  throw runtime.exception("file is not exist.")
end
var outfs=iostream.fstream("./test.txt",iostream.openmode.out)
outfs.println("#Clone of file.csc")
while !infs.eof()
  var str=infs.getline()
  if !str.empty()
    system.out.println(str)
    outfs.println(str)
  end
end
