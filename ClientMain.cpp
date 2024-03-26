#include "MClient.h"
using namespace std;
int main()
{
    MClient c("localhost", 5050);
    if (c.m_connect() == false)
    {
        cout << "连接失败" << endl;
        return -1;
    }
    while (true)
    {
        string msg;
        cout << "请输入内容：" << endl;
        cin >> msg;
        if (c.m_send(msg) == false)
        {
            cout << "发送失败" << endl;
            return -1;
        }
        cout << "发送成功" << endl;
    }
}