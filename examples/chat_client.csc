@charset:utf8
import network, imgui, imgui_font
using network
using imgui
# String
function escape_string(msg)
    var str = new string
    foreach ch in msg
        switch ch
            default
                str += ch
            end
            case '@'
                str += char.from_ascii(1)
            end
            case ':'
                str += char.from_ascii(2)
            end
            case ';'
                str += char.from_ascii(3)
            end
        end
    end
    return str
end
function unescape_string(msg)
    var str = new string
    foreach ch in msg
        switch ch
            default
                str += ch
            end
            case char.from_ascii(1)
                str += '@'
            end
            case char.from_ascii(2)
                str += ':'
            end
            case char.from_ascii(3)
                str += ';'
            end
        end
    end
    return str
end
# Network
var sock=new udp.socket
sock.open_v4()
var server=null
# ImGui
system.file.remove("./imgui.ini")
var app=window_application(0.5*imgui.get_monitor_width(0),0.5*imgui.get_monitor_height(0),"CovScript Chat Room")
var font=add_font_extend_cn(imgui_font.source_han_sans, 18)
style_color_light()
var show_confirm=false
var show_about=false
var name=host_name()
var msg=new string
var id=0
function config()
    var window_opened=true
    var port="88088"
    loop
        if app.is_closed()
            system.exit(0)
        end
        app.prepare()
        push_font(font)
        begin_main_menu_bar()
            menu_item("CovScript Chat Room: Configure","",false)
        end_main_menu_bar()
        begin_window("configure",window_opened,{flags.no_resize,flags.no_move,flags.no_collapse,flags.no_title_bar})
            set_window_pos(vec2(0,22))
            set_window_size(vec2(app.get_window_width(),app.get_window_height()))
            text("Welcome to CovScript Chat Room!")
            separator()
            input_text("Server Port",port,128)
            separator()
            input_text("Your Name",name,128)
            separator()
            if button("Confirm") || is_key_pressed(get_key_index(keys.enter))
                end_window()
                pop_font()
                app.render()
                break
            end
        end_window()
        pop_font()
        app.render()
    end
    server=udp.endpoint_broadcast(port.to_number())
    sock.set_opt_broadcast(true)
end
function connect()
    sock.send_to("JOIN@"+name,server)
    id=sock.receive_from(32,server)
    sock.set_opt_broadcast(false)
end
var message=new string
var keyboard_focus=true
var scroll_focus=true
function chatroom()
    var window_opened=true
    var height=40
    begin_window("chatroom_text",window_opened,{flags.no_resize,flags.no_move,flags.no_collapse,flags.no_title_bar})
        set_window_size(vec2(app.get_window_width(),app.get_window_height()-height-22))
        set_window_pos(vec2(0,22))
        text(msg)
        separator()
        text("History message")
        if scroll_focus
            set_scroll_here()
            scroll_focus=false
        end
    end_window()
    begin_window("chatroom_input",window_opened,{flags.no_resize,flags.no_move,flags.no_collapse,flags.no_title_bar})
        set_window_size(vec2(app.get_window_width(),height))
        set_window_pos(vec2(0,app.get_window_height()-height))
        if keyboard_focus
            set_keyboard_focus_here()
            keyboard_focus=false
        end
        input_text("",message,512)
        same_line()
        if button("Send") || is_key_pressed(get_key_index(keys.enter))
            if !message.empty()
                sock.send_to("MSG@"+id+":"+escape_string(message),server)
                message=new string
            end
            keyboard_focus=true
            scroll_focus=true
        end
    end_window()
end
function confirm()
    var confirm_window=true
    begin_window("Confirm",confirm_window,{flags.always_auto_resize,flags.no_collapse})
        text("Do you want to exit?")
        spacing()
        if button("Yes") || is_key_pressed(to_integer('Y'))
            sock.send_to("EXIT@"+id,server)
            system.exit(0)
        end
        same_line()
        if button("No") || !confirm_window
            show_confirm=false
        end
    end_window()
end
function about()
    var about_opened=true
    begin_window("About",about_opened,{flags.always_auto_resize,flags.no_collapse})
        if !about_opened
            show_about=false
        end
        text("Covariant Script Programming Language")
        bullet()
        text("STD Version:"+runtime.std_version)
        bullet()
        text("Import Path:"+runtime.get_import_path())
        text("Dear ImGui Extension")
        if button("Ok")
            show_about=false
        end
    end_window()
end
config()
connect()
loop
    app.prepare()
    push_font(font)
    if app.is_closed()
        sock.send_to("EXIT@"+id,server)
        system.exit(0)
    end
    if is_key_pressed(get_key_index(keys.escape))
        show_confirm=true
    end
    if begin_main_menu_bar()
        menu_item("CovScript Chat Room","",false)
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
            if menu_item("Exit","ESC",true)
                show_confirm=true
            end
            end_menu()
        end
        if begin_menu("Help",true)
            if menu_item("About this software","",true)
                show_about=true
            end
            end_menu()
        end
        end_main_menu_bar()
    end
    chatroom()
    if show_confirm
        confirm()
    end
    if show_about
        about()
    end
    sock.send_to("SYNC@"+id,server)
    var buff=sock.receive_from(1000,server)
    if buff!="NULL"
        foreach it in buff.split({';'})
            msg+=unescape_string(it)+"\n"
        end
    end
    pop_font()
    app.render()
end