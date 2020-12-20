@charset: utf8
var mons = {"一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"}
var days = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"}

function align(str)
    if str.size == 1
        return "0" + str
    else
        return str
    end
end

function get_time_str(t)
    var hour = null
    if t.hour < 13
        hour = " 上午 " + align(to_string(t.hour))
    else
        hour = " 下午 " + align(to_string(t.hour - 12))
    end
    return to_string(t.year + 1900) + "年" + mons[t.mon] + to_string(t.mday) + "日 " + days[t.wday] + align(hour) + ":" + align(to_string(t.min)) + ":" + align(to_string(t.sec))
end

loop
    system.console.clrscr()
    system.out.println("本地时间：" + get_time_str(runtime.local_time()))
    system.out.println("标准时间：" + get_time_str(runtime.utc_time()))
    runtime.delay(100)
end