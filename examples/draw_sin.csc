var fd=10
function main()
    var pic=darwin.get_drawable()
    darwin.set_frame_limit(60)
    const var pix=darwin.pixel('@',darwin.blue,darwin.white)
    var x=0
    loop
        darwin.fit_drawable()
        darwin.clear_drawable(pic)
        for i=0 to darwin.get_width(pic)-1
            darwin.draw_pixel(pic,i,0.5*(darwin.get_height(pic)+math.sin((i+x)/fd)*darwin.get_height(pic)),pix)
        end
        darwin.update_drawable()
        ++x
    end
end
darwin.load("./darwin.module")
fd=darwin.input_box("","Please input the period:",to_string(fd),true)
if(typeid fd!=typeid number)
  runtime.error("Request number.")
end
main()
