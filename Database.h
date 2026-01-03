#pragma once
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <string>

class Database {
private:
    sql::Driver* driver;
    sql::Connection* con;
    std::string server;
    std::string username;
    std::string password;
    std::string schema;

public:
    Database();
    ~Database();
    sql::Connection* getConnection();
};
