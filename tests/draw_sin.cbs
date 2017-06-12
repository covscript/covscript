define fd=10
function main()
    darwin.set_frame_limit(60)
    constant pix=darwin.pixel('@',true,false,darwin.blue,darwin.white)
    darwin.fit_drawable()
    darwin.clear_drawable()
    define x=0
    loop
        darwin.fit_drawable()
        darwin.clear_drawable()
        for i=0 to darwin.get_width()-1
            darwin.draw_point(i,0.5*(darwin.get_height()+math.sin((i+x)/fd)*darwin.get_height()),pix)
        end
        darwin.update_drawable()
        ++x
    end
end
darwin.load()
fd=darwin.input_box("","Please input the period:",to_string(fd),true)
if(!is_number(fd))
  runtime.error("Request number.")
end
main()
