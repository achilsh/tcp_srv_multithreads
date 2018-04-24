#include "t_socket.hpp"

namespace T_TCP
{
    Sock::Sock(const std::string& sIp,  int iPort): m_sSrvIp(sIp), m_iSrvPort(iPort), m_iFd(-1), m_err(-1)
    {
        CreateSock();
    }

    Sock::~Sock()
    {
        if (m_iFd > 0)
        {
            close(m_iFd);
            m_iFd = -1;
        }
    }

    int Sock::CreateSock()
    {
       m_iFd = ::socket( AF_INET, SOCK_STREAM, 0); 
       if (m_iFd <= 0)
       {
           SetErr(SOCK_ERR);
           std::cout << "create local socket fd fail" << std::endl;
           return -1;
       }

       int iOptFlags;
       if ((iOptFlags = ::fcntl(m_iFd,F_GETFL,0)) < 0 
            || ::fcntl(m_iFd, F_SETFL, iOptFlags |  O_NONBLOCK) < 0)
       {
           perror("setting O_NONBLOCK");
           SetErr(SOCK_ERR);
           ::close(m_iFd);
           m_iFd = -1;
           return -2;
       }
       return m_iFd;
    }

    bool Sock::SetSockOpt()
    {
        if (m_iFd <= 0)
        {
            return false;
        }
        //
        int iOptFlag = 1;
        int iRet = setsockopt(m_iFd,  SOL_SOCKET, SO_REUSEADDR, (void*)& iOptFlag, sizeof(iOptFlag));
        if (iRet < 0)
        {
            SetErr(SOCK_ERR);
            perror("setsockopt SO_REUSEADDR fail");
            return false;
        }

        iRet = ::setsockopt(m_iFd, SOL_SOCKET, SO_KEEPALIVE,(void*)&iOptFlag,sizeof(iOptFlag));
        if (iRet < 0)
        {
            SetErr(SOCK_ERR);
            perror("setsockopt SO_KEEPALIVE fail");
            return false;
        }

        iRet = ::setsockopt(m_iFd, IPPROTO_TCP, TCP_NODELAY, (void*)&iOptFlag,sizeof(iOptFlag));
        if (iRet < 0)
        {
            SetErr(SOCK_ERR);
            perror("setsockopt TCP_NODELAY fail");
            return false;
        }
        return true;
    }

    int Sock::GerErr()
    {
        return m_err;
    }
    //
    void Sock::SetErr(int err)
    {
        m_err = err;
    }
    //
    int Sock::GetSockFd()
    {
        return m_iFd;
    }
    //
    int Sock::Send(const char* pSendBuf, int iLen)
    {
        if (pSendBuf == NULL  || iLen < 0 || m_iFd <= 0)
        {
            return -1;
        }
        int iRet = ::send(m_iFd, pSendBuf, iLen, 0);
        if (iRet < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                SetErr(SOCK_AGAIN);
                return -1;
            }

            SetErr(SOCK_ERR);
            perror("send err");
            return -1;
        }
        return iRet;
    }

    int Sock::Recv(char* pRcvBuf, int iMaxLen)
    {
        if (pRcvBuf == NULL || m_iFd <= 0)
        {
            return -1;
        }
        int iRet = ::recv(m_iFd, pRcvBuf, iMaxLen, 0);
        if (iRet < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                SetErr(SOCK_AGAIN);
                return -1;
            }
            SetErr(SOCK_ERR);
            perror("recv fail");
            return -2;
        }
        return iRet;
    }

    bool Sock::Listen()
    {
        if (m_iFd <= 0)
        {
            return false;
        }

        SetSockOpt();
        struct sockaddr_in addrListen;
        memset(&addrListen, 0, sizeof(struct sockaddr_in));

        addrListen.sin_family = AF_INET;
        addrListen.sin_addr.s_addr = inet_addr(m_sSrvIp.c_str());
        addrListen.sin_port = ::htons(m_iSrvPort);
        int addrLen = sizeof(addrListen);

        int iRet = ::bind(m_iFd, (struct sockaddr *)&addrListen, sizeof(addrListen));
        if (iRet < 0)
        {
            SetErr(SOCK_ERR);
            perror("bind socket fail");
            return false;
        }
        iRet = ::listen(m_iFd, 1024);
        if (iRet < 0)
        {
            SetErr(SOCK_ERR);
            perror("listen socket fail");
            return false;
        }
        return true;
    }
    //
    bool Sock::Connect()
    {
        if (m_iFd <= 0)
        {
            return false;
        }

        struct sockaddr_in addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET; 
        addrRemote.sin_port = ::htons(m_iSrvPort);
        inet_pton(AF_INET, m_sSrvIp.c_str(), &addrRemote.sin_addr);

        int iRet = ::connect(m_iFd, (struct sockaddr*)&addrRemote, sizeof(addrRemote));
        if (iRet < 0)
        {
            if (errno == EINPROGRESS)
            {
                std::cout << "connect is connecting....“" << std::endl;
                return true;
            }
            perror("connect err");
            SetErr(SOCK_ERR);
            return false;
        }
        return true;
    }

    int Sock::SockAccept(struct sockaddr* pClientAddr)
    {
       if (pClientAddr == NULL)
       {
           return -1;
       }

       socklen_t addrLen = sizeof(*pClientAddr);
       int iRet = ::accept(m_iFd, pClientAddr, &addrLen); 
       if (iRet < 0)
       {
           SetErr(SOCK_ERR);
           perror("accept fail");
           return iRet;
       }
       return iRet;
    }
    //
}
