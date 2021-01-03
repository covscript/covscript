import code128
import imgui
using  imgui

var app = window_application(640, 400, "BarCode Generator(Code 128)")
style_color_light()

var barcode = new string
var total_size = 0

var is_opened = true
var code = new string
var slice = 2
var height = 50
var padding = 0
var scale = 1

function draw_callback(x1, y1, x2, y2, color)
    add_rect_filled(vec2(x1, y1), vec2(x2, y2), color ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1), 0)
end

while !app.is_closed()
    app.prepare()
    begin_window("Test", is_opened, {flags.no_move, flags.no_resize, flags.no_title_bar})
        set_window_size(vec2(app.get_window_width(), app.get_window_height()))
        set_window_pos(vec2(0, 0))
        input_text("BarCode", code, 128)
        same_line()
        if button("Generate")
            barcode = code128.gen_smart(code)
            total_size = 0
            foreach ch in barcode do total_size += to_integer(ch) - to_integer('0')
        end
        slider_float("Slice", slice, 1, 10)
        slider_float("Height", height, 20, 90)
        if height - 2*padding*slice < 10
            padding = (height - 10)/slice/2
        end
        slider_float("Padding", padding, 0, (height - 10)/slice/2)
        slider_float("Scale", scale, 1, 6)
        code128.draw_code(draw_callback, barcode, slice*scale, height*scale, padding, 0.5*(app.get_window_width() - slice*scale*(total_size + padding)), 0.5*(app.get_window_height() - scale*height), null)
    end_window()
    app.render()
end