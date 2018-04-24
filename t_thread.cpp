#include "t_thread.hpp"

namespace T_TCP
{
    PthreadBase::PthreadBase() :m_iPid(-1), m_bRun(false),
                                m_pInitLock(NULL), m_pInitCond(NULL),
                                m_pReadyThreadNums(NULL)
    {
    }

    PthreadBase::PthreadBase(pthread_mutex_t* pInitLock, pthread_cond_t* pInitCond, int* pInitNums)
        :m_iPid(-1), m_bRun(false),m_pInitLock(pInitLock), m_pInitCond(pInitCond),
        m_pReadyThreadNums(pInitNums)
    {
    }

    PthreadBase::~PthreadBase()
    {
    }
    void PthreadBase::Start()
    {
        int iRet = pthread_create(&m_iPid, NULL, Entry, this);
        if (iRet != 0)
        {
            return ;
        }
    }

    void PthreadBase::Stop()
    {
        m_bRun = false;
    }

    void* PthreadBase::Entry(void* pData)
    {
        PthreadBase* pThis =  (PthreadBase*)pData;
        if (pThis == NULL)
        {
            return NULL;
        }

        if (pThis->Init() < 0)
        {
            return NULL;
        }

        pThis->SetRun();
        while(pThis->IsRun())
        {
            pThis->main();
        }
    }

    void PthreadBase::JoinWork()
    {
       pthread_join(m_iPid, NULL); 
    }

    void PthreadBase::RegistePthreadToPool()
    {
        if (m_pInitLock == NULL || m_pInitCond == NULL || m_pReadyThreadNums == NULL)
        {
            return ;
        }
        pthread_mutex_lock(m_pInitLock);
        ++(*m_pReadyThreadNums);
        pthread_cond_signal(m_pInitCond);
        pthread_mutex_unlock(m_pInitLock);
    }
}
