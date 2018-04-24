/**
 * @file: t_client.hpp
 * @brief:
 *      tcp client 实现文件，采用多线程connect & send/recv 数据
 *      每个线程去connect tcp server. 每个线程内可重复发送多次。
 *      线程数和收发次数 参数化输入
 *         
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-19
 */

#ifndef _T_CLIENT_HPP_
#define _T_CLIENT_HPP_

#ifdef __cplusplus 
    extern "C" {
#endif 

#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
    }
#endif

#include <vector>
#include <iostream>
#include <string>
#include "t_thread.hpp"
#include "t_proto.hpp"

using namespace T_TCP;
using namespace std;


namespace T_CLIENT 
{

    /**
     * @brief: 一个客户端实现，其实也是一个线程的实现
     *    包括常规的connect, recv/send 接口的实现
     */
    class TestOneClient: public PthreadBase
    {
        public:
         TestOneClient(const std::string& sRemoteIp, int iRemotePort, int iSendTimes);
         virtual ~TestOneClient();
        protected:
         int main();
         virtual int  Init();
        private:
         std::string m_sRemoteIp;
         int m_iRemotePort;
         int m_iTestNums;
         int m_iFd;
         struct sockaddr_in  m_stDestAddr;
         BusiCodec* m_pCodec;
    };

    //////////////////////////
    /**
     * @brief: 
     *    包括命令解析和多个client 实例的创建和启动。
     */
    class TestClient 
    {
        public:
         TestClient(int argc, char **argv);
         virtual ~TestClient(); 
         int main();
        
        private:
         void Usage(const char *pBinName);
         bool ParseCmd(int argc, char **argv);

        private:
         bool m_bOk;
         int m_iClientNums;
         std::string m_sRemoteIp;
         int m_iRemotePort;
         int m_iReqNums;
    };
}

#endif 

