#include "Login.h"
#include "report.h"
#include "Database.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <conio.h>   
#include <string>

#include "Menu.h"
#include "User.h"
using namespace std;

bool authenticateUser(const std::string& username, const std::string& password,
std::string& role, int& userID)
{
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return false;

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT userID, role FROM user WHERE username = ? AND password = ?"
            )
        );

        pstmt->setString(1, username);
        pstmt->setString(2, password);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            userID = res->getInt("userID");
            role = res->getString("role");
            return true;
        }
        return false;
    }
    catch (sql::SQLException& e) {
        cerr << "Login error: " << e.what() << endl;
        return false;
    }
}

string getPassword() {
    string password;
    char ch;

    while (true) {
        ch = _getch();   // Read char WITHOUT echo

        if (ch == '\r') {          // ENTER key
            cout << endl;
            break;
        }
        else if (ch == '\b') {     // BACKSPACE
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";   // Remove *
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
    }
    return password;
}

bool loginMenu() {
    string username, password, role;
    int userID = -1;
    cout << "                _____       _____" << endl;
    cout << "     .........   {     }     {     }" << endl;
    cout << "    (>>\\zzzzzz [======================]" << endl;
    cout << "    ( <<<\\lllll_\\\\ _        _____    \\\\" << endl;
    cout << "   _,`-,\\<   __#:\\\\::_    __#:::_:__  \\\\" << endl;
    cout << "  /    . `--,#::::\\\\:::___#::::/__+_\\ _\\\\" << endl;
    cout << " /  _  .`-    `--,/_~~~~~~~~~~~~~~~~~~~~  -,_ " << endl;
    cout << ":,// \\ .         .  '--,____________   ______`-," << endl;
    cout << " :: o |.         .  ___ \\_____||____\\+/     ||~ \\" << endl;
    cout << " :;   ;-,_       . ,' _`,\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\\" << endl;
    cout << " \\ \\_/ _ :`-,_   . ; / \\\\ ====================== /" << endl;
    cout << "  \\__/~ /     `-,.; ; o |\\___[~~~]_ASCII__[~~~]__:" << endl;
    cout << "     ~~~          ; :   ;~ ;  ~~~         ;~~~::;" << endl;
    cout << "                   \\ \\_/ ~/               ::::::;" << endl;
    cout << "                    \\_/~~/                 \\:::/ " << endl;
    cout << "                      ~~~                   ~~~" << endl;
    cout << "\n-----------------------------------------------\n\n";
    cout << "    KIDS ELECTRIC VEHICLE RENTAL MANAGEMENT    \n\n";
    cout << "-----------------------------------------------\n";
    cout << "Login: \n";
    cout << "Type x to exit system \n\n";

    cout << "Username: ";
    cin >> username;
    if (username == "x") exit(0);

    cout << "Password: ";
    password = getPassword();
    if (password == "x") exit(0);

    if (authenticateUser(username, password, role, userID)) {
        cout << "\nLOGIN SUCCESSFUL. WELCOME " << username
            << " (" << role << ")\n";

        if (role == "Admin") {
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
        adminMenu(userID);
    }
        else {
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            staffMenu(userID);
        }
        return true;
    }
    else {
        cout << "\nInvalid username or password.\n";
        cout << "Press ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return false;

    }
}


void adminMenu(int userID) {
    string input;
    int choice;
    do {
        cout << "\n-----------------------------------\n\n";
        cout << "             ADMIN MENU    \n\n";
        cout << "-----------------------------------\n";
        cout << "1. Kids Electric Vehicle Menu (CRUD)\n";
        cout << "2. Add Staff Account\n";
        cout << "3. View Sales Report\n";
        cout << "4. View Vehicle Utilization Report\n";
        cout << "5. Logout\n";

        while (true) {
            cout << "Enter number: ";
            cin >> input;

            // Check digits only
            if (!all_of(input.begin(), input.end(), ::isdigit)) {
                cout << "Invalid input. Numbers only.\n";
                continue;
            }

            try {
                choice = stoi(input);
            }
            catch (...) {
                cout << "Invalid number. Try again.\n";
                continue;
            }

            if (choice >= 1 && choice <= 5) break;
            cout << "Please enter a number between 1 and 5.\n";
        }


        switch (choice) {
        case 1:
            system("cls");
            MainMenu(userID); 
            break;
        case 2:
            system("cls");
            UserMenu();
            break;
        case 3:
            system("cls");
            salesReport();
            break;
        case 4:
            system("cls");
            kidsEVUtilizationReport();
                break;
        case 5:
            system("cls");
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 5);
}


void staffMenu(int userID) {
    cout << "\n=== STAFF MENU ===\n";
    MainMenu(userID);   
}

