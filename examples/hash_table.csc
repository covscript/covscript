import hash_table
var ht=hash_table.create(20)
ht->insert("Hello":2)
ht->insert("World":4)
ht->insert("Hahaha":5)
++ht->get("Hello")
ht->print()
