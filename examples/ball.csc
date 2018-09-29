import darwin
function main()
#初始化和设置
	darwin.load()
	var pic=darwin.get_drawable()
	var jump_frame=darwin.ui.input_box("Console Pinball Game","Please set the difficulty:","3",true)
	constant num3={1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1}
	constant num2={1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1}
	constant num1={0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0}
	constant chG={1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1}
	constant chO={1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1}
	constant chExc={0,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0}
#显示开始的倒计时
	darwin.set_frame_limit(1)
	darwin.fit_drawable()
	function draw_ch(ch,w,h,x,y)
		var r=0
		while(r<h)
			var c=0
			while(c<w)
				if(ch[r*6+c]==1)
					pic.draw_pixel(c+x,r+y,darwin.pixel(' ',darwin.red,darwin.red))
				end
				++c
			end
			++r
		end
	end
	darwin.update_drawable()
	pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
	draw_ch(num3,6,5,0.5*pic.get_width()-3,0.5*pic.get_height()-2.5)
	darwin.update_drawable()
	pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
	draw_ch(num2,6,5,0.5*pic.get_width()-3,0.5*pic.get_height()-2.5)
	darwin.update_drawable()
	pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
	draw_ch(num1,6,5,0.5*pic.get_width()-3,0.5*pic.get_height()-2.5)
	darwin.update_drawable()
	pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
	draw_ch(chG,6,5,0.5*pic.get_width()-10,0.5*pic.get_height()-2.5)
	draw_ch(chO,6,5,0.5*pic.get_width()-2,0.5*pic.get_height()-2.5)
	draw_ch(chExc,6,5,0.5*pic.get_width()+5,0.5*pic.get_height()-2.5)
	darwin.update_drawable()
#主循环
	darwin.set_frame_limit(60)
	var score=0
	var zkh=0.5*pic.get_height()
	var x=2
	var y=0
	var z1=0.5*pic.get_height()-0.5*zkh
	var z2=z1
	var xj=true
	var yj=true
	var delay_frame=0
	loop
		pic.fill(darwin.pixel(' ',darwin.white,darwin.blue))
		pic.draw_line(0,z1,0,z1+zkh,darwin.pixel('#',darwin.cyan,darwin.cyan))
		pic.draw_line(pic.get_width()-1,z2,pic.get_width()-1,z2+zkh,darwin.pixel('#',darwin.pink,darwin.pink))
		if(delay_frame>jump_frame)
			delay_frame=0
			if(x<=1)
				if(y<z1||y>z1+zkh)
					break
				end
				xj=true
				x=1
				++score
			end
			if(y<=0)
				yj=true
				y=0
			end
			if(x>=pic.get_width()-2)
				if(y<z2||y>z2+zkh)
					break
				end
				xj=false
				x=pic.get_width()-2
				++score
			end
			if(y>=pic.get_height()-1)
				yj=false
				y=pic.get_height()-1
			end
			if(xj)
				++x
			else
				--x
			end
			if(yj)
				++y
			else
				--y
			end
		else
			++delay_frame
		end
		if(darwin.is_kb_hit())
			switch darwin.get_kb_hit()
			case 'a'
				if(xj)
					if(z2>0)
						--z2
					end
				else
					if(z1>0)
						--z1
					end
				end
			end
			case 'z'
				if(xj)
					if(z2+zkh<pic.get_height()-1)
						++z2
					end
				else
					if(z1+zkh<pic.get_height()-1)
						++z1
					end
				end
			end
			end
		end
		pic.draw_pixel(x,y,darwin.pixel('@',darwin.red,darwin.yellow))
		var info=to_string(score)
		info="Score:"+info
		pic.draw_string(0,0,info,darwin.pixel(' ',darwin.white,darwin.black))
		darwin.update_drawable()
	end
#结束后处理得分
	var str=to_string(score)
	system.out.println("You Die!Your Score is "+str+".")
	if(score<5)
		system.out.println("Very Bad.Just try lower difficulty.")
	end
	if(score>=5&&score<10)
		system.out.println("Not Bad.You can get higher score.")
	end
	if(score>=10&&score<50)
		system.out.println("Good.")
	end
	if(score>=50&&score<100)
		system.out.println("Very Great!")
	end
	if(score>=100)
		system.out.println("So Amazing!")
	end
end
main()
