darwin.load()
darwin.message_box("CovScript Installer","Thanks for downloading the covscript.","Next")
define in=darwin.input_box("CovScript Installer","Please input the install path:","/usr/bin",false)
system.println(in)
