#pragma once
#include "Database.h"
void CustomerMenu();
bool showSelectedCustomer(sql::Connection* con, int customerID);
void addCustomer();
void viewCustomer();