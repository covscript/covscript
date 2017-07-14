const var a=10
switch system.input()
	case "Hello"+"!"
		system.println("Hello!")
	end
	case (2*(3+2))^2
		system.println("100")
	end
	case -1
		system.println("-1")
	end
	case {0,1,2}[2]
		system.println("2")
	end
	case {to_integer(3.14),2}.at(0)
		system.println("3")
	end
	case {darwin.pixel(' ',true,false,darwin.white,darwin.black),"Darwin"}.at(1)
		system.println("Darwin")
	end
	case "x"+(to_string(math.pi)).cut(5)
		system.println("x3.14")
	end
	case a
	end
end
