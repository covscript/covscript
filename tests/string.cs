darwin.load()
var str=darwin.input_box("","Please enter a string:","",false)
var pix=darwin.pixel(' ',true,false,darwin.white,darwin.black)
loop
	darwin.fit_drawable()
	darwin.clear_drawable()
	darwin.draw_string(0.5*(darwin.get_width()-str.size()),0.5*darwin.get_height(),str,pix)
	darwin.update_drawable()
end
