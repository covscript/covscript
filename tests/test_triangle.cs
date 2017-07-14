function main()
	darwin.load()
	darwin.fit_drawable()
	define points= {{0,0},{darwin.get_width()-1,0},{0.5*darwin.get_width(),0.5*darwin.get_height()}}
	define focus=2
	loop
		if(darwin.is_kb_hit())
			switch darwin.get_kb_hit()
				case 'w'
					--points[focus][1]
				end
				case 's'
					++points[focus][1]
				end
				case 'a'
					--points[focus][0]
				end
				case 'd'
					++points[focus][0]
				end
				case '1'
					focus=0
				end
				case '2'
					focus=1
				end
				case '3'
					focus=2
				end
			end
		end
		darwin.fit_drawable()
		darwin.clear_drawable()
		darwin.fill_triangle(points[0][0],points[0][1],points[1][0],points[1][1],points[2][0],points[2][1],darwin.pixel(' ',true,false,darwin.black,darwin.white))
		darwin.draw_point(points[focus][0],points[focus][1],darwin.pixel('#',true,false,darwin.black,darwin.white))
		darwin.update_drawable()
	end
end
main()
