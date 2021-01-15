@charset: gbk
var mons = {"һ��", "����", "����", "����", "����", "����", "����", "����", "����", "ʮ��", "ʮһ��", "ʮ����"}
var days = {"������", "����һ", "���ڶ�", "������", "������", "������", "������"}

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
        hour = " ���� " + align(to_string(t.hour))
    else
        hour = " ���� " + align(to_string(t.hour - 12))
    end
    return to_string(t.year + 1900) + "��" + mons[t.mon] + to_string(t.mday) + "�� " + days[t.wday] + align(hour) + ":" + align(to_string(t.min)) + ":" + align(to_string(t.sec))
end

loop
    system.console.clrscr()
    system.out.println("����ʱ�䣺" + get_time_str(runtime.local_time()))
    system.out.println("��׼ʱ�䣺" + get_time_str(runtime.utc_time()))
    runtime.delay(100)
end