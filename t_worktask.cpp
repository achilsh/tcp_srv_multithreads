#include  "t_worktask.hpp"
#include  <unistd.h>
#include "t_eventpipe.hpp"
#include "t_eventconn.hpp"

namespace T_TCP
{
    WorkerTask::WorkerTask(): m_iNotifyRecvFd(-1), m_iNofitySendFd(-1), m_PthreadEventBase(NULL)
    {
    }

    WorkerTask:: WorkerTask(pthread_mutex_t* pInitLock, pthread_cond_t* pInitCond, int* pInitNums)
        :PthreadBase(pInitLock, pInitCond, pInitNums), 
        m_iNotifyRecvFd(-1),m_iNofitySendFd(-1), m_PthreadEventBase(NULL), m_pConnItemList(NULL),
        m_pConnItemListFree(NULL)
    {
    }

    WorkerTask::~WorkerTask()
    {
        event_base_free(m_PthreadEventBase);
        m_PthreadEventBase = NULL;
        if (m_iNotifyRecvFd > 0)
        {
            ::close(m_iNotifyRecvFd); m_iNotifyRecvFd = -1;
        }

        if (m_iNofitySendFd > 0)
        {
            ::close(m_iNofitySendFd);  m_iNofitySendFd = -1;
        }
        
        for (std::map<int, ConnBase*>::iterator it = m_mpAcceptConn.begin(); it != m_mpAcceptConn.end(); )
        {
            if (it->second)
            {
                delete it->second;
            }
            if (it->first > 0)
            {
                ::close(it->first);
            }
            m_mpAcceptConn.erase(it++);
        }
    }

    int WorkerTask::Init() 
    {
        struct event_config *ev_config = event_config_new();
        event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);
        m_PthreadEventBase = event_base_new_with_config(ev_config);
        event_config_free(ev_config);
        std::cout << "event base init, addr: " << m_PthreadEventBase << std::endl;
        if (m_PthreadEventBase == NULL)
        {
            m_bRun = false;
            return -1;
        }

        int fds[2];
        if (::pipe(fds))
        {
            perror("can't not notify pipe");
            return -1;
        }
        //
        m_iNotifyRecvFd =  fds[0];
        m_iNofitySendFd =  fds[1];

        //need to delete 
        PipeConn* pNotifyConn = new PipeConn(m_iNotifyRecvFd, this);
        AddRConn(pNotifyConn);
        std::cout << "create pipe, notify send cmd fd: " << m_iNofitySendFd << ", notify recv cmd fd: "
            << m_iNotifyRecvFd << std::endl;
        return 0;
    }

    void WorkerTask::AddRConn(ConnBase* pEvent)
    {
        pEvent->AddEvent(m_PthreadEventBase, EV_READ | EV_PERSIST, this);
    }

    int WorkerTask::main()
    {
        RegistePthreadToPool();
        //
        event_base_dispatch(m_PthreadEventBase);
        std::cout << "thread exit " << std::endl;
        m_bRun = false;
        return 0;
    }

    bool WorkerTask::NotifyRecvConn(char *pBuf, int iLen)
    {
        if (pBuf == NULL)
        {
            return false;
        }
        if (iLen < 1)
        {
            return false;
        }
        if (m_pConnItemList == NULL || m_pConnItemListFree == NULL)
        {
            return false;
        }

        switch(pBuf[0])
        {
            case 'c':
                {
                    //从队列中取出一个新连接请求.
                    QueueItem* pItem =  m_pConnItemList->PopBack();
                    int iFd = 0;
                    if (pItem != NULL)
                    {
                        iFd = pItem->iConnFd;
                        //发起一个连接, 需要从连接池中分配一个空闲连接
                        AcceptConn* pConn = new AcceptConn(iFd, this);
                        AddRConn(pConn);
                        std::cout << "pipe recv new conn cmd, new fd: " << iFd << std::endl;

                        m_mpAcceptConn.insert(std::pair<int, ConnBase*>(iFd, pConn)); //when close iFd, delete pConn
                        m_pConnItemListFree->PushFront(pItem);
                    }
                }
               break;

            default:
                break;
        }
        return true;
    }

    void WorkerTask::DeleteAcceptConn(int fd)
    {
        if (fd <=0)
        {
            return ;
        }
        //
        std::map<int, ConnBase*>::iterator it = m_mpAcceptConn.find(fd);
        if (it != m_mpAcceptConn.end())
        {
            delete it->second;
            m_mpAcceptConn.erase(it);
        }
    }

    bool WorkerTask::NotifySendConn(QueueSafe<QueueItem>* pItemList, QueueSafe<QueueItem>* pItemListFree)
    {
        if (pItemList == NULL)
        {
            return false;
        }
        char buf[1];
        buf [0] = 'c';
        if (::write(m_iNofitySendFd, buf, 1 ) != 1)
        {
            perror("write cmd c on notifysendcmd pipe fail");
            return false;
        }
        m_pConnItemList = pItemList;
        m_pConnItemListFree = pItemListFree;
        return true;
    }
}
