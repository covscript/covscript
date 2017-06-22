constant head_pix=darwin.pixel('#',true,false,darwin.white,darwin.black)
constant body_pix=darwin.pixel('+',true,false,darwin.white,darwin.black)
constant food_pix=darwin.pixel('@',true,false,darwin.white,darwin.black)
define snake_body={}
define snake_head={0,0}
define food={0,0}
define heading=2
constant init_long=5
define god_mode=false
define cross_wall=false
constant hard=2
define score=0

function die()
	define str0="You die!Your score is "+score
	str0.append(".")
	define str1="Press any key to continue..."
	loop
		if(darwin.is_kb_hit())
			darwin.get_kb_hit()
			break
		end
		darwin.clear_drawable()
		darwin.fill_drawable(darwin.pixel(' ',true,false,darwin.white,darwin.red))
		darwin.draw_string(0.5*(darwin.get_width()-str0.size()),0.5*darwin.get_height(),str0,darwin.pixel(' ',true,false,darwin.white,darwin.red))
		darwin.draw_string(0.5*(darwin.get_width()-str1.size()),darwin.get_height()-1,str1,darwin.pixel(' ',true,false,darwin.white,darwin.red))
		darwin.update_drawable()
	end
end

function gen_food()
	define is_fit=true
	while(is_fit)
		food[0]=runtime.randint(0,darwin.get_width()-1)
		food[1]=runtime.randint(0,darwin.get_height()-1)
		is_fit=false
		foreach it iterate snake_body
			if(it[0]==food[0]&&it[1]==food[1])
				is_fit=true
			end
		end
	end
end

function start()
	snake_head={init_long,to_integer(0.5*darwin.get_height())}
	for i=0 to init_long-2
		snake_body.push_back({snake_head[0]-i,snake_head[1]})
	end
	gen_food()
	define frame=0
	define pause=false
	loop
		if(darwin.is_kb_hit())
			switch darwin.get_kb_hit()
				case 'w'
					if(!pause&&(god_mode||heading!=4))
						heading=3
					end
				end
				case 's'
					if(!pause&&(god_mode||heading!=3))
						heading=4
					end
				end
				case 'a'
					if(!pause&&(god_mode||heading!=2))
						heading=1
					end
				end
				case 'd'
					if(!pause&&(god_mode||heading!=1))
						heading=2
					end
				end
				case '\t'
					if(pause)
						pause=false
					else
						pause=true
					end
				end
			end
		end
		if(!pause&&frame==hard)
			switch heading
				case 1
					--snake_head[0]
				end
				case 2
					++snake_head[0]
				end
				case 3
					--snake_head[1]
				end
				case 4
					++snake_head[1]
				end
			end
			if(cross_wall)
				if(snake_head[0]<0)
					snake_head[0]=snake_head[0]+darwin.get_width()
				end
				if(snake_head[0]>darwin.get_width()-1)
					snake_head[0]=snake_head[0]-darwin.get_width()
				end
				if(snake_head[1]<0)
					snake_head[1]=snake_head[1]+darwin.get_height()
				end
				if(snake_head[1]>darwin.get_height()-1)
					snake_head[1]=snake_head[1]-darwin.get_height()
				end
			else
				if(snake_head[0]<0||snake_head[0]>darwin.get_width()-1||snake_head[1]<0||snake_head[1]>darwin.get_height()-1)
					die()
					return 0
				end
			end
			if(!god_mode)
				foreach it iterate snake_body
					if(it[0]==snake_head[0]&&it[1]==snake_head[1])
						die()
						return 0
					end
				end
			end
			snake_body.push_front(snake_head)
			if(snake_head[0]==food[0]&&snake_head[1]==food[1])
				gen_food()
				score=score+to_integer(10*(1.0/hard))
			else
				snake_body.pop_back()
			end
			frame=0
		end
		darwin.clear_drawable()
		darwin.draw_string(0,0,"Score:"+to_string(score),darwin.pixel(' ',true,false,darwin.black,darwin.white))
		if(cross_wall)
			darwin.draw_string(0,1,"Cross Wall",darwin.pixel(' ',true,false,darwin.white,darwin.blue))
		end
		if(god_mode)
			darwin.draw_string(0,2,"God Mode",darwin.pixel(' ',true,false,darwin.white,darwin.red))
		end
		if(pause)
			darwin.draw_string(0,3,"Pause",darwin.pixel(' ',true,false,darwin.white,darwin.yellow))
		end
		foreach it iterate snake_body
			darwin.draw_point(it[0],it[1],body_pix)
		end
		darwin.draw_point(snake_head[0],snake_head[1],head_pix)
		darwin.draw_point(food[0],food[1],food_pix)
		darwin.update_drawable()
		if(!pause)
			++frame
		end
	end
end

for i=1 to system.args.size()-1
	switch system.args.at(i)
		case "--god-mode"
			god_mode=true
		end
		case "--cross-wall"
			cross_wall=true
		end
		default
			runtime.error("Unknown arguments")
		end
	end
end
darwin.load()
darwin.message_box("Greedy Snake","Press any key to start","OK")
darwin.fit_drawable()
loop
	snake_body.clear()
	heading=2
	start()
end
