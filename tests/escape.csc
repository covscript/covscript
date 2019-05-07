var str="\t\"hello\"\n\tworld"
foreach ch in str
  if(ch=='\t')
    system.out.println("Boom!")
  end
  system.out.print(ch)
end
system.out.println("")
