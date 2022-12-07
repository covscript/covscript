function rr(k)
    return ++k
end
var vv = [](...args)->++args[0]
var rl = [](k)->++k
var sl = []()->(++i, self)
var el = []()->++i
function test(lam, args)
    var ts = runtime.time()
    lam(args...)
    return runtime.time() - ts
end
var avg_rr = 0, avg_vv = 0, avg_rl = 0, avg_sl = 0, avg_el = 0, pass = 2
system.console.clrscr()
loop
    var time = 0, i = 0
    # Regular
    foreach it in range(100000) do time += test(rr, {i})
    avg_rr += time
    foreach it in range(system.console.terminal_width()) do system.out.print(" ")
    system.console.gotoxy(0, 0)
    system.out.println("rr: " + time + ", avg: " + to_integer(avg_rr/pass))
    # Variable
    time = 0; i = 0
    foreach it in range(100000) do time += test(vv, {i})
    avg_vv += time
    foreach it in range(system.console.terminal_width()) do system.out.print(" ")
    system.console.gotoxy(0, 1)
    system.out.println("vv: " + time + ", avg: " + to_integer(avg_vv/pass))
    # Regular Lambda
    time = 0; i = 0
    foreach it in range(100000) do time += test(rl, {i})
    avg_rl += time
    foreach it in range(system.console.terminal_width()) do system.out.print(" ")
    system.console.gotoxy(0, 2)
    system.out.println("rl: " + time + ", avg: " + to_integer(avg_rl/pass))
    # Empty Argument, but refered self
    time = 0; i = 0
    foreach it in range(100000) do time += test(sl, {})
    avg_sl += time
    foreach it in range(system.console.terminal_width()) do system.out.print(" ")
    system.console.gotoxy(0, 3)
    system.out.println("sl: " + time + ", avg: " + to_integer(avg_sl/pass))
    # Empty Argument, no refered self
    time = 0; i = 0
    foreach it in range(100000) do time += test(el, {})
    avg_el += time
    foreach it in range(system.console.terminal_width()) do system.out.print(" ")
    system.console.gotoxy(0, 4)
    system.out.println("el: " + time + ", avg: " + to_integer(avg_el/pass))
    system.console.gotoxy(0, 0)
    ++pass
end