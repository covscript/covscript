import "./hash_table.csp"
var ht=hash_table.create(20)
hash_table.insert(ht,"Hello":2)
++hash_table.get(ht,"Hello")
hash_table.print(ht)
