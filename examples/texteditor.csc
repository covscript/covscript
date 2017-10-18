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
        this.file_stream=iostream.fstream(file_path,iostream.openmode.in)
        if !this.file_stream.good()
            throw runtime.exception("Open file error.")
        end
        darwin.fit_drawable()
        this.pic=darwin.get_drawable()
    end
    function preload_text_buffer()
        while !this.file_stream.eof()&&this.text_buffer.size()-this.y_offset<=this.pic.get_height()
            this.text_buffer.push_back(this.file_stream.getline())
        end
    end
    function render_text()
        this.pic.clear()
        this.pic.fill(darwin.pixel(' ',darwin.white,darwin.white))
        this.border_size=(to_string(this.text_buffer.size())).size()+2
        this.pic.fill_rect(0,0,this.border_size,this.pic.get_height(),darwin.pixel(' ',darwin.blue,darwin.blue))
        for y=0 to math.min(this.pic.get_height(),this.text_buffer.size()-this.y_offset)-1
            this.pic.draw_string(1,y,to_string(y+this.y_offset+1),darwin.pixel(' ',darwin.white,darwin.blue))
            for x=0 to math.min(this.pic.get_width()-this.border_size,(this.text_buffer[y+this.y_offset]).size()-this.x_offset)-1
                this.pic.draw_pixel(x+this.border_size,y,darwin.pixel(this.text_buffer[y+this.y_offset][x+this.x_offset],darwin.black,darwin.white))
            end
        end
    end
    function jump_line()
        if this.cursor_x+this.x_offset>(this.text_buffer[this.cursor_y+this.y_offset]).size()
            var line_size=(this.text_buffer[this.cursor_y+this.y_offset]).size()
            var scr_width=this.pic.get_width()-this.border_size
            if line_size<this.x_offset
                if line_size>=scr_width
                    this.x_offset=line_size-scr_width+1
                    this.cursor_x=scr_width-1
                else
                    this.x_offset=0
                    this.cursor_x=line_size
                end
            else
                this.cursor_x=line_size-this.x_offset
            end
        end
    end
    function main()
        this.init()
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
                        if this.cursor_y>0
                            --this.cursor_y
                        else
                            if this.y_offset>0
                                --this.y_offset
                            end
                        end
                        this.jump_line()
                    end
                    case 's'
                        if this.cursor_y+this.y_offset<this.text_buffer.size()-1
                            if this.cursor_y<this.pic.get_height()-1
                                ++this.cursor_y
                            else
                                ++this.y_offset
                            end
                        end
                        this.jump_line()
                    end
                    case 'a'
                        if this.cursor_x>0
                            --this.cursor_x
                        else
                            if this.x_offset>0
                                --this.x_offset
                            else
                                if this.cursor_y>0
                                    --this.cursor_y
                                    this.cursor_x=(this.text_buffer[this.cursor_y+this.y_offset]).size()
                                    if this.border_size+this.cursor_x+this.x_offset>this.pic.get_width()-1
                                        this.x_offset=this.cursor_x-this.pic.get_width()+this.border_size+1
                                        this.cursor_x=this.pic.get_width()-this.border_size-1
                                    end
                                else
                                    if this.y_offset>0
                                        --this.y_offset
                                    end
                                end
                            end
                        end
                    end
                    case 'd'
                        if this.cursor_x+this.x_offset>=(this.text_buffer[this.cursor_y+this.y_offset]).size()
                            this.cursor_x=0
                            this.x_offset=0
                            if this.cursor_y+this.y_offset<this.text_buffer.size()-1
                                if this.cursor_y<this.pic.get_height()-1
                                    ++this.cursor_y
                                else
                                    ++this.y_offset
                                end
                            end
                        else
                            if this.cursor_x<this.pic.get_width()-this.border_size-1
                                ++this.cursor_x
                            else
                                ++this.x_offset
                            end
                        end
                    end
                end
            end
            darwin.fit_drawable()
            this.cursor_x=math.min(this.cursor_x,this.pic.get_width()-this.border_size-1)
            this.cursor_y=math.min(this.cursor_y,this.pic.get_height()-1)
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
            if this.pic.get_width()!=old_w
                invalidate=true
                old_w=this.pic.get_width()
            end
            if this.pic.get_height()!=old_h
                invalidate=true
                old_h=this.pic.get_height()
            end
            if invalidate
                this.preload_text_buffer()
                this.render_text()
                if show_cursor
                    this.pic.draw_pixel(this.cursor_x+this.border_size,this.cursor_y,darwin.pixel(' ',darwin.black,darwin.black))
                end
                darwin.update_drawable()
                invalidate=false
            end
        end
    end
end
(new texteditor).main()
