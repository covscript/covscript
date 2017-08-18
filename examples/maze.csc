darwin.load()
darwin.fit_drawable()
var xsize=10
var ysize=10
var findroad=false
var xlong=2*xsize-1
var ylong=2*ysize-1
var mapstyle=1
var map=new array
var cango={{1,2},{1,4},{2,3},{3,4}}
var draw=darwin.get_drawable()
var redpixel=darwin.pixel('#',darwin.red,darwin.red)
var whitepixel=darwin.pixel('@',darwin.white,darwin.white)
function addvoidmap()
	map.clear()
	for i=0 to xlong*ylong-1
		map.push_back(0)
	end
end
function newmap1()
	for x=0 to xlong-1
		map[x]=1
		map[x+(ylong-1)*xlong]=1
	end
	for y=0 to ylong-1
		map[y*xlong]=1
		map[(y+1)*xlong-1]=1
	end
	var ls=(xsize-2)*(ysize-2)
	while ls!=0
		var x=runtime.randint(1,xsize-2)
		var y=runtime.randint(1,ysize-2)
		var cangoit=cango[runtime.randint(0,3)]
		while map[2*x+2*y*xlong]==0

			var whe=cangoit[runtime.randint(0,1)]

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
	
	draw.clear()

	for x=0 to xlong-1

		for y=0 to ylong-1

			if map[x+y*xlong]==1

				draw.draw_pixel(2*x,y,whitepixel)

				draw.draw_pixel(2*x+1,y,whitepixel)

			else

				if map[x+y*xlong]==2 and findroad==true

					draw.draw_pixel(2*x,y,redpixel)

					draw.draw_pixel(2*x+1,y,redpixel)

				end

			end

		end

	end

end
function resize()

	#xsize=math.max(2,darwin.ui.input_box("change size","input x_size","10",true)))

	#ysize=math.max(2,darwin.ui.input box("change size","input y_size","10",true)))

	addmap()

end

function addmap()

	addvoidmap()

	newmap1()

	adraw()

	darwin.update_drawable()

end

addmap()

loop

	if darwin.is_kb_hit()==true

		switch darwin.get_kb_hit()

			case 'f'
				findroad=not findroad

			end

			case 'n'
				addmap()

			end

			case 's'

				resize()

			end

		end

	end

end