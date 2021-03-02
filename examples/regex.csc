import regex
var reg=regex.build("\\S+")
@begin
var s="Some people, when confronted with a problem, think "+
"\"I know, I'll use regular expressions.\""+
" Now they have two problems."
@end
loop
    var res=reg.search(s)
    if !res.ready()||res.empty()
        break
    end
    system.out.println(res.str(0))
    s=res.suffix()
end