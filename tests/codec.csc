import codec
@begin
var json_str =
"{" +
    "\"null\":null," +
    "\"bool\":true," +
    "\"int\":12345," +
    "\"real\":3.14," +
    "\"string\":\"Hello\"," +
    "\"array\":[" +
    "    {\"null\":null, \"bool\":true},"+
    "    {\"int\":12345, \"real\":3.14}" +
    "]," +
    "\"object\":{" +
        "\"bool\":true," +
        "\"int\":12345," +
        "\"real\":3.14"  +
    "}" +
"}"
@end
var json = null, obj = null
json = codec.json.from_stream(iostream.ifstream("./test.json"))
json.to_stream(iostream.ofstream("./out.json"))
json = codec.json.from_string(json_str)
obj = codec.json.to_var(json)
obj.array[1].real = math.constants.e
json = codec.json.from_var(obj)
system.out.print(codec.json.to_string(json))
@begin
obj = {
    "null":null,
    "bool":true,
    "int":12345,
    "real":3.14,
    "string":"Hello",
    "array":{
        {"null":null, "bool":true}.to_hash_map(),
        {"int":12345, "real":3.14}.to_hash_map()
    },
    "object":{
        "null":null,
        "bool":true,
        "int":12345,
        "real":3.14
    }.to_hash_map()
}.to_hash_map()
@end
json.to_stream(system.out)
system.out.println("")
