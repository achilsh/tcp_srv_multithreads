#include "t_tcpsrv.hpp"
#include "t_eventlisten.hpp"

namespace  T_TCP
{
    TcpSrv::TcpSrv(const std::string& sIp, int iPort, int threadNums): m_bInit(false), p_mListenSock(NULL),
                    m_sLocalIp(sIp), m_iLocalPort(iPort), m_pEventBase(NULL), m_pThreadPool(NULL),
                    m_iThreadPoolNums(threadNums), p_mListenConn(NULL)

    {
        m_bInit = Init();
    }

    //
    TcpSrv::~TcpSrv()
    {
        m_bInit = false;
        if (p_mListenSock)
        {
            delete p_mListenSock;
            p_mListenSock = NULL;
        }

        if (m_pThreadPool)
        {
            delete m_pThreadPool;
            m_pThreadPool = NULL;
        }

        if (p_mListenConn)
        {
            delete p_mListenConn;
            p_mListenConn = NULL;
        }

        if (m_pAcceptConnListFree)
        {
            delete m_pAcceptConnListFree;
            m_pAcceptConnListFree = NULL;
        }
        //
        if (m_pEventBase)
        {
            event_base_free(m_pEventBase); 
            m_pEventBase = NULL;
        }
    }

    //
    bool TcpSrv::Init()
    {
        if (p_mListenSock == NULL)
        {
            p_mListenSock = new Sock(m_sLocalIp, m_iLocalPort);
        }

        bool bRet = p_mListenSock->Listen();
        if (bRet == false)
        {
            std::cout << "Listen sock fail, err no: " << p_mListenSock->GerErr() << std::endl;
            return false;
        }

        struct event_config *ev_config = event_config_new();
        event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);
        m_pEventBase = event_base_new_with_config(ev_config);
        event_config_free(ev_config);
        std::cout << "tcp srv event base init, addr: " << m_pEventBase << std::endl;

       m_pAcceptConnListFree =  new QueueSafe<QueueItem>(100);
       std::cout << "create 100 len free-queue for accept_conn_item" << std::endl;
        // 
        std::cout << "tcp srv init succ" << std::endl;
        return true;
    }

    bool TcpSrv::RegisteAcceptConn()
    {
       p_mListenConn= new  ListenConn(p_mListenSock->GetSockFd(), (void*)this); 
       if (p_mListenConn == NULL)
       {
           return false;
       }
        
       bool bRet =  p_mListenConn->AddEvent(m_pEventBase, EV_READ|EV_PERSIST, this);
       if (false == bRet)
       {
            delete p_mListenConn;
            p_mListenConn = NULL;
            std::cout << "add read event in listen conn fail(1)" << std::endl;
            return false;
       }
       std::cout << "add read event in listen conn succ" << std::endl;
       return true;
    }

    bool TcpSrv::Run()
    {
        if (m_bInit == false)
        {
            return false;
        }
        if (RegisteAcceptConn() == false)
        {
            std::cout << "register accept conn to event base fail" << std::endl;
            return false;
        }

        //init thread pools
        m_pThreadPool = new PthreadPools<WorkerTask>(m_iThreadPoolNums);
        if ( false ==  m_pThreadPool->StartAllThreads() )
        {
            std::cout << "start threads nums fail" << std::endl;
            return false;
        }

        //
        int iRet = event_base_loop(m_pEventBase, 0);
        if (iRet != 0)
        {
            std::cout << "event loop exit. " << std::endl;
            return false;
        }
        return true;
    }

    bool TcpSrv::Accept()
    {
        struct sockaddr_in clientAddr;
        int iAcceptFd = p_mListenSock->SockAccept((struct sockaddr*)&clientAddr);
        if (iAcceptFd < 0)
        {
            std::cout << "accept new connect fail" << std::endl;
            return false;
        }
        std::cout <<  "accept new client conn, new fd: " << iAcceptFd << std::endl;
        //从线程池中分配一个事件线程，用于独立接收报文和发送报文.
        //so,需要有个线程池.
        WorkerTask* pFreeTaskWork = m_pThreadPool->AllocateThread();
        if (pFreeTaskWork == NULL)
        {
            std::cout << " get free work task from thread pools fail" << std::endl;
            return false;
        }

        //
        QueueItem* pItem  =  m_pAcceptConnListFree->PopBack();
        pItem->iConnFd = iAcceptFd;
        m_AcceptConnListWorking.PushFront(pItem);
        
        //拼接一个新连接数据队列，并向发送任务的pipe上发送新请求命令字.
        if (false == pFreeTaskWork->NotifySendConn(&m_AcceptConnListWorking, m_pAcceptConnListFree))
        {
            std::cout << "send notify new conn cmd fail " << std::endl;
            return false;
        }
        std::cout << "send new conn cmd by pipe, new fd: " << iAcceptFd << std::endl;
        return true;
    }

}
