#include "Database.h"
#include <iostream>
using namespace std;

Database::Database() {
    server = "tcp://127.0.0.1:3306";
    username = "root";
    password = "";
    schema = "kidselectricvehicledb";

    try {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        con->setSchema(schema);
        /*cout << "Database connected successfully.\n";*/
    }
    catch (sql::SQLException& e) {
        cout << "Database connection failed: " << e.what() << endl;
        con = nullptr;
    }
}

Database::~Database() {
    if (con) delete con;
}

sql::Connection* Database::getConnection() {
    return con;
}
