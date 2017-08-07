const var pic_path="./cdpf_record"
darwin.load()
darwin.fit_drawable()
var pic=darwin.get_drawable()
var ts=runtime.time()
var x_inc=true
var x=0
var y=0.5*pic.get_height()
var count=0
while runtime.time()-ts<3000
    pic.clear()
    pic.draw_pixel(x,y,darwin.pixel('@',darwin.white,darwin.blue))
    pic.save_to_file(pic_path+to_string(count)+".cdpf")
    ++count
    if x_inc
        ++x
    else
        --x
    end
    if x<=0
        x_inc=true
    end
    if x>=pic.get_width()-1
        x_inc=false
    end
end
for i=1 to count
    pic.load_from_file(pic_path+to_string(i-1)+".cdpf")
    darwin.update_drawable()
end
