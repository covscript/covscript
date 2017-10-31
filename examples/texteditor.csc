struct texteditor
    var text_buffer=new array
    var file_stream=null
    var border_size=0
    var x_offset=0
    var y_offset=0
    var cursor_x=0
    var cursor_y=0
    var pic=null
    function init()
        darwin.load()
        darwin.set_frame_limit(60)
        var file_path=null
        if system.args.size()>1
            file_path=system.args[1]
        else
            file_path=darwin.ui.input_box("Text Editor","Please enter a file path:","",false)
        end
        file_stream=iostream.fstream(file_path,iostream.openmode.in)
        if !file_stream.good()
            throw runtime.exception("Open file error.")
        end
        darwin.fit_drawable()
        pic=darwin.get_drawable()
    end
    function preload_text_buffer()
        while !file_stream.eof()&&text_buffer.size()-y_offset<=pic.get_height()
            text_buffer.push_back(file_stream.getline())
        end
    end
    function render_text()
        pic.clear()
        pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
        border_size=(to_string(text_buffer.size())).size()+2
        pic.fill_rect(0,0,border_size,pic.get_height(),darwin.pixel(' ',darwin.blue,darwin.blue))
        for y=0 to math.min(pic.get_height(),text_buffer.size()-y_offset)-1
            pic.draw_string(1,y,to_string(y+y_offset+1),darwin.pixel(' ',darwin.white,darwin.blue))
            for x=0 to math.min(pic.get_width()-border_size,(text_buffer[y+y_offset]).size()-x_offset)-1
                pic.draw_pixel(x+border_size,y,darwin.pixel(text_buffer[y+y_offset][x+x_offset],darwin.black,darwin.white))
            end
        end
    end
    function jump_line()
        if cursor_x+x_offset>(text_buffer[cursor_y+y_offset]).size()
            var line_size=(text_buffer[cursor_y+y_offset]).size()
            var scr_width=pic.get_width()-border_size
            if line_size<x_offset
                if line_size>=scr_width
                    x_offset=line_size-scr_width+1
                    cursor_x=scr_width-1
                else
                    x_offset=0
                    cursor_x=line_size
                end
            else
                cursor_x=line_size-x_offset
            end
        end
    end
    function main()
        init()
        var invalidate=false
        var show_cursor=true
        var clock=runtime.time()
        var old_w=0
        var old_h=0
        loop
            if darwin.is_kb_hit()
                invalidate=true
                switch darwin.get_kb_hit()
                    case 'q'
                        system.exit(0)
                    end
                    case 'w'
                        if cursor_y>0
                            --cursor_y
                        else
                            if y_offset>0
                                --y_offset
                            end
                        end
                        jump_line()
                    end
                    case 's'
                        if cursor_y+y_offset<text_buffer.size()-1
                            if cursor_y<pic.get_height()-1
                                ++cursor_y
                            else
                                ++y_offset
                            end
                        end
                        jump_line()
                    end
                    case 'a'
                        if cursor_x>0
                            --cursor_x
                        else
                            if x_offset>0
                                --x_offset
                            else
                                if cursor_y>0
                                    --cursor_y
                                    cursor_x=(text_buffer[cursor_y+y_offset]).size()
                                    if border_size+cursor_x+x_offset>pic.get_width()-1
                                        x_offset=cursor_x-pic.get_width()+border_size+1
                                        cursor_x=pic.get_width()-border_size-1
                                    end
                                else
                                    if y_offset>0
                                        --y_offset
                                    end
                                end
                            end
                        end
                    end
                    case 'd'
                        if cursor_x+x_offset>=(text_buffer[cursor_y+y_offset]).size()
                            cursor_x=0
                            x_offset=0
                            if cursor_y+y_offset<text_buffer.size()-1
                                if cursor_y<pic.get_height()-1
                                    ++cursor_y
                                else
                                    ++y_offset
                                end
                            end
                        else
                            if cursor_x<pic.get_width()-border_size-1
                                ++cursor_x
                            else
                                ++x_offset
                            end
                        end
                    end
                end
            end
            darwin.fit_drawable()
            cursor_x=math.min(cursor_x,pic.get_width()-border_size-1)
            cursor_y=math.min(cursor_y,pic.get_height()-1)
            if invalidate
                show_cursor=true
                clock=runtime.time()
            else
                if runtime.time()-clock>=500
                    invalidate=true
                    show_cursor=!show_cursor
                    clock=runtime.time()
                end
            end
            if pic.get_width()!=old_w
                invalidate=true
                old_w=pic.get_width()
            end
            if pic.get_height()!=old_h
                invalidate=true
                old_h=pic.get_height()
            end
            if invalidate
                preload_text_buffer()
                render_text()
                if show_cursor
                    pic.draw_pixel(cursor_x+border_size,cursor_y,darwin.pixel(' ',darwin.black,darwin.black))
                end
                darwin.update_drawable()
                invalidate=false
            end
        end
    end
end
(new texteditor).main()
