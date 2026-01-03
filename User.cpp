#include "User.h"
#include "Database.h"
#include <iostream>
#include <iomanip>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>
using namespace std;

void viewUsers() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT * FROM user")
        );

        cout << "\n=== USER LIST ===\n";
        cout << left
            << setw(5) << "ID"
            << setw(12) << "Username"
            << setw(25) << "Full Name"
            << setw(10) << "Role"
            << setw(20) << "Contact"
            << endl;

        cout << string(90, '-') << endl;

        while (res->next()) {
            cout << left
                << setw(5) << res->getInt("userID")
                << setw(12) << res->getString("username")
                << setw(25) << res->getString("fullName")
                << setw(10) << res->getString("role")
                << setw(20) << res->getString("contact")
                << endl;
        }
        cout << string(90, '-') << endl;

    }
    catch (sql::SQLException& e) {
        cerr << "Error retrieving user: " << e.what() << endl;
    }
}

bool showSelectedUser(sql::Connection* con, int userID) {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM user WHERE userID = ?")
        );
        pstmt->setInt(1, userID);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            cout << "No user found with ID " << userID << ".\n";
            return false;
        }

        cout << "\n=== SELECTED USER ===\n\n";
        cout << left
            << setw(5) << "ID"
            << setw(12) << "Username"
            << setw(25) << "Full Name"
            << setw(10) << "Role"
            << setw(20) << "Contact"
            << endl;

        cout << string(90, '-') << endl;

        cout << left
            << setw(5) << res->getInt("userID")
            << setw(12) << res->getString("username")
            << setw(25) << res->getString("fullName")
            << setw(10) << res->getString("role")
            << setw(20) << res->getString("contact")
            << endl;

        cout << string(90, '-') << endl;

        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
}

void addUser() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    while (true) {
        string username, password, fullName, role, contact;

        cout << "\n=== ADD NEW USER ===\n";
        cout << "(Type 'exit' anytime to return)\n\n";

        cin.ignore();

        // Username
        cout << "Enter Username: ";
        getline(cin, username);
        if (username == "exit") {
            system("cls");
            return;
        }
        // Password
        cout << "Enter Password: ";
        getline(cin, password);
        if (password == "exit") {
            system("cls");
            return;
        }

        // Full Name
        cout << "Enter Full Name: ";
        getline(cin, fullName);
        if (fullName == "exit") {
            system("cls");
            return;
        }

        // Role
        while (true) {
            cout << "Enter Role (Admin/Staff): ";
            getline(cin, role);
            if (role == "exit") {
                system("cls");
                return;
            }

            if (role == "Admin" || role == "admin" || role == "Staff" || role == "staff")
                break;

            cout << "Incorrect role. Use Admin or Staff.\n";
        }

        // Contact
        cout << "Enter Contact: ";
        getline(cin, contact);
        if (contact == "exit") {
            system("cls");
            return;
        }

        try {
            unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "INSERT INTO user (username, password, role, fullName, contact) "
                    "VALUES (?, ?, ?, ?, ?)"
                )
            );

            pstmt->setString(1, username);
            pstmt->setString(2, password);
            pstmt->setString(3, role);
            pstmt->setString(4, fullName);
            pstmt->setString(5, contact);

            pstmt->execute();
            cout << "\nUSER ADDED SUCCESSFULLY\n";


        }
        catch (sql::SQLException& e) {
            cerr << "Error: " << e.what() << endl;
        }
        // OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to Add again: ";
        cin >> opt;

        if (opt == "m" || opt == "M") {
            system("cls");
            return;
        }
        if (opt == "r" || opt == "R") {
            system("cls");
            continue;
        }
        return;
    }
}

void updateUser() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== UPDATE USER ===\n";
    viewUsers();

    string input;
    int id;

    while (true) {
        cout << "\nEnter User ID to update: ";
        cin >> input;

        if (input == "exit") {
            system("cls");
            return;
        }
        try {
            id = stoi(input);
            break;
        }
        catch (...) {
            cout << "Invalid ID.\n";
        }
    }

    if (!showSelectedUser(con, id)) return;

    cin.ignore();

    string newUsername, newPassword, newFullName, newRole, newContact;

    cout << "Enter New Username: ";
    getline(cin, newUsername);

    cout << "Enter New Password: ";
    getline(cin, newPassword);

    cout << "Enter New Full Name: ";
    getline(cin, newFullName);

    cout << "Enter New Role (Admin/Staff): ";
    getline(cin, newRole);

    cout << "Enter New Contact: ";
    getline(cin, newContact);

    char confirm;
    cout << "\nConfirm update? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "Update cancelled.\n";
        return;
    }

    unique_ptr<sql::PreparedStatement> pstmt(
        con->prepareStatement(
            "UPDATE user SET "
            "username = IF(?='', username, ?), "
            "password = IF(?='', password, ?), "
            "fullName = IF(?='', fullName, ?), "
            "role = IF(?='', role, ?), "
            "contact = IF(?='', contact, ?) "
            "WHERE userID = ?"
        )
    );

    pstmt->setString(1, newUsername);
    pstmt->setString(2, newUsername);

    pstmt->setString(3, newPassword);
    pstmt->setString(4, newPassword);

    pstmt->setString(5, newFullName);
    pstmt->setString(6, newFullName);

    pstmt->setString(7, newRole);
    pstmt->setString(8, newRole);

    pstmt->setString(9, newContact);
    pstmt->setString(10, newContact);

    pstmt->setInt(11, id);

    try {
        int rows = pstmt->executeUpdate();
        if (rows > 0) {
            cout << "\nUSER UPDATED SUCCESSFULLY\n";
            showSelectedUser(con, id);
        }
        else {
            cout << "No changes made.\n";
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error updating user: " << e.what() << endl;
    }
    // OPTION: MENU or REPEAT
    string opt;
    cout << "\nEnter 'm' for Menu or 'r' to update again: ";
    cin >> opt;

    if (opt == "m" || opt == "M") {
        system("cls");
        return;
    }
    if (opt == "r" || opt == "R") {
        system("cls");
        updateUser();
        return;
    }
}

void deleteUser() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== DELETE USER ===\n";
    viewUsers();

    string input;
    int id;

    while (true) {
        cout << "\nEnter User ID to delete: ";
        cin >> input;

        if (input == "exit") {
            system("cls");
            return;
        }
        try {
            id = stoi(input);
            break;
        }
        catch (...) {
            cout << "Invalid ID.\n";
        }
    }
        if (!showSelectedUser(con, id)) return;

        char confirm;
        cout << "\nAre you sure? (Y/N): ";
        cin >> confirm;

        if (confirm != 'Y' && confirm != 'y') {
            cout << "Deletion cancelled.\n";
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            return;
        }

        try {
            unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("DELETE FROM user WHERE userID = ?")
            );
            pstmt->setInt(1, id);

            int rows = pstmt->executeUpdate();
            if (rows > 0)
                cout << "\nUSER DELETED\n";
            else
                cout << "\nUser not found.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting user: " << e.what() << endl;
        }
        // OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to delete again: ";
        cin >> opt;

        if (opt == "m" || opt == "M") {
            system("cls");
            return;
        }
        if (opt == "r" || opt == "R") {
            system("cls");
            deleteUser();
            return;
        }
}

void UserMenu() {
    int choice;

    do {
        cout << "\n\n----------------------------------";
        cout << "\n           USER  MENU             \n";
        cout << "----------------------------------\n";
        cout << "\n1. VIEW Users\n";
        cout << "2. ADD User\n";
        cout << "3. UPDATE User\n";
        cout << "4. DELETE User\n";
        cout << "5. Return to Main Menu\n";

        while (true) {
            cout << "\nEnter choice: ";

            if (cin >> choice && choice >= 1 && choice <= 5)
                break;

            cout << "Invalid input.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1: 
            system("cls");
            viewUsers(); 
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            break;
        case 2:
            system("cls");
            addUser(); 
            break;
        case 3: 
            system("cls");
            updateUser(); 
            break;
        case 4: 
            system("cls");
            deleteUser(); 
            break;
        case 5:
            system("cls");
            cout << "Returning...\n"; 
            break;
        }
    } while (choice != 5);
}
