constant gravity=10
constant air_drop=5
constant delta_time=0.1
constant mass=0.5
constant size=0.01
define speed_x=15
define speed_y=0
define posit_x=0
define posit_y=0
function run()
	define ax=0
	define ay=0
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
	loop
		darwin.fit_drawable()
		darwin.clear_drawable()
		darwin.draw_string(0,0,"Simple Sandbox",darwin.pixel(' ', true,false, darwin.black, darwin.white))
		if(posit_x<0)
			speed_x=math.abs(speed_x)
		end
		if(posit_y<0)
			speed_y=math.abs(speed_y)
		end
		if(posit_x>darwin.get_width()-1)
			speed_x=-math.abs(speed_x)
		end
		if(posit_y>darwin.get_height()-1)
			speed_y=-math.abs(speed_y)
		end
		run()
		darwin.draw_point(posit_x,posit_y,darwin.pixel('@', true,false, darwin.white, darwin.black))
		darwin.update_drawable()
	end
end
main()
