#include <iostream>
#include "t_eventconn.hpp"
#include "t_worktask.hpp"
#include "t_proto.hpp"
#include <errno.h>

#include <string>

namespace T_TCP
{
    const int AcceptConn::CNT_STATIC_BUF_MAX_LEN;
    //
    AcceptConn::AcceptConn(int ifd, void* pData):ConnBase(ifd), m_pData((WorkerTask*)pData), m_pCodec(NULL), 
                 m_pRecvBuf(NULL), m_iCurrRecvOffSet(0),
                 m_pSendBuf(NULL), m_iCurrSendOffSet(0)
    {
        m_pRecvBuf = new char[AcceptConn::CNT_STATIC_BUF_MAX_LEN];
        m_pSendBuf = new char[AcceptConn::CNT_STATIC_BUF_MAX_LEN];
        m_pCodec = new BusiCodec(1);
    }

    AcceptConn::~AcceptConn()
    {
        delete [] m_pRecvBuf;
        delete [] m_pSendBuf;
        delete m_pCodec;
        m_iCurrRecvOffSet = 0;
        m_iCurrSendOffSet = 0;
    }

    bool AcceptConn::AddEvent(struct event_base *pEvBase, int iEvent, void *arg)
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
        else 
        {
            event_assign(&m_stWEvent, pEvBase, m_iFd, iEvent, WriteCallBack, this);
            event_add(&m_stWEvent, NULL);
        }
        return true;
    }

    void AcceptConn::ReadCallBack(int iFd, short sEvent, void *pData)
    {
        if (iFd <= 0)
        {
            return ;
        }

        AcceptConn* pConn = (AcceptConn*)pData;
        if (pConn == NULL)
        {
            return ;
        }
        pConn->DoRead();
    }
    //
    bool AcceptConn::DoRead()
    {
        std::cout << "on fd: " << m_iFd << " , begin to recv data" << std::endl;
        int iRet = ::recv(m_iFd, m_pRecvBuf + m_iCurrRecvOffSet, CNT_STATIC_BUF_MAX_LEN - m_iCurrRecvOffSet, 0);
        if (iRet < 0)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                std::cout << "recv err from client " << std::endl;
                m_iCurrRecvOffSet = 0;
                m_iCurrSendOffSet = 0;
                m_pData->DeleteAcceptConn(m_iFd);
                return false;
            }
            return true;
        }

        if (iRet == 0)
        {
            std::cout << "remote client closet socket fd: " << m_iFd << std::endl;
            //clear recv buf data;
            m_iCurrRecvOffSet = 0;
            m_iCurrSendOffSet = 0;
            m_pData->DeleteAcceptConn(m_iFd);
            return true;
        }
        //
        m_iCurrRecvOffSet += iRet;
        
        char msgBuf[1024];
        memset(msgBuf, 0, sizeof(msgBuf));
        int iMsgLen = 0;
        enum CODERET ret = m_pCodec->DeCode(m_pRecvBuf, m_iCurrRecvOffSet, msgBuf, &iMsgLen);        
        if (ret == RET_RECV_NOT_COMPLETE)
        {
            return true;
        }
        if (ret == RET_RECV_OK)
        {
            std::string printMsg;
            printMsg.assign(msgBuf, iMsgLen - sizeof(int));
            std::cout << "recv msg data: " << printMsg << std::endl;
            ::memmove(m_pRecvBuf, m_pRecvBuf + iMsgLen, m_iCurrRecvOffSet - iMsgLen );
            m_iCurrRecvOffSet -=  iMsgLen;

            //next give response to client, then close.
            struct event_base *pEvBase = event_get_base(&m_stREvent);
            char sSendBuf[1024];
            memset(sSendBuf,0,sizeof(sSendBuf));
            snprintf(sSendBuf,sizeof(sSendBuf),  "client: %s, srv recv: this is response", printMsg.c_str()); 
           
            ret = m_pCodec->EnCode(sSendBuf, strlen(sSendBuf) + 1, m_pSendBuf, &m_iCurrSendOffSet);
            if (ret != RET_RECV_OK)
            {
                std::cout << "encode send buf data fail" << std::endl;
                return false;
            }
            AddEvent(pEvBase, EV_WRITE, this);
        }
        //
        return true;
    }

    void AcceptConn::WriteCallBack(int iFd, short sEvent, void *pData)
    {
        if (iFd <= 0)
        {
            return ;
        }
        AcceptConn* pConn = (AcceptConn*)pData;
        if (pConn == NULL)
        {
            return ;
        }
        pConn->DoWrite();
    }

    bool AcceptConn::DoWrite()
    {
        if (m_iCurrSendOffSet > 0 && m_pSendBuf)
        {
            int iRet = ::write(m_iFd, m_pSendBuf, m_iCurrSendOffSet);
            if (iRet < 0)
            {
                if (errno != EAGAIN && errno !=  EWOULDBLOCK)
                {
                    std::cout << "write data fail in fd: " << m_iFd << std::endl;
                    m_pData->DeleteAcceptConn(m_iFd);
                    return false;
                }
                return true;
            }
            else 
            {
                ::memmove(m_pSendBuf, m_pSendBuf + iRet, m_iCurrSendOffSet - iRet);
                m_iCurrSendOffSet -= iRet;
            }

            if (m_iCurrSendOffSet > 0)
            {
                struct event_base *pEvBase = event_get_base(&m_stWEvent);
                std::cout << "this is other data buf need to send, so add write event again." << std::endl;
                AddEvent(pEvBase, EV_WRITE, this);
            }
            return true;
        }
        return false;
    }

    bool AcceptConn::DelEvent(struct event_base *pEvBase, int iEvent, void *arg)
    {
        return true;
    }
}
