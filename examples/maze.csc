# Maze Program
# This program is written by Big_True any modified by Michael Lee

import darwin

darwin.load()
darwin.fit_drawable()
var dpic=darwin.get_drawable()

var xsize=10
var ysize=10
var xposition=0
var yposition=0
var pic=darwin.picture(xsize*4-2,ysize*2-1)
var find=false
var xlong=2*xsize-1
var ylong=2*ysize-1
var map=new array
var cango={{1,2},{1,4},{2,3},{3,4}}

constant redpixel=darwin.pixel(' ',darwin.red,darwin.red)
constant whitepixel=darwin.pixel(' ',darwin.white,darwin.white)
constant cyanpixel=darwin.pixel(' ',darwin.cyan,darwin.cyan)

function addvoidmap()
	map.clear()
	map[xlong*ylong-1]=0
end

function findroad()
	var x=1
	var y=1
	var face=2
	map[1+xlong]=2
	while x!=xlong-2 or y!=ylong-2
		var done=false
		switch face
			case 2
				if map[(x-1)+y*xlong]==0 and done==false
					map[(x-1)+y*xlong]=2
					x=x-1
					face=4
					done=true
				end
				if map[(x-1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x-1
					face=4
					done=true
				end
				if map[x+(y+1)*xlong]==0 and done==false
					map[x+(y+1)*xlong]=2
					y=y+1
					face=2
					done=true
				end
				if map[x+(y+1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y+1
					face=2
					done=true
				end
				if map[(x+1)+y*xlong]==0 and done==false
					map[(x+1)+y*xlong]=2
					x=x+1
					face=6
					done=true
				end
				if map[(x+1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x+1
					face=6
					done=true
				end
				if map[x+(y-1)*xlong]==0 and done==false
					map[x+(y-1)*xlong]=2
					y=y-1
					face=8
					done=true
				end
				if map[x+(y-1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y-1
					face=8
					done=true
				end
			end
			case 6
				if map[x+(y+1)*xlong]==0 and done==false
					map[x+(y+1)*xlong]=2
					y=y+1
					face=2
					done=true
				end
				if map[x+(y+1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y+1
					face=2
					done=true
				end
				if map[(x+1)+y*xlong]==0 and done==false
					map[(x+1)+y*xlong]=2
					x=x+1
					face=6
					done=true
				end
				if map[(x+1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x+1
					face=6
					done=true
				end
				if map[x+(y-1)*xlong]==0 and done==false
					map[x+(y-1)*xlong]=2
					y=y-1
					face=8
					done=true
				end
				if map[x+(y-1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y-1
					face=8
					done=true
				end
				if map[(x-1)+y*xlong]==0 and done==false
					map[(x-1)+y*xlong]=2
					x=x-1
					face=4
					done=true
				end
				if map[(x-1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x-1
					face=4
					done=true
				end
			end
			case 8
				if map[(x+1)+y*xlong]==0 and done==false
					map[(x+1)+y*xlong]=2
					x=x+1
					face=6
					done=true
				end
				if map[(x+1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x+1
					face=6
					done=true
				end
				if map[x+(y-1)*xlong]==0 and done==false
					map[x+(y-1)*xlong]=2
					y=y-1
					face=8
					done=true
				end
				if map[x+(y-1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y-1
					face=8
					done=true
				end
				if map[(x-1)+y*xlong]==0 and done==false
					map[(x-1)+y*xlong]=2
					x=x-1
					face=4
					done=true
				end
				if map[(x-1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x-1
					face=4
					done=true
				end
				if map[x+(y+1)*xlong]==0 and done==false
					map[x+(y+1)*xlong]=2
					y=y+1
					face=2
					done=true
				end
				if map[x+(y+1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y+1
					face=2
					done=true
				end
			end
			case 4
				if map[x+(y-1)*xlong]==0 and done==false
					map[x+(y-1)*xlong]=2
					y=y-1
					face=8
					done=true
				end
				if map[x+(y-1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y-1
					face=8
					done=true
				end
				if map[(x-1)+y*xlong]==0 and done==false
					map[(x-1)+y*xlong]=2
					x=x-1
					face=4
					done=true
				end
				if map[(x-1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x-1
					face=4
					done=true
				end
				if map[x+(y+1)*xlong]==0 and done==false
					map[x+(y+1)*xlong]=2
					y=y+1
					face=2
					done=true
				end
				if map[x+(y+1)*xlong]==2 and done==false
					map[x+y*xlong]=0
					y=y+1
					face=2
					done=true
				end
				if map[(x+1)+y*xlong]==0 and done==false
					map[(x+1)+y*xlong]=2
					x=x+1
					face=6
					done=true
				end
				if map[(x+1)+y*xlong]==2 and done==false
					map[x+y*xlong]=0
					x=x+1
					face=6
					done=true
				end
			end
		end
	end
	map[(xlong-2)+(ylong-2)*xlong]=2
end

function newmap()
	for x=0,x<xlong,++x
		map[x]=1
		map[x+(ylong-1)*xlong]=1
	end
	for y=0,y<ylong,++y
		map[y*xlong]=1
		map[(y+1)*xlong-1]=1
	end
	var ls=(xsize-2)*(ysize-2)
	while ls!=0
		var x=math.randint(1,xsize-2)
		var y=math.randint(1,ysize-2)
		var cangoit=cango[math.randint(0,3)]
		while map[2*x+2*y*xlong]==0
			var whe=cangoit[math.randint(0,1)]
			map[2*x+2*y*xlong]=1
			ls--
			switch whe
				case 1
					map[2*x+(2*y-1)*xlong]=1
					y--
				end
				case 2
					map[2*x+2*y*xlong+1]=1
					x++
				end
				case 3
					map[2*x+(2*y+1)*xlong]=1
					y++
				end
				case 4
					map[2*x+2*y*xlong-1]=1
					x--
				end
			end
		end
	end
end

function adraw()
	pic.clear()
	for x=0,x<xlong,++x
		for y=0,y<ylong,++y
			if map[x+y*xlong]==1
				pic.draw_pixel(2*x,y,whitepixel)
				pic.draw_pixel(2*x+1,y,whitepixel)
			else
				if map[x+y*xlong]==2 and find==true
					pic.draw_pixel(2*x,y,redpixel)
					pic.draw_pixel(2*x+1,y,redpixel)
				end
			end
		end
	end
	pic.draw_pixel(2,1,redpixel)
	pic.draw_pixel(3,1,redpixel)
	pic.draw_pixel(xlong*2-3,ylong-2,redpixel)
	pic.draw_pixel(xlong*2-4,ylong-2,redpixel)
	pic.draw_rect(0,0,2*xlong,ylong,cyanpixel)
	pic.draw_rect(1,0,2*xlong-2,ylong,cyanpixel)
end

function resize()
	xsize=math.max(2,to_integer(darwin.ui.input_box("change size","Set width",to_string(xsize),true)))
	ysize=math.max(2,to_integer(darwin.ui.input_box("change size","Set height",to_string(ysize),true)))
	pic.resize(xsize*4-2,ysize*2-1)
	xlong=2*xsize-1
	ylong=2*ysize-1
	addmap()
end

function addmap()
	addvoidmap()
	newmap()
	findroad()
	adraw()
	darwin.update_drawable()
end

addmap()
loop
	if darwin.is_kb_hit()==true
		switch darwin.get_kb_hit()
			case 'f'
				find=not find
				adraw()
				darwin.update_drawable()
			end
			case 'n'
				addmap()
			end
			case 's'
				resize()
			end
			case 'q'
				darwin.exit()
				system.exit(0)
			end
			case 'i'
				yposition++
			end
			case 'k'
				yposition--
			end
			case 'j'
				xposition++
			end
			case 'l'
				xposition--
			end
		end
	end
	darwin.fit_drawable()
	dpic.clear()
	dpic.draw_picture(0.5*(dpic.get_width()-pic.get_width())+xposition,0.5*(dpic.get_height()-pic.get_height())+yposition,pic)
	darwin.update_drawable()
end
