#pragma once
#include <iostream>
using namespace std;
class User
{
private:
    string name;
    int fd;

public:
    User(string name, int fd);
    string getName() const;
    int getFd() const;
    ~User();
};
