class test
    var str = "Hello"
    function to_string()
        return str
    end
end
@begin
system.out.println({
    {}, null, 3,14 : 'A', typeid system.out, typeid test,
    range(10), new test, {gcnew test, 3,14, 'A'}.to_list()
})
@end
@begin
system.out.println({
    "abc" : "def",
    "ghi" : null
}.to_hash_map())
@end
@begin
system.out.println({
    "abc", "def",
    "ghi"
}.to_hash_set())
@end