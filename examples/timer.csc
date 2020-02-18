var args=context.cmd_args
args.pop_front()
var cmd=new string
foreach it in args do cmd+=args+" "
var time=runtime.time()
system.run(cmd)
system.out.println(runtime.time()-time)