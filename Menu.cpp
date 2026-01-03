#include "Menu.h"
#include "KidsElectricVehicle.h"
#include "Rental.h"
#include "customer.h"

#include <iostream>
using namespace std;

void MainMenu(int userID) {
    int choice;
    do {
        cout << "\n-----------------------------------------------\n\n";
        cout << "    KIDS ELECTRIC VEHICLE RENTAL MANAGEMENT    \n\n";
        cout << "-----------------------------------------------\n";
        cout << "\nSELECT OPTIONS\n";
        cout << "\n1. Kids Electric Vehicle Module\n";
        cout << "2. Customer Management\n";
        cout << "3. Rental Management\n";
        cout << "4. Logout\n";
        
        while (true) {
            cout << "\nEnter your choice: ";

            if (cin >> choice) {
                if (choice >= 1 && choice <= 5) {
                    break;
                }
                else {
                    cout << "Please enter a valid number.\n";
                }
            }
            else {
                cout << "Invalid input. Please enter numbers only.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }


        switch (choice) {
        case 1:
            system("cls");
            KidsElectricVehicleMenu();
            break;
        case 2: 
            system("cls");
            CustomerMenu(); 
            break;
        case 3: 
            system("cls");
            RentalMenu(userID); 
            break;
        case 4: 
            system("cls");
            cout << "Exiting program...\n"; 
            break;
        default: 
            cout << "Option not yet implemented.\n"; 
            break;
        }
    } while (choice != 4);
}
