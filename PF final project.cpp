#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

string ADMINUSER = "ADMIN";
string ADMINPASS = "ADMIN123";

string uid;

struct Teacher {
    string id;
    string name;
    string subject;
    string password;
};

struct AMark {
    double quizes[5] = { 0 };
    double assigns[3] = { 0 };
    double mids[2] = { 0 };
    double final = 0;
};

struct Student {
    string id;
    string password;
    string name;
    string cls;
    vector<string> subjects;
    vector<string> grades;
    vector<AMark> evals;
    double cgpa = 0.0;
};

vector<Teacher> teachers;
vector<Student> students;

void saveTeachersToFile() {
    ofstream fout;
    fout.open("teachers.txt", ios::out);
    for (int i = 0; i < teachers.size(); i++) {
        fout << teachers[i].id << "," << teachers[i].name << "," << teachers[i].subject << endl;
    }
    fout.close();
    fout.open("Tpasses.txt");
    for (int i = 0; i < teachers.size(); i++) {
        fout << teachers[i].id << " " << teachers[i].password << endl;
    }
    fout.close();
}

void saveStudentsToFile() {
    ofstream fout;
    fout.open("students.txt", ios::out);
    for (int i = 0; i < students.size(); i++) {
        fout << students[i].id << "," << students[i].name << "," << students[i].cls << ",";
        for (int j = 0; j < students[i].subjects.size(); j++) {
            fout << students[i].subjects[j] << ":" << students[i].grades[j];
            if (students[i].subjects.size() - 1) fout << "|";
        }
        fout << "," << students[i].cgpa << endl;
    }
    fout.close();
    fout.open("marks.txt", ios::out);
    for (int i = 0; i < students.size(); i++) {
        fout << students[i].id << " ";
        fout << students[i].password << " ";
        for (int j = 0; j < students[i].subjects.size(); j++) {
            for (int k = 0; k < 5; k++) {
                fout << students[i].evals[j].quizes[k] << " ";
            }
            for (int k = 0; k < 3; k++) {
                fout << students[i].evals[j].assigns[k] << " ";
            }
            for (int k = 0; k < 2; k++) {
                fout << students[i].evals[j].mids[k] << " ";
            }
            fout << students[i].evals[j].final << " ";
        }
        fout << endl;
    }
    fout.close();
}

void loadTeachersFromFile() {
    ifstream fin;
    fin.open("teachers.txt", ios::in);
    teachers.clear();
    string line;
    while (getline(fin, line)) {
        Teacher teacher;
        int pos1 = line.find(',');
        int pos2 = line.rfind(',');
        teacher.id = line.substr(0, pos1);
        teacher.name = line.substr(pos1 + 1, pos2 - pos1 - 1);
        teacher.subject = line.substr(pos2 + 1);
        teachers.push_back(teacher);
    }
    fin.close();
    fin.open("Tpasses.txt");
    for (int i = 0; i < teachers.size(); i++) {
        fin >> teachers[i].id;
        fin >> teachers[i].password;
    }
    fin.close();
}

void loadStudentsFromFile() {
    ifstream fin;
    fin.open("students.txt");
    if (!fin.is_open()) {
        cerr << "Error: Unable to open students.txt file.\n";
        return;
    }

    students.clear();
    string line;

    while (getline(fin, line)) {
        Student student;

        // Find positions of the commas
        int pos1 = line.find(',');
        int pos2 = line.rfind(',');

        // Extract ID, Name, and Class
        student.id = line.substr(0, pos1);
        int nameEnd = line.find(',', pos1 + 1);
        student.name = line.substr(pos1 + 1, nameEnd - pos1 - 1);

        int classEnd = line.find(',', nameEnd + 1);
        student.cls = line.substr(nameEnd + 1, classEnd - nameEnd - 1);

        // Extract subjects and grades
        string subjectsGrades = line.substr(classEnd + 1, pos2 - classEnd - 1);
        int current = 0;

        while ((current = subjectsGrades.find('|')) != string::npos) {
            string pair = subjectsGrades.substr(0, current);
            int colon = pair.find(':');
            if (colon != string::npos) {
                student.subjects.push_back(pair.substr(0, colon));
                student.grades.push_back(pair.substr(colon + 1));
                student.evals.emplace_back(AMark()); // Add default evaluation marks for each subject
            }
            subjectsGrades.erase(0, current + 1);
        }

        // Handle the last subject-grade pair if not processed
        if (!subjectsGrades.empty()) {
            int colon = subjectsGrades.find(':');
            if (colon != string::npos) {
                student.subjects.push_back(subjectsGrades.substr(0, colon));
                student.grades.push_back(subjectsGrades.substr(colon + 1));
                student.evals.emplace_back(AMark());
            }
        }

        // Extract CGPA
        student.cgpa = stod(line.substr(pos2 + 1)); // Fixing the misplaced GPA parsing

        // Add the student to the list
        students.push_back(student);
    }
    fin.close();
    fin.open("marks.txt");
    for (int i = 0; i < students.size(); i++) {
        fin >> students[i].id;
        fin >> students[i].password;
        for (int j = 0; j < students[i].subjects.size(); j++) {
            for (int k = 0; k < 5; k++) {
                fin >> students[i].evals[j].quizes[k];
            }
            for (int k = 0; k < 3; k++) {
                fin >> students[i].evals[j].assigns[k];
            }
            for (int k = 0; k < 2; k++) {
                fin >> students[i].evals[j].mids[k];
            }
            fin >> students[i].evals[j].final;
        }
    }
    fin.close();
}

bool isValidid(bool isteacher, string id, bool isnew = true) {
    if (id.size() != 4) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        if (id[i] < '0' || id[i] > '9')
            return false;
    }
    if (isnew) {
        if (isteacher) {
            for (int i = 0; i < teachers.size(); i++) {
                if (id == teachers[i].id)
                    return false;
            }
        }
        else {
            for (int i = 0; i < students.size(); i++) {
                if (id == students[i].id)
                    return false;
            }
        }
    }
    return true;
}

void addTeacher() {
    cin.ignore();
    string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(0));
    Teacher teacher;
    cout << "Enter Teacher ID: ";
    getline(cin, teacher.id);
    while (!isValidid(true, teacher.id)) {
        cout << "Invalid! try again: ";
        getline(cin, teacher.id);
    }
    cout << "Enter Teacher Name: ";
    getline(cin, teacher.name);
    while (teacher.name.size() == 0) {
        cout << "you must Enter something! ";
        getline(cin, teacher.name);
    }
    cout << "Enter Teacher Subject: ";
    getline(cin, teacher.subject);
    while (teacher.subject.size() == 0) {
        cout << "you must Enter something! ";
        getline(cin, teacher.subject);
    }
    for (int i = 0; i < 8; ++i)
    {
        teacher.password += chars[rand() % chars.size()];
    }
    teachers.push_back(teacher);
    saveTeachersToFile();
    cout << "Teacher added successfully.\n";
}

void deleteTeacher() {
    cin.ignore();
    string id;
    cout << "Enter Teacher ID to delete: ";
    cin >> id;
    while (!isValidid(true, id, false)) {
        cout << "Invalid! try again: ";
        cin >> id;
    }
    for (int i = 0; i < teachers.size(); i++) {
        if (teachers[i].id == id) {
            for (int j = i; j < teachers.size() - 1; j++) {
                teachers[j].id = teachers[j + 1].id;
                teachers[j].name = teachers[j + 1].name;
                teachers[j].subject = teachers[j + 1].subject;
                teachers[j].password = teachers[j + 1].password;
            }
            teachers.pop_back();
            saveTeachersToFile();
            cout << "Teacher deleted successfully.\n";
            return;
        }
    }
    cout << "Teacher not found.\n";
}

double calculate(string grade) {
    switch (grade[0]) {
    case 'A':
        if (grade.size() == 2 && grade[1] == '+')return 4;
        if (grade.size() == 2 && grade[1] == '-')return 3.67;
        return 4;
    case 'B':
        if (grade.size() == 2 && grade[1] == '+')return 3.3;
        if (grade.size() == 2 && grade[1] == '-')return 2.7;
        return 3.0;
    case 'C':
        if (grade.size() == 2)return 2.3;
        return 2.0;
    case 'D':return 1.7;
    case 'F':return 1.5;
    }
    return 0;
}

void addStudent() {
    cin.ignore();
    string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    Student student;
    cout << "Enter Student ID: ";
    getline(cin, student.id);
    while (!isValidid(false, student.id)) {
        cout << "Invalid! try again:";
        getline(cin, student.id);
    }
    cout << "Enter Student Name: ";
    getline(cin, student.name);
    cout << "Enter Student Class: ";
    getline(cin, student.cls);
    int numSubjects;
    cout << "Enter number of subjects: ";
    cin >> numSubjects;
    while (cin.fail()) {
        cin.clear();
        cin.ignore(100, '\n');
        cout << "INVALID INPUT!try again: ";
        cin >> numSubjects;
    }
    for (int i = 0; i < numSubjects; ++i) {
        string subject;
        string grade;
        cout << "Enter Subject " << i + 1 << ": ";
        cin >> subject;
        cin.ignore();
        student.subjects.push_back(subject);
        student.grades.push_back("F");
        student.evals.emplace_back(AMark());
    }
    student.cgpa = 1.3;

    for (int i = 0; i < 8; ++i)
    {
        student.password += chars[rand() % chars.size()];
    }
    students.push_back(student);
    saveStudentsToFile();
    cout << "Student added successfully.\n";
}

void deleteStudent() {
    cin.ignore();
    string id;
    cout << "Enter Student ID to delete: ";
    getline(cin, id);
    while (!isValidid(false, id, false)) {
        cout << "Invalid! try again:";
        getline(cin, id);
    }
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == id) {
            for (int j = i; j < students.size() - 1; j++) {
                students[j].id = students[j + 1].id;
                students[j].name = students[j + 1].name;
                students[j].cls = students[j + 1].cls;
                students[j].cgpa = students[j + 1].cgpa;
                students[j].password = students[j + 1].cgpa;
                students[j].subjects.clear();
                students[j].grades.clear();
                students[j].evals.clear();
                for (int k = 0; k < students[j + 1].subjects.size(); k++) {
                    students[j].evals.emplace_back(AMark());
                    students[j].subjects.push_back(students[j + 1].subjects[k]);
                    students[j].grades.push_back(students[j + 1].grades[k]);
                    for (int l = 0; l < 5; l++) {
                        students[j].evals[k].quizes[l] = students[j + 1].evals[k].quizes[l];
                    }
                    for (int l = 0; l < 3; l++) {
                        students[j].evals[k].assigns[l] = students[j + 1].evals[k].assigns[l];
                    }
                    for (int l = 0; l < 2; l++) {
                        students[j].evals[k].mids[l] = students[j + 1].evals[k].mids[l];
                    }
                    students[j].evals[k].final = students[j + 1].evals[k].final;
                }
            }
            students.pop_back();
            saveStudentsToFile();
            cout << "Student deleted successfully.\n";
            return;
        }
    }
    cout << "Student not found.\n";
}

void viewGrades() {
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == uid) {
            cout << "Grades for " << students[i].name << ":\n";
            for (int j = 0; j < students[i].subjects.size(); j++) {
                cout << students[i].subjects[j] << ": " << students[i].grades[j] << endl;
            }
            return;
        }
    }
}

void viewCGPA() {
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == uid) {
            cout << "CGPA for " << students[i].name << ": " << fixed << setprecision(2) << students[i].cgpa << endl;
            return;
        }
    }
}

void adminMenu() {
    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Teacher\n2. Delete Teacher\n3. Exit\nChoose an option: ";
        cin >> choice;
        while (cin.fail()) {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Dont Enter invalid stuff!";
            cin >> choice;
        }
        switch (choice) {
        case 1: addTeacher(); break;
        case 2: deleteTeacher(); break;
        case 3: break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 3);
}

bool loginAdmin() {
    cin.ignore();
    string user;
    string pass;
    cout << "Enter Admin User:";
    getline(cin, user);
    cout << "Enter Admin Password:";
    getline(cin, pass);
    if (user != ADMINUSER) {
        cout << "Access denied! username is incorrect" << endl;
        return false;
    }
    if (pass != ADMINPASS) {
        cout << "Access denied! password is incorrect" << endl;
        return false;
    }
    cout << "Access granted!" << endl;
    return true;

}

bool loginTeacher() {
    cin.ignore();
    string password;
    cout << "Enter teacher id:";
    getline(cin, uid);
    while (!isValidid(true, uid, false)) {
        cout << "Invalid! try again: ";
        getline(cin, uid);
    }
    for (int i = 0; i < teachers.size(); i++) {
        if (teachers[i].id == uid) {
            cout << "Enter your password:";
            getline(cin, password);
            if (teachers[i].password == password) {
                cout << "Access granted!" << endl;
                return true;
            }
            cout << "Access denied password is incorrect!" << endl;
            return false;
        }
    }
    cout << "Access denied no such id found!" << endl;
    return false;
}

bool loginStudent() {
    cin.ignore();
    string password;
    cout << "Enter student id:";
    cin >> uid;
    while (!isValidid(true, uid, false)) {
        cout << "Invalid! try again: ";
        getline(cin, uid);
    }
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == uid) {
            cin.ignore();
            cout << "Enter your password:";
            getline(cin, password);
            if (students[i].password == password) {
                cout << "Access granted!" << endl;
                return true;
            }
            cout << "Access denied password is incorrect!" << endl;
            return false;
        }
    }
    cout << "Access denied no such id found!" << endl;
    return false;
}

void changeStudentpass() {
    cin.ignore();
    string pass;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == uid) {
            cout << "Enter the old password: ";
            cin >> pass;
            cin.ignore();
            if (pass == students[i].password) {
                cout << "Enter the new password: ";
                cin >> pass;
                cin.ignore();
                students[i].password = pass;
                saveStudentsToFile();
                cout << "password changed!" << endl;
                return;
            }
            cout << "old password not correct!" << endl;
            return;
        }
    }
}

void changeTeacherpass() {
    cin.ignore();
    string pass;
    for (int i = 0; i < teachers.size(); i++) {
        if (teachers[i].id == uid) {
            cout << "Enter the old password: ";
            cin >> pass;
            cin.ignore();
            if (pass == teachers[i].password) {
                cout << "Enter the new password: ";
                cin >> pass;
                cin.ignore();
                teachers[i].password = pass;
                saveTeachersToFile();
                cout << "password changed!" << endl;
                return;
            }
            cout << "old password not correct!" << endl;
            return;
        }
    }
}

void averageCGPA() {
    double totalcgpa = 0;
    for (int i = 0; i < students.size(); i++) {
        totalcgpa += students[i].cgpa;
    }
    cout << "Average CGPA for all students: " << totalcgpa / students.size() << endl;
}

void listPassedStudents() {
    for (int i = 0; i < students.size(); i++) {
        if (students[i].cgpa >= 2.0) {
            cout << "Student" << i + 1 << ":" << students[i].id << "," << students[i].name << "," << students[i].cgpa << endl;
        }
    }
}

void listFailedStudents() {
    for (int i = 0; i < students.size(); i++) {
        if (students[i].cgpa < 2.0) {
            cout << "Student" << i + 1 << ":" << students[i].id << "," << students[i].name << "," << students[i].cgpa << endl;
        }
    }
}

void assignMarks() {
    cin.ignore();
    string id;
    int choice, num;
    double totalmarks, obtainedmarks, sumofallmarks, totalGrades;
    cout << "Enter student id:";
    getline(cin, id);
    while (!isValidid(false, id, false)) {
        cout << "Invalid! try again:";
        getline(cin, id);
    }
    for (int i = 0; i < students.size(); i++) {
        if (students[i].id == id) {
            for (int j = 0; j < teachers.size(); j++) {
                if (teachers[j].id == uid) {
                    for (int k = 0; k < students[i].subjects.size(); k++) {
                        if (students[i].subjects[k] == teachers[j].subject) {
                            do {
                                sumofallmarks = 0;
                                totalGrades = 0;
                                cout << "Enter the choice to enter marks\n1. Quizes\n2. Assignments\n3. Mids\n4. Finals\n5. Exit\nEnter choice: ";
                                cin >> choice;
                                while (cin.fail()) {
                                    cin.clear();
                                    cin.ignore(100, '\n');
                                    cout << "Dont Enter invalid stuff!";
                                    cin >> choice;
                                }
                                switch (choice) {
                                case 1:
                                    cout << "Enter the quiz number:";
                                    cin >> num;
                                    while (cin.fail() || num > 5 || num < 1) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> num;
                                    }
                                    cout << "Enter the total marks:";
                                    cin >> totalmarks;
                                    while (cin.fail()) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> totalmarks;
                                    }
                                    cout << "Enter obtained marks:";
                                    cin >> obtainedmarks;
                                    while (cin.fail() || obtainedmarks > totalmarks) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> obtainedmarks;
                                    }
                                    students[i].evals[k].quizes[num - 1] = obtainedmarks / totalmarks * 2.5;

                                    for (int l = 0; l < 5; l++)sumofallmarks += students[i].evals[k].quizes[l];
                                    for (int l = 0; l < 3; l++)sumofallmarks += students[i].evals[k].assigns[l];
                                    for (int l = 0; l < 2; l++)sumofallmarks += students[i].evals[k].mids[l];
                                    sumofallmarks += students[i].evals[k].final;

                                    if (sumofallmarks > 80)students[i].grades[k] = "A+";
                                    else if (sumofallmarks > 75)students[i].grades[k] = "A";
                                    else if (sumofallmarks > 70)students[i].grades[k] = "A-";
                                    else if (sumofallmarks > 65)students[i].grades[k] = "B+";
                                    else if (sumofallmarks > 60)students[i].grades[k] = "B";
                                    else if (sumofallmarks > 55)students[i].grades[k] = "B-";
                                    else if (sumofallmarks > 50)students[i].grades[k] = "C+";
                                    else if (sumofallmarks > 45)students[i].grades[k] = "C";
                                    else if (sumofallmarks > 40)students[i].grades[k] = "D";
                                    else students[i].grades[k] = "F";

                                    for (int l = 0; l < students[i].grades.size(); l++) {
                                        totalGrades += calculate(students[i].grades[k]);
                                    }
                                    students[i].cgpa = totalGrades / students[i].grades.size();
                                    saveStudentsToFile();
                                    break;
                                case 2:
                                    cout << "Enter the assignment number:";
                                    cin >> num;
                                    while (cin.fail() || num > 3 || num < 1) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> num;
                                    }
                                    cout << "Enter the total marks:";
                                    cin >> totalmarks;
                                    while (cin.fail()) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> totalmarks;
                                    }
                                    cout << "Enter obtained marks:";
                                    cin >> obtainedmarks;
                                    while (cin.fail() || obtainedmarks > totalmarks) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> obtainedmarks;
                                    }
                                    students[i].evals[k].assigns[num - 1] = obtainedmarks / totalmarks * 2.5;

                                    for (int l = 0; l < 5; l++)sumofallmarks += students[i].evals[k].quizes[l];
                                    for (int l = 0; l < 3; l++)sumofallmarks += students[i].evals[k].assigns[l];
                                    for (int l = 0; l < 2; l++)sumofallmarks += students[i].evals[k].mids[l];
                                    sumofallmarks += students[i].evals[k].final;

                                    if (sumofallmarks > 80)students[i].grades[k] = "A+";
                                    else if (sumofallmarks > 75)students[i].grades[k] = "A";
                                    else if (sumofallmarks > 70)students[i].grades[k] = "A-";
                                    else if (sumofallmarks > 65)students[i].grades[k] = "B+";
                                    else if (sumofallmarks > 60)students[i].grades[k] = "B";
                                    else if (sumofallmarks > 55)students[i].grades[k] = "B-";
                                    else if (sumofallmarks > 50)students[i].grades[k] = "C+";
                                    else if (sumofallmarks > 45)students[i].grades[k] = "C";
                                    else if (sumofallmarks > 40)students[i].grades[k] = "D";
                                    else students[i].grades[k] = "F";

                                    for (int l = 0; l < students[i].grades.size(); l++) {
                                        totalGrades += calculate(students[i].grades[k]);
                                    }
                                    students[i].cgpa = totalGrades / students[i].subjects.size();
                                    saveStudentsToFile();
                                    break;
                                case 3:
                                    cout << "Enter the Mid number:";
                                    cin >> num;
                                    while (cin.fail() || num > 2 || num < 1) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> num;
                                    }
                                    cout << "Enter the total marks:";
                                    cin >> totalmarks;
                                    while (cin.fail()) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> totalmarks;
                                    }
                                    cout << "Enter obtained marks:";
                                    cin >> obtainedmarks;
                                    while (cin.fail() || obtainedmarks > totalmarks) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> obtainedmarks;
                                    }
                                    students[i].evals[k].mids[num - 1] = obtainedmarks / totalmarks * 15;

                                    for (int l = 0; l < 5; l++)sumofallmarks += students[i].evals[k].quizes[l];
                                    for (int l = 0; l < 3; l++)sumofallmarks += students[i].evals[k].assigns[l];
                                    for (int l = 0; l < 2; l++)sumofallmarks += students[i].evals[k].mids[l];
                                    sumofallmarks += students[i].evals[k].final;

                                    if (sumofallmarks > 80)students[i].grades[k] = "A+";
                                    else if (sumofallmarks > 75)students[i].grades[k] = "A";
                                    else if (sumofallmarks > 70)students[i].grades[k] = "A-";
                                    else if (sumofallmarks > 65)students[i].grades[k] = "B+";
                                    else if (sumofallmarks > 60)students[i].grades[k] = "B";
                                    else if (sumofallmarks > 55)students[i].grades[k] = "B-";
                                    else if (sumofallmarks > 50)students[i].grades[k] = "C+";
                                    else if (sumofallmarks > 45)students[i].grades[k] = "C";
                                    else if (sumofallmarks > 40)students[i].grades[k] = "D";
                                    else if (sumofallmarks > 35)students[i].grades[k] = "F";
                                    else students[i].grades[k] = "N";

                                    for (int l = 0; l < students[i].grades.size(); l++) {
                                        totalGrades += calculate(students[i].grades[k]);
                                    }
                                    students[i].cgpa = totalGrades / students[i].subjects.size();
                                    saveStudentsToFile();
                                    break;
                                case 4:
                                    cout << "Enter the total marks:";
                                    cin >> totalmarks;
                                    while (cin.fail()) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> totalmarks;
                                    }
                                    cout << "Enter obtained marks:";
                                    cin >> obtainedmarks;
                                    while (cin.fail() || obtainedmarks > totalmarks) {
                                        cin.clear();
                                        cin.ignore(100, '\n');
                                        cout << "Dont Enter invalid stuff!";
                                        cin >> obtainedmarks;
                                    }
                                    students[i].evals[k].final = obtainedmarks / totalmarks * 50;

                                    for (int l = 0; l < 5; l++)sumofallmarks += students[i].evals[k].quizes[l];
                                    for (int l = 0; l < 3; l++)sumofallmarks += students[i].evals[k].assigns[l];
                                    for (int l = 0; l < 2; l++)sumofallmarks += students[i].evals[k].mids[l];
                                    sumofallmarks += students[i].evals[k].final;

                                    if (sumofallmarks > 80)students[i].grades[k] = "A+";
                                    else if (sumofallmarks > 75)students[i].grades[k] = "A";
                                    else if (sumofallmarks > 70)students[i].grades[k] = "A-";
                                    else if (sumofallmarks > 65)students[i].grades[k] = "B+";
                                    else if (sumofallmarks > 60)students[i].grades[k] = "B";
                                    else if (sumofallmarks > 55)students[i].grades[k] = "B-";
                                    else if (sumofallmarks > 50)students[i].grades[k] = "C+";
                                    else if (sumofallmarks > 45)students[i].grades[k] = "C";
                                    else if (sumofallmarks > 40)students[i].grades[k] = "D";
                                    else if (sumofallmarks > 35)students[i].grades[k] = "F";
                                    else students[i].grades[k] = "N";

                                    for (int l = 0; l < students[i].grades.size(); l++) {
                                        totalGrades += calculate(students[i].grades[k]);
                                    }
                                    students[i].cgpa = totalGrades / students[i].subjects.size();
                                    saveStudentsToFile();
                                }
                            } while (choice != 5);
                            return;
                        }
                    }
                    cout << "This student does not study your subject!" << endl;
                }
            }
        }
    }
}

void teacherMenu() {
    int choice;
    do {
        cout << "\nTeacher Menu:\n";
        cout << "1. Add Student\n2. Delete Student\n3. Assign marks\n4. Show average CGPA\n5. List all passing students\n";
        cout << "6. list all failing students\n7. Change password\n8. Exit\nChoose an option: ";
        cin >> choice;
        while (cin.fail()) {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Dont Enter invalid stuff!";
            cin >> choice;
        }
        switch (choice) {
        case 1:addStudent(); break;
        case 2:deleteStudent(); break;
        case 3:assignMarks(); break;
        case 4:averageCGPA(); break;
        case 5:listPassedStudents(); break;
        case 6:listFailedStudents(); break;
        case 7:changeTeacherpass(); break;
        case 8:break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 8);
}

void studentMenu() {
    int choice;
    do {
        cout << "\nStudent Menu:\n";
        cout << "1. View Grades\n2. View CGPA\n3. Change password\n4. Exit\nChoose an option: ";
        cin >> choice;
        while (cin.fail()) {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Dont Enter invalid stuff!";
            cin >> choice;
        }
        switch (choice) {
        case 1: viewGrades(); break;
        case 2: viewCGPA(); break;
        case 3: changeStudentpass(); break;
        case 4: break;
        default: cout << "Invalid choice.\n";
        }
    } while (choice != 4);
}

int main() {
    loadTeachersFromFile();
    loadStudentsFromFile();
    int role;
    do {
        cout << "\nMain Menu:\n";
        cout << "1. Admin\n2. Teacher\n3. Student\n4. Exit\nChoose your role: ";
        cin >> role;
        while (cin.fail()) {
            cin.clear();
            cin.ignore(100, '\n');
            cout << "Dont Enter invalid stuff!";
            cin >> role;
        }
        switch (role) {
        case 1: if (loginAdmin())adminMenu(); break;
        case 2: if (loginTeacher())teacherMenu(); break;
        case 3: if (loginStudent())studentMenu(); break;
        case 4: break;
        default: cout << "Invalid role.\n";
        }
    } while (role != 4);
    return 0;
}