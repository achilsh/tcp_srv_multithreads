#include  "t_eventlisten.hpp"
#include  "t_tcpsrv.hpp"

namespace T_TCP
{
    ListenConn::ListenConn(int ifd, void* pData):ConnBase(ifd), m_pData((TcpSrv*)pData)
    {
    }
    ListenConn::~ListenConn()
    {
    }

    bool ListenConn::AddEvent(struct event_base *pEvBase, int iEvent, void *arg)
    {
        if (pEvBase == NULL)
        {
            return false;
        }
        
        if (EV_READ & iEvent)
        {
            if ( 0  != event_assign(&m_stREvent, pEvBase, m_iFd, iEvent, ReadCallBack, this))
            {
                std::cout << "event_assign() fail for read in listenconn" << std::endl;
                return false;
            }
            if ( 0 !=  event_add(&m_stREvent, NULL) )
            {
                std::cout << "event_add() fail for read in listenconn" << std::endl;
                return false;
            }
            std::cout << "add event succ in listenconn (2)" << std::endl;
            return true;
        }
        else if (iEvent & EV_WRITE)
        {
            event_assign(&m_stWEvent, pEvBase, m_iFd, iEvent, WriteCallBack, this);
            event_add(&m_stWEvent, NULL);
            return true;
        }
        else 
        {
        
        }

        return false;
    }

    void ListenConn::WriteCallBack(int iFd, short sEvent, void *pData)
    {
        //listen conn need not to write event.
    }

    void ListenConn::ReadCallBack(int iFd, short sEvent, void *pData) 
    {
        ListenConn* pConn = (ListenConn*) pData; 
        if (pConn == NULL)
        {
            return;
        }
        std::cout << "listen conn recv read event, client new conn" << std::endl;
        pConn->DoRead();
    }
    
    bool ListenConn::DoRead()
    {
        TcpSrv* tcpSrv = (TcpSrv*) m_pData;
        return tcpSrv->Accept();
    }

    bool ListenConn::DelEvent(struct event_base *pEvBase, int iEvent, void *arg)
    {
        std::cout << "call default op in DelEvent" << std::endl;
        return true;
    }

}
