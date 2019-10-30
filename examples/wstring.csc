#!/usr/bin/env cs

# This program requires branch string-enhance

constant echo = system.out.println

class String
    ##### private:
    var __chars = {}

    var __0x80 = 128
    var __0xA0 = 160
    var __0xB0 = 176
    var __0xC0 = 192
    var __0xD0 = 208
    var __0xE0 = 224
    var __0xF0 = 240
    var __0xED = 237
    var __0x10000 = 65536
    var __0x0f = 15
    var __0x1f = 31
    var __0x3f = 63

    ##### private:
    function __assert(cond)
        if !cond; throw runtime.exception("assertion failed"); end
    end

    function __bitwiseAnd8(x, y)
        return math.bitwise_and32(to_integer(x), to_integer(y))
    end

    function __codePointAt(csStr, position)
        return csStr.char_at(position)
    end

    function __isChar8(csStr, position)
        __assert(position + 1 <= csStr.size())
        return __bitwiseAnd8(__codePointAt(csStr, position), __0x80) == 0
    end

    function __isChar16(csStr, position)
        __assert(position + 2 <= csStr.size())
        @begin
        return __bitwiseAnd8(__codePointAt(csStr, position), __0xE0) == __0xC0
            && __bitwiseAnd8(__codePointAt(csStr, position + 1), __0xC0) == __0x80
        @end
    end

    function __isChar24(csStr, position)
        __assert(position + 3 <= csStr.size())
        @begin
        return __bitwiseAnd8(__codePointAt(csStr, position), __0xF0) == __0xE0
            && __bitwiseAnd8(__codePointAt(csStr, position + 1), __0xC0) == __0x80
            && __bitwiseAnd8(__codePointAt(csStr, position + 2), __0xC0) == __0x80
        @end
    end

    function __isChar32(csStr, position)
        __assert(position + 6 <= csStr.size())
        @begin
        return __codePointAt(csStr, position) == __0xED
            && __bitwiseAnd8(__codePointAt(csStr, position + 1), __0xF0) == __0xA0
            && __bitwiseAnd8(__codePointAt(csStr, position + 2), __0xC0) == __0x80
            && __codePointAt(csStr, position + 3) == __0xED
            && __bitwiseAnd8(__codePointAt(csStr, position + 4), __0xF0) == __0xB0
            && __bitwiseAnd8(__codePointAt(csStr, position + 5), __0xC0) == __0x80
        @end
    end

    function __getChar8String(csStr, position)
        return csStr.substr(position, 1)
    end

    function __getChar16String(csStr, position)
        return csStr.substr(position, 2)
    end

    function __getChar24String(csStr, position)
        return csStr.substr(position, 3)
    end

    function __getChar32String(csStr, position)
        return csStr.substr(position, 4)
    end

    function __initWithCSString(csStr)
        for i = 0, i < csStr.size(), i
            if __isChar8(csStr, i)
                __chars.push_back(__getChar8String(csStr, i))
                ++i
                continue
            end
            if __isChar16(csStr, i)
                __chars.push_back(__getChar16String(csStr, i))
                i += 2
                continue
            end
            if __isChar24(csStr, i)
                # Chinese characters are char24
                __chars.push_back(__getChar24String(csStr, i))
                i += 3
                continue
            end
            if __isChar32(csStr, i)
                __chars.push_back(__getChar8String(csStr, i))
                i += 4
                continue
            end
        end
    end

    function __initWithChars(chars)
        __chars = chars
    end

    function __wrap(chars)
        var s = new String
        s.__initWithChars(chars)
        return s
    end

    ###### public:
    function size()
        return __chars.size()
    end

    function charAt(position)
        return __chars[position]
    end

    function substr(position)
        var cs = clone(__chars)
        foreach i in range(position)
            cs.pop_front()
        end
        return __wrap(cs)
    end

    function substr_(position, length)
        var cs = clone(__chars)
        foreach i in range(position)
            cs.pop_front()
        end
        foreach i in range(cs.size() - length)
            cs.pop_back()
        end
        return __wrap(cs)
    end

    function show()
        var result = ""
        foreach s in __chars
            result += s
        end
        return result
    end

    ###### public override:
    function equal(orig)
        if this == orig
            return true
        end
        if typeid this != typeid orig
            return false
        end
        return this.__chars == orig.__chars;
    end
end

function str(csStr)
    var s = new String
    s.__initWithCSString(csStr)
    return s
end

echo("=== 测试中文")
var s0 = "你好"
var s1 = str("你好")
echo(s0.size())
echo(s1.size())
echo(s1.substr(1).show())
echo(s1.substr(1) == str("好"))

echo("=== 测试中英文混合")
var s2 = str("哈Hello你好")
echo(s2.size())
echo(s2.substr_(1, 5).show())
echo(s2.substr_(1, 5) == str("Hello"))
echo(s2.substr_(6, 2).show())
echo(s2.substr_(6, 2) == str("你好"))

echo("=== 测试中英空格混合")
var s3 = str("哈 Hello 你好")
echo(s3.size())
echo(s3.substr_(2, 5).show())
echo(s3.substr_(2, 5) == str("Hello"))
echo(s3.substr_(8, 2).show())
echo(s3.substr_(8, 2) == str("你好"))
