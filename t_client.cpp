#include "t_client.hpp"

namespace T_CLIENT
{
    TestOneClient::TestOneClient(const std::string& sRemoteIp, int iRemotePort, int iSendTimes)
        :m_sRemoteIp(sRemoteIp), m_iRemotePort(iRemotePort), m_iTestNums(iSendTimes), m_iFd(-1), 
        m_pCodec(NULL)
    {
    }

    TestOneClient::~TestOneClient()
    {
        if (m_iFd > 0)
        {
            close(m_iFd);
            m_iFd  = -1;
        }
        ///
        if (m_pCodec)
        {
            delete m_pCodec;
            m_pCodec = NULL;
        }
    }

    int  TestOneClient::Init()
    {
        m_iFd = socket( AF_INET,  SOCK_STREAM , 0 );
        if (m_iFd < 0)
        {
            std::cout << "sockect() call fail" << std::endl;
            return -1;
        }

        m_stDestAddr.sin_family = AF_INET;
        m_stDestAddr.sin_addr.s_addr = inet_addr(const_cast<char*> (m_sRemoteIp.c_str()));
        m_stDestAddr.sin_port=htons(m_iRemotePort);
        int iRet = ::connect( m_iFd,(struct sockaddr *)&m_stDestAddr, sizeof(m_stDestAddr) );
        if (iRet < 0)
        {
            perror("connect remote tcp srv fail");
            return -1;
        }
        m_pCodec = new BusiCodec(1);

        return 0;
    }
    //
    int TestOneClient::main()
    {
        for (int i = 0; i < m_iTestNums; ++i)
        {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d'client is test!", i);
            int iSendLen = 0;
            char sSendBuf[1024] = {0};
            if ( RET_RECV_OK !=  m_pCodec->EnCode(buf, strlen(buf)+1, sSendBuf, &iSendLen) )
            {
                std::cout << "encode client data fail" << std::endl;
                break;
            }

            std::string sBuf;
            sBuf.assign(buf, strlen(buf)+1);
            int iRet = 0;
            while ( iSendLen > 0 )
            {
                iRet = ::send(m_iFd, sSendBuf, iSendLen, 0);
                if (iRet < 0)
                {
                    std::cout << "send client data fail" << std::endl;
                    break;
                }
                iSendLen -= iRet;
            }
            std::cout << "send  client data  done, next  to recv response, send data: " << sBuf << std::endl;

            char rBuf[1024]; 
            char rBodybuf[1024];
            ::memset(rBuf, 0, sizeof(rBuf));
            iRet = 0;
            int iRLen = 0;
            int iDecodeMsgLen = 0;
            
            do  {
                iRet = ::recv(m_iFd, rBuf + iRet, sizeof(rBuf) - iRet, 0);
                if (iRet < 0)
                {
                    std::cout << "recv data from srv fail" << std::endl;
                    break;
                }
                else if (iRet == 0)
                {
                    break;
                }

                iRLen += iRet;
                enum CODERET retDecod = m_pCodec->DeCode(rBuf, iRLen, rBodybuf, &iDecodeMsgLen );
                if (retDecod == RET_RECV_OK)
                {
                    break;
                }
                else 
                {

                }
            } while(1);

            std::string sRBuf;
            sRBuf.assign(rBodybuf, iDecodeMsgLen - sizeof(4));
            std::cout << "recv reponse: " << sRBuf << std::endl;
        }
        this->Stop();
        return 0;
    }

    /***********************************************************************
    *
    * 
    **********************************************************************/
    TestClient::TestClient(int argc, char **argv) :m_bOk(false), m_iClientNums(0),m_iReqNums(0) 
    {
        if (ParseCmd(argc, argv) == false)
        {
            Usage(argv[0]);
            return ;
        }
        m_bOk = true;
    }

    TestClient::~TestClient()
    {
    }

    bool TestClient::ParseCmd(int argc, char **argv)
    {
        if (argc <5)
        {   
            std::cout << "client run param less than 5" << std::endl;
            return false;
        }

        if (strlen(argv[1]) == 0)
        {
            std::cout << "remote ip is empty" << std::endl;
            return false;
        }
        m_sRemoteIp.assign(argv[1]);

        int iPort = ::atoi(argv[2]);
        if (iPort <= 0)
        {
            std::cout << "remote port less than 0" << std::endl;
            return false;
        }
        m_iRemotePort = iPort;
        
        m_iClientNums = ::atoi(argv[3]);
        if (m_iClientNums <= 0)
        {
            std::cout << "client nums less than 0 " << std::endl;
            return false;
        }

        m_iReqNums = ::atoi(argv[4]);
        if  (m_iReqNums <= 0)
        {
            std::cout << "req nums for each client less than 0 " << std::endl;
            return false;
        }
        return true;
    }

    void TestClient::Usage(const char *pBinName)
    {
        std::cout <<"Usage: " << pBinName << "  remoteIp  " << "   remotePort  " << 
            " clientnums  " << " reqnum_in_one_client" <<std::endl; 
    }

    //
    int TestClient::main()
    {
        std::vector<TestOneClient*> vTestClientNode;
        for (int i = 0;  i < m_iClientNums; ++i)
        {
            TestOneClient* pNode = new TestOneClient(m_sRemoteIp, m_iRemotePort, m_iReqNums);
            vTestClientNode.push_back(pNode);
        }

        for (std::vector<TestOneClient*>::iterator it = vTestClientNode.begin(); it != vTestClientNode.end(); ++it)
        {
            (*it)->Start();
        }
        std::cout << "all client thread start " << std::endl;

        for (std::vector<TestOneClient*>::iterator it = vTestClientNode.begin(); it != vTestClientNode.end(); )
        {
            (*it)->JoinWork();
            delete (*it);
            it = vTestClientNode.erase(it);
        }
        return 0;
    }

}


//////////////////////////////////////////////
/////////////////////////////////////////////
int main(int argc, char **argv)
{
    T_CLIENT::TestClient client(argc, argv);
    client.main();
    
    return 0;
}
