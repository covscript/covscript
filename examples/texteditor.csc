struct texteditor
    var text_buffer=new array
    var file_stream=null
    var x_offset=0
    var y_offset=0
    var pic=null
    function init()
        darwin.load()
        darwin.set_frame_limit(60)
        var file_path=null
        if system.args.size()>1
            file_path=system.args.at(1)
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
        var max_size=(to_string(this.text_buffer.size())).size()+2
        this.pic.fill_rect(0,0,max_size,this.pic.get_height(),darwin.pixel(' ',darwin.blue,darwin.blue))
        for y=0 to math.min(this.pic.get_height(),this.text_buffer.size()-this.y_offset)-1
            this.pic.draw_string(1,y,to_string(y+this.y_offset+1),darwin.pixel(' ',darwin.white,darwin.blue))
            for x=0 to math.min(this.pic.get_width()-max_size,(this.text_buffer[y+this.y_offset]).size()-this.x_offset)-1
                this.pic.draw_pixel(x+max_size,y,darwin.pixel(this.text_buffer[y+this.y_offset][x+this.x_offset],darwin.black,darwin.white))
            end
        end
    end
    function main()
        this.init()
        var invalidate=false
        var old_w=0
        var old_h=0
        loop
            if darwin.is_kb_hit()
                invalidate=true
                switch darwin.get_kb_hit()
                    case 'w'
                        if this.y_offset>0
                            --this.y_offset
                        end
                    end
                    case 's'
                        ++this.y_offset
                    end
                    case 'a'
                        if this.x_offset>0
                            --this.x_offset
                        end
                    end
                    case 'd'
                        ++this.x_offset
                    end
                end
            end
            darwin.fit_drawable()
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
                darwin.update_drawable()
                invalidate=false
            else
                runtime.delay(1)
            end
        end
    end
end
(new texteditor).main()
