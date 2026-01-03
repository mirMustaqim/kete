#include "Database.h"
#include "Rental.h"
#include "customer.h"
#include <iostream>
#include <iomanip>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>
#include <ctime>
using namespace std;


void viewRental() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());

        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery(
                "SELECT "
                "r.rentalID, "
                "c.customerName, "
                "c.customerNumber, "
                "v.vehicleName, "
                "r.rentDuration AS duration, "
                "r.rentDateTime AS startTime, "
                "r.expectedReturn, "
                "r.endrentDateTime, "
                "IFNULL(p.totalPrice, 0) AS totalPrice, "
                "r.rentalStatus, "
                "IFNULL(p.paymentStatus, 'Unpaid') AS paymentStatus "
                "FROM rental r "
                "JOIN customer c ON r.customerID = c.customerID "
                "JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "LEFT JOIN payment p ON r.rentalID = p.rentalID "
                "ORDER BY r.rentalID DESC"
            )
        );

        cout << "\n============== RENTAL LIST ==============\n";

        // Table Header
        cout << left
            << setw(5) << "ID"
            << setw(14) << "CustomerName"
            << setw(13) << "Phone"
            << setw(12) << "Vehicle"
            << setw(12) << "Minutes"
            << setw(20) << "StartTime"
            << setw(22) << "ExpectedReturn"
            << setw(20) << "ActualReturn"
            << setw(13) << "Price(RM)"
            << setw(13) << "Status"
            << setw(15) << "Payment"
            << endl;

        cout << string(150, '-') << endl;

        // Table Rows
        while (res->next()) {
            cout << left
                << setw(5) << res->getInt("rentalID")
                << setw(14) << res->getString("customerName")
                << setw(13) << res->getString("customerNumber")
                << setw(14) << res->getString("vehicleName")
                << setw(8) << res->getInt("duration")
                << setw(22) << res->getString("startTime")
                << setw(22) << res->getString("expectedReturn")
                << setw(22) << res->getString("endrentDateTime")
                << setw(12) << fixed << setprecision(2) << res->getDouble("totalPrice")
                << setw(13) << res->getString("rentalStatus")
                << setw(15) << res->getString("paymentStatus")
                << endl;
        }

        cout << string(150, '-') << endl;
    }
    catch (sql::SQLException& e) {
        cerr << "Error retrieving rental records: " << e.what() << endl;
    }
}


void viewActiveRental() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n========== ACTIVE / ONGOING RENTALS ==========\n";

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT r.rentalID, c.customerName, c.customerNumber , v.vehicleName, "
                "r.rentDateTime, r.expectedReturn, r.rentDuration, r.endrentDateTime, "
                "p.totalPrice, p.paymentStatus, r.rentalStatus "
                "FROM rental r "
                "JOIN customer c ON r.customerID = c.customerID "
                "JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "JOIN payment p ON r.rentalID = p.rentalID "
                "WHERE r.rentalStatus = 'Ongoing' "
                "ORDER BY r.rentalID ASC"
            )
        );

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        bool found = false;

        cout << left 
            << setw(5) << "ID"
            << setw(14) << "CustomerName"
            << setw(13) << "Phone"
            << setw(12) << "Vehicle"
            << setw(12) << "Minutes"
            << setw(20) << "StartTime"
            << setw(22) << "ExpectedReturn"
            << setw(20) << "ActualReturn"
            << setw(13) << "Price(RM)"
            << setw(13) << "Status"
            << setw(15) << "Payment"
            << endl;

        cout << string(150, '-') << endl;

        while (res->next()) {
            found = true;

            cout << left 
                << setw(5) << res->getInt("rentalID")
                << setw(14) << res->getString("customerName")
                << setw(13) << res->getString("customerNumber")
                << setw(14) << res->getString("vehicleName")
                << setw(8) << res->getInt("rentDuration")
                << setw(22) << res->getString("rentDateTime")
                << setw(22) << res->getString("expectedReturn")
                << setw(22) << res->getString("endrentDateTime")
                << setw(12) << res->getDouble("totalPrice")
                << setw(13) << res->getString("rentalStatus")
                << setw(15) << res->getString("paymentStatus")
                << endl;

        }
        cout << string(150, '-') << endl;

        if (!found) {
            cout << "\nNo active rentals found.\n";
            return;
        }

    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }
}

void searchRental() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== SEARCH RENTAL BY CUSTOMER NUMBER ===\n";
    cout << "(Type 'exit' to return)\n\n";

    string customerNumber;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        cout << "Enter Customer Number: ";
        getline(cin, customerNumber);

        if (customerNumber == "exit" || customerNumber == "Exit") {
            system("cls");
            return;
        }

        if (customerNumber.empty()) {
            cout << "Customer number cannot be empty.\n";
            continue;
        }

        // numbers only validation
        if (customerNumber.find_first_not_of("0123456789") != string::npos) {
            cout << "Invalid input. Numbers only.\n";
            continue;
        }

        break;
    }

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT "
                "r.rentalID, "
                "c.customerName, "
                "c.customerNumber, "
                "v.vehicleName, "
                "r.rentDuration AS duration, "
                "r.rentDateTime AS startTime, "
                "r.expectedReturn, "
                "r.endrentDateTime, "
                "IFNULL(p.totalPrice, 0) AS totalPrice, "
                "r.rentalStatus, "
                "IFNULL(p.paymentStatus, 'Unpaid') AS paymentStatus "
                "FROM rental r "
                "JOIN customer c ON r.customerID = c.customerID "
                "JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "LEFT JOIN payment p ON r.rentalID = p.rentalID "
                "WHERE c.customerNumber = ? "
                "ORDER BY r.rentalID DESC"
            )
        );

        pstmt->setString(1, customerNumber);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            cout << "\nNo rentals found for customer number: "
                << customerNumber << "\n";
        }
        else {
            cout << "\n============== RENTAL SEARCH RESULT ==============\n";

            cout << left
                << setw(5) << "ID"
                << setw(14) << "CustomerName"
                << setw(13) << "Phone"
                << setw(12) << "Vehicle"
                << setw(12) << "Minutes"
                << setw(20) << "StartTime"
                << setw(22) << "ExpectedReturn"
                << setw(20) << "ActualReturn"
                << setw(13) << "Price(RM)"
                << setw(13) << "Status"
                << setw(15) << "Payment"
                << endl;

            cout << string(150, '-') << endl;

            do {
                cout << left
                    << setw(5) << res->getInt("rentalID")
                    << setw(14) << res->getString("customerName")
                    << setw(13) << res->getString("customerNumber")
                    << setw(14) << res->getString("vehicleName")
                    << setw(8) << res->getInt("duration")
                    << setw(22) << res->getString("startTime")
                    << setw(22) << res->getString("expectedReturn")
                    << setw(22) << res->getString("endrentDateTime")
                    << setw(12) << fixed << setprecision(2) << res->getDouble("totalPrice")
                    << setw(13) << res->getString("rentalStatus")
                    << setw(15) << res->getString("paymentStatus")
                    << endl;
            } while (res->next());

            cout << string(150, '-') << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error searching rentals: " << e.what() << endl;
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
        searchRental();
        return;
    }
}


bool showSelectedRental(sql::Connection* con, int rentalID) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT "
                "r.rentalID, "
                "c.customerName, "
                "c.customerNumber, "
                "v.vehicleName, "
                "r.rentDuration AS duration, "
                "r.rentDateTime AS startTime, "
                "r.expectedReturn, "
                "r.endrentDateTime, "
                "IFNULL(p.totalPrice, 0) AS totalPrice, "
                "r.rentalStatus, "
                "IFNULL(p.paymentStatus, 'Unpaid') AS paymentStatus "
                "FROM rental r "
                "JOIN customer c ON r.customerID = c.customerID "
                "JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "LEFT JOIN payment p ON r.rentalID = p.rentalID "
                "WHERE r.rentalID = ?"
            )
        );

        pstmt->setInt(1, rentalID);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            std::cout << " No rental found with ID " << rentalID << ".\n";
            return false;
        }

        // Header
        std::cout << "\n=========== RENTAL DETAILS ===========\n\n";
        std::cout << std::left
            << setw(5) << "ID"
            << setw(14) << "CustomerName"
            << setw(13) << "Phone"
            << setw(12) << "Vehicle"
            << setw(12) << "Minutes"
            << setw(20) << "StartTime"
            << setw(22) << "ExpectedReturn"
            << setw(20) << "ActualReturn"
            << setw(13) << "Price(RM)"
            << setw(13) << "Status"
            << setw(15) << "Payment"
            << "\n";

        std::cout << std::string(145, '-') << "\n";

        // Data row
        std::cout << std::left
            << setw(5) << res->getInt("rentalID")
            << setw(14) << res->getString("customerName")
            << setw(13) << res->getString("customerNumber")
            << setw(14) << res->getString("vehicleName")
            << setw(8) << res->getInt("duration")
            << setw(22) << res->getString("startTime")
            << setw(22) << res->getString("expectedReturn")
            << setw(22) << res->getString("endrentDateTime")
            << setw(12) << fixed << setprecision(2) << res->getDouble("totalPrice")
            << setw(13) << res->getString("rentalStatus")
            << setw(15) << res->getString("paymentStatus")
            << "\n";

        std::cout << std::string(150, '-') << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << "\n";
        return false;
    }
}

string generateReceiptNo(int rentalID) {

    time_t now = time(0);

    ostringstream oss;
    oss << "RCPT" << now << rentalID;

    return oss.str();
}

void printReceipt(int rentalID, int staffID) {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        // Get rental, customer, vehicle, and payment info
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT r.rentalID, r.rentDuration, r.rentDateTime, r.endrentDateTime, "
                "c.customerName, c.customerNumber, "
                "v.vehicleName, v.ratePerHour, "
                "p.totalPrice, p.receiptNo, p.paymentDateTime "
                "FROM rental r "
                "JOIN customer c ON r.customerID = c.customerID "
                "JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "JOIN payment p ON r.rentalID = p.rentalID "
                "WHERE r.rentalID = ? AND p.paymentStatus = 'Paid'"
            )
        );
        pstmt->setInt(1, rentalID);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) {
            cout << "No paid rental found with ID " << rentalID << ". Receipt not generated.\n";
            return;
        }

        // Prepare receipt content
        string storeName = "KIDS ELECTRIC VEHICLE RENTAL";
        string storeAddress = "123 Main Street, Kuala Lumpur, Malaysia";
        string businessNumber = "BN12345678";

        cout << "\n==========================================\n";
        cout << storeName << "\n";
        cout << storeAddress << "\n";
        cout << "Business No: " << businessNumber << "\n";
        cout << "------------------------------------------\n";
        cout << "Staff ID: " << staffID << "\n";
        cout << "---------------------------------------\n";
        cout << "Order Details:\n";
        cout << left << setw(15) << "Vehicle"
            << setw(16) << "Duration(min)"
            << setw(13) << "Price(RM)" << "\n";
        cout << left << setw(15) << res->getString("vehicleName")
            << setw(16) << res->getInt("rentDuration")
            << setw(13) << fixed << setprecision(2) << res->getDouble("totalPrice") << "\n";
        cout << "---------------------------------------\n";
        cout << "Sub Total: RM " << fixed << setprecision(2) << res->getDouble("totalPrice") << "\n";
        cout << "Receipt No: " << res->getString("receiptNo") << "\n";
        cout << "Payment Date: " << res->getString("paymentDateTime") << "\n";
        cout << "=======================================\n\n";

    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error generating receipt: " << e.what() << endl;
    }
}

void addRental(int userID) {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    string input;
    string customerNumber;
    int customerID = 0;
    int vehicleID = 0;
    int rentDuration = 0;

    viewCustomer();

    cout << "\n============== MAKE RENTAL ==============\n";
    cout << "(Type 'exit' anytime to cancel)\n\n";



  // 1. GET CUSTOMER USING PHONE NUMBER
    cout << "Enter Customer Phone Number: ";
    cin >> customerNumber;

    if (customerNumber == "exit") {
        system("cls");
        return;
    }
    int id = -1;

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT customerID FROM customer WHERE customerNumber = ?"
            )
        );

        pstmt->setString(1, customerNumber);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            id = res->getInt("customerID");
            customerID = id;    
        }
        else {
            system("cls");
            cout << "Customer NOT found with that phone number.\n";
            addCustomer();
            return;
        }
    }
    catch (sql::SQLException& e) {
        cout << "Error retrieving customer: " << e.what() << endl;
        return;
    }

    // Show customer details
    if (!showSelectedCustomer(con, customerID)) {
        return;
    }



    // 2. SHOW AVAILABLE VEHICLES (Status = Available & Condition = Good)

    cout << "\n========= AVAILABLE VEHICLES =========\n\n";

    try {
        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(
            stmt->executeQuery(
                "SELECT vehicleID, vehicleName, ratePerHour, vehicleCondition, status "
                "FROM KidsElectricVehicle "
                "WHERE status = 'Available' AND vehicleCondition = 'Good'"
            )
        );

        cout << left
            << setw(10) << "ID"
            << setw(20) << "Name"
            << setw(15) << "Rate/hour"
            << setw(15) << "Condition"
            << setw(15) << "Status" << endl;

        cout << string(75, '-') << endl;

        while (res->next()) {
            cout << left
                << setw(10) << res->getInt("vehicleID")
                << setw(20) << res->getString("vehicleName")
                << setw(15) << res->getDouble("ratePerHour")
                << setw(15) << res->getString("vehicleCondition")
                << setw(15) << res->getString("status") << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error showing vehicles: " << e.what() << endl;
    }


    // 3. SELECT VEHICLE ID
    while (true) {
        cout << "\nEnter Vehicle ID to Rent: ";
        cin >> input;
        
        if (input == "exit") {
            system("cls");
        return;
    }
        try {
            vehicleID = stoi(input);

            unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT vehicleID FROM KidsElectricVehicle "
                    "WHERE vehicleID = ? AND status='Available' AND vehicleCondition='Good'"
                )
            );
            pstmt->setInt(1, vehicleID);

            unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (res->next()) break;

            cout << "Invalid or unavailable Vehicle ID.\n";
        }
        catch (...) {
            cout << "Invalid Vehicle ID.\n";
        }
    }


    // 4. ENTER RENT DURATION
    while (true) {
        cout << "Enter Rent Duration (Minutes): ";
        cin >> input;

        if (input == "exit") {
            system("cls");
            return;
        }
        try {
            rentDuration = stoi(input);
            if (rentDuration > 0) break;
        }
        catch (...) {}

        cout << "Invalid duration.\n";
    }


    // 5. CALCULATE TOTAL PRICE USING ratePerHour

    double rate = 0;

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT ratePerHour FROM KidsElectricVehicle WHERE vehicleID = ?")
        );
        pstmt->setInt(1, vehicleID);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) rate = res->getDouble("ratePerHour");
    }
    catch (...) {}

    double totalAmount = (rentDuration / 60.0) * rate;

    cout << "\nTotal Rental Price: RM " << fixed << setprecision(2) << totalAmount << endl;

    // 6. CONFIRM PRICE + ASK PAYMENT STATUS
    cin.ignore();
    string paymentStatus;

    while (true) {
        cout << "\nEnter Payment Status (Paid/Unpaid): ";
        getline(cin, paymentStatus);

        if (paymentStatus == "Paid" || paymentStatus == "paid" ||
            paymentStatus == "Unpaid" || paymentStatus == "unpaid")
            break;

        cout << "Invalid. Enter Paid or Unpaid.\n";
    }

    if (paymentStatus == "paid") paymentStatus = "Paid";
    if (paymentStatus == "unpaid") paymentStatus = "Unpaid";


    // 7. FINAL CONFIRMATION BEFORE INSERTING ANYTHING
    char confirmFinal;
    cout << "\nCreate rental + payment record now? (Y/N): ";
    cin >> confirmFinal;

    if (confirmFinal != 'Y' && confirmFinal != 'y') {
        cout << "\n Rental creation cancelled. Nothing saved.\n";
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    // 8. INSERT RENTAL INTO DATABASE
    int newRentalID = 0;

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO rental (vehicleID, customerID, userID, rentDuration, "
                "rentDateTime, expectedReturn, rentalStatus) "
                "VALUES (?, ?, ?, ?, NOW(), DATE_ADD(NOW(), INTERVAL ? MINUTE), 'Ongoing')"
            )
        );

        pstmt->setInt(1, vehicleID);
        pstmt->setInt(2, customerID);
        pstmt->setInt(3, userID);
        pstmt->setInt(4, rentDuration);
        pstmt->setInt(5, rentDuration);
        pstmt->execute();

        // get new rental id
        unique_ptr<sql::Statement> stmt(con->createStatement());
        unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
        if (res->next()) newRentalID = res->getInt("id");

        // mark vehicle unavailable
        unique_ptr<sql::PreparedStatement> pstmt2(
            con->prepareStatement("UPDATE KidsElectricVehicle SET status='Unavailable' WHERE vehicleID=?")
        );
        pstmt2->setInt(1, vehicleID);
        pstmt2->execute();

        cout << "\nRental created successfully!\n";
    }
    catch (sql::SQLException& e) {
        cerr << "Error inserting rental: " << e.what() << endl;
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    // 9. INSERT PAYMENT (PAID OR UNPAID)
    try {
        unique_ptr<sql::PreparedStatement> pstmt;

        if (paymentStatus == "Paid") {
            string receiptNo = generateReceiptNo(newRentalID);  // generate receipt number

            pstmt.reset(con->prepareStatement(
                "INSERT INTO payment (rentalID, paymentDateTime, totalPrice, paymentStatus, receiptNo) "
                "VALUES (?, NOW(), ?, 'Paid', ?)"
            ));
            pstmt->setInt(1, newRentalID);
            pstmt->setDouble(2, totalAmount);
            pstmt->setString(3, receiptNo);  // save receipt number
        }

        else {  // Unpaid
            pstmt.reset(con->prepareStatement(
                "INSERT INTO payment (rentalID, paymentDateTime, totalPrice, paymentStatus) "
                "VALUES (?, NULL, ?, 'Unpaid')"
            ));
            pstmt->setInt(1, newRentalID);
            pstmt->setDouble(2, totalAmount);
        }

        pstmt->execute();
        cout << " Payment record created (" << paymentStatus << ").\n";
    }
    catch (sql::SQLException& e) {
        cerr << "Error inserting payment: " << e.what() << endl;
    }

    cout << "\n";
    showSelectedRental(con, newRentalID);
    if (paymentStatus == "Paid") {
        cout << "\nGenerating receipt...\n";
        printReceipt(newRentalID, userID);

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
        addRental(userID);
        return;
    }

}

void deleteRental() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    try {
        cout << "\n=== DELETE RENTAL ===\n";

        // Show all rentals first
        viewRental();

        int rentalID;
        string input;
        while (true) {
            cout << "Enter Rent ID: ";
            cin >> input;

            if (input == "exit") {
                system("cls");
                return;
            }

            try {
                rentalID = stoi(input);
                if (rentalID > 0)
                    break;
                else
                    cout << "Invalid rentID. Must be more than 0.\n";
            }
            catch (...) {
                cout << "Invalid rentID. Numbers only.\n";
            }
        }


        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << " Invalid input.\n";
            return;
        }

        // Show selected rental
        cout << "\nChecking rental ID " << rentalID << "...\n";

        if (!showSelectedRental(con, rentalID)) {
            cout << " Rental ID does not exist.\n";
            return;
        }

        // STEP 1: GET VEHICLE ID BEFORE DELETING
        int vehicleID = -1;
        {
            unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT vehicleID FROM rental WHERE rentalID = ?")
            );
            pstmt->setInt(1, rentalID);
            unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (res->next()) {
                vehicleID = res->getInt("vehicleID");
            }
        }

        if (vehicleID == -1) {
            cout << "VehicleID not found.\n";
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            return;
        }

        // Confirm deletion
        char confirm;
        cout << "\nAre you sure you want to delete this rental? (Y/N): ";
        cin >> confirm;

        if (toupper(confirm) != 'Y') {
            cout << "Deletion cancelled.\n";
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            return;
        }

        // Delete payment first
        {
            std::unique_ptr<sql::PreparedStatement> delPay(
                con->prepareStatement("DELETE FROM payment WHERE rentalID = ?")
            );
            delPay->setInt(1, rentalID);
            delPay->executeUpdate();
        }

        // Delete rental
        {
            std::unique_ptr<sql::PreparedStatement> delRent(
                con->prepareStatement("DELETE FROM rental WHERE rentalID = ?")
            );
            delRent->setInt(1, rentalID);
            delRent->executeUpdate();
        }

        //STEP 2: SET VEHICLE TO AVAILABLE 
        {
            std::unique_ptr<sql::PreparedStatement> updateVehicle(
                con->prepareStatement("UPDATE KidsElectricVehicle SET status = 'Available' WHERE vehicleID = ?")
            );
            updateVehicle->setInt(1, vehicleID);
            updateVehicle->executeUpdate();
        }

        cout << "\nRental deleted and vehicle (ID " << vehicleID
            << ") is now AVAILABLE again.\n";
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << "\n";
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
        deleteRental();
        return;
    }
}

void updateRental() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== UPDATE RENTAL ===\n";
    cout << "(Type 'exit' anytime to return to menu)\n\n";

    viewRental();

    int rentalID;
    string input;

    while (true) {
        cout << "\nEnter Rental ID to update (numeric): ";
        cin >> input;

        if (input == "exit" || input.empty()) {
            system("cls");
            cout << "Returning to menu...\n";
            return;
        }

        try {
            rentalID = stoi(input);
            if (rentalID <= 0) throw invalid_argument("Invalid ID");
            break;
        }
        catch (...) {
            cout << "Enter a valid numeric ID.\n";
        }
    }

    if (!showSelectedRental(con, rentalID)) {
        return;  // rental not found
    }

    // Fetch current vehicleID and rate
    int oldVehicleID = 0;
    double ratePerHour = 0;
    int oldDuration = 0;

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT r.vehicleID, r.rentDuration, v.ratePerHour "
                "FROM rental r JOIN KidsElectricVehicle v ON r.vehicleID = v.vehicleID "
                "WHERE r.rentalID = ?"
            )
        );
        pstmt->setInt(1, rentalID);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            oldVehicleID = res->getInt("vehicleID");
            oldDuration = res->getInt("rentDuration");
            ratePerHour = res->getDouble("ratePerHour");
        }
        else {
            cout << "Rental not found.\n";
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            return;
        }
    }
    catch (...) {
        cout << "Error reading rental data.\n";
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    // ENTER NEW VALUES
    string newVehicle, newDuration, newStatus;

    cin.ignore();

    cout << "ENTER New Vehicle ID): ";
    getline(cin, newVehicle);

    cout << "ENTER New Duration in Minutes): ";
    getline(cin, newDuration);

    cout << "ENTER Payment Status (Paid/Unpaide): ";
    getline(cin, newStatus);

    // CONFIRM UPDATE

    char confirm;
    cout << "\nAre you sure you want to update this rental? (Y/N): ";
    cin >> confirm;

    if (confirm != 'Y' && confirm != 'y') {
        cout << "\nUpdate cancelled.\n";
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }


    // VEHICLE UPDATE LOGIC
    int newVehicleID = oldVehicleID;

    if (!newVehicle.empty()) {
        try {
            newVehicleID = stoi(newVehicle);

            // Validate new vehicle
            unique_ptr<sql::PreparedStatement> check(
                con->prepareStatement(
                    "SELECT vehicleID FROM KidsElectricVehicle "
                    "WHERE vehicleID = ? AND status = 'Available' AND vehicleCondition = 'Good'"
                )
            );
            check->setInt(1, newVehicleID);
            unique_ptr<sql::ResultSet> res(check->executeQuery());

            if (!res->next()) {
                cout << "Invalid or unavailable vehicle.\n";
                cout << "\nPress ENTER to continue...\n\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                system("cls");
                return;
            }

            // Free old vehicle
            unique_ptr<sql::PreparedStatement> freeOld(
                con->prepareStatement("UPDATE KidsElectricVehicle SET status='Available' WHERE vehicleID=?")
            );
            freeOld->setInt(1, oldVehicleID);
            freeOld->execute();

            // Occupy new vehicle
            unique_ptr<sql::PreparedStatement> occupyNew(
                con->prepareStatement("UPDATE KidsElectricVehicle SET status='Unavailable' WHERE vehicleID=?")
            );
            occupyNew->setInt(1, newVehicleID);
            occupyNew->execute();
        }
        catch (...) {
            cout << "Invalid vehicle input.\n";
            return;
        }
    }

    // DURATION & PRICE UPDATE

    int finalDuration = oldDuration;
    double newTotalPrice = 0;

    if (!newDuration.empty()) {
        try {
            finalDuration = stoi(newDuration);
            if (finalDuration <= 0) throw invalid_argument("Invalid duration");

            newTotalPrice = (finalDuration / 60.0) * ratePerHour;
        }
        catch (...) {
            cout << "Invalid duration.\n";
            return;
        }
    }


    // PAYMENT STATUS

    string finalStatus = newStatus.empty() ? "" : newStatus;

    // EXECUTE UPDATE

    try {
        // Update rental
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "UPDATE rental SET "
                "vehicleID = IF(? = '', vehicleID, ?), "
                "rentDuration = IF(? = '', rentDuration, ?), "
                "expectedReturn = IF(? = '', expectedReturn, DATE_ADD(rentDateTime, INTERVAL ? MINUTE)) "
                "WHERE rentalID = ?"
            )
        );

        pstmt->setString(1, newVehicle);
        pstmt->setInt(2, newVehicle.empty() ? 0 : newVehicleID);

        pstmt->setString(3, newDuration);
        pstmt->setInt(4, newDuration.empty() ? 0 : finalDuration);

        pstmt->setString(5, newDuration);
        pstmt->setInt(6, newDuration.empty() ? 0 : finalDuration);

        pstmt->setInt(7, rentalID);

        pstmt->executeUpdate();

        // Update payment only if duration changed OR status changed
        unique_ptr<sql::PreparedStatement> pay(
            con->prepareStatement(
                "UPDATE payment SET "
                "totalPrice = IF(? = '', totalPrice, ?), "
                "paymentStatus = IF(? = '', paymentStatus, ?) "
                "WHERE rentalID = ?"
            )
        );

        pay->setString(1, newDuration);
        pay->setDouble(2, newDuration.empty() ? 0.0 : newTotalPrice);

        pay->setString(3, finalStatus);
        pay->setString(4, finalStatus.empty() ? "" : finalStatus);

        pay->setInt(5, rentalID);

        pay->executeUpdate();

        cout << "\nRENTAL UPDATED SUCCESSFULLY.\n";
        showSelectedRental(con, rentalID);
    }
    catch (sql::SQLException& e) {
        cerr << "Error updating rental: " << e.what() << endl;
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
        updateRental();
        return;
    }
}

void returnVehicle(int userID) {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n========== RETURN VEHICLE ==========\n";
    cout << "(Type 'exit' anytime to cancel)\n";

    // 1. SHOW ACTIVE RENTALS
    viewActiveRental();

    // 2. ENTER RENTAL ID
    string input;
    int rentalID = -1;

    while (true) {
        cout << "\nEnter Rental ID: ";
        cin >> input;

        if (input == "exit" || input == "EXIT" || input == "Exit") {
            system("cls");
            cout << "Returning...\n";
            return;
        }

        try {
            rentalID = stoi(input);
            if (rentalID > 0) break;
        }
        catch (...) {}

        cout << "Invalid Rental ID.\n";
    }

    // 3. SHOW SELECTED RENTAL
    if (!showSelectedRental(con, rentalID)) {
        cout << "Rental not found.\n";
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    // 4. GET VEHICLE ID + PAYMENT STATUS
    int vehicleID = -1;
    string currentPaymentStatus = "";

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT r.vehicleID, p.paymentStatus "
                "FROM rental r JOIN payment p ON r.rentalID = p.rentalID "
                "WHERE r.rentalID = ?"
            )
        );
        pstmt->setInt(1, rentalID);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            vehicleID = res->getInt("vehicleID");
            currentPaymentStatus = res->getString("paymentStatus");
        }
        else {
            cout << "Rental not found.\n";
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            return;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "Error fetching rental info: " << e.what() << endl;
        cout << "\nPress ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");
        return;
    }

    // 5. ENTER PAYMENT STATUS ONLY IF NOT PAID YET
    string newPaymentStatus = currentPaymentStatus;

    if (currentPaymentStatus == "Paid") {
        cout << "\nGenerating receipt...\n";
        printReceipt(rentalID, userID);

    }
    else {
        cin.ignore();
        while (true) {
            cout << "\nEnter NEW Payment Status (Paid / Unpaid): ";
            getline(cin, newPaymentStatus);

            if (newPaymentStatus == "exit") {
                cout << "Returning...\n";
                system("cls");
                return;
            }

            if (newPaymentStatus == "Paid" || newPaymentStatus == "paid") {
                newPaymentStatus = "Paid";
                cout << "\nGenerating receipt...\n";
                printReceipt(rentalID, userID);
                break;
            }

            if (newPaymentStatus == "Unpaid" || newPaymentStatus == "unpaid") {
                newPaymentStatus = "Unpaid";
                break;
            }

            cout << "Invalid. Must be Paid or Unpaid.\n";
        }
    }

    // 6. Ask for confirmation
    char confirm;
    cout << "\nAre you sure you want to return this vehicle? (y/n): ";
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        cout << "Return cancelled.\n";
        system("cls");
        return;
    }

    // 7. UPDATE RENTAL, PAYMENT (IF NEEDED), VEHICLE
    try {
        // A) Update rental → Complete
        unique_ptr<sql::PreparedStatement> updateRental(
            con->prepareStatement(
                "UPDATE rental SET rentalStatus='Complete', endrentDateTime=NOW() "
                "WHERE rentalID=?"
            )
        );
        updateRental->setInt(1, rentalID);
        updateRental->executeUpdate();

        // B) Update payment only if not already Paid
        if (currentPaymentStatus != "Paid" && newPaymentStatus == "Paid") {
            string receiptNo = generateReceiptNo(rentalID);  // generate receipt number

            unique_ptr<sql::PreparedStatement> updatePayment(
                con->prepareStatement(
                    "UPDATE payment SET paymentStatus=?, paymentDateTime=NOW(), receiptNo=? "
                    "WHERE rentalID=?"
                )
            );
            updatePayment->setString(1, newPaymentStatus);
            updatePayment->setString(2, receiptNo);  // save receipt number
            updatePayment->setInt(3, rentalID);
            updatePayment->executeUpdate();
        }


        // C) Update vehicle → Available
        unique_ptr<sql::PreparedStatement> updateVehicle(
            con->prepareStatement(
                "UPDATE KidsElectricVehicle SET status='Available' WHERE vehicleID=?"
            )
        );
        updateVehicle->setInt(1, vehicleID);
        updateVehicle->executeUpdate();

        cout << "\nVehicle Returned Successfully!\n";
        cout << "Rental completed.\n";

        if (currentPaymentStatus != "Paid")
            cout << "Payment updated.\n";
        else
            cout << "Payment already completed earlier.\n";

        cout << "Vehicle marked as Available.\n";

        cout << "\n--- UPDATED RENTAL DETAILS ---\n";
        showSelectedRental(con, rentalID);
    }
    catch (sql::SQLException& e) {
        cerr << "Error completing return: " << e.what() << endl;
    }
    // OPTION: MENU or REPEAT
    string opt;
    cout << "\nEnter 'm' for Menu or 'r' to ReturnVehicle again: ";
    cin >> opt;

    if (opt == "m" || opt == "M") {
        cout << "Returning to menu...\n";
        system("cls");
        return;
    }
    if (opt == "r" || opt == "R") {
        system("cls");
        returnVehicle(userID);
        return;
    }
}

void RentalMenu(int userID) {
    int choice;

    do {
        cout << "\n\n================ RENTAL MENU ================\n";
        cout << "1. VIEW RENTAL\n";
        cout << "2. SEARCH RENTAL\n";
        cout << "3. MAKE RENTAL\n";
        cout << "4. RETURN Kids Electric Vehicle\n";
        cout << "5. UPDATE RENTAL\n";
        cout << "6. DELETE RENTAL\n";
        cout << "7. Return to Main Interface\n";

        cout << "\nEnter your choice: ";

        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter again: ";
        }

        switch (choice) {
        case 1: 
            system("cls");
            viewRental(); 
            cout << "\nPress ENTER to continue...\n\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            system("cls");
            break;
        case 2:
            system("cls");
            searchRental();
            break;
        case 3: 
            system("cls");
            addRental(userID); 
            break;
        case 4: 
            system("cls");
            returnVehicle(userID);
            break;
        case 5:
            system("cls");
            updateRental(); 
            break;
        case 6: 
            system("cls");
            deleteRental(); 
            break;
        case 7: 
            system("cls");
            cout << "Returning...\n"; 
            break;
        default: 
            cout << "Invalid choice.\n"; 
            break;
        }

    } while (choice != 7);
}
