# Mini DBMS System (C++ / Qt)

A Mini Database Management System implemented in C++ with a Qt GUI, supporting SQL-like commands, custom parsing, and file-based persistence using JSON.

This project demonstrates core DBMS concepts such as:

Database & table management

SQL parsing

Data validation

CRUD operations

GUI integration with a backend engine

🚀 Features

SQL-like command support

Multiple databases & tables

Persistent storage using JSON files

Primary Key validation

SELECT with column filtering and WHERE clause

UPDATE & DELETE operations

Error handling for invalid SQL

Qt GUI for interactive execution

🛠️ Technologies & Tools

C++ (OOP)

Qt 6 (Widgets)

CMake

STL (map, vector, filesystem)

JSON (file-based storage)

Git & GitHub

📁 Project Structure
MiniDBMS_GUI/
│
├── core/
│   ├── Database.cpp / Database.h
│   ├── Parser.cpp / Parser.h
│   ├── Table.cpp / Table.h
│   ├── Column.h
│   ├── Row.h
│   └── Utils.cpp / Utils.h
│
├── data/
│   └── school/
│       
│
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
├── CMakeLists.txt
└── .gitignore

⚙️ Architecture Overview

Parser
Responsible for parsing SQL commands into structured objects (CreateTableCmd, InsertCmd, SelectCmd, etc.).

Database Engine
Executes parsed commands, manages tables in memory, and syncs them with JSON files on disk.

Table Layer
Stores columns, types, and rows, and provides insert/update/delete operations.

Qt GUI
Acts as the frontend, sending SQL commands to the engine and displaying results in a table or output box.

🧪 Supported SQL Queries & Examples
🔹 Create Database
CREATE DATABASE school;
USE school;

🔹 Create Table
```sql
CREATE TABLE students (
    id INT,
    name STRING,
    grade STRING
);
```

🔹 SELECT (Empty Table)
```sql
SELECT * FROM students;
```

🔹 INSERT – Single Row (With Column List)
```sql
INSERT INTO students (id, name, grade)
VALUES (1, "Ahmed", 80);

INSERT INTO students (id, name, grade)
VALUES (2, "Sara", 85);
```
```sql
SELECT * FROM students;
```


🔹 INSERT – Without Column List
```sql
INSERT INTO students VALUES (3, "Hany", "19");
```

🔹 INSERT – Multiple Rows
```sql
INSERT INTO students (id, name, grade)
VALUES
(5, "Lina", 98),
(6, "Neno", 55),
(7, "Mohamed", 77);

INSERT INTO students VALUES
(8, "Hamed", "88"),
(9, "Omar", "70");
```
```sql
SELECT * FROM students;
```

🔹 INSERT Validation (Expected Error)
```sql
INSERT INTO students (name, grade)
VALUES ("Ahmed", 80);
```

❌ Error: number of values does not match columns

🔹 SELECT Queries (Columns + WHERE)
```sql
SELECT * FROM students;
SELECT name, grade FROM students;
SELECT grade FROM students;
SELECT name, grade FROM students WHERE id = "1";
SELECT * FROM students WHERE name = "Hamed";
```
🔹 UPDATE Queries
```sql
UPDATE students SET grade = "20";
SELECT * FROM students;
```
```sql
🔹 Invalid SQL Handling
INSRT INTO students VALUES (5, "Test", "50");
CREATE students;
```

❌ Invalid SQL syntax handled safely

🔹 Create Another Table
```sql
CREATE TABLE courses (
    student_id INT,
    course STRING
);
```
```sql
INSERT INTO courses VALUES (1, "Programming");
INSERT INTO courses VALUES (2, "Databases");
```

```sql
SELECT * FROM courses;
```

🔹 DELETE Queries
```sql
SELECT * FROM students;

DELETE FROM students WHERE name = "Omar";
SELECT * FROM students;

DELETE FROM students;
```

🔹 Verify Empty Table
```sql
SELECT * FROM students;
```
🔹 DROP TABLE
```sql
DROP TABLE students;
SELECT * FROM students;
```
🔹 DROP DATABASE
```sql
DROP DATABASE school;
USE school;
```

🧠 Implementation Highlights

SQL commands are parsed manually (no external DB libraries).

Tables are stored in memory using std::map<std::string, Table>.

Data persistence is achieved via JSON files.

Qt GUI communicates with the DB engine using string-based execution.

SELECT results are returned using a special marker (__TABLE__) to distinguish tabular output.

🎯 Learning Outcomes

Strong understanding of DBMS fundamentals

Practice with OOP design in C++

Experience integrating GUI with backend logic

