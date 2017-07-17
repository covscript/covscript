darwin.load("./darwin.module")
var pic=darwin.get_drawable()
var expr=runtime.build(darwin.input_box("Sketchpad","Please enter a expression:","",false))
var x_offset=0
var y_offset=0
var zoom=5
var dzoom=1
var center_x=0
var center_y=0
const var pix=darwin.pixel('+',darwin.blue,darwin.black)
function f(x)
  return runtime.solve(expr)
end
loop
  darwin.fit_drawable()
  darwin.clear_drawable(pic)
  if(darwin.is_kb_hit())
    switch darwin.get_kb_hit()
    	case 'w'
			++y_offset
    	end
    	case 's'
    	  --y_offset
    	end
    	case 'a'
    	  ++x_offset
    	end
    	case 'd'
    	  --x_offset
    	end
    	case 'q'
    	  zoom=zoom+dzoom
    	end
    	case 'e'
    		if(zoom>2)
    			zoom=zoom-dzoom
    		end
    	end
    end
  end
  center_x=0.5*darwin.get_width(pic)
  center_y=0.5*darwin.get_height(pic)
  for i=0 to darwin.get_width(pic)-2
    darwin.draw_line(pic,i,center_y-f((i-center_x-x_offset)/(2*zoom))*zoom+y_offset,i+1,center_y-f((i+1-center_x-x_offset)/(2*zoom))*zoom+y_offset,pix)
  end
  darwin.draw_line(pic,0,center_y+y_offset,darwin.get_width(pic)-1,center_y+y_offset,darwin.pixel('-',darwin.white,darwin.black))
  darwin.draw_line(pic,center_x+x_offset,0,center_x+x_offset,darwin.get_height(pic)-1,darwin.pixel('|',darwin.white,darwin.black))
  darwin.draw_pixel(pic,center_x+x_offset,center_y+y_offset,darwin.pixel('+',darwin.white,darwin.black))
  darwin.draw_pixel(pic,center_x+x_offset,0,darwin.pixel('^',darwin.white,darwin.black))
  darwin.draw_pixel(pic,darwin.get_width(pic)-1,center_y+y_offset,darwin.pixel('>',darwin.white,darwin.black))
  var i=-to_integer((center_x+x_offset)/(zoom*2))
  while(center_x+x_offset+2*i*zoom<darwin.get_width(pic)-1)
  	if(i!=0)
  		darwin.draw_pixel(pic,center_x+x_offset+2*i*zoom,center_y+y_offset,darwin.pixel('|',darwin.white,darwin.black))
  		var str=to_string(i)
  		darwin.draw_string(pic,center_x+x_offset+2*i*zoom-0.5*str.size(),center_y+y_offset+1,str,darwin.pixel(' ',darwin.white,darwin.black))
	end
	++i
  end
  var i=to_integer((center_y-y_offset)/zoom)
  while(center_y+y_offset+i*zoom>0)
	if(i!=0)
  		darwin.draw_pixel(pic,center_x+x_offset,center_y+y_offset+i*zoom,darwin.pixel('-',darwin.white,darwin.black))
  		var str=to_string(-i)
  		darwin.draw_string(pic,center_x+x_offset-str.size(),center_y+y_offset+i*zoom,str,darwin.pixel(' ',darwin.white,darwin.black))
	end
	--i
  end
  darwin.draw_string(pic,0,0,"Sketchpad",darwin.pixel(' ',darwin.black,darwin.white))
  darwin.update_drawable()
end
