#include "MSever.h"
using namespace std;
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
                    close(evs[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evs[i].data.fd, 0);
                }
                else
                {
                    cout << "收到消息：" << msg << endl;
                }
            }
        }
    }
}