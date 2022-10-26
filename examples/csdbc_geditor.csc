import picasso
import csdbc
using picasso.imgui
struct db_editor extends picasso.base_window
    var db=null
    var result=null
    var table_name=null
    var table_list=new array
    var column_info=null
    var text_buff=new string
    var selected=0
    var app_width=0
    var app_height=0
    function initialize() override
        this.win_flags={flags.no_collapse,flags.no_resize,flags.no_move}
        this.title="CSDBC Database Editor"
        this.on_close.add_listener([](win)->system.exit(0))
        this.show()
    end
    function load(table)
        table_name=table
        result=db.exec("SELECT * FROM "+table)
        column_info=db.column_info(table)
    end
    function on_present_handle(act)
        app_width=act.imgui_app.get_window_width()
        app_height=act.imgui_app.get_window_height()
        return true
    end
    function on_layout() override
        var opened=false
        text("Table: "+table_name)
        columns(column_info.size,"",true)
        separator()
        foreach it in column_info
            text(it.name)
            next_column()
        end
        var id=0
        foreach row in result
		    foreach it in row
			    if get_column_index()==0
                    separator()
                end
                opened=false
                selectable(to_string(it.data)+"##data",opened)
                if is_item_hovered()
                    if is_mouse_clicked(0)
                        open_popup("Edit Value##popup"+id)
                        text_buff=to_string(it.data)
                    end
                    if is_mouse_clicked(1)
                        open_popup("Delete Value##popup"+id)
                    end
                end
                opened=true
                if begin_popup_modal("Edit Value##popup"+id,opened,{flags.no_move,flags.always_auto_resize})
                    input_text("##iptxt"+id,text_buff,128)
                    same_line()
                    if button("Confirm")
                        var stmt=db.prepare("UPDATE "+table_name+" SET "+it.name+"=? WHERE "+it.name+"=?")
                        stmt.bind(0,text_buff)
                        stmt.bind(1,it.data)
                        stmt.just_exec()
                        load(table_name)
                        close_current_popup()
                        end_popup()
                        return
                    end
                    end_popup()
                end
                opened=true
                if begin_popup_modal("Delete Value##popup"+id,opened,{flags.no_move,flags.always_auto_resize})
                    text("Do you want to delete this line of table?")
                    if button("Yes")
                        var stmt=db.prepare("DELETE FROM "+table_name+" WHERE "+it.name+"=?")
                        stmt.bind(0,text_buff)
                        stmt.bind(1,it.data)
                        stmt.just_exec()
                        load(table_name)
                        close_current_popup()
                        end_popup()
                        return
                    end
                    same_line()
                    if button("No")
                        close_current_popup()
                    end
                    end_popup()
                end
                ++id
                next_column()
		    end
	    end
        separator()
        foreach it in column_info
            opened=false
            selectable(it.data + "##select"+id,opened)
            if is_item_hovered()&&is_mouse_clicked(0)
                open_popup("Edit Value##popup"+id)
                text_buff=it.data
            end
            opened=true
            if begin_popup_modal("Edit Value##popup"+id,opened,{flags.no_move,flags.always_auto_resize})
                #system.out.println(id)
                input_text("##iptxt"+id,text_buff,128)
                same_line()
                if button("Confirm")
                    it.data=text_buff
                    close_current_popup()
                end
                end_popup()
            end
            ++id
            next_column()
        end
        columns(1,"",true)
        separator()
        if button("Insert")
            var sql="INSERT INTO "+table_name+"("
            foreach it in column_info
                sql+=it.name+","
            end
            sql.cut(1)
            sql+=") VALUES("
            foreach it in column_info
                sql+="\'"+it.data+"\',"
                it.data=new string
            end
            sql.cut(1)
            sql+=")"
            db.just_exec(sql)
            load(table_name)
        end
    end
    function on_draw() override
        var opened=true
        if begin_popup_modal("Change Table",opened,{flags.no_move,flags.always_auto_resize})
            text("Exist Tables")
            list_box("##table_list",selected,table_list)
            if button("Select")||is_item_hovered()&&is_mouse_double_clicked(0)
                table_name=table_list.at(selected)
                load(table_name)
                close_current_popup()
            end
            end_popup()
        end
        set_window_pos(vec2(0,0))
        set_window_size(vec2(app_width,app_height))
    end
end

import csdbc_mysql

var act=gcnew picasso.base_activity
act->title="CSDBC Database Editor"
var conn_str = "Driver={MariaDB ODBC 3.1 Unicode Driver};SERVER=" + context.cmd_args[1] + ";USER=" + context.cmd_args[2] + ";PASSWORD=" + context.cmd_args[3] + ";DATABASE=" + context.cmd_args[4] + ";PORT=3306"
var win=gcnew db_editor
act->on_present.add_listener(win->on_present_handle)
win->db=csdbc_mysql.connect_str(conn_str)
win->load(context.cmd_args[5])
act->add_window(win)
act->start()