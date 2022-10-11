import sqlite
import picasso
import sqlite_ext
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
        this.title="CovScript Database Editor"
        this.on_close.add_listener([](win)->system.exit(0))
        this.show()
    end
    function open(path)
        if system.file.exist(path)
            db=sqlite.open(path)
        else
            throw runtime.exception("DB do not exists.")
        end
    end
    function load(table)
        table_name=table
        result=sqlite_ext.exec(db,"select * from "+table)
        column_info=sqlite_ext.column_info(db,table)
    end
    function on_present_handle(act)
        app_width=act.imgui_app.get_window_width()
        app_height=act.imgui_app.get_window_height()
        return true
    end
    function on_layout() override
        var opened=false
        selectable("Table: "+table_name,opened)
        if is_item_hovered()&&is_mouse_clicked(0)
            open_popup("Change Table")
            text_buff=table_name
            selected=0
            table_list=sqlite_ext.table_list(db)
        end
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
                selectable(to_string(it.data),opened)
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
                    text("Type:"+it.type)
                    input_text("",text_buff,128)
                    same_line()
                    if button("Confirm")
                        var stmt=db.prepare("update "+table_name+" set "+it.name+"=? where "+it.name+"=?")
                        switch it.sql_type
                            case sqlite.integer
                                stmt.bind_integer(1,text_buff.to_number())
                                stmt.bind_integer(2,it.data)
                            end
                            case sqlite.real
                                stmt.bind_real(1,text_buff.to_number())
                                stmt.bind_real(2,it.data)
                            end
                            case sqlite.text
                                stmt.bind_text(1,text_buff)
                                stmt.bind_text(2,it.data)
                            end
                        end
                        stmt.exec()
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
                        var stmt=db.prepare("delete from "+table_name+" where "+it.name+"=?")
                        switch it.sql_type
                            case sqlite.integer
                                stmt.bind_integer(1,it.data)
                            end
                            case sqlite.real
                                stmt.bind_real(1,it.data)
                            end
                            case sqlite.text
                                stmt.bind_text(1,it.data)
                            end
                        end
                        stmt.exec()
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
            selectable(it.data,opened)
            if is_item_hovered()&&is_mouse_clicked(0)
                open_popup("Edit Value##popup"+id)
                text_buff=it.data
            end
            opened=true
            if begin_popup_modal("Edit Value##popup"+id,opened,{flags.no_move,flags.always_auto_resize})
                text("Type:"+it.type)
                input_text("",text_buff,128)
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
            var sql="insert into "+table_name+"("
            foreach it in column_info
                sql+=it.name+","
            end
            sql.cut(1)
            sql+=") values("
            foreach it in column_info
                sql+="\'"+it.data+"\',"
                it.data=new string
            end
            sql.cut(1)
            sql+=")"
            sqlite_ext.exec(db,sql)
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
struct db_viewer extends picasso.base_window
    var db=null
    var table_list=null
    var selected=0
    var parent_act=null
    function initialize() override
        this.title="Select a table"
        this.on_close.add_listener([](win)->system.exit(0))
        this.show()
    end
    function open(path)
        if system.file.exist(path)
            db=sqlite.open(path)
        else
            throw runtime.exception("DB do not exists.")
        end
        var tables=sqlite_ext.exec(db,"select name from sqlite_master where type=\'table\' order by name")
        table_list=new array
        foreach row in tables
            foreach col in row
                table_list.push_back(col.data)
            end
        end
    end
    function on_draw() override
        text("Exist Tables")
        list_box("##table_list",selected,table_list)
        if button("Select")||is_item_hovered()&&is_mouse_double_clicked(0)
            var win=gcnew db_editor
            win->db=db
            win->load(table_list.at(selected))
            parent_act->on_present.add_listener(win->on_present_handle)
            parent_act->add_window(win)
            this.win_opened=false
        end
    end
end
var act=gcnew picasso.base_activity
act->title="CovScript Database Editor"
act->on_start.add_listener([](act)->typeid style_color_dark()!=typeid null)
function on_close_handle(path)
    if typeid path!=typeid string
        system.exit(0)
    end
    var win=gcnew db_viewer
    win->parent_act=act
    win->open(path)
    act->add_window(win)
    return true
end
var win=gcnew picasso.file_explorer
win->title="Open File"
win->message="Select a database"
win->filters.push_back(".*\\.db")
win->on_close.add_listener(on_close_handle)
win->read_path()
win->show()
act->add_window(win)
act->start()