system.console.echo(false)
system.console.clrscr()
system.out.println("Width:"+to_string(system.console.terminal_width()))
system.out.println("Height:"+to_string(system.console.terminal_height()))
system.out.println("Hello,")
system.console.gotoxy(6,2)
system.out.println("World")
system.out.println("Press q to exit.")
loop
	if system.console.kbhit()
		var ch=system.console.getch()
		if ch=='q'
			break
		end
		system.out.put(ch)
		system.out.flush()
	end
end
system.console.clrscr()
system.console.echo(true)
