#pragma once
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <poll.h>
using namespace std;
class MSever
{
private:
    unsigned short port;
    sockaddr_in addr;
    int listen_fd = -1;

public:
    MSever(unsigned short port);
    MSever(const MSever &) = delete;
    bool m_recv(int fd, string &msg, int time_out);
    bool m_send(int fd, const string &msg);
    bool recvn(int fd, char *buffer, int len);
    bool sendn(int fd, const char *buffer, int len);
    int accept_client();
    int get_sever_fd() const;
    ~MSever();
};