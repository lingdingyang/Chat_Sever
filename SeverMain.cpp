#include "MSever.h"
#include "User.h"
#include "Cmd.h"
#include <jsoncpp/json/json.h>
#include <map>
using namespace std;
Json::Reader reader;
Json::FastWriter fw;
map<int, string> fd_to_name;
map<string, int> name_to_fd;
int main()
{
    MSever s(5050);
    int epollfd = epoll_create(1);
    epoll_event ev;
    ev.data.fd = s.get_sever_fd();
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, s.get_sever_fd(), &ev);
    epoll_event evs[10];
    int listenfd = s.get_sever_fd();
    while (true)
    {
        int inds = epoll_wait(epollfd, evs, 10, 0);
        if (inds == -1)
        {
            perror("epoll wait error");
            return -1;
        }
        if (inds == 0)
        {
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
                if (s.m_recv(evs[i].data.fd, msg, 0) == false)
                {
                    cout << "连接断开" << endl;
                    name_to_fd.erase(fd_to_name[evs[i].data.fd]);
                    fd_to_name.erase(evs[i].data.fd);
                    close(evs[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evs[i].data.fd, 0);
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
                        if (fd_to_name.find(evs[i].data.fd) == fd_to_name.end())
                        {
                            if (name_to_fd.find(name) != name_to_fd.end())
                            {
                                Json::Value obj;
                                obj["data"] = 0;
                                cout << "用户名存在" << endl;
                                s.m_send(evs[i].data.fd, fw.write(obj));
                            }
                            else
                            {
                                fd_to_name[evs[i].data.fd] = name;
                                name_to_fd[name] = evs[i].data.fd;
                                Json::Value obj;
                                obj["data"] = 1;
                                cout << "登录成功" << endl;
                                s.m_send(evs[i].data.fd, fw.write(obj));
                            }
                        }
                        else
                        {
                            Json::Value obj;
                            obj["data"] = 0;
                            cout << "重复登录" << endl;
                            s.m_send(evs[i].data.fd, fw.write(obj));
                        }
                    }
                    else if (v["cmd"].asInt() == OP::LIST)
                    {
                        cout << "列表指令" << endl;
                        string res = "";
                        for (auto ii = fd_to_name.begin(); ii != fd_to_name.end(); ++ii)
                        {
                            res += ii->second + " ";
                        }
                        cout << "待发送的列表：" << res << endl;
                        s.m_send(evs[i].data.fd, res);
                    }
                    else if (v["cmd"].asInt() == OP::SEND)
                    {
                        cout << "发送指令" << endl;
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