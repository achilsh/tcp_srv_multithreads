/**
 * @file: t_client.hpp
 * @brief: 
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

