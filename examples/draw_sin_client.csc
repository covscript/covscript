import darwin
import network
import remote_display
using iostream
using network
var fd=10
var client=new remote_display.client
client.connect_local()
function main()
    var pic=darwin.get_drawable()
    darwin.set_frame_limit(60)
    constant pix=darwin.pixel('@',darwin.blue,darwin.white)
    var x=0
    loop
        if darwin.is_kb_hit()
            switch darwin.get_kb_hit()
                case 'q'
                    system.exit(0)
                end
            end
        end
        darwin.fit_drawable()
        pic.clear()
        for i=0,i<pic.get_width(),++i
            pic.draw_pixel(i,0.5*(pic.get_height()+math.sin((i+x)/fd)*pic.get_height()),pix)
        end
        client.transfer(pic)
        darwin.update_drawable()
        ++x
    end
end
darwin.load()
fd=darwin.ui.input_box("","Please input the period:",to_string(fd),true)
if(typeid fd!=typeid number)
  throw runtime.exception("Request number.")
end
main()
