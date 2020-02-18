import darwin
darwin.load()
var pic=darwin.get_drawable()
var expr=context.build(darwin.ui.input_box("Sketchpad","Please enter a expression:","",false))
var x_offset=0
var y_offset=0
var zoom=5
var dzoom=1
var center_x=0
var center_y=0
constant pix=darwin.pixel('+',darwin.blue,darwin.black)
function f(x)
  return context.solve(expr)
end
loop
  darwin.fit_drawable()
  pic.clear()
  if(darwin.is_kb_hit())
    switch darwin.get_kb_hit()
			case 'x'
				system.exit(0)
			end
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
  center_x=0.5*pic.get_width()
  center_y=0.5*pic.get_height()
  for i=0,i<=pic.get_width()-2,++i
    pic.draw_line(i,center_y-f((i-center_x-x_offset)/(2*zoom))*zoom+y_offset,i+1,center_y-f((i+1-center_x-x_offset)/(2*zoom))*zoom+y_offset,pix)
  end
  pic.draw_line(0,center_y+y_offset,pic.get_width()-1,center_y+y_offset,darwin.pixel('-',darwin.white,darwin.black))
  pic.draw_line(center_x+x_offset,0,center_x+x_offset,pic.get_height()-1,darwin.pixel('|',darwin.white,darwin.black))
  pic.draw_pixel(center_x+x_offset,center_y+y_offset,darwin.pixel('+',darwin.white,darwin.black))
  pic.draw_pixel(center_x+x_offset,0,darwin.pixel('^',darwin.white,darwin.black))
  pic.draw_pixel(pic.get_width()-1,center_y+y_offset,darwin.pixel('>',darwin.white,darwin.black))
  var i=-to_integer((center_x+x_offset)/(zoom*2))
  while(center_x+x_offset+2*i*zoom<pic.get_width()-1)
  	if(i!=0)
  		pic.draw_pixel(center_x+x_offset+2*i*zoom,center_y+y_offset,darwin.pixel('|',darwin.white,darwin.black))
  		var str=to_string(i)
  		pic.draw_string(center_x+x_offset+2*i*zoom-0.5*str.size,center_y+y_offset+1,str,darwin.pixel(' ',darwin.white,darwin.black))
	end
	++i
  end
  i=to_integer((center_y-y_offset)/zoom)
  while(center_y+y_offset+i*zoom>0)
	if(i!=0)
  		pic.draw_pixel(center_x+x_offset,center_y+y_offset+i*zoom,darwin.pixel('-',darwin.white,darwin.black))
  		var str=to_string(-i)
  		pic.draw_string(center_x+x_offset-str.size,center_y+y_offset+i*zoom,str,darwin.pixel(' ',darwin.white,darwin.black))
	end
	--i
  end
  pic.draw_string(0,0,"Sketchpad",darwin.pixel(' ',darwin.black,darwin.white))
  darwin.update_drawable()
end
