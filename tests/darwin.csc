import darwin
iostream.setprecision(12)
darwin.load()
var expr=darwin.ui.input_box("test-darwin","test1","math.sin(x)",false)
darwin.exit()
expr=context.build(expr)
var a=3
var b=4
var f=[](x)->context.solve(expr)
loop
    var c=(a+b)/2
    system.out.println(c)
    if f(a)*f(c)<0
        b=c
    else
        a=c
    end
until b-a<10^(-6)
system.out.println("Finish.Press any key to show the result...")
system.console.getch()
darwin.load()
darwin.ui.message_box("test-darwin","Result="+to_string(b),"OK")
