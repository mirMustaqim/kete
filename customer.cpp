#include "customer.h"
#include "Database.h"
#include <iostream>

#include <iomanip>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>
using namespace std;

void viewCustomer() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT * FROM customer")
        );

        // header
        std::cout << "\n=== CUSTOMER LIST ===\n";
        std::cout << std::left
            << std::setw(10) << "ID"
            << std::setw(30) << "Customer Name"
            << std::setw(15) << "Customer Number"
            << std::endl;

        std::cout << std::string(85, '-') << std::endl;

        // rows
        while (res->next()) {

            std::cout << std::left
                << std::setw(10) << res->getString("customerID")
                << std::setw(30) << res->getString("customerName")
                << std::setw(20) << res->getString("customerNumber")
                << std::endl;
        }


    }
    catch (sql::SQLException& e) {
        std::cerr << "Error retrieving data: " << e.what() << std::endl;
    }
}


bool showSelectedCustomer(sql::Connection* con, int customerID) {
    try {

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM customer WHERE customerID = ?")
        );
        pstmt->setInt(1, customerID);


        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());


        if (!res->next()) {
            cout << " No customer found with ID " << customerID << ".\n";
            return false;
        }


        cout << "\n=== SELECTED CUSTOMER DETAILS ===\n\n";
        cout << std::left
            << std::setw(10) << "ID"
            << std::setw(20) << "customerName"
            << std::setw(15) << "customerNumber"
            << endl;
        cout << std::string(85, '-') << endl;


        int id = res->getInt("customerID");

        cout << std::left
            << std::setw(10) << id
            << std::setw(20) << res->getString("customerName")
            << std::setw(20) << res->getString("customerNumber")

            << endl;

        cout << std::string(85, '-') << endl;

        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Error retrieving vehicle: " << e.what() << endl;
        return false;
    }
}

void searchCustomer() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== SEARCH CUSTOMER ===\n";
    cout << "(Search by Customer Number | Type 'exit' to return)\n\n";

    string input;

    // Clear buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        cout << "Enter Customer Number: ";
        getline(cin, input);

        if (input == "exit" || input == "Exit") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }

        if (input.empty()) {
            cout << "Customer number cannot be empty.\n";
            continue;
        }

        // digits only validation
        if (input.find_first_not_of("0123456789") != string::npos) {
            cout << "Invalid input. Numbers only.\n";
            continue;
        }

        break;
    }

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT * FROM customer WHERE customerNumber = ?"
            )
        );
        pstmt->setString(1, input);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            cout << "\nNo customer found with number: " << input << "\n";
        }
        else {
            cout << "\n=== SEARCH RESULT ===\n\n";
            cout << left
                << setw(10) << "ID"
                << setw(30) << "Customer Name"
                << setw(20) << "Customer Number"
                << endl;
            cout << string(85, '-') << endl;

            do {
                cout << left
                    << setw(10) << res->getInt("customerID")
                    << setw(30) << res->getString("customerName")
                    << setw(20) << res->getString("customerNumber")
                    << endl;
            } while (res->next());

            cout << string(85, '-') << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error searching customer: " << e.what() << endl;
    }

    // OPTION: MENU or REPEAT
    string opt;
    cout << "\nEnter 'm' for Menu or 'r' to Search again: ";
    cin >> opt;

    if (opt == "m" || opt == "M") {
        system("cls");
        return;
    }
    if (opt == "r" || opt == "R") {
        system("cls");
        searchCustomer();
        return;
    }
}


void addCustomer() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    string name, number;
    int custnum;
    char option;

    cout << "\n=== ADD CUSTOMER ===\n";
    cout << "(Type 'exit' anytime to return to menu)\n\n";

        // Clear buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // === Customer Name ===
        while (true) {
            cout << "Enter Customer Name: ";
            cin >> name;
            if (name == "exit" || name == "Exit") { cout << "Returning to menu...\n"; system("cls"); return; }
            if (!name.empty())
                break;
            cout << "Customer name cannot be empty.\n";
        }


        while (true) {
            cout << "Enter Customer Number: ";
            cin >> number;
            if (number == "exit" || number == "Exit") { cout << "Returning to menu...\n"; system("cls"); return; }
            try {
                custnum = stod(number);
                if (custnum < 0) throw invalid_argument("negative");
                break;
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid number.\n";
            }
        }

        // === Insert into Database ===
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "INSERT INTO customer (customerName, customerNumber) "
                    "VALUES (?, ?)"
                )
            );

            pstmt->setString(1, name);
            pstmt->setString(2, number);
            pstmt->execute();
            cout << "\nSUCCESSFULLY ADDED TO (CUSTOMER) DATABASE\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding customer: " << e.what() << endl;
        }
        // OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to Add again: ";
        cin >> opt;

        if (opt == "m" || opt == "M") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }
        if (opt == "r" || opt == "R") {
            system("cls");
            addCustomer();
            return;
        }
    cout << "Returning to menu...\n";
}


void updateCustomer() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== UPDATE CUSTOMER ===\n";
    cout << "(Type 'exit' anytime to return to menu)\n\n";

    viewCustomer();

    int id;
    string input;

    // 1. ASK FOR Customer ID
    while (true) {
        cout << "\nEnter Customer ID to Update (numeric): ";
        cin >> input;

        if (input == "exit") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }

        try {
            id = stoi(input);
            if (id <= 0) {
                cout << "ID must be greater than 0.\n";
                continue;
            }
        }
        catch (...) {
            cout << "Invalid ID format.\n";
            continue;
        }

        if (!showSelectedCustomer(con, id)) {
            cout << "Customer ID does NOT exist. Please try again.\n";
            continue;
        }

        break;
    }

    string newName, newNumber;

    cin.ignore();
    cout << "ENTER New Customer Name: ";
    getline(cin, newName);
    if (newName == "exit") {
        system("cls");
        return;
    }

    while (true) {
        cout << "Enter Customer Number: ";
        getline(cin, newNumber);

        if (newNumber == "exit") {
            system("cls");
            return;
        }

        if (newNumber.empty()) break; // skip update

        // validate digits only
        if (newNumber.find_first_not_of("0123456789") != string::npos) {
            cout << "Invalid input. Numbers only.\n";
            continue;
        }

        break;
    }


    char confirm;
    cout << "\nAre you sure you want to update this vehicle? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "\nUpdate cancelled.\n";
        // OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to Update again: ";
        cin >> opt;

        if (opt == "m" || opt == "M") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }
        if (opt == "r" || opt == "R") {
            system("cls");
            updateCustomer();
            return;
        }
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->prepareStatement(
            "UPDATE customer SET "
            "customerName = IF(? = '', customerName, ?), "
            "customerNumber = IF(? = '', customerNumber, ?) "
            "WHERE customerID = ?"
        )
    );

    pstmt->setString(1, newName);
    pstmt->setString(2, newName);

    pstmt->setString(3, newNumber);
    pstmt->setString(4, newNumber);

    pstmt->setInt(5, id);

    try {
        int rows = pstmt->executeUpdate();
        if (rows > 0) {
            showSelectedCustomer(con, id);
            cout << "\nCUSTOMER UPDATE SUCCESFULLY.\n";
        }

        else
            cout << "\nCustomer not found or no changes made.\n";
    }
    catch (sql::SQLException& e) {
        cerr << "Error updating vehicle: " << e.what() << endl;
    }
    // OPTION: MENU or REPEAT
    string opt;
    cout << "\nEnter 'm' for Menu or 'r' to Update again: ";
    cin >> opt;

    if (opt == "m" || opt == "M") {
        cout << "Returning to menu...\n";
        system("cls");
        return;
    }
    if (opt == "r" || opt == "R") {
        system("cls");
        updateCustomer();
        return;
    }
}


void deleteCustomer() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== DELETE CUSTOMER ===\n";
    cout << "(Type 'exit' anytime to return to menu)\n\n";

    viewCustomer();

    string input;
    int id;


    // 1. ASK FOR Customer ID
    while (true) {
        cout << "\nEnter Customer ID to Delete (numeric): ";
        cin >> input;

        if (input == "exit") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }

        try {
            id = stoi(input);
            if (id <= 0) {
                cout << "ID must be greater than 0.\n";
                continue;
            }
        }
        catch (...) {
            cout << "Invalid ID format.\n";
            continue;
        }

        if (!showSelectedCustomer(con, id)) {
            cout << "Customer ID does NOT exist. Please try again.\n";
            continue;
        }

        break;
    }

    // 3. CHECK FOREIGN KEY — RENTAL
    try {
        unique_ptr<sql::PreparedStatement> checkFK(
            con->prepareStatement(
                "SELECT rentalID FROM rental WHERE customerID = ? LIMIT 1"
            )
        );

        checkFK->setInt(1, id);
        unique_ptr<sql::ResultSet> res(checkFK->executeQuery());

        if (res->next()) {
            cout << "\nCANNOT DELETE CUSTOMER!" << endl;
            cout << "This customer has rental history (Rental ID: "
                << res->getInt("rentalID") << ")\n";
            cout << "You must delete/archive rental records first.\n";
            return;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error validating foreign key: " << e.what() << endl;
        return;
    }

    // 4. CONFIRM DELETE
    char confirm;
    cout << "Are you sure you want to delete this customer? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "\nDeletion cancelled.\n";
        // OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to Delete again: ";
        cin >> opt;

        if (opt == "m" || opt == "M") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }
        if (opt == "r" || opt == "R") {
            system("cls");
            deleteCustomer();
            return;
        }
        return;
    }


    // 5. DELETE CUSTOMER
    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "DELETE FROM customer WHERE customerID = ?"
            )
        );
        pstmt->setInt(1, id);

        int rows = pstmt->executeUpdate();

        if (rows > 0)
            cout << "\nCustomer deleted successfully.\n";
        else
            cout << "\nCustomer not found or already deleted.\n";
    }
    catch (sql::SQLException& e) {
        cerr << "Error deleting customer: " << e.what() << endl;
    }
    // OPTION: MENU or REPEAT
    string opt;
    cout << "\nEnter 'm' for Menu or 'r' to Delete again: ";
    cin >> opt;

    if (opt == "m" || opt == "M") {
        cout << "Returning to menu...\n";
        system("cls");
        return;
    }
    if (opt == "r" || opt == "R") {
        system("cls");
        deleteCustomer();
        return;
    }
}


void CustomerMenu() {
    int choice;
    do {
        cout << "\n\n----------------------------------";
        cout << "\n           CUSTOMER MENU        \n";
        cout << "----------------------------------\n";
        cout << "\nSELECT OPTION\n";
        cout << "\n1. VIEW Customer\n";
        cout << "2. SEARCH Customer\n";
        cout << "3. ADD Customer\n";
        cout << "4. UPDATE Customer\n";
        cout << "5. DELETE Customer\n";
        cout << "6. Return to Main Menu\n";

        while (true) {
            cout << "\nEnter your choice: ";

            if (cin >> choice) {
                //  check range
                if (choice >= 1 && choice <= 6) {
                    break;
                }
                else {
                    cout << "Please enter a number between 1 and 6.\n";
                }
            }
            else {
                // Input failed
                cout << "Invalid input. Please enter numbers only.\n";
                cin.clear(); // reset the error state
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard wrong input
            }
        }

        switch (choice) {
        case 1: 
            system("cls");
            viewCustomer();
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            break;
        case 2:
            system("cls");
            searchCustomer();
            break;
        case 3: 
            system("cls");
            addCustomer();
            break;
        case 4: 
            system("cls");
            updateCustomer(); 
            break;
        case 5: 
            system("cls");
            deleteCustomer();
            break;
        case 6: 
            system("cls");
            cout << "Returning...\n";
            break;
        default:
            cout << "Invalid choice.\n";
            break;
        }
    } while (choice != 6);
}
