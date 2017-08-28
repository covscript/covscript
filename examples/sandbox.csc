const var gravity=10
const var air_drop=5
const var delta_time=0.1
const var mass=0.5
const var size=0.01
var speed_x=15
var speed_y=0
var posit_x=0
var posit_y=0
function run()
	var ax=0
	var ay=0
	if(speed_x>0)
		ax=ax-(air_drop*size*speed_x)/mass
	end
	if(speed_y>0)
		ay=(mass*gravity-air_drop*size*speed_y)/mass
	end
	if(speed_y==0)
		ay=(mass*gravity)/mass
	end
	if(speed_y<0)
		ay=(mass*gravity+math.abs(air_drop*size*speed_y))/mass
	end
	posit_x=posit_x+speed_x*delta_time+0.5*ax*delta_time^2
	posit_y=posit_y+speed_y*delta_time+0.5*ay*delta_time^2
	speed_x=speed_x+ax*delta_time
	speed_y=speed_y+ay*delta_time
end

function main()
	darwin.load()
	var pic=darwin.get_drawable()
	loop
		darwin.fit_drawable()
		pic.clear()
		pic.draw_string(0,0,"Simple Sandbox",darwin.pixel(' ',darwin.black,darwin.white))
		if(posit_x<0)
			speed_x=math.abs(speed_x)
		end
		if(posit_y<0)
			speed_y=math.abs(speed_y)
		end
		if(posit_x>pic.get_width()-1)
			speed_x=-math.abs(speed_x)
		end
		if(posit_y>pic.get_height()-1)
			speed_y=-math.abs(speed_y)
		end
		run()
		pic.draw_pixel(posit_x,posit_y,darwin.pixel('@',darwin.white,darwin.black))
		darwin.update_drawable()
	end
end
main()
