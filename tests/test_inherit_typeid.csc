class class1
end
class class2
end
class child1 extends class1
end
class child2 extends class1
end
class child3 extends child1
end
if typeid class1 == typeid (new class1)
    system.out.println("Pass Test 0")
end
if typeid class1 != typeid class2
    system.out.println("Pass Test 1")
end
if typeid child1 != typeid child2
    system.out.println("Pass Test 2")
end
if is_a(typeid child1, typeid class1)
    system.out.println("Pass Test 3")
end
if is_a(typeid child2, typeid class1)
    system.out.println("Pass Test 4")
end
if !is_a(typeid child1, typeid child2)
    system.out.println("Pass Test 5")
end
if is_a(typeid child3, typeid class1)
    system.out.println("Pass Test 6")
end
if is_a(typeid child3, typeid child1)
    system.out.println("Pass Test 7")
end
if !is_a(typeid child3, typeid child2)
    system.out.println("Pass Test 8")
end
