darwin.load()
darwin.ui.message_box("CovScript Installer","Thanks for downloading the covscript.","Next")
var in=darwin.ui.input_box("CovScript Installer","Please input the install path:","/usr/bin",false)
system.out.println(in)
