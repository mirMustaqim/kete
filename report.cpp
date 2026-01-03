#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "Database.h"
#include "report.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
using namespace std;

void salesReport() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    while (true) {
        cout << "\n========== MONTHLY SALES REPORT ==========\n";
        cout << "(Type 'exit' anytime to return to menu)\n\n";

        // 1. ASK YEAR
        string input;
        int year;

        cout << "Enter Year (e.g., 2024): ";
        cin >> input;

        if (input == "exit" || input == "EXIT") {
            system("cls");
            return;
        }
        try {
            year = stoi(input);
            if (year < 2000 || year > 2100) throw invalid_argument("Invalid");
        }
        catch (...) {
            cout << "Invalid year. Try again.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // 2. Prepare monthly data
        vector<double> sales(12, 0.0);

        try {
            unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT MONTH(paymentDateTime) AS m, SUM(totalPrice) AS total "
                    "FROM payment "
                    "WHERE paymentStatus='Paid' AND YEAR(paymentDateTime)=? "
                    "GROUP BY MONTH(paymentDateTime)"
                )
            );

            pstmt->setInt(1, year);
            unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            while (res->next()) {
                int month = res->getInt("m");
                double total = res->getDouble("total");

                if (month >= 1 && month <= 12)
                    sales[month - 1] = total;
            }
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
            return;
        }

        // 3. Check if there is ANY sales in that year
        bool hasSales = false;
        for (double s : sales)
            if (s > 0) hasSales = true;

        if (!hasSales) {
            cout << "\n❗ No sales recorded for year " << year << ".\n";
        }

        // Month labels
        vector<string> months = {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec"
        };

        // Find highest sale for scaling graph
        double maxSale = *max_element(sales.begin(), sales.end());

        cout << "\n============= SALES GRAPH FOR " << year << " =============\n\n";

        for (int i = 0; i < 12; i++) {
            cout << left << setw(4) << months[i] << "| ";

            int barLength = (maxSale == 0 ? 0 : (sales[i] / maxSale) * 30);

            for (int j = 0; j < barLength; j++)
                cout << "#";

            cout << "  RM " << fixed << setprecision(2) << sales[i] << "\n";
        }

        // 4. SUMMARY
        double totalYear = 0;
        for (double s : sales) totalYear += s;

        cout << "\n=============================================\n";
        cout << "Total Sales for " << year << ": RM "
            << fixed << setprecision(2) << totalYear << "\n";
        cout << "=============================================\n";

        // 5. OPTION: MENU or REPEAT
        string opt;
        cout << "\nEnter 'm' for Menu or 'r' to view again: ";
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

void kidsEVUtilizationReport() {
    Database db;
    sql::Connection* con = db.getConnection();
    if (!con) return;

    cout << "\n=== KIDS ELECTRIC VEHICLE UTILIZATION REPORT ===\n";

    try {
        // 1. Get Kids Electric Vehicle Data + Rentals
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT kev.vehicleID, kev.vehicleName, kev.ratePerHour, kev.purchaseCost, "
                "COUNT(r.rentalID) AS totalRentals, "
                "SUM(r.rentDuration) AS totalHours "
                "FROM kidselectricvehicle kev "
                "LEFT JOIN rental r ON kev.vehicleID = r.vehicleID "
                "AND r.rentalStatus IN ('Ongoing', 'Complete') "
                "GROUP BY kev.vehicleID"
            )
        );

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        vector<string> names;
        vector<int> rentals;
        vector<double> hours;
        vector<double> prices;

        int totalVehicles = 0;
        double totalInventoryValue = 0;

        int maxRental = -1;
        string mostRentedName = "N/A";

        // 2. TABLE HEADER
        cout << left
            << setw(5) << "ID"
            << setw(25) << "Vehicle Name"
            << setw(15) << "Total Rentals"
            << setw(15) << "Total Hours"
            << setw(15) << "Purchase Cost"
            << endl;

        cout << string(80, '-') << endl;

        // 3. FETCH ROWS
        while (res->next()) {
            int id = res->getInt("vehicleID");
            string name = res->getString("vehicleName");
            int totalR = res->getInt("totalRentals");
            double hr = res->getDouble("totalHours");
            double cost = res->getDouble("purchaseCost");

            cout << left
                << setw(5) << id
                << setw(25) << name
                << setw(15) << totalR
                << setw(15) << fixed << setprecision(1) << hr
                << setw(15) << fixed << setprecision(2) << cost
                << endl;

            names.push_back(name);
            rentals.push_back(totalR);
            hours.push_back(hr);
            prices.push_back(cost);

            totalVehicles++;
            totalInventoryValue += cost;

            if (totalR > maxRental) {
                maxRental = totalR;
                mostRentedName = name;
            }
        }
        cout << string(80, '-') << endl;
        cout << "\n";

        // 4. BAR GRAPH (Based on Rentals)
        cout << "=== RENTAL GRAPH (By Total Rentals) ===\n\n";
        for (int i = 0; i < names.size(); i++) {
            cout << setw(20) << left << names[i] << " | ";
            int barLength = rentals[i];
            if (barLength > 50) barLength = 50; 

            for (int j = 0; j < barLength; j++)
                cout << "#";

            cout << " (" << rentals[i] << ")\n";
        }

        // 5. SUMMARY
        cout << "\n=============================================\n";
        cout << "Most Rented Kids Electric Vehicle : " << mostRentedName << "\n";
        cout << "Total Number of Kids EV          : " << totalVehicles << "\n";
        cout << "Total Inventory Value            : RM "
            << fixed << setprecision(2) << totalInventoryValue << "\n";
        cout << "=============================================\n";

        cout << "Press ENTER to continue...\n\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        system("cls");

    }
    catch (sql::SQLException& e) {
        cerr << "Error generating report: " << e.what() << endl;
    }
}


