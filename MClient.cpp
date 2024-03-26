#include "MClient.h"

bool MClient::m_recv(string &msg, int time_out)
{
    if (sever_fd == -1)
    {
        return false;
    }
    if (time_out >= 0)
    {
        pollfd poll_fd;
        poll_fd.fd = sever_fd;
        poll_fd.events = POLLIN;
        if (poll(&poll_fd, 1, time_out * 1000) <= 0)
        {
            return false;
        }
    }
    int read_len;
    if (recvn((char *)&read_len, 4) == false)
    {
        return false;
    }
    msg.clear();
    msg.resize(read_len);
    if (recvn(msg.data(), read_len) == false)
    {
        return false;
    }
    return true;
}

bool MClient::m_send(const string &msg)
{
    if (sever_fd == -1)
    {
        return false;
    }
    int write_len = msg.length();
    if (sendn((char *)&write_len, 4) == false)
    {
        cout << "发送失败1" << endl;
        return false;
    }
    if (sendn(msg.c_str(), write_len) == false)
    {
        cout << "发送失败2" << endl;
        return false;
    }
    return true;
}

bool MClient::recvn(char *buffer, int len)
{
    int r_all = len;
    int r_already = 0;
    int r_left = len;
    int r;
    while (r_already < r_all)
    {
        r = recv(sever_fd, buffer, r_left, 0);
        if (r <= 0)
        {
            return false;
        }
        r_already += r;
        r_left = r_all - r_already;
    }
    return true;
}

bool MClient::sendn(const char *buffer, int len)
{
    int s_all = len;
    int s_already = 0;
    int s_left = len;
    int s;
    while (s_all > s_already)
    {
        s = send(sever_fd, buffer, s_left, 0);
        if (s <= 0)
        {
            return false;
        }
        s_already += s;
        s_left = s_all - s_already;
    }
    return true;
}

MClient::MClient(const string &ip, unsigned short port) : ip(ip), port(port)
{
    sever_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sever_fd == -1)
    {
        perror("socket error");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *h = gethostbyname(ip.c_str());
    if (h == 0)
    {
        perror("get ip error");
        return;
    }
    memcpy(&addr.sin_addr, h->h_addr_list[0], h->h_length);
}

bool MClient::m_connect()
{
    if (connect(sever_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect error");
        return false;
    }
    return true;
}

MClient::~MClient()
{
    if (sever_fd == -1)
    {
        close(sever_fd);
        sever_fd = -1;
    }
}
