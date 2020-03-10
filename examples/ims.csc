import sqlite

namespace people
    struct data
        var name=new string
        var age=new number
        var sex=new string
        var id=new string
    end

    struct record
        var id=new string
        var data=new string
    end
end

function exec_sql(db,sql,callback,arg)
    var stmt=db.prepare(sql)
    loop
        stmt.exec()
        callback(stmt,arg)
    until stmt.done()
end

function create_table(db,path)
    function callback(stmt,arg)
    end
    db=sqlite.open(path)
    exec_sql(db,"CREATE TABLE PEOPLE_DATA(NAME TEXT,AGE INT,SEX TEXT,ID TEXT)",callback,null)
    exec_sql(db,"CREATE TABLE PEOPLE_RECORD(ID TEXT,DATA TEXT)",callback,null)
end

function read_data_by_name(db,name)
    var stmt=db.prepare("SELECT * FROM PEOPLE_DATA WHERE NAME=?")
    stmt.bind_text(1,name)
    stmt.exec()
    var dat=gcnew people.data
    dat->name=stmt.column_text(0)
    dat->age=stmt.column_integer(1)
    dat->sex=stmt.column_text(2)
    dat->id=stmt.column_text(3)
    return dat
end

function read_data_by_id(db,id)
    var stmt=db.prepare("SELECT * FROM PEOPLE_DATA WHERE ID=?")
    stmt.bind_text(1,id)
    stmt.exec()
    var dat=gcnew people.data
    dat->name=stmt.column_text(0)
    dat->age=stmt.column_integer(1)
    dat->sex=stmt.column_text(2)
    dat->id=stmt.column_text(3)
    return dat
end

function add_data(db,dat)
    var stmt=db.prepare("INSERT INTO PEOPLE_DATA VALUES(?,?,?,?)")
    stmt.bind_text(1,dat->name)
    stmt.bind_integer(2,dat->age)
    stmt.bind_text(3,dat->sex)
    stmt.bind_text(4,dat->id)
    stmt.exec()
end

var print=[](val)->system.out.print(val)
var println=[](val)->system.out.println(val)
var db=null

function print_data(dat)
    println("People Data:")
    print("Name:")
    println(dat->name)
    print("Age:")
    println(dat->age)
    print("Sex:")
    println(dat->sex)
    print("ID:")
    println(dat->id)
end

function main_f1()
    loop
    	println("")
        println("CovScript IMS Data Finder")
        println("1.Find by Name")
        println("2.Find by ID")
        println("3.Return Main Menu")
        switch system.console.getch()
            case '1'
                print("Please enter the name of people that you want to find:")
                print_data(read_data_by_name(db,system.in.getline()))
            end
            case '2'
                print("Please enter the id of people that you want to find:")
                print_data(read_data_by_id(db,system.in.getline()))
            end
            case '3'
                return
            end
        end
    end
end

function main_f2()
	println("")
    println("CovScript IMS Data Recoder")
    var dat=gcnew people.data
    print("Name:")
    dat->name=system.in.getline()
    print("Age:")
    dat->age=(system.in.getline()).to_number()
    print("Sex:")
    dat->sex=system.in.getline()
    print("ID:")
    dat->id=system.in.getline()
    add_data(db,dat)
end

function main_menu()
    loop
        println("")
        println("CovScript IMS Main Menu")
        println("Please select a function:")
        println("1.Query data")
        println("2.Insert data")
        println("3.Return Start Menu")
        println("4.Exit CovScript IMS")
        switch system.console.getch()
            case '1'
                main_f1()
            end
            case '2'
                main_f2()
            end
            case '3'
                   return
            end
            case '4'
                system.exit(0)
            end
        end
    end
end

function start_f1()
    system.console.clrscr()
    print("Please enter the path of database:")
    create_table(db,system.in.getline())
    main_menu()
end

function start_f2()
    system.console.clrscr()
    print("Please enter the path of database:")
    db=sqlite.open(system.in.getline())
    main_menu()
end

function start_menu()
    loop
        system.console.clrscr()
        println("CovScript IMS Start Menu")
        println("Please select a function:")
        println("1.Create Database")
        println("2.Open Database")
        println("3.Exit CovScript IMS")
        switch system.console.getch()
            case '1'
                start_f1()
            end
            case '2'
                start_f2()
            end
            case '3'
                system.exit(0)
            end
        end
    end
end

try
    start_menu()
catch e
    println(e.what)
end