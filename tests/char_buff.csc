var buff = new iostream.char_buff
system.out.println(type(buff))
var os = buff.get_ostream()
os.print(123.456)
os.print(" Hello")
var is = buff.get_istream()
system.out.println(is.input())
loop; until !is.get().isspace()
is.unget()
loop
    var ch = is.get()
    if !is.eof()
        system.out.put(ch)
    else
        system.out.println("")
        break
    end
end
system.out.println(buff.get_string())