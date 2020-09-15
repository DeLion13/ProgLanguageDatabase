#ifndef USER_H
#define USER_H
#pragma once

#include <string>

using std::string;

struct User {
    int     id;
    string  username;
    string  password_hash;
};

struct Status {
    int login = 0;
    int registration = 0;
};

#endif // USER_H
