#include "User.h"

User::User(string name, int fd) : name(name), fd(fd)
{
}

string User::getName() const
{
    return name;
}

int User::getFd() const
{
    return fd;
}

User::~User()
{
}
