#include "MSever.h"
#include "User.h"
#include "Cmd.h"
#include <jsoncpp/json/json.h>
#include <map>
#include "Error.h"
using namespace std;
#define CLIENT_COUNT 1024
Json::Reader reader;
Json::FastWriter fw;
map<int, string> fd_to_name;
map<string, int> name_to_fd;
string get_user_name(int fd)
{
    if (fd_to_name.find(fd) == fd_to_name.end())
    {
        return "";
    }
    return fd_to_name[fd];
}
int get_user_fd(const string &name)
{
    if (name_to_fd.find(name) == name_to_fd.end())
    {
        return -1;
    }
    return name_to_fd[name];
}
int epollfd = epoll_create(1);
MSever s(5050);
epoll_event evs[CLIENT_COUNT];
void add_user(int fd, const string &name)
{
    if (fd_to_name.find(fd) == fd_to_name.end())
    {
        if (name_to_fd.find(name) != name_to_fd.end())
        {
            Json::Value obj;
            obj["data"] = 0;
            cout << "用户名存在" << endl;
            s.m_send(fd, fw.write(obj));
        }
        else
        {
            fd_to_name[fd] = name;
            name_to_fd[name] = fd;
            Json::Value obj;
            obj["data"] = 1;
            cout << "登录成功" << endl;
            s.m_send(fd, fw.write(obj));
        }
    }
    else
    {
        Json::Value obj;
        obj["data"] = 0;
        cout << "重复登录" << endl;
        s.m_send(fd, fw.write(obj));
    }
}
void del_user(int fd, const string &name)
{
    name_to_fd.erase(name);
    fd_to_name.erase(fd);
    close(fd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
}
string message_to_json(const OP &cmd, const string &data, const string &sender, const string &recver, const ERROR_CODE &error = ERROR_CODE::NO_ERROR)
{
    Json::Value v;
    v["op"] = cmd;
    v["data"] = data;
    v["sender"] = sender;
    v["recver"] = recver;
    v["error"] = error;
    return fw.write(v);
}
void send_list(const string &sender, int sender_fd)
{
    string res = "";
    for (auto ii = fd_to_name.begin(); ii != fd_to_name.end(); ++ii)
    {
        res += ii->second + " ";
    }
    s.m_send(sender_fd, message_to_json(OP::LIST, res, "sever", sender));
}
void send_msg(const string &sender, const string &recver, const string &content, int sender_fd)
{
    if (name_to_fd.find(recver) == name_to_fd.end())
    {
        s.m_send(sender_fd, message_to_json(OP::SEND_BACK, content, sender, recver, ERROR_CODE::SEND_FAIL));
        return;
    }
    int send_fd = name_to_fd[recver];
    if (s.m_send(send_fd, message_to_json(OP::SEND, content, sender, recver)) == false)
    {
        s.m_send(sender_fd, message_to_json(OP::SEND_BACK, content, sender, recver, ERROR_CODE::SEND_FAIL));
    }
    else
    {
        s.m_send(sender_fd, message_to_json(OP::SEND_BACK, content, sender, recver, ERROR_CODE::NO_ERROR));
    }
}
int main()
{
    epoll_event ev;
    ev.data.fd = s.get_sever_fd();
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, s.get_sever_fd(), &ev);
    int listenfd = s.get_sever_fd();
    while (true)
    {
        int inds = epoll_wait(epollfd, evs, 10, 2 * 1000);
        if (inds == -1)
        {
            perror("epoll wait error");
            return -1;
        }
        if (inds == 0)
        {
            cout << "time out" << endl;
            continue;
        }
        for (int i = 0; i < inds; i++)
        {
            if (evs[i].data.fd == listenfd)
            {
                int clientfd = s.accept_client();
                if (clientfd == -1)
                {
                    return -1;
                }
                ev.data.fd = clientfd;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                cout << "连接成功" << endl;
            }
            else
            {
                string msg;
                bool recv_res = s.m_recv(evs[i].data.fd, msg, 0);
                if (recv_res == false)
                {
                    cout << "连接断开" << endl;
                    del_user(evs[i].data.fd, fd_to_name[evs[i].data.fd]);
                }
                else
                {
                    cout << "收到消息：" << msg << endl;
                    Json::Value v;
                    reader.parse(msg, v);
                    if (v["cmd"].asInt() == OP::LOGIN)
                    {
                        cout << "登录指令" << endl;
                        string name = v["data"].asString();
                        add_user(evs[i].data.fd, name);
                    }
                    else if (v["cmd"].asInt() == OP::LIST)
                    {
                        cout << "列表指令" << endl;

                        send_list(fd_to_name[evs[i].data.fd], evs[i].data.fd);
                    }
                    else if (v["cmd"].asInt() == OP::SEND)
                    {
                        cout << "发送指令" << endl;
                        Json::Value obj;
                        string sender = v["sender"].asString();
                        string recver = v["recver"].asString();
                        string content = v["data"].asString();
                        cout << "发送者：" << sender << " 接受者：" << recver << " 内容：" << content << endl;
                        send_msg(sender, recver, content, evs[i].data.fd);
                    }
                    else
                    {
                        cout << "wrong cmd" << endl;
                    }
                }
            }
        }
    }
}