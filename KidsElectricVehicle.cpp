#include "KidsElectricVehicle.h"
#include "Database.h"
#include <iostream>

#include <iomanip>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>
using namespace std;

void viewVehicles() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT * FROM KidsElectricVehicle")
        );

        // header
        std::cout << "\n=== KIDS ELECTRIC VEHICLE LIST ===\n";
        std::cout << std::left
            << std::setw(10) << "ID"
            << std::setw(20) << "Vehicle Name"
            << std::setw(15) << "Rate/Hour"
            << std::setw(15) << "Purchase Cost"
            << std::setw(15) << "Condition"
            << std::setw(10) << "Status"
            << std::endl;

        std::cout << std::string(85, '-') << std::endl;

        // rows
        while (res->next()) {
      /*      int id = res->getInt("vehicleID");*/
            double rate = res->getDouble("ratePerHour");
            double cost = res->getDouble("purchaseCost");

            std::ostringstream rateStr, costStr;
            rateStr << "RM " << std::fixed << std::setprecision(2) << rate;
            costStr << "RM " << std::fixed << std::setprecision(2) << cost;

            std::cout << std::left
                << std::setw(10) << res->getString("vehicleID")
                << std::setw(20) << res->getString("vehicleName")
                << std::setw(15) << rateStr.str()
                << std::setw(15) << costStr.str()
                << std::setw(15) << res->getString("vehicleCondition")
                << std::setw(10) << res->getString("status")
                << std::endl;
        }


    }
    catch (sql::SQLException& e) {
        std::cerr << "Error retrieving data: " << e.what() << std::endl;
    }
}


bool showSelectedVehicle(sql::Connection* con, int vehicleID) {
    try {
    
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT * FROM KidsElectricVehicle WHERE vehicleID = ?")
        );
        pstmt->setInt(1, vehicleID);

     
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

       
        if (!res->next()) {
            //cout << "No vehicle found with ID " << vehicleID << ".\n";
            return false;
        }

        // Print header (same as viewVehicles)
        cout << "\n=== SELECTED VEHICLE DETAILS ===\n\n";
        cout << std::left
            << std::setw(10) << "ID"
            << std::setw(20) << "Vehicle Name"
            << std::setw(15) << "Rate/Hour"
            << std::setw(15) << "Purchase Cost"
            << std::setw(15) << "Condition"
            << std::setw(10) << "Status"
            << endl;
        cout << std::string(85, '-') << endl;

        // Print the selected vehicle in table form
        int id = res->getInt("vehicleID");
        double rate = res->getDouble("ratePerHour");
        double cost = res->getDouble("purchaseCost");

        std::ostringstream rateStr, costStr;
        rateStr << "RM " << std::fixed << std::setprecision(2) << rate;
        costStr << "RM " << std::fixed << std::setprecision(2) << cost;

        cout << std::left
            << std::setw(10) << id
            << std::setw(20) << res->getString("vehicleName")
            << std::setw(15) << rateStr.str()
            << std::setw(15) << costStr.str()
            << std::setw(15) << res->getString("vehicleCondition")
            << std::setw(10) << res->getString("status")
            << endl;

        cout << std::string(85, '-') << endl;

        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Error retrieving vehicle: " << e.what() << endl;
        return false;
    }
}

void searchVehicle() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== SEARCH KIDS ELECTRIC VEHICLE ===\n";
    cout << "(Search by Vehicle Name | Type 'exit' to return)\n\n";

    string input;

    // clear buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        cout << "Enter Vehicle Name: ";
        getline(cin, input);

        if (input == "exit" || input == "Exit") {
            cout << "Returning to menu...\n";
            system("cls");
            return;
        }

        if (input.empty()) {
            cout << "Vehicle name cannot be empty.\n";
            continue;
        }

        break;
    }

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT * FROM KidsElectricVehicle "
                "WHERE vehicleName LIKE ?"
            )
        );

        pstmt->setString(1, "%" + input + "%");

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            cout << "\nNo vehicle found matching: " << input << "\n";
        }
        else {
            cout << "\n=== SEARCH RESULT ===\n\n";
            cout << left
                << setw(10) << "ID"
                << setw(20) << "Vehicle Name"
                << setw(15) << "Rate/Hour"
                << setw(15) << "Purchase Cost"
                << setw(15) << "Condition"
                << setw(10) << "Status"
                << endl;

            cout << string(85, '-') << endl;

            do {
                double rate = res->getDouble("ratePerHour");
                double cost = res->getDouble("purchaseCost");

                ostringstream rateStr, costStr;
                rateStr << "RM " << fixed << setprecision(2) << rate;
                costStr << "RM " << fixed << setprecision(2) << cost;

                cout << left
                    << setw(10) << res->getInt("vehicleID")
                    << setw(20) << res->getString("vehicleName")
                    << setw(15) << rateStr.str()
                    << setw(15) << costStr.str()
                    << setw(15) << res->getString("vehicleCondition")
                    << setw(10) << res->getString("status")
                    << endl;

            } while (res->next());

            cout << string(85, '-') << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error searching vehicle: " << e.what() << endl;
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
        searchVehicle();
        return;
    }
}


void addVehicle() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    string name, condition, status, input;
    double ratePerHour = 0.0, purchaseCost = 0.0;
    char option;

    cout << "\n=== ADD KIDS ELECTRIC VEHICLE ===\n";
    cout << "(Type 'x' anytime to return to menu)\n\n";

        // Clear buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // === Vehicle Name ===

        while (true) {
            cout << "Enter Vehicle Name: ";
            cin >> name;
            if (name == "x" || name == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
            if (!name.empty())
                break;
            cout << "Vehicle name cannot be empty.\n";
        }

        // === Rate per Hour ===
        while (true) {
            cout << "Enter Rate per Hour: ";
            cin >> input;
            if (input == "x" || input == "x") { cout << "Returning to menu...\n"; system("cls"); return; }
            try {
                ratePerHour = stod(input);
                if (ratePerHour < 0) throw invalid_argument("negative");
                break;
            }
            catch (...) {
                cout << " Invalid input. Please enter a valid number.\n";
            }
        }

        // === Purchase Cost ===
        while (true) {
            cout << "Enter Purchase Cost: ";
            cin >> input;
            if (input == "x" || input == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
            try {
                purchaseCost = stod(input);
                if (purchaseCost < 0) throw invalid_argument("negative");
                break;
            }
            catch (...) {
                cout << "Invalid input. Please enter a valid number.\n";
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline

        // === Condition ===
        while (true) {
            cout << "Enter Condition (Good/Damaged): ";
            getline(cin, condition);
            if (condition == "x" || condition == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
            if (condition == "Good" || condition == "good" || condition == "Damaged" || condition == "damaged")
                break;
            else
                cout << "Please enter only 'Good' or 'Damaged'.\n";
        }

        // === Status ===
        while (true) {
            cout << "Enter Status (Available/Unavailable): ";
            getline(cin, status);
            if (status == "x" || status == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
            if (status == "Available" || status == "available" || status == "Unavailable" || status == "unavailable")
                break;
            else
                cout << "Please enter only 'Available' or 'Unavailable'.\n";
        }

        // === Insert into Database ===
        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "INSERT INTO KidsElectricVehicle (vehicleName, ratePerHour, purchaseCost, vehicleCondition, status) "
                    "VALUES (?, ?, ?, ?, ?)"
                )
            );

            pstmt->setString(1, name);
            pstmt->setDouble(2, ratePerHour);
            pstmt->setDouble(3, purchaseCost);
            pstmt->setString(4, condition);
            pstmt->setString(5, status);

            pstmt->execute();
            cout << "\nSUCCESSFULLY ADDED TO (KIDS ELECTRIC VEHICLE) DATABASE\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding vehicle: " << e.what() << endl;
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
            addVehicle();
            return;
        }

}


void updateVehicle() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== UPDATE KIDS ELECTRIC VEHICLE ===\n";
    cout << "(Type 'x' anytime to return to menu)\n\n";

    viewVehicles();

    int id;
    string input,newinput;

    while (true) {
        cout << "\nEnter Vehicle ID to update (numeric): ";
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

        if (!showSelectedVehicle(con, id)) {
            cout << "Vehicle ID does NOT exist. Please try again.\n";
            continue;
        }

        break;
    }

    string newName, newCondition, newStatus;
    double newRate = 0.0, newCost = 0.0;

    cin.ignore();
    cout << "ENTER New Vehicle Name: ";
    getline(cin, newName);
    if (newName == "x" || newName == "X") {
        system("cls");
        return;
    }

    while (true) {
        cout << "Enter Rate per Hour: ";
        getline(cin, newinput);
        if (newinput == "x" || newinput == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
        if (newinput.empty()) {
            break;
        }
        try {
            newRate = stod(newinput);
            if (newRate < 0) throw invalid_argument("negative");
            break;
        }
        catch (...) {
            cout << " Invalid input. Please enter a valid number.\n";
        }
    }

    while (true) {
        cout << "Enter Purchase Cost: ";
        getline(cin, newinput);
        if (newinput == "x" || newinput == "X") { cout << "Returning to menu...\n"; system("cls"); return; }
        if (newinput.empty()) {
            break;
        }
        try {
            newCost = stod(newinput);
            if (newCost < 0) throw invalid_argument("negative");
            break;
        }
        catch (...) {
            cout << " Invalid input. Please enter a valid number.\n";
        }
    }

    // Condition
    while (true) {
        cout << "New Condition (Good/Damaged): ";
        getline(cin, newCondition);

        if (newCondition == "x" || newCondition == "X") return;
        if (newCondition.empty()) break;

        if (newCondition == "Good" || newCondition == "Damaged")
            break;

        cout << "Enter only 'Good' or 'Damaged'.\n";
    }

    // Status
    while (true) {
        cout << "New Status (Available/Unavailable): ";
        getline(cin, newStatus);

        if (newStatus == "x" || newStatus == "X") return;
        if (newStatus.empty()) break;

        if (newStatus == "Available" || newStatus == "Unavailable")
            break;

        cout << "Enter only 'Available' or 'Unavailable'.\n";
    }

    char confirm;
    cout << "\nAre you sure you want to update this vehicle? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "\nUpdate cancelled.\n";
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(
        con->prepareStatement(
            "UPDATE KidsElectricVehicle SET "
            "vehicleName = IF(? = '', vehicleName, ?), "
            "ratePerHour = IF(? = '', ratePerHour, ?), "
            "purchaseCost = IF(? = '', purchaseCost, ?), "
            "vehicleCondition = IF(? = '', vehicleCondition, ?), "
            "status = IF(? = '', status, ?) "
            "WHERE vehicleID = ?"
        )
    );

    pstmt->setString(1, newName);
    pstmt->setString(2, newName);

    pstmt->setDouble(3, newRate);
    pstmt->setDouble(4, newRate);

    pstmt->setDouble(5, newCost);
    pstmt->setDouble(6, newCost);

    pstmt->setString(7, newCondition);
    pstmt->setString(8, newCondition);

    pstmt->setString(9, newStatus);
    pstmt->setString(10, newStatus);

    pstmt->setInt(11, id);

    try {
        int rows = pstmt->executeUpdate();
        if (rows > 0){
            showSelectedVehicle(con, id);
            cout << "\nVEHICLE UPDATE SUCCESFULLY.\n";
        }

        else
            cout << "\nVehicle not found or no changes made.\n";
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
        updateVehicle();
        return;
    }
}



void deleteVehicle() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== DELETE KIDS ELECTRIC VEHICLE ===\n";
    cout << "(Type 'exit' anytime to return to menu)\n\n";

    viewVehicles();

    string input;
    int id;

    // 1. ASK FOR VEHICLE ID
    while (true) {
        cout << "\nEnter Vehicle ID to Delete (numeric): ";
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

        if (!showSelectedVehicle(con, id)) {
            cout << "Vehicle ID does NOT exist. Please try again.\n";
            continue;
        }

        break;
    }

    // 3. CHECK FOREIGN KEY — RENTAL
    try {
        unique_ptr<sql::PreparedStatement> checkFK(
            con->prepareStatement(
                "SELECT rentalID FROM rental WHERE vehicleID = ? LIMIT 1"
            )
        );
        checkFK->setInt(1, id);

        unique_ptr<sql::ResultSet> res(checkFK->executeQuery());

        if (res->next()) {
            cout << "\nCANNOT DELETE VEHICLE!" << endl;
            cout << "This vehicle is linked to rental ID: "
                << res->getInt("rentalID") << endl;
            cout << "You must delete or archive the rental record first.\n";
            return;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error checking rental foreign key: " << e.what() << endl;
        return;
    }


    // 4. CONFIRM DELETE
    char confirm;
    cout << "\nAre you sure you want to DELETE this vehicle? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "\nDeletion cancelled.\n";
        system("cls");
        return;
    }

    // 5. DELETE VEHICLE
    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "DELETE FROM KidsElectricVehicle WHERE vehicleID = ?"
            )
        );

        pstmt->setInt(1, id);
        int rows = pstmt->executeUpdate();

        if (rows > 0)
            cout << "\nVEHICLE DELETED SUCCESSFULLY.\n";
        else
            cout << "\nNo vehicle found or already deleted.\n";
    }
    catch (sql::SQLException& e) {
        cerr << "Error deleting vehicle: " << e.what() << endl;
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
        deleteVehicle();
        return;
    }
}



void KidsElectricVehicleMenu() {
    int choice;
    do {
        cout << "\n\n----------------------------------";
        cout << "\n    KIDS ELECTRIC VEHICLE MENU    \n";
        cout << "----------------------------------\n";
        cout << "\nSELECT OPTION\n";
        cout << "\n1. VIEW Vehicles\n";
        cout << "2. SEARCH Vehicle\n";
        cout << "3. ADD Vehicle\n";
        cout << "4. UPDATE Vehicle\n";
        cout << "5. DELETE Vehicle\n";
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
            viewVehicles();
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            break;
        case 2:
            system("cls");
            searchVehicle();
            break;
        case 3:
            system("cls");
            addVehicle();
            break;
        case 4: 
            system("cls");
            updateVehicle(); 
            break;
        case 5:
            system("cls");
            deleteVehicle(); 
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
