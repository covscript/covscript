import darwin
var dpic=darwin.picture(0,0)
function save()
	var pic=darwin.get_drawable()
	var str0="Please enter a file path"
	var buff=new string
	var run=true
	while run
		if darwin.is_kb_hit()
			var ch=darwin.get_kb_hit()
			switch to_integer(ch)
				default
					buff+=ch
				end
				case 127
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\b')
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\n')
					dpic.save_to_file(buff)
					run=false
				end
				case to_integer('\r')
					dpic.save_to_file(buff)
					run=false
				end
			end
		end
		pic.clear()
		var str_size=math.max(str0.size+2,buff.size+2)
		pic.draw_rect(0.5*(pic.get_width()-str_size-2),0.5*pic.get_height()-2,str_size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.5*(pic.get_width()-str_size),0.5*pic.get_height()-1,str_size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.5*(pic.get_width()-str_size-2)+1,0.5*pic.get_height()-2,str0,darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.5*(pic.get_width()-str_size)+1,0.5*pic.get_height(),buff,darwin.pixel(' ',darwin.white,darwin.cyan))
		darwin.update_drawable()
	end
end
function about()
	var pic=darwin.get_drawable()
	var str0="About This Software"
	var buff="Copyright (C) 2017-2022 Michael Lee"
	var run=true
	while run
		if darwin.is_kb_hit()
			darwin.get_kb_hit()
			run=false
		end
		pic.clear()
		var str_size=math.max(str0.size+2,buff.size+2)
		pic.draw_rect(0.5*(pic.get_width()-str_size-2),0.5*pic.get_height()-2,str_size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.5*(pic.get_width()-str_size),0.5*pic.get_height()-1,str_size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.5*(pic.get_width()-str_size-2)+1,0.5*pic.get_height()-2,str0,darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.5*(pic.get_width()-str_size)+1,0.5*pic.get_height(),buff,darwin.pixel(' ',darwin.white,darwin.cyan))
		darwin.update_drawable()
	end
end
function open_exist()
	var pic=darwin.get_drawable()
	var str0="Please enter a file path"
	var buff=new string
	var run=true
	while run
		if darwin.is_kb_hit()
			var ch=darwin.get_kb_hit()
			switch to_integer(ch)
				default
					buff+=ch
				end
				case 127
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\b')
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\n')
					dpic.load_from_file(buff)
					run=false
				end
				case to_integer('\r')
					dpic.load_from_file(buff)
					run=false
				end
			end
		end
		pic.clear()
		var str_size=math.max(str0.size+2,buff.size+2)
		pic.draw_rect(0.5*(pic.get_width()-str_size-2),0.5*pic.get_height()-2,str_size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.5*(pic.get_width()-str_size),0.5*pic.get_height()-1,str_size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.5*(pic.get_width()-str_size-2)+1,0.5*pic.get_height()-2,str0,darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.5*(pic.get_width()-str_size)+1,0.5*pic.get_height(),buff,darwin.pixel(' ',darwin.white,darwin.cyan))
		darwin.update_drawable()
	end
end
function new_picture()
	var pic=darwin.get_drawable()
	var buff0=new string
	var buff1=new string
	var but0="Default"
	var but1="OK"
	var select=0
	var limit=5
	var run=true
	while run
		if darwin.is_kb_hit()
			var ch=darwin.get_kb_hit()
			switch to_integer(ch)
				case to_integer('s')
					if select<3
						++select
					end
				end
				case to_integer('w')
					if select>0
						--select
					end
				end
				case 127
					switch select
						case 0
							if !buff0.empty()
								buff0.cut(1)
							end
						end
						case 1
							if !buff1.empty()
								buff1.cut(1)
							end
						end
					end
				end
				case to_integer('\b')
					switch select
						case 0
							if !buff0.empty()
								buff0.cut(1)
							end
						end
						case 1
							if !buff1.empty()
								buff1.cut(1)
							end
						end
					end
				end
				case to_integer('\t')
					if select<3
						++select
					else
						select=0
					end
				end
				case to_integer('\n')
					switch select
						default
							if select<2
								++select
							end
						end
						case 2
							buff0=to_string(pic.get_width()-2)
							buff1=to_string(pic.get_height()-2)
						end
						case 3
							dpic.clear()
							dpic.resize(buff0.to_number(),buff1.to_number())
							dpic.fill(darwin.pixel(' ',darwin.black,darwin.white))
							run=false
						end
					end
				end
				case to_integer('\r')
					switch select
						default
							if select<2
								++select
							end
						end
						case 2
							buff0=to_string(pic.get_width()-2)
							buff1=to_string(pic.get_height()-2)
						end
						case 3
							dpic.clear()
							dpic.resize(buff0.to_number(),buff1.to_number())
							dpic.fill(darwin.pixel(' ',darwin.black,darwin.white))
							run=false
						end
					end
				end
				default
					switch select
						case 0
							if to_integer(ch)>47&&to_integer(ch)<58&&buff0.size<limit
								buff0+=ch
							end
						end
						case 1
							if to_integer(ch)>47&&to_integer(ch)<58&&buff1.size<limit
								buff1+=ch
							end
						end
						default
							dpic.clear()
							dpic.resize(buff0.to_number(),buff1.to_number())
							dpic.fill(darwin.pixel(' ',darwin.black,darwin.white))
							run=false
						end
					end
				end
			end
		end
		darwin.fit_drawable()
		pic.clear()
		pic.draw_rect(0.25*pic.get_width(),0.25*pic.get_height(),0.5*pic.get_width(),0.5*pic.get_height(),darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.25*pic.get_width()+1,0.25*pic.get_height()+1,0.5*pic.get_width()-2,0.5*pic.get_height()-2,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+1,0.25*pic.get_height(),"CDPF Drawer v1.0-New Picture",darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.25*pic.get_width()+4,0.25*pic.get_height()+2,"Picture Width:",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.25*pic.get_height()+4,"Picture Height:",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_line(0.25*pic.get_width()+4,0.25*pic.get_height()+3,0.25*pic.get_width()+4+limit,0.25*pic.get_height()+3,darwin.pixel(' ',darwin.white,darwin.white))
		pic.draw_line(0.25*pic.get_width()+4,0.25*pic.get_height()+5,0.25*pic.get_width()+4+limit,0.25*pic.get_height()+5,darwin.pixel(' ',darwin.white,darwin.white))
		pic.draw_string(0.25*pic.get_width()+4,0.25*pic.get_height()+3,buff0,darwin.pixel(' ',darwin.black,darwin.white))
		pic.draw_string(0.25*pic.get_width()+4,0.25*pic.get_height()+5,buff1,darwin.pixel(' ',darwin.black,darwin.white))
		if select==2
			pic.draw_rect(0.25*pic.get_width()+3,0.75*pic.get_height()-5,but0.size+2,3,darwin.pixel('#',darwin.blue,darwin.blue))
		else
			pic.draw_rect(0.25*pic.get_width()+3,0.75*pic.get_height()-5,but0.size+2,3,darwin.pixel(' ',darwin.blue,darwin.blue))
		end
		pic.draw_string(0.25*pic.get_width()+4,0.75*pic.get_height()-4,but0,darwin.pixel(' ',darwin.white,darwin.blue))
		if select==3
			pic.draw_rect(0.25*pic.get_width()+3+but0.size+4,0.75*pic.get_height()-5,but1.size+2,3,darwin.pixel('#',darwin.blue,darwin.blue))
		else
			pic.draw_rect(0.25*pic.get_width()+3+but0.size+4,0.75*pic.get_height()-5,but1.size+2,3,darwin.pixel(' ',darwin.blue,darwin.blue))
		end
		pic.draw_string(0.25*pic.get_width()+4+but0.size+4,0.75*pic.get_height()-4,but1,darwin.pixel(' ',darwin.white,darwin.blue))
		if select<2
			pic.draw_string(0.25*pic.get_width()+2,0.25*pic.get_height()+3+2*select,"->",darwin.pixel(' ',darwin.white,darwin.cyan))
		end
		darwin.update_drawable()
	end
end
function start()
	var pic=darwin.get_drawable()
	var select=0
	var run=true
	while run
		if darwin.is_kb_hit()
			switch darwin.get_kb_hit()
				case '\t'
					if select<3
						++select
					else
						select=0
					end
				end
				case 's'
					if select<3
						++select
					end
				end
				case 'w'
					if select>0
						--select
					end
				end
				case '\n'
					switch(select)
						case 0
							new_picture()
							run=false
						end
						case 1
							open_exist()
							run=false
						end
						case 2
							about()
						end
						case 3
							darwin.exit()
							system.exit(0)
							run=false
						end
					end
				end
				case '\r'
					switch(select)
						case 0
							new_picture()
							run=false
						end
						case 1
							open_exist()
							run=false
						end
						case 2
							about()
						end
						case 3
							darwin.exit()
							system.exit(0)
							run=false
						end
					end
				end
			end
		end
		darwin.fit_drawable()
		pic.clear()
		pic.draw_rect(0.25*pic.get_width(),0.25*pic.get_height(),0.5*pic.get_width(),0.5*pic.get_height(),darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.25*pic.get_width()+1,0.25*pic.get_height()+1,0.5*pic.get_width()-2,0.5*pic.get_height()-2,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+1,0.25*pic.get_height(),"CDPF Drawer v1.0-Welcome",darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-6),"Getting start",darwin.pixel(' ',darwin.white,darwin.red))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-6)+2,"New Picture",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-6)+3,"Open exist Picture",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-6)+4,"About This Software",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-6)+5,"Exit",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+2,0.5*(pic.get_height()-6)+2+select,"->",darwin.pixel(' ',darwin.white,darwin.cyan))
		darwin.update_drawable()
	end
end
function menu()
	var pic=darwin.get_drawable()
	var select=0
	var run=true
	while run
		if darwin.is_kb_hit()
			switch darwin.get_kb_hit()
				case '\t'
					if select<5
						++select
					else
						select=0
					end
				end
				case 's'
					if select<5
						++select
					end
				end
				case 'w'
					if select>0
						--select
					end
				end
				case '\n'
					switch select
						case 0
							return
						end
						case 1
							save()
							return
						end
						case 2
							new_picture()
						end
						case 3
							open_exist()
						end
						case 4
							about()
						end
						case 5
							darwin.exit()
							system.exit(0)
						end
					end
					run=false
				end
				case '\r'
					switch select
						case 0
							return
						end
						case 1
							save()
							return
						end
						case 2
							new_picture()
						end
						case 3
							open_exist()
						end
						case 4
							about()
						end
						case 5
							darwin.exit()
							system.exit(0)
						end
					end
					run=false
				end
			end
		end
		darwin.fit_drawable()
		pic.clear()
		pic.draw_rect(0.25*pic.get_width(),0.25*pic.get_height(),0.5*pic.get_width(),0.5*pic.get_height(),darwin.pixel(' ',darwin.black,darwin.blue))
		pic.fill_rect(0.25*pic.get_width()+1,0.25*pic.get_height()+1,0.5*pic.get_width()-2,0.5*pic.get_height()-2,darwin.pixel(' ',darwin.black,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+1,0.25*pic.get_height(),"CDPF Drawer v1.0-Menu",darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8),"Pause",darwin.pixel(' ',darwin.white,darwin.red))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+2,"Continue",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+3,"Save",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+4,"New Picture",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+5,"Open exist Picture",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+6,"About This Software",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+4,0.5*(pic.get_height()-8)+7,"Exit",darwin.pixel(' ',darwin.white,darwin.cyan))
		pic.draw_string(0.25*pic.get_width()+2,0.5*(pic.get_height()-8)+2+select,"->",darwin.pixel(' ',darwin.white,darwin.cyan))
		darwin.update_drawable()
	end
end
function main()
	var pic=darwin.get_drawable()
	var mode=0
	var str_size=0
	var str0="  Please Select The Front Color(Key 1~8)  "
	var str1="  Please Select The Back Color(Key 1~8)  "
	var str2="  Please Select The Character  "
	var str3="Please Input String"
	var buff=new string
	var fmod=false
	var vertex=new array
	var run=true
	var cx=0
	var cy=0
	var pix_ch='@'
	var pix_fc=darwin.black
	var pix_bc=darwin.white
	while run
		var pix=darwin.pixel(pix_ch,pix_fc,pix_bc)
		if darwin.is_kb_hit()
			if mode==4
				var ch=darwin.get_kb_hit()
				switch to_integer(ch)
				default
					buff+=ch
				end
				case 127
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\b')
					if !buff.empty()
						buff.cut(1)
					end
				end
				case to_integer('\n')
					dpic.draw_string(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],buff,pix)
					buff.clear()
					vertex.clear()
					mode=0
				end
				case to_integer('\r')
					dpic.draw_string(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],buff,pix)
					buff.clear()
					vertex.clear()
					mode=0
				end
				end
			else
				if mode==7
					pix_ch=darwin.get_kb_hit()
					mode=0
				else
				switch darwin.get_kb_hit()
				case '1'
					switch mode
					case 5
						pix_fc=darwin.white
					end
					case 6
						pix_bc=darwin.white
					end
					end
					mode=0
				end
				case '2'
					switch mode
					case 5
						pix_fc=darwin.black
					end
					case 6
						pix_bc=darwin.black
					end
					end
					mode=0
				end
				case '3'
					switch mode
					case 5
						pix_fc=darwin.red
					end
					case 6
						pix_bc=darwin.red
					end
					end
					mode=0
				end
				case '4'
					switch mode
					case 5
						pix_fc=darwin.green
					end
					case 6
						pix_bc=darwin.green
					end
					end
					mode=0
				end
				case '5'
					switch mode
					case 5
						pix_fc=darwin.blue
					end
					case 6
						pix_bc=darwin.blue
					end
					end
					mode=0
				end
				case '6'
					switch mode
					case 5
						pix_fc=darwin.pink
					end
					case 6
						pix_bc=darwin.pink
					end
					end
					mode=0
				end
				case '7'
					switch mode
					case 5
						pix_fc=darwin.yellow
					end
					case 6
						pix_bc=darwin.yellow
					end
					end
					mode=0
				end
				case '8'
					switch mode
					case 5
						pix_fc=darwin.cyan
					end
					case 6
						pix_bc=darwin.cyan
					end
					end
					mode=0
				end
				case 's'
					++cy
				end
				case 'w'
					--cy
				end
				case 'd'
					++cx
				end
				case 'a'
					--cx
				end
				case 'q'
					dpic.draw_pixel(0.5*dpic.get_width()+cx+2,0.5*dpic.get_height()+cy,pix)
				end
				case 'e'
					switch mode
					case 0
						if fmod
							vertex.push_back({cx,cy})
							mode=11
							fmod=false
						else
							mode=1
						end
					end
					case 1
						mode=0
					end
					case 11
						dpic.draw_line(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],0.5*dpic.get_width()+cx+2,0.5*dpic.get_height()+cy,pix)
						vertex.clear()
						mode=0
					end
					end
				end
				case 'r'
					switch mode
					case 0
						vertex.push_back({cx,cy})
						if fmod
							mode=12
							fmod=false
						else
							mode=2
						end
					end
					case 2
						dpic.draw_rect(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],cx-vertex[0][0],cy-vertex[0][1],pix)
						vertex.clear()
						mode=0
					end
					case 12
						dpic.fill_rect(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],cx-vertex[0][0],cy-vertex[0][1],pix)
						vertex.clear()
						mode=0
					end
					end
				end
				case 't'
					switch mode
					case 0
						vertex.push_back({cx,cy})
						if fmod
							mode=23
							fmod=false
						else
							mode=3
						end
					end
					case 3
						vertex.push_back({cx,cy})
						mode=13
					end
					case 13
						dpic.draw_triangle(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],0.5*dpic.get_width()+2+vertex[1][0],0.5*dpic.get_height()+vertex[1][1],0.5*dpic.get_width()+cx+2,0.5*dpic.get_height()+cy,pix)
						vertex.clear()
						mode=0
					end
					case 23
						vertex.push_back({cx,cy})
						mode=33
					end
					case 33
						dpic.fill_triangle(0.5*dpic.get_width()+2+vertex[0][0],0.5*dpic.get_height()+vertex[0][1],0.5*dpic.get_width()+2+vertex[1][0],0.5*dpic.get_height()+vertex[1][1],0.5*dpic.get_width()+cx+2,0.5*dpic.get_height()+cy,pix)
						vertex.clear()
						mode=0
					end
					end
				end
				case 'g'
					vertex.push_back({cx,cy})
					if mode==0
						mode=4
					else
						mode=0
					end
				end
				case 'z'
					if mode==0
						mode=5
					else
						mode=0
					end
					end
				case 'x'
					if mode==0
						mode=6
					else
						mode=0
					end
					end
				case 'c'
					if mode==0
						mode=7
					else
						mode=0
					end
				end
				case 'f'
					fmod=!fmod
				end
				case 'm'
					menu()
				end
				end
				end
			end
		end
		darwin.fit_drawable()
		switch mode
		case 1
			dpic.draw_pixel(0.5*dpic.get_width()+cx+2,0.5*dpic.get_height()+cy,pix)
		end
		end
		pic.clear()
		pic.draw_picture(0.5*(pic.get_width()-dpic.get_width()),0.5*(pic.get_height()-dpic.get_height()),dpic)
		pic.draw_rect(0,0,pic.get_width(),pic.get_height(),darwin.pixel(' ',darwin.black,darwin.blue))
		pic.draw_string(1,0,"CDPF Drawer v1.0",darwin.pixel(' ',darwin.white,darwin.blue))
		if fmod
			pic.draw_string(1,pic.get_height()-1,"Function Mode",darwin.pixel(' ',darwin.white,darwin.blue))
		end
		switch mode
		case 2
			pic.draw_rect(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],cx-vertex[0][0],cy-vertex[0][1],pix)
		end
		case 3
			pic.draw_line(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		end
		case 4
			str_size=math.max(str3.size+2,buff.size+2)
			pic.draw_rect(0.5*(pic.get_width()-str_size-2),0.5*pic.get_height()-2,str_size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
			pic.fill_rect(0.5*(pic.get_width()-str_size),0.5*pic.get_height()-1,str_size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
			pic.draw_string(0.5*(pic.get_width()-str_size-2)+1,0.5*pic.get_height()-2,str3,darwin.pixel(' ',darwin.white,darwin.blue))
			pic.draw_string(0.5*(pic.get_width()-str_size)+1,0.5*pic.get_height(),buff,darwin.pixel(' ',darwin.white,darwin.cyan))
		end
		case 5
			pic.draw_rect(0.5*(pic.get_width()-str0.size-2),0.5*pic.get_height()-2,str0.size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
			pic.fill_rect(0.5*(pic.get_width()-str0.size),0.5*pic.get_height()-1,str0.size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
			pic.draw_string(0.5*(pic.get_width()-str0.size-2)+1,0.5*pic.get_height()-2,"Select Front Color",darwin.pixel(' ',darwin.white,darwin.blue))
			pic.draw_string(0.5*(pic.get_width()-str0.size),0.5*pic.get_height(),str0,darwin.pixel(' ',darwin.white,darwin.cyan))
		end
		case 6
			pic.draw_rect(0.5*(pic.get_width()-str1.size-2),0.5*pic.get_height()-2,str1.size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
			pic.fill_rect(0.5*(pic.get_width()-str1.size),0.5*pic.get_height()-1,str1.size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
			pic.draw_string(0.5*(pic.get_width()-str1.size-2)+1,0.5*pic.get_height()-2,"Select Back Color",darwin.pixel(' ',darwin.white,darwin.blue))
			pic.draw_string(0.5*(pic.get_width()-str1.size),0.5*pic.get_height(),str1,darwin.pixel(' ',darwin.white,darwin.cyan))
		end
		case 7
			pic.draw_rect(0.5*(pic.get_width()-str2.size-2),0.5*pic.get_height()-2,str2.size+2,5,darwin.pixel(' ',darwin.black,darwin.blue))
			pic.fill_rect(0.5*(pic.get_width()-str2.size),0.5*pic.get_height()-1,str2.size,3,darwin.pixel(' ',darwin.black,darwin.cyan))
			pic.draw_string(0.5*(pic.get_width()-str2.size-2)+1,0.5*pic.get_height()-2,"Select Character",darwin.pixel(' ',darwin.white,darwin.blue))
			pic.draw_string(0.5*(pic.get_width()-str2.size),0.5*pic.get_height(),str2,darwin.pixel(' ',darwin.white,darwin.cyan))
		end
		case 11
			pic.draw_line(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		end
		case 12
			pic.fill_rect(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],cx-vertex[0][0],cy-vertex[0][1],pix)
		end
		case 13
			pic.draw_triangle(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],0.5*pic.get_width()+2+vertex[1][0],0.5*pic.get_height()+vertex[1][1],0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		end
		case 23
			pic.draw_line(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		end
		case 33
			pic.fill_triangle(0.5*pic.get_width()+2+vertex[0][0],0.5*pic.get_height()+vertex[0][1],0.5*pic.get_width()+2+vertex[1][0],0.5*pic.get_height()+vertex[1][1],0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		end
		end
		pic.draw_string(0.5*pic.get_width()+cx,0.5*pic.get_height()+cy,"->",darwin.pixel(' ',darwin.white,darwin.black))
		pic.draw_pixel(0.5*pic.get_width()+cx+2,0.5*pic.get_height()+cy,pix)
		darwin.update_drawable()
	end
end
darwin.load()
darwin.set_frame_limit(60)
try
	start()
	main()
catch e
	system.out.println(e.what)
end
darwin.exit()
