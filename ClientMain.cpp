#include "MClient.h"
#include "Cmd.h"
#include <jsoncpp/json/json.h>
#include <thread>
using namespace std;
Json::FastWriter fw;
Json::Reader reader;
MClient c("localhost", 5050);
string name;
void handle_recv()
{
    cout << "线程开始" << endl;
    string msg;
    while (true)
    {
        if (c.m_recv(msg, -1))
        {
            cout << "收到内容：" << msg << endl;
        }
    }
}

int main()
{
    if (c.m_connect() == false)
    {
        cout << "连接失败" << endl;
        return -1;
    }
    cout << "连接成功" << endl;
    while (1)
    {
        string input;
        string res;
        cout << "请输入用户名" << endl;
        cin >> input;
        Json::Value v;
        v["cmd"] = OP::LOGIN;
        v["data"] = input;
        if (c.m_send(fw.write(v)) == false)
        {
            cout << "登录失败" << endl;
            return -1;
        }
        if (c.m_recv(res, -1) == false)
        {
            cout << "接收登录结果失败" << endl;
            return -1;
        }
        reader.parse(res, v);
        if (v["data"] == 1)
        {
            cout << "登录成功" << endl;
            name = input;
            break;
        }
        else
        {
            cout << "登录失败，请重试" << endl;
        }
    }

    thread t(handle_recv);
    while (true)
    {
        string opt;
        cout << "请输入指令：（ 1：获取列表 2：发送）" << endl;
        cin >> opt;
        if (opt == "1")
        {
            Json::Value v;
            v["cmd"] = OP::LIST;
            c.m_send(fw.write(v));
            cout << "发送列表请求" << endl;
        }
        else if (opt == "2")
        {
            string recv_name;
            string msg;
            cout << "输入目标用户名" << endl;
            cin >> recv_name;
            Json::Value v;
            cout << "请输入内容" << endl;
            cin >> msg;
            v["cmd"] = OP::SEND;
            v["data"] = msg;
            v["sender"] = name;
            v["recver"] = recv_name;
            c.m_send(fw.write(v));
            cout << "发送列表请求" << endl;
        }
        else
        {
            cout << "指令错误" << endl;
        }
    }
}