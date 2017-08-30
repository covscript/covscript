var fd=10
function main()
    var pic=darwin.get_drawable()
    darwin.set_frame_limit(60)
    const var pix=darwin.pixel('@',darwin.blue,darwin.white)
    var x=0
    loop
        darwin.fit_drawable()
        pic.clear()
        for i=0 to pic.get_width()-1
            pic.draw_pixel(i,0.5*(pic.get_height()+math.sin((i+x)/fd)*pic.get_height()),pix)
        end
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
