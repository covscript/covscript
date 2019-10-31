import imgui
using imgui

constant win_w = 640, win_h = 640, pic_a = 100, pic_prec = 0.01, pic_thick = 5, poffset_x = 320, poffset_y = 390

constant get_coord = [](a, t) -> vec2(a*(2*math.sin(t) - math.sin(2*t)) + poffset_x, win_h - a*(2*math.cos(t) - math.cos(2*t)) - poffset_y)

var app = window_application(win_w, win_h, "CovScript Drawpad"), win_opened = true, t = -4, color = vec4(1, 0, 0, 1)

while !app.is_closed() && win_opened
    app.prepare()
    begin_window("Cardioid Drawpad", win_opened, {flags.no_move, flags.no_resize, flags.no_collapse})

        set_window_size(vec2(win_w, win_h))
        set_window_pos(vec2(0, 0))
        style_color_light()

        foreach t in range(-math.constants.pi + pic_prec, math.constants.pi - pic_prec, pic_prec)
            var coord_1 = get_coord(pic_a, t), coord_2 = get_coord(pic_a, t + pic_prec)
            add_line(coord_1, coord_2, color, pic_thick)
        end

        var coord_1 = get_coord(pic_a, -math.constants.pi + pic_prec), coord_2 = get_coord(pic_a, math.constants.pi - pic_prec)
        add_line(coord_1, coord_2, color, pic_thick)

        add_text(get_font(), 16, vec2(0.5*win_w - 24, 0.5*win_h), color, "Cardioid")

    end_window()
    app.render()
end