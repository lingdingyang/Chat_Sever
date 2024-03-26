#include "MSever.h"

MSever::MSever(unsigned short port) : port(port)
{
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket error");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind error");
        return;
    }
    if (listen(listen_fd, 5) == -1)
    {
        perror("listen error");
        return;
    }
}

bool MSever::m_recv(int fd, string &msg, int time_out)
{
    if (fd == -1)
    {
        return false;
    }
    if (time_out >= 0)
    {
        pollfd poll_fd;
        poll_fd.fd = fd;
        poll_fd.events = POLLIN;
        if (poll(&poll_fd, 1, time_out * 1000) <= 0)
        {
            return false;
        }
    }
    int read_len;
    if (recvn(fd, (char *)&read_len, 4) == false)
    {
        return false;
    }
    msg.clear();
    msg.resize(read_len);
    if (recvn(fd, msg.data(), read_len) == false)
    {
        return false;
    }
    return true;
}

bool MSever::m_send(int fd, const string &msg)
{
    if (fd == -1)
    {
        return false;
    }
    int write_len = msg.length();
    if (sendn(fd, (char *)&write_len, 4) == false)
    {
        return false;
    }
    if (sendn(fd, msg.c_str(), write_len) == false)
    {
        return false;
    }
    return true;
}

bool MSever::recvn(int fd, char *buffer, int len)
{
    int r_all = len;
    int r_already = 0;
    int r_left = len;
    int r;
    while (r_already < r_all)
    {
        r = recv(fd, buffer, r_left, 0);
        if (r <= 0)
        {
            return false;
        }
        r_already += r;
        r_left = r_all - r_already;
    }
    return true;
}

bool MSever::sendn(int fd, const char *buffer, int len)
{
    int s_all = len;
    int s_already = 0;
    int s_left = len;
    int s;
    while (s_all > s_already)
    {
        s = send(fd, buffer, s_left, 0);
        if (s <= 0)
        {
            return false;
        }
        s_already += s;
        s_left = s_all - s_already;
    }
    return true;
}

int MSever::accept_client()
{
    sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd;
    client_fd = accept(listen_fd, (sockaddr *)&client, &client_len);
    if (client_fd == -1)
    {
        perror("accept error");
        return -1;
    }
    return client_fd;
}

int MSever::get_sever_fd() const
{
    return listen_fd;
}

MSever::~MSever()
{
}
