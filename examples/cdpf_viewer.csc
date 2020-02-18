import darwin
function main()
	if context.cmd_args.size<2
		system.out.println("Wrong size of arguments.")
		return
	end
	darwin.load()
	var pic=darwin.picture(0,0)
	pic.load_from_file(context.cmd_args.at(1))
	var head="Covariant Darwin Picture File Viewer 1.0"
	var help="Q:Exit W:Up S:Down A:Left D:Right"
	var info="File:\""+context.cmd_args[1]+"\" Width:"+to_string(pic.get_width())+"pix Height:"+to_string(pic.get_height())+"pix"
	var dpic=darwin.get_drawable()
	var x=0
	var y=0
	var running=true
	while running
		if darwin.is_kb_hit()
			switch darwin.get_kb_hit()
				case 'w'
					--y
					end
					case 's'
					++y
					end
					case 'a'
					--x
					end
					case 'd'
					++x
					end
					case 'q'
					running=false
					end
			end
		end
		darwin.fit_drawable()
		dpic.fill(darwin.pixel(' ',darwin.white,darwin.white))
		dpic.draw_picture((dpic.get_width()-pic.get_width())/2+x,(dpic.get_height()-pic.get_height())/2+y,pic)
		dpic.draw_line(0,0,dpic.get_width()-1,0,darwin.pixel(' ',darwin.blue,darwin.blue))
		dpic.draw_line(0,1,dpic.get_width()-1,1,darwin.pixel(' ',darwin.cyan,darwin.cyan))
		dpic.draw_line(0,dpic.get_height()-1,dpic.get_width()-1,dpic.get_height()-1,darwin.pixel(' ',darwin.blue,darwin.blue))
		dpic.draw_string(0,0,head,darwin.pixel(' ',darwin.white,darwin.blue))
		dpic.draw_string(0,1,help,darwin.pixel(' ',darwin.white,darwin.cyan))
		dpic.draw_string(0,dpic.get_height()-1,info,darwin.pixel(' ',darwin.white,darwin.blue))
		darwin.update_drawable()
	end
	darwin.exit()
	system.exit(0)
end
main()
