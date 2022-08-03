@charset: utf8

namespace node_type
    constant add = 0
    constant sub = 1
    constant mul = 2
    constant div = 3
end

struct expr_tree_node
    var left = null
    var right = null
    var value = 0
end

struct expr_gen_opt
    # 0 = integer, 1 = decimal
    var operand_type = 0
    # 0 is excluded
    var operand_range = {2, 99}
    var output_range = {0, 1000}
    var expr_depth_range = {2, 5}
    var excluded_operator = {}.to_hash_set()
    var calculation_error = 1
end

var log = false

function decimal_loss(opt, val)
    var str = to_string(val)
    var pos = str.find(".", 0)
    var comp = 0
    if pos != -1
        if to_integer(str[pos + opt.calculation_error + 1]) > to_integer('4')
            comp = 0.1^opt.calculation_error
        end
        str = str.substr(0, pos + opt.calculation_error + 1)
    end
    return str.to_number() + comp
end

function gen_operand(opt)
    var val = 0
    if opt.operand_type == 0
        loop
            val = math.randint(opt.operand_range[0], opt.operand_range[1])
        until val != 0
    else
        loop
            val = math.rand(opt.operand_range[0], opt.operand_range[1])
        until math.abs(val) > 0.1^opt.calculation_error
        val = decimal_loss(opt, val)
    end
    var node = new expr_tree_node
    node.value = val
    if log
        system.out.println("#VAL" + val)
    end
    return move(node)
end

function gen_expr(depth, opt)
    var dep = depth - 1
    if dep > 0
        var root = new expr_tree_node
        loop
            root.value = math.randint(0, 3)
        until !opt.excluded_operator.exist(root.value)
        if log
            system.out.println("#OP" + root.value)
        end
        if math.randint(0, 1) == 0
            if log
                system.out.println("#BL")
                system.out.println("#L" + dep)
            end
            root.left = gen_operand(opt)
            if log
                system.out.println("#R" + dep)
            end
            root.right = gen_expr(dep, opt)
        else
            if log
                system.out.println("#BR")
                system.out.println("#L" + dep)
            end
            root.left = gen_expr(dep, opt)
            if log
                system.out.println("#R" + dep)
            end
            root.right = gen_operand(opt)
        end
        return move(root)
    else
        return gen_operand(opt)
    end
end

function gen(opt)
    return gen_expr(math.randint(opt.expr_depth_range[0], opt.expr_depth_range[1]), opt)
end

function eval(expr)
    if expr.left == null || expr.right == null
        return expr.value
    end
    switch expr.value
        case 0
            return eval(expr.left) + eval(expr.right)
        end
        case 1
            return eval(expr.left) - eval(expr.right)
        end
        case 2
            return eval(expr.left) * eval(expr.right)
        end
        case 3
            return eval(expr.left) / eval(expr.right)
        end
    end
end

function print_impl(ofs, flag, expr)
    if expr.left == null || expr.right == null
        ofs.print(expr.value)
        return
    end
    if flag && expr.value < 2
        ofs.print("(")
    end
    var nxt_flag = expr.value < 2 ? false : true
    print_impl(ofs, nxt_flag, expr.left)
    switch expr.value
        case 0
            ofs.print(" + ")
        end
        case 1
            ofs.print(" - ")
            if expr.right.left != null && expr.right.right != null
                if expr.right.value == 1
                    nxt_flag = true
                end
            end
        end
        case 2
            ofs.print(" × ")
        end
        case 3
            ofs.print(" ÷ ")
        end
    end
    print_impl(ofs, nxt_flag, expr.right)
    if flag && expr.value < 2
        ofs.print(")")
    end
end

function print(ofs, expr)
    print_impl(ofs, false, expr)
end

import imgui, imgui_font
using  imgui

#foreach it in range(10)
#    var expr = gen(opt)
#    print(system.out, expr)
#    system.out.println(" = " + eval(expr))
#end

var app = window_application(400, 640, "Four problem generators")
var font = add_font_extend_cn(imgui_font.source_han_sans, 18)
var window_opened = false
var opt = null
var txt_buffs = null
var chk_values = null
var expr_list = new array
var have_ans = false
var output = new string
var error_val = -1
var have_error = false
style_color_light()

function reset()
    opt = new expr_gen_opt
    txt_buffs = {"2", "99", "0", "1000", "2", "5", "5", "1"}
    chk_values = {true, true, true, true}
end

reset()

@begin
var error_map = {
    0:{vec4(0, 0, 255, 255), "不能为空"},
    1:{vec4(255, 0, 0, 255), "格式有误"},
    2:{vec4(255, 0, 255, 255), "范围有误"}
}.to_hash_map()
@end

function check_format(str)
    if str.empty()
        return 0
    end
    var multi_dot = false
    foreach i in range(str.size)
        var ascii = to_integer(str[i])
        if ascii < to_integer('0') || ascii > to_integer('9')
            if i == 0 && str[i] == '-' && str.size > 1
                continue
            else
                if !multi_dot && i != 0 && str[i] == '.' && str.size - i > 1
                    multi_dot = true
                    continue
                else
                    return 1
                end
            end
        end
    end
    return -1
end

function check_range(str0, str1)
    var err = 0
    if (err = check_format(str0)) != -1
        return check_format(str1)
    end
    if (err = check_format(str1)) != -1
        return err
    end
    if str0.to_number() <= str1.to_number()
        return -1
    else
        return 2
    end
end

function check_expr_range(expr, rbeg, rend)
    if expr.left == null || expr.right == null
        return expr.value
    end
    var val = 0
    switch expr.value
        case 0
            val = check_expr_range(expr.left, rbeg, rend) + check_expr_range(expr.right, rbeg, rend)
        end
        case 1
            val = check_expr_range(expr.left, rbeg, rend) - check_expr_range(expr.right, rbeg, rend)
        end
        case 2
            val = check_expr_range(expr.left, rbeg, rend) * check_expr_range(expr.right, rbeg, rend)
        end
        case 3
            val = check_expr_range(expr.left, rbeg, rend) / check_expr_range(expr.right, rbeg, rend)
        end
    end
    if val >= rbeg && val <= rend
        return val
    else
        throw runtime.exception("FAILED!!!")
    end
end

function run_gen()
    foreach it in txt_buffs
        if check_format(it) != -1
            return
        end
    end
    opt.operand_range = {txt_buffs[0].to_number(), txt_buffs[1].to_number()}
    opt.output_range = {txt_buffs[2].to_number(), txt_buffs[3].to_number()}
    opt.expr_depth_range = {txt_buffs[4].to_number(), txt_buffs[5].to_number()}
    opt.calculation_error = txt_buffs[7].to_number()
    foreach it in range(4)
        if !chk_values[it]
            opt.excluded_operator.insert(it)
        end
    end
    expr_list = new array
    foreach it in range(txt_buffs[6].to_number())
        loop
            var expr = gen(opt)
            var val = eval(expr)
            try
                check_expr_range(expr, opt.output_range[0], opt.output_range[1])
                expr_list.push_back(expr : val)
                break
            catch e
                continue
            end
        end
    end
end

function gen_output(ans)
    var sstr = new iostream.char_buff
    foreach it in expr_list
        link os = sstr.get_ostream()
        print(os, it.first)
        if ans
            os.println(" = " + decimal_loss(opt, it.second))
        else
            os.println(" =")
        end
    end
    output = sstr.get_string()
end

loop
    if app.is_closed()
        system.exit(0)
    end
    app.prepare()
    push_font(font)
    begin_window("Main", window_opened, {flags.no_resize, flags.no_move, flags.no_collapse, flags.no_title_bar})
        set_window_pos(vec2(0, 0))
        set_window_size(vec2(app.get_window_width(), app.get_window_height()))
        var have_error = false
        text("数值类型")
        radio_button("整数", opt.operand_type, 0)
        same_line()
        radio_button("小数", opt.operand_type, 1)
        text("数值范围")
        input_text("最小值##txt0", txt_buffs[0], 32)
        if (error_val = check_format(txt_buffs[0])) != -1
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        input_text("最大值##txt1", txt_buffs[1], 32)
        if (error_val = check_range(txt_buffs[0], txt_buffs[1])) != -1
            have_error = true
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        text("结果范围")
        input_text("最小值##txt2", txt_buffs[2], 32)
        if (error_val = check_format(txt_buffs[2])) != -1
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        input_text("最大值##txt3", txt_buffs[3], 32)
        if (error_val = check_range(txt_buffs[2], txt_buffs[3])) != -1
            have_error = true
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        text("运算类型")
        check_box("加法", chk_values[0])
        same_line()
        check_box("减法", chk_values[1])
        same_line()
        check_box("乘法", chk_values[2])
        same_line()
        check_box("除法", chk_values[3])
        text("表达式深度范围")
        input_text("最小值##txt4", txt_buffs[4], 32)
        if (error_val = check_format(txt_buffs[4])) != -1
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        input_text("最大值##txt5", txt_buffs[5], 32)
        if (error_val = check_range(txt_buffs[4], txt_buffs[5])) != -1
            have_error = true
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        end
        text("生成表达式数量")
        input_text("##txt6", txt_buffs[6], 32)
        var width = get_item_width()
        if (error_val = check_format(txt_buffs[6])) != -1
            have_error = true
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        else
            if txt_buffs[6].find(".", 0) != -1
                have_error = true
                same_line()
                text_colored(vec4(255, 0, 0, 255), "格式有误")
            else
                if txt_buffs[6].to_number() < 1
                    have_error = true
                    same_line()
                    text_colored(vec4(255, 0, 255, 255), "范围有误")
                end
            end
        end
        text("保留精度")
        input_text("##txt7", txt_buffs[7], 32)
        if (error_val = check_format(txt_buffs[7])) != -1
            have_error = true
            same_line()
            link arr = error_map[error_val]
            text_colored(arr[0], arr[1])
        else
            if txt_buffs[7].find(".", 0) != -1
                have_error = true
                same_line()
                text_colored(vec4(255, 0, 0, 255), "格式有误")
            else
                if txt_buffs[7].to_number() < 1
                    have_error = true
                    same_line()
                    text_colored(vec4(255, 0, 255, 255), "范围有误")
                end
            end
        end
        text("输出内容")
        columns(2, "#OUTPUT_AREA", false)
        set_column_width(0, width)
        push_item_width(width)
        input_text_multiline("##OUTPUT", output, 1024)
        pop_item_width()
        next_column()
        check_box("答案", have_ans)
        if button("开始生成") && !have_error
            run_gen()
            gen_output(have_ans)
        end
        if button("输出文本")
            gen_output(have_ans)
        end
        if button("清空输出")
            output = new string
        end
        if button("恢复默认")
            reset()
        end
    end_window()
    pop_font()
    app.render()
end