import darwin
import network
import remote_display
using iostream
using network
darwin.load()
darwin.set_frame_limit(60)
var server=new remote_display.server
server.sock.set_timeout(120000)
server.init_local()
loop
    server.transfer(darwin.get_drawable())
    darwin.update_drawable()
    server.finish()
end