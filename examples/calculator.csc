function is_op(ch)
    return ch == '+' || ch == '-' || ch == '*' || ch == '/'
end

class calculator
    var op_stack = new array
    var num_stack = new array

    function lex(expr)
        var status = 0, last_status = 2, bracket_lev = 0, expected_neg = true
        var buff = ""
        for i = 0, i < expr.size, null
            var ch = expr[i]
            switch status
                case 0
                    if char.isspace(ch)
                        continue
                    end
                    if char.isdigit(ch) || ch == '.'
                        status = 1
                        continue
                    end
                    if is_op(ch)
                        status = 2
                        continue
                    end
                    if ch == '(' 
                        status = 3
                        expected_neg = true
                        continue
                    end
                    if ch == ')' 
                        status = 4
                        continue
                    end
                    throw runtime.exception("Wrong Format Expression")
                end
                case 1
                    if last_status == 1
                        throw runtime.exception("Wrong Format Expression")
                    end
                    if !char.isdigit(ch) && ch!='.'
                        num_stack.push_back(last_status == -1 ? -buff.to_number() : buff.to_number())
                        last_status = status
                        status = 0
                        buff = ""
                    else
                        buff += ch
                        ++i
                    end
                end
                case 2
                    if expected_neg && ch == '-'
                        last_status = -1
                        status = 0
                        ++i
                        continue
                    end
                    if last_status == 2 || last_status == -1
                        if ch == '-'
                            last_status = -1
                            status = 0
                            ++i
                            continue
                        else
                            throw runtime.exception("Wrong Format Expression")
                        end
                    end
                    op_stack.push_back(ch)
                    last_status = status
                    status = 0
                    ++i
                end
                case 3
                    ++bracket_lev
                    num_stack.push_back(ch)
                    last_status = 2
                    status = 0
                    ++i
                    continue
                end
                case 4
                    --bracket_lev
                    num_stack.push_back(ch)
                    last_status = 1
                    status = 0
                    ++i
                end
            end
            expected_neg = false
        end
        if status == 1
            num_stack.push_back(last_status == -1 ? -buff.to_number() : buff.to_number())
            last_status = status
        end
        if last_status == 2
            throw runtime.exception("Hanging operator.")
        end
        if bracket_lev != 0
            throw runtime.exception("Bracket does not closed.")
        end
    end

    function parse(rec)
        while !num_stack.empty()
            var lhs = num_stack.front
            num_stack.pop_front()
            if lhs == '('
                lhs = parse(0)
            end
            if !num_stack.empty() && num_stack.front == ')'
                num_stack.pop_front()
                return lhs
            end
            if op_stack.empty()
                return lhs
            end
            var op = op_stack.front
            if op == '+' || op == '-'
                if rec == 1
                    return lhs
                end
                op_stack.pop_front()
                var rhs = parse(1)
                if op == '+'
                    num_stack.push_front(lhs + rhs)
                else
                    num_stack.push_front(lhs - rhs)
                end
            else
                op_stack.pop_front()
                var rhs = num_stack.front
                num_stack.pop_front()
                if rhs == '('
                    rhs = parse(0)
                end
                if op == '*'
                    num_stack.push_front(lhs * rhs)
                else
                    num_stack.push_front(lhs / rhs)
                end
            end
        end
    end

    function eval(expr)
        lex(expr)
        return parse(0)
    end
end

loop
    system.out.println((new calculator).eval(system.in.getline()))
end