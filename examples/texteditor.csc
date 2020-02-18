# Simple Text Editor using Darwin UCGL
# Written in Covariant Script by Michael Lee
import darwin
# Key Mapping
namespace keymap
    constant key_esc = char.from_ascii(27)
    constant key_enter_unix = '\r'
    constant key_enter_win32 = '\n'
    constant key_delete_unix = char.from_ascii(127)
    constant key_delete_win32 = '\b'
    constant key_tab = '\t'
    constant key_up = 'w'
    constant key_down = 's'
    constant key_left = 'a'
    constant key_right = 'd'
    constant key_mode = 'i'
    constant key_quit = 'q'
    constant key_reload = 'r'
    constant key_save = 'x'
    constant key_find = 'f'
    constant key_info = 'v'
end
# FSM Status
namespace await_process_type
    constant normal = 0
    constant quit = 1
    constant reload = 2
end

namespace editor_status_type
    constant normal = 0
    constant asking = 1
    constant confirm = 2
    constant info = 3
    constant setup = 4
    constant notfound = 5
    constant finding = 6
    constant replace = 7
end
# Process Class
class texteditor
# Cursors
    var cursor_visible = true, cursor_time = 0, cursor_x = 0, cursor_y = 0
# Rendering
    var render_border = 0, render_offx = 0, render_offy = 0
    var last_win_width = 0, last_win_height = 0
# Buffers
    var find_target = new string
    var char_buffer = new string
    var file_buffer = new array
    var file_path = new string
# Status
    var await_process = await_process_type.normal
    var editor_status = editor_status_type.normal
    var find_x = 0, find_y = 0
    var text_modified = false
    var insert_mode = false
    var found_text = false
    var expect_txt = false
# Screen Buffer
    var pic = null
# Settings
    var tab_indent = 4
# Files
    function load_file(path)
        var in = iostream.fstream(file_path, iostream.openmode.in)
        for line = new string, !in.eof(), file_buffer.push_back(line)
            line = in.getline()
            for i = 0, i < line.size, ++i
                if line[i] == '\t'
                    line.assign(i, ' ')
                    for count = 0, count < tab_indent - 1, ++count do line.insert(i, ' ')
                end
            end
        end
    end

    function save_file(path)
        var out = iostream.fstream(path, iostream.openmode.out)
        for idx = 0, idx < file_buffer.size - 1, ++idx do out.println(file_buffer[idx])
        out.print(file_buffer[file_buffer.size - 1])
        text_modified = false
    end
# Attributes
    function text_area_width()
        return pic.get_width() - render_border - 5
    end

    function text_area_height()
        return pic.get_height() - 2
    end
    
    function text_offset_x()
        return render_offx + cursor_x
    end

    function text_offset_y()
        return render_offy + cursor_y
    end

    function current_line()
        return file_buffer[text_offset_y()]
    end
# Utilities
    function is_validate_path_char(ch)
        return char.isalnum(ch) || ch == ' ' || ch == '\\' || ch == '/'
    end

    function force_refresh()
        cursor_visible = false
        cursor_time = 0
    end

    function adjust_cursor(_x_offset)
        var x_offset = _x_offset
        if x_offset < cursor_x + render_offx
            x_offset = cursor_x + render_offx - x_offset
            while x_offset > 0 && cursor_x > 0
                --x_offset
                --cursor_x
            end
            while x_offset > 0 && render_offx > 0
                --x_offset
                --render_offx
            end
            if cursor_x == 0 && render_offx < 4
                swap(cursor_x, render_offx)
            end
        else
            while cursor_x + render_offx < x_offset && cursor_x < pic.get_width() - 1
                ++cursor_x
            end
            while cursor_x + render_offx < x_offset && cursor_x + render_offx < current_line().size
                ++render_offx
            end
        end
    end
# Keyboard Events
    function key_up()
        if cursor_y > 0
            --cursor_y
        else
            if render_offy > 0
                --render_offy
            end
        end
        if cursor_x + render_offx > current_line().size
            adjust_cursor(current_line().size)
        end
    end

    function key_down()
        if cursor_y < text_area_height() - 1
            ++cursor_y
        else
            if render_offy + text_area_height() < file_buffer.size
                ++render_offy
            end
        end
        if cursor_x + render_offx > current_line().size
            adjust_cursor(current_line().size)
        end
    end

    function key_left()
        if cursor_x > 0
            --cursor_x
        else
            if render_offx > 0
                --render_offx
            else
                if cursor_y + render_offy > 0
                    if render_offy > 0
                        --render_offy
                    else
                        if cursor_y > 0
                            --cursor_y
                        end
                    end
                    adjust_cursor(current_line().size)
                end
            end
        end
    end

    function key_right()
        if cursor_x < text_area_width()
            if text_offset_x() < current_line().size
                ++cursor_x
            else
                if text_offset_y() < file_buffer.size - 1
                    cursor_x = render_offx = 0
                    key_down()
                end
            end
        else
            if (text_offset_x() < current_line().size)
                ++render_offx
            else
                if render_offy + text_area_height() < file_buffer.size
                    render_offx = 0
                    ++render_offy
                end
            end
        end
    end
    function key_enter()
        var line = current_line()
        var line_current = line.substr(0, text_offset_x())
        var line_next = line.substr(text_offset_x(), line.size)
        current_line() = line_current
        file_buffer.insert(file_buffer.begin.next_n(text_offset_y() + 1), line_next)
        key_down()
        cursor_x = render_offx = 0
        text_modified = true
    end
    function key_delete()
        var line = current_line()
        if cursor_x + render_offx == 0
            if text_offset_y() != 0
                key_up()
                adjust_cursor(current_line().size)
                current_line().append(line)
                file_buffer.erase(file_buffer.begin.next_n(text_offset_y() + 1))
                text_modified = true
            end
        else
            if line.size > 0
                current_line().erase(text_offset_x() - 1, 1)
                key_left()
                text_modified = true
            end
        end
    end
# Text Finding
    function reset_find()
        find_x = find_y = 0
        found_text = false
        expect_txt = false
    end

    function find()
        while find_y < file_buffer.size
            var line = file_buffer[find_y]
            var pos = line.find(find_target, expect_txt ? find_x + 1 : 0)
            if pos != -1
                if !expect_txt || pos > find_x
                    found_text = true
                    expect_txt = true
                    find_x = pos
                    break
                end
            end
            expect_txt = false
            find_x = 0
            ++find_y
        end
        if found_text
            if find_y == file_buffer.size
                reset_find()
                find()
            end
            cursor_x = render_offx = 0
            cursor_y = 0
            if find_y < file_buffer.size - text_area_height()
                render_offy = find_y > 4 ? find_y - 4 : 0
            else
                render_offy = file_buffer.size - text_area_height()
            end
            if find_x + find_target.size > text_area_width()
                render_offx = find_x + find_target.size - text_area_width() + 1
            end
        end
    end
# Events
    function window_resized()
        if last_win_width != pic.get_width() || last_win_height != pic.get_height()
            last_win_width = pic.get_width()
            last_win_height = pic.get_height()
            while cursor_x >= text_area_width()
                --cursor_x
                ++render_offx
            end
            return true
        else
            return false
        end
    end

    function keyboard_input()
        if await_process != await_process_type.normal
            return true
        end
        if darwin.is_kb_hit()
            force_refresh()
            if insert_mode
                var key = darwin.get_kb_hit()
                if key != keymap.key_esc
                    switch key
                        case keymap.key_enter_unix
                            key_enter()
                        end
                        case keymap.key_enter_win32
                            key_enter()
                        end
                        case keymap.key_delete_unix
                            key_delete()
                        end
                        case keymap.key_delete_win32
                            key_delete()
                        end
                        case keymap.key_tab
                            foreach i in range(tab_indent) do current_line().insert(text_offset_x(), ' ')
                            adjust_cursor(text_offset_x() + tab_indent);
                            text_modified = true
                        end
                        default
                            current_line().insert(text_offset_x(), key)
                            adjust_cursor(text_offset_x() + 1);
                            text_modified = true
                        end
                    end
                else
                    insert_mode = false
                end
            else
                switch char.tolower(darwin.get_kb_hit())
                    case keymap.key_up
                        key_up()
                    end
                    case keymap.key_down
                        key_down()
                    end
                    case keymap.key_left
                        key_left()
                    end
                    case keymap.key_right
                        key_right()
                    end
                    case keymap.key_mode
                        insert_mode = true
                    end
                    case keymap.key_save
                        save_file(file_path)
                    end
                    case keymap.key_reload
                        if text_modified
                            await_process = await_process_type.reload
                            editor_status = editor_status_type.asking
                        else
                            file_buffer.clear()
                            load_file(file_path)
                        end
                    end
                    case keymap.key_quit
                        if text_modified
                            await_process = await_process_type.quit
                            editor_status = editor_status_type.asking
                        else
                            system.exit(0)
                        end
                    end
                    case keymap.key_find
                        find_target.clear()
                        char_buffer.clear()
                        editor_status = editor_status_type.setup
                    end
                    case keymap.key_info
                        var char_count = 0
                        foreach line in file_buffer do char_count += line.size
                        char_buffer = to_string(file_buffer.size) + " line(s), " + to_string(char_count) + " character(s)"
                        editor_status = editor_status_type.info
                    end
                    default
                        insert_mode = true
                    end
                end
            end
            return true
        else
            return false
        end
    end

    function cursor_timer()
        var time = runtime.time() - cursor_time
        if time > 500
            return true
        else
            return false
        end
    end
# Rendering
    function render_linenum()
        render_border = to_string(file_buffer.size).size
        for i = 0, i < text_area_height(), ++i
            pic.draw_line(2, i + 1, 1 + render_border, i + 1, darwin.pixel(' ', darwin.white, darwin.white))
            var txt = to_string(render_offy + i + 1)
            pic.draw_string(2 + (render_border - txt.size), i + 1, txt, darwin.pixel(' ', darwin.black, darwin.white))
        end
    end

    function render_text()
        for y = 0, y < text_area_height() && y + render_offy < file_buffer.size, ++y
            for x = 0, x < text_area_width() && x + render_offx < file_buffer[y + render_offy].size, ++x
                var ch = file_buffer[y + render_offy][x + render_offx]
                if found_text && x + render_offx >= find_x && x + render_offx < find_x + find_target.size && y + render_offy == find_y
                    pic.draw_pixel(x + 3 + render_border, y + 1, darwin.pixel(ch, darwin.white, darwin.pink))
                else
                    pic.draw_pixel(x + 3 + render_border, y + 1, darwin.pixel(ch, darwin.white, darwin.black))
                end
            end
        end
    end

    function render_cursor()
        var time = runtime.time()
        if time - cursor_time > 500
            cursor_visible = !cursor_visible
            cursor_time = time
        end
        if cursor_visible
            pic.draw_pixel(cursor_x + 3 + render_border, cursor_y + 1, darwin.pixel(' ', darwin.white, darwin.white))
        end
    end

    function draw_basic_frame()
        pic.fill(darwin.pixel(' ', darwin.white, darwin.black))
        pic.draw_line(0, 0, pic.get_width() - 1, 0, darwin.pixel(' ', darwin.blue, darwin.blue))
        if text_modified
            pic.draw_string(2, 0, "Darwin UCGL Text Editor: " + file_path + " (Unsaved)", darwin.pixel(' ', darwin.white, darwin.blue))
        else
            pic.draw_string(2, 0, "Darwin UCGL Text Editor: " + file_path, darwin.pixel(' ', darwin.white, darwin.blue))
        end
        pic.draw_line(0, 0, 0, pic.get_height() - 1, darwin.pixel(' ', darwin.blue, darwin.blue))
        pic.draw_line(1, 0, 1, pic.get_height() - 1, darwin.pixel(' ', darwin.blue, darwin.blue))
        pic.draw_line(pic.get_width() - 1, 0, pic.get_width() - 1, pic.get_height() - 1, darwin.pixel(' ', darwin.blue, darwin.blue))
        pic.draw_line(pic.get_width() - 2, 0, pic.get_width() - 2, pic.get_height() - 1, darwin.pixel(' ', darwin.blue, darwin.blue))
        render_linenum()
        render_text()
    end
# Event Handling
    function exec_await_process()
        switch (await_process)
            case await_process_type.quit
                system.exit(0)
            end
            case await_process_type.reload
                text_modified = false
                file_buffer.clear()
                load_file(file_path)
            end
        end
        await_process = await_process_type.normal
    end
# Running Status
    function run_normal()
        darwin.fit_drawable()
        if window_resized() || keyboard_input() || cursor_timer()
            draw_basic_frame()
            pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.blue, darwin.blue))
            if insert_mode
                pic.draw_string(2, pic.get_height() - 1, "INSERT (Press ESC to exit)", darwin.pixel(' ', darwin.white, darwin.blue))
            else
                pic.draw_string(2, pic.get_height() - 1, "WASD: Move I: Insert X: Save R: Reload F: Find V: Info Q: Exit", darwin.pixel(' ', darwin.white, darwin.blue))
            end
            render_cursor()
            darwin.update_drawable()
        else
            runtime.delay(15)
        end
    end

    function run_unsaved_asking()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.red, darwin.red))
        pic.draw_string(2, pic.get_height() - 1, "File unsaved, continue without saving? (Yes(Y), No(N) or other key to cancel)", darwin.pixel(' ', darwin.white, darwin.red))
        if darwin.is_kb_hit()
            switch char.tolower(darwin.get_kb_hit())
                case 'y'
                    exec_await_process()
                    editor_status = editor_status_type.normal
                    force_refresh()
                end
                case 'n'
                    editor_status = editor_status_type.confirm
                    char_buffer = file_path
                end
                default
                    await_process = await_process_type.normal
                    editor_status = editor_status_type.normal
                    force_refresh()
                end
            end
        end
        darwin.update_drawable()
    end

    function run_unsaved_confirm()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.cyan, darwin.cyan))
        pic.draw_string(2, pic.get_height() - 1, "Save to: " + char_buffer, darwin.pixel(' ', darwin.white, darwin.cyan))
        if darwin.is_kb_hit()
            var key = 0
            switch key = char.tolower(darwin.get_kb_hit())
                case keymap.key_delete_unix
                    if !char_buffer.empty()
                        char_buffer.cut(1)
                    end
                end
                case keymap.key_delete_win32
                    if !char_buffer.empty()
                        char_buffer.cut(1)
                    end
                end
                case keymap.key_enter_unix
                    save_file(char_buffer)
                    exec_await_process()
                    editor_status = editor_status_type.normal
                    force_refresh()
                end
                case keymap.key_enter_win32
                    save_file(char_buffer)
                    exec_await_process()
                    editor_status = editor_status_type.normal
                    force_refresh()
                end
                default
                    if is_validate_path_char(key)
                        char_buffer += key
                    end
                end
            end
        end
        darwin.update_drawable()
    end

    function run_find_setup()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.white, darwin.white))
        pic.draw_string(2, pic.get_height() - 1, "Find: " + find_target, darwin.pixel(' ', darwin.black, darwin.white))
        if darwin.is_kb_hit()
            var key = 0
            switch key = char.tolower(darwin.get_kb_hit())
                case keymap.key_delete_unix
                    if !find_target.empty()
                        find_target.cut(1)
                    end
                end
                case keymap.key_delete_win32
                    if !find_target.empty()
                        find_target.cut(1)
                    end
                end
                case keymap.key_enter_unix
                    if find_target.empty()
                        editor_status = editor_status_type.normal
                        force_refresh()
                        break
                    end
                    find()
                    if (found_text)
                        editor_status = editor_status_type.finding
                    else
                        editor_status = editor_status_type.notfound
                    end
                end
                case keymap.key_enter_win32
                    if find_target.empty()
                        editor_status = editor_status_type.normal
                        force_refresh()
                        break
                    end
                    find()
                    if (found_text)
                        editor_status = editor_status_type.finding
                    else
                        editor_status = editor_status_type.notfound
                    end
                end
                default
                    if !char.iscntrl(key)
                        find_target += key
                    end
                end
            end
        end
        darwin.update_drawable()
    end

    function run_notfound()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.white, darwin.white))
        pic.draw_string(2, pic.get_height() - 1, "Find: \"" + find_target + "\" not found (Press Q to close)", darwin.pixel(' ', darwin.black, darwin.white))
        if darwin.is_kb_hit()
            if char.tolower(darwin.get_kb_hit()) == 'q'
                editor_status = editor_status_type.normal
                force_refresh()
            end
        end
        darwin.update_drawable()
    end

    function run_finding()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.white, darwin.white))
        pic.draw_string(2, pic.get_height() - 1, "Find: \"" + find_target + "\" (Next(N) Replace(R) Quit(Q))", darwin.pixel(' ', darwin.black, darwin.white))
        if darwin.is_kb_hit()
            switch char.tolower(darwin.get_kb_hit())
                case 'n'
                    find()
                end
                case 'r'
                    editor_status = editor_status_type.replace
                end
                case 'q'
                    reset_find()
                    if text_offset_x() > current_line().size
                        adjust_cursor(current_line().size)
                    end
                    editor_status = editor_status_type.normal
                    force_refresh()
                end
            end
        end
        darwin.update_drawable()
    end

    function run_replace()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.pink, darwin.pink))
        pic.draw_string(2, pic.get_height() - 1, "Replace: " + char_buffer, darwin.pixel(' ', darwin.white, darwin.pink))
        if darwin.is_kb_hit()
            var key = 0
            switch key = char.tolower(darwin.get_kb_hit())
                case keymap.key_delete_unix
                    if !char_buffer.empty()
                        char_buffer.cut(1)
                    end
                end
                case keymap.key_delete_win32
                    if !char_buffer.empty()
                        char_buffer.cut(1)
                    end
                end
                case keymap.key_enter_unix
                    if char_buffer != find_target
                        text_modified = true
                        file_buffer[find_y].replace(find_x, find_target.size, char_buffer)
                        find()
                        if found_text
                            editor_status = editor_status_type.finding
                        else
                            reset_find()
                            if text_offset_x() > current_line().size
                                adjust_cursor(current_line().size)
                            end
                            editor_status = editor_status_type.notfound
                        end
                    end
                end
                case keymap.key_enter_win32
                    if char_buffer != find_target
                        text_modified = true
                        file_buffer[find_y].replace(find_x, find_target.size, char_buffer)
                        find()
                        if found_text
                            editor_status = editor_status_type.finding
                        else
                            reset_find()
                            if text_offset_x() > current_line().size
                                adjust_cursor(current_line().size)
                            end
                            editor_status = editor_status_type.notfound
                        end
                    end
                end
                default
                    if !char.iscntrl(key)
                        char_buffer += key
                    end
                end
            end
        end
        darwin.update_drawable()
    end

    function run_info()
        darwin.fit_drawable()
        draw_basic_frame()
        pic.draw_line(2, pic.get_height() - 1, pic.get_width() - 3, pic.get_height() - 1, darwin.pixel(' ', darwin.yellow, darwin.yellow))
        pic.draw_string(2, pic.get_height() - 1, "File Info: " + char_buffer + " (Press Q to close)", darwin.pixel(' ', darwin.white, darwin.yellow))
        if darwin.is_kb_hit()
            if char.tolower(darwin.get_kb_hit()) == 'q'
                editor_status = editor_status_type.normal
                force_refresh()
            end
        end
        darwin.update_drawable()
    end
# Main Function
    function run(args)
        if args.size > 2
            system.out.println("Wrong Arguments")
        end
        darwin.load()
        darwin.set_frame_limit(60)
        if args.size == 2
            file_path = args[1]
        else
            file_path = darwin.ui.input_box("Darwin UCGL Text Editor", "Please enter a file path: ", "", false)
        end
        load_file(file_path)
        darwin.fit_drawable()
        pic = darwin.get_drawable()
        loop
            switch editor_status
                case editor_status_type.normal
                    run_normal()
                end
                case editor_status_type.asking
                    run_unsaved_asking()
                end
                case editor_status_type.confirm
                    run_unsaved_confirm()
                end
                case editor_status_type.setup
                    run_find_setup()
                end
                case editor_status_type.notfound
                    run_notfound()
                end
                case editor_status_type.finding
                    run_finding()
                end
                case editor_status_type.replace
                    run_replace()
                end
                case editor_status_type.info
                    run_info()
                end
            end
        end
    end
end
# Start Process
(new texteditor).run(context.cmd_args)