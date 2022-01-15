#!/usr/bin/env cs
#
# Covariant Script Package Manager
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Copyright (C) 2017-2022 Michael Lee(李登淳)
#
# Email:   lee@covariant.cn, mikecovlee@163.com
# Github:  https://github.com/mikecovlee
# Website: http://covscript.org.cn

constant version_str="1.1.0"
struct package_info
    var name=null
    var type=null
end
function get_package_info(path)
    var info=gcnew package_info
    var pos=path.find(".",0)
    if pos==-1
        throw runtime.exception("Unrecongnized File.")
    end
    info->name=path.substr(0,pos)
    info->type=path.substr(pos+1,path.size-pos-1)
    return info
end
function get_file_name(path)
    var pos=path.rfind(to_string(system.path.separator),-1)
    if pos!=-1
        return path.substr(pos+1,path.size-pos-1)
    else
        return path
    end
end
var path=runtime.get_import_path().split({system.path.delimiter}).back
function gui()
    # Force disable the optimization to avoid
    # the problems in some devices which do not support the opengl.
    var ext_name="picasso"
    var picasso=context.import(runtime.get_import_path(),ext_name)
    using picasso.imgui
    # About Window
    struct about_win extends picasso.message_box
        function initialize() override
            this.title="About"
        end
        function on_layout() override
            text("CovScript Package Manager")
            bullet()
            text("Version "+version_str)
        end
    end
    # Install
    struct inst_win extends picasso.file_explorer
        var gui=null
        var confirm_inst=gcnew picasso.question_box
        function on_close_handle(_path)
            if typeid _path!=typeid string
                return false
            end
            var file_name=get_file_name(_path)
            if system.file.exists(path+system.path.separator+file_name)
                confirm_inst->show()
                return false
            end
            system.file.copy(_path,path+system.path.separator+file_name)
            gui->status="Installation succeed."
            gui->load()
            return false
        end
        function confirm_inst_on_close_handle(arg)
            if arg
                gui->status="Installation succeed."
            end
            return false
        end
        function initialize() override
            this.title="Install"
            this.message="Please select a package"
            this.filters.push_back(".*\\.cse")
            this.filters.push_back(".*\\.csp")
            this.on_close.add_listener(on_close_handle)
            confirm_inst->title="Install"
            confirm_inst->question="Target exists.Overwrite?"
            confirm_inst->on_close.add_listener(confirm_inst_on_close_handle)
        end
    end
    # Uninstall
    struct uninst_win extends picasso.question_box
        var gui=null
        function on_close_handle(arg)
            if arg
                var info=gui->packages.at(gui->select)
                var extension_name=path+system.path.separator+info->name+".cse"
                var package_name=path+system.path.separator+info->name+".csp"
                if !system.file.remove(extension_name)&&!system.file.remove(package_name)
                    gui->status="Uninstall failed.("+info->name+")"
                else
                    gui->status="Uninstall succeed.("+info->name+")"
                end
                gui->load()
            end
            return false
        end
        function initialize() override
            this.title="Uninstall"
            this.on_close.add_listener(on_close_handle)
        end
        function on_layout() override
            var info=gui->packages.at(gui->select)
            if info->type=="cse"
                text("Do you want to uninstall the extension \""+info->name+"\"?")
            else
                text("Do you want to uninstall the package \""+info->name+"\"?")
            end
        end
    end
    # Main Window
    struct gui_window extends picasso.base_window
        var confirm=gcnew picasso.question_box
        var about=gcnew about_win
        var install=gcnew inst_win
        var uninstall=gcnew uninst_win
        var select=0
        var status="No error."
        var items=new array
        var packages=new array
        function load()
            select=0
            items=new array
            packages=new array
            var info=system.path.scan(path)
            foreach it in info
                if it.type==system.path.type.reg
                    var pack_info=get_package_info(it.name)
                    packages.push_back(pack_info)
                    items.push_back(pack_info->name)
                end
            end
        end
        function menu()
            if begin_menu("Tools",true)
                if begin_menu("Styles",true)
                    if menu_item("Classic","",true)
                        style_color_classic()
                    end
                    if menu_item("Light","",true)
                        style_color_light()
                    end
                    if menu_item("Dark","",true)
                        style_color_dark()
                    end
                    end_menu()
                end
                end_menu()
            end
            if begin_menu("Help",true)
                if menu_item("About this software","",true)
                    about->show()
                end
                end_menu()
            end
            if menu_item("Exit","Esc",true)
                confirm->show()
            end
        end
        function confirm_on_close_handle(arg)
            if arg
                system.exit(0)
            end
            return false
        end
        function on_close_handle(arg)
            confirm->show()
            arg=true
            return false
        end
        function initialize() override
            load()
            this.win_flags.push_back(flags.menu_bar)
            this.title="Cspkg"
            this.on_close.add_listener(on_close_handle)
            confirm->title="Confirm"
            confirm->question="Do you want to exit?"
            confirm->on_close.add_listener(confirm_on_close_handle)
        end
        function on_layout() override
            var info=packages.at(select)
            text(status)
            list_box("##list",select,items)
            if begin_popup_item("list")
                if menu_item("Refresh","",true)
                    load()
                end
                if begin_menu("Property",true)
                    if info->type=="cse"
                        menu_item("CovScript Extension","",false)
                    else
                        menu_item("CovScript Package","",false)
                    end
                    menu_item("Name: "+info->name,"",false)
                    end_menu()
                end
                if menu_item("Uninstall","",true)
                    uninstall->show()
                end
                end_popup()
            end
            if info->type=="cse"
                text("CovScript Extension")
            else
                text("CovScript Package")
            end
            text("Name: "+info->name)
            spacing()
        end
        function on_draw() override
            if begin_menu_bar()
                menu()
                end_menu_bar()
            end
            if begin_popup_window()
                menu()
                end_popup()
            end
            if button("Refresh List")
                load()
            end
            same_line()
            if button("Install New")
                install->show()
                install->read_path()
            end
            same_line()
            if button("Uninstall")
                uninstall->show()
            end
        end
    end
    struct gui_activity extends picasso.base_activity
        var win=gcnew gui_window
        function on_start_handle(arg) override
            style_color_light()
            return false
        end
        function on_present_handle(arg) override
            if is_key_pressed(get_key_index(keys.escape))
                win->confirm->show()
            end
            return false
        end
        function initialize() override
            this.title="CovScript Package Manager"
            win->install->gui=win
            win->uninstall->gui=win
            win->show()
            this.add_window(win)
            this.add_window(win->confirm)
            this.add_window(win->about)
            this.add_window(win->install)
            this.add_window(win->install->confirm_inst)
            this.add_window(win->uninstall)
            this.on_start.add_listener(on_start_handle)
            this.on_present.add_listener(on_present_handle)
        end
    end
    (new gui_activity).start()
end
function answer(question)
    loop
        system.out.println(question+"(y/n)")
        switch system.console.getch().tolower()
            case 'y'
                return true
            end
            case 'n'
                return false
            end
            default
                system.out.println("Please press \'y\' for yes or \'n\' for not.")
            end
        end
    end
end
function draw_separator()
    for i=1,i<=system.console.terminal_width(),++i
        system.out.print("-")
    end
    system.out.println("")
end
# Main Program
if context.cmd_args.size<2
    system.out.println("Error: No command.")
    system.out.println("Enter \"--help\" to view help.")
    system.exit(-1)
end
switch context.cmd_args.at(1)
    default
        system.out.println("Unsupported Function.")
        system.exit(-1)
    end
    case "--install"
        if context.cmd_args.size!=3
            system.out.println("Wrong syntax.")
            system.exit(-1)
        end
        var file_name=get_file_name(clone(context.cmd_args.at(2)))
        if system.file.exists(path+system.path.separator+file_name)
            if !answer("Target exists.Overwrite?")
                system.out.println("Installation aborted.")
                system.exit(0)
            end
        end
        system.file.copy(context.cmd_args.at(2),path+system.path.separator+file_name)
        system.out.println("Installation succeed.")
    end
    case "--uninstall"
        if context.cmd_args.size!=3
            system.out.println("Wrong syntax.")
            system.exit(-1)
        end
        var extension_name=path+system.path.separator+context.cmd_args.at(2)+".cse"
        var package_name=path+system.path.separator+context.cmd_args.at(2)+".csp"
        if !system.file.remove(extension_name)&&!system.file.remove(package_name)
            system.out.println("Uninstall failed.")
            system.exit(-1)
        else
            system.out.println("Uninstall succeed.")
            system.exit(0)
        end
    end
    case "--list"
        system.out.println("Installed Package/Extension")
        draw_separator()
        var info=system.path.scan(path)
        var packages=new array
        foreach it in info
            if it.type==system.path.type.reg
                packages.push_back(get_package_info(it.name))
            end
        end
        var max_size=0
        foreach it in packages
            if it->name.size>max_size
                max_size=it->name.size
            end
        end
        system.out.print("Name")
        for i=0,i<=max_size,++i
            system.out.print(" ")
        end
        system.out.println("Type")
        draw_separator()
        foreach it in packages
            system.out.print(it->name)
            for i=0,i<=max_size-it->name.size,++i
                system.out.print(" ")
            end
            system.out.print("    ")
            if it->type=="cse"
                system.out.println("CovScript Extension")
            else
                system.out.println("CovScript Package")
            end
        end
    end
    case "--gui"
        gui()
    end
    case "--help"
@begin
system.out.print(
    "Covariant Script Package Manager "+version_str+"\n"+
    "    --install   <path>    Install a CovScript Package/Extension\n"+
    "    --uninstall <name>    Uninstall a CovScript Package/Extension\n"+
    "    --list                List Installed CovScript Package/Extension\n"+
    "    --gui                 Run Cspkg GUI.\n"+
    "    --help                Print help infomation.\n"+
    "    --version             Print version info.\n"
)
@end
    end
    case "--version"
        system.out.println("cspkg v"+version_str)
    end
end