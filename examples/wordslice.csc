#!/usr/bin/env cs

# This program requires branch string-enhance

constant echo = system.out.println

class String
    var chars = {}

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

    ##### internal functions
    function __assert(cond)
        if !cond; throw "assertion failed"; end
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
        return clone(csStr).substr(position, 1)
    end

    function __getChar16String(csStr, position)
        return clone(csStr).substr(position, 2)
    end

    function __getChar24String(csStr, position)
        return clone(csStr).substr(position, 3)
    end

    function __getChar32String(csStr, position)
        return clone(csStr).substr(position, 4)
    end

    function __initWith(csStr)
        for i = 0, i < csStr.size(), i
            if __isChar8(csStr, i)
                chars.push_back(__getChar8String(csStr, i))
                ++i
                continue
            end
            if __isChar16(csStr, i)
                chars.push_back(__getChar16String(csStr, i))
                i += 2
                continue
            end
            if __isChar24(csStr, i)
                # Chinese characters are char24
                chars.push_back(__getChar24String(csStr, i))
                i += 3
                continue
            end
            if __isChar32(csStr, i)
                chars.push_back(__getChar8String(csStr, i))
                i += 4
                continue
            end
        end
    end

    ###### public functions
    function size()
        return chars.size()
    end

    ###### override functions
    function equal(orig)
        if this == orig
            return true
        end
        if typeid this != typeid orig
            return false
        end
        return this.chars == orig.chars;
    end
end

function str(csStr)
    var s = new String
    s.__initWith(csStr)
    return s
end

var s0 = "你好"
var s1 = str("你好")
echo(s0.size())
echo(s1.size())
