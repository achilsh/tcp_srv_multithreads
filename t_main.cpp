/**
 * @file: t_main.cpp
 * @brief: 
 *       tcp 服务的入口
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-24
 */

#include <iostream>
#include <stdlib.h>

#include "t_tcpsrv.hpp"

using namespace std;
using namespace T_TCP;

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << argv[0] << "  ip  port   thread_num" << std::endl;
        return 0;
    }
    string sIp(argv[1]);
    int iPort = ::atoi(argv[2]);
    int iRunThreadNums = ::atoi(argv[3]);
    if (iRunThreadNums <= 0)
    {
        iRunThreadNums = 3;
    }

    TcpSrv tcpSrv(sIp, iPort, iRunThreadNums);
    if (tcpSrv.Run() == false) 
    {
        std::cout << "run tcp routine fail" << std::endl;
    } 
    else 
    {
        std::cout << "tcp main routine exit " << std::endl;
    }

    return 0;
}
