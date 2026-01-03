#pragma once
#include <string>

bool authenticateUser(const std::string& username,
    const std::string& password,
    std::string& role,
    int& userID);

bool loginMenu();
void adminMenu(int userID);
void staffMenu(int userID);

