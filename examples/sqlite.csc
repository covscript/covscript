import sqlite_ext
import sqlite
function print_result(stmt)
	for i=0,i<stmt.column_count(),++i
		system.out.print(stmt.column_decltype(i)+"\t")
		system.out.print(stmt.column_name(i))
		system.out.print("=\t")
		switch stmt.column_type(i)
			case sqlite.integer
				system.out.println(stmt.column_integer(i))
			end
			case sqlite.real
				system.out.println(stmt.column_real(i))
			end
			case sqlite.text
				system.out.println(stmt.column_text(i))
			end
		end
	end
end
var db=sqlite.open("test.db")
var stmt=db.prepare("select * from test where b=?")
stmt.bind_real(1,3.14)
stmt.exec()
print_result(stmt)
stmt.reset()
stmt.clear_bindings()
stmt.bind_real(1,6.28)
stmt.exec()
print_result(stmt)
stmt=null
loop
	system.out.print(">>")
	var result=sqlite_ext.exec(db,system.in.getline())
	foreach row in result
		foreach it in row
			system.out.print(it.type+"\t")
			system.out.print(it.name)
			system.out.print("=\t")
			system.out.println(it.data)
		end
	end
end
