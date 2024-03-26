#pragma once
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <vector>
using namespace std;
class MClient
{
private:
    unsigned short port;
    string ip;
    sockaddr_in addr;
    int sever_fd = -1;
    int client_count;
    vector<int> client_fds;

public:
    MClient(const string &ip, unsigned short port);
    MClient(const MClient &) = delete;
    bool m_connect();
    bool m_recv(string &msg, int time_out);
    bool m_send(const string &msg);
    bool recvn(char *buffer, int len);
    bool sendn(const char *buffer, int len);
    ~MClient();
};