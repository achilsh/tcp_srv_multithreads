#include "t_eventpipe.hpp"
#include "t_worktask.hpp"

namespace T_TCP
{
    PipeConn::PipeConn(int ifd, void* pData): ConnBase(ifd), m_pData(pData)
    {
    }
    //
    PipeConn::~PipeConn()
    {
    }

    bool PipeConn::AddEvent(struct event_base *pEvBase, int iEvent, void *arg)
    {
        if (pEvBase == NULL)
        {
            return false;
        }
        if (EV_READ & iEvent)
        {
            event_assign(&m_stREvent, pEvBase, m_iFd, iEvent, ReadCallBack, this);
            event_add(&m_stREvent, NULL); 
        }
        else if (iEvent & EV_WRITE)
        {
            event_assign(&m_stWEvent, pEvBase, m_iFd, iEvent, WriteCallBack, this);
            event_add(&m_stWEvent, NULL);
        }
        else 
        {

        }
        return true;
    }

    void PipeConn::WriteCallBack(int iFd, short sEvent, void *pData)
    {
    }

    void PipeConn::ReadCallBack(int iFd, short sEvent, void *pData)
    {
        char buf[1];
        if (::read(iFd, buf, 1) != 1)
        {
            std::cout << "read buf from pipe fail" << std::endl;
            return ;
        }
        PipeConn* pConn = (PipeConn*) pData; 
        if (pConn == NULL)
        {
            std::cout << "input param is empty" << std::endl;
            return;
        }
        pConn->DoRead(buf, sizeof(buf));
    }

    void PipeConn::DoRead(char *pData, int iLen)
    {
        WorkerTask* pTask = (WorkerTask*)m_pData;
        pTask->NotifyRecvConn(pData, iLen);
    }
    
    //TODO:
    bool PipeConn::DelEvent(struct event_base *pEvBase, int iEvent, void *arg)
    {
        return true;
    }
}
