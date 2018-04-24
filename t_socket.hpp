/**
 * @file: t_socket.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-23
 */

#ifndef _t_socket_hpp_
#define _t_socket_hpp_
//
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
//
#include <iostream>
#include <string>
//
namespace T_TCP
{
    #define SOCK_OK     (0)
    #define SOCK_AGAIN  (0)
    #define SOCK_ERR    errno
   
    class Sock 
    {
     public:
      Sock(const std::string &sIp, int iPort);
      virtual ~Sock();
      int GetSockFd();
      bool Listen();
      bool Connect();

      int SockAccept(struct sockaddr* pClientAddr);
      
      int Send(const char* pSendBuf, int iLen);
      int Recv(char* pRcvBuf, int iMaxLen);
      int GerErr();
     private:
      bool SetSockOpt();
      int CreateSock();
      void SetErr(int err);
     private:
      std::string m_sSrvIp;
      int m_iSrvPort;
      int m_iFd;
      int m_err;
    };
}
#endif
