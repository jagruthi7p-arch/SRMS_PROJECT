# SRMS_PROJECT
This project provides a strong foundation for understanding real-world file-based data management using C. It is ideal for academic submissions, demonstrations, and practical learning of C programming concepts.
📝 Student Record Management System in C

A role-based student information management system built using the C programming language.
The project demonstrates file handling, input validation, menu-driven programming, and multi-level access control.

📌 Overview

The Student Record Management System is a console-based C application designed to securely store, manage, and retrieve student records.
The system uses external text files for permanent storage and supports multiple user roles with different access permissions.

The program showcases the practical use of:

File handling (student.txt, credentials.txt)

Authentication & authorization

String processing & validation

Modular programming

Cross-platform compatibility (Windows & Linux)

🔐 User Roles & Permissions
Role	Display	Search	Add	Update	Delete
Admin	✔	✔	✔	✔	✔
Staff	✔	✔	✔	✔	✖
User	✔	✖	✖	✖	✖
Guest	✔	✔	✖	✖	✖
🛠️ Features

Secure login from credentials.txt

Role-based access control

Permanent data storage in student.txt

Add, display, search, update, and delete student records

Duplicate roll number and duplicate name protection

Numeric-only validation for marks

BOM/CRLF-safe string handling

Safe update/delete using temporary file

Menu-driven user interface

📁 File Structure
SRMS_PROJECT/
│
├── student_app.c          # Main source code
├── student.txt            # Student records file
├── credentials.txt        # Login credentials & role definitions
├── README.md              # GitHub documentation
└── screenshots/           # (Optional) screenshots for UI preview

⚙️ Compilation Instructions
Windows (MSVC / cl.exe)

Open Developer Command Prompt:

cl.exe /Zi /EHsc /Fe:student_app.exe student_app.c

Windows (MinGW / GCC)
gcc student_app.c -o student_app.exe

Linux / macOS (GCC)
gcc student_app.c -o student_app
./student_app

▶️ How to Run

Make sure credentials.txt and student.txt exist (empty file is allowed for student.txt).

Run the program:

Windows:
student_app.exe

Linux:
./student_app


Enter username and password when prompted.

🔑 Sample credentials.txt format
admin 123 ADMIN
staff 123 STAFF
user 123 USER
guest 123 GUEST

📸 Screenshots (Optional)

Add your screenshots inside a screenshots/ folder and reference them here:

![Login Menu](screenshots/login.png)
![Admin Menu](screenshots/admin_menu.png)

🚀 Future Enhancements

These features can be added to improve the system:

Graphical User Interface (GUI)

Database integration (SQLite/MySQL)

Password encryption instead of plain text

Sorting and filtering student data

Export records to CSV/PDF

Attendance & performance module

Network-based multi-user support

Backup and restore system

📚 References

AticleWorld – “Student Record System in C (Mini Project)”

ITSourceCode – “Student Record System Project in C with Source Code”

CodeWithC – “Mini Project in C – Student Record System”

GeeksforGeeks – “What is DFD in Software Engineering?”

IBM – “Data Flow Diagram — Understanding DFDs”

📄 License

This project is free to use for educational and academic purposes.
You may modify and distribute it with proper attribution.
