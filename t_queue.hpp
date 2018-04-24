/**
 * @file: t_queue.hpp
 * @brief: 队列的总元素个数(空闲队列和正在工作) 是一直增大.
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-23
 */
#ifndef _t_queue_hpp_
#define _t_queue_hpp_

#include <string.h>
#include <queue>
#include <pthread.h>

namespace T_TCP
{
    template<typename T>
    class QueueSafe
    {
     public:
      QueueSafe(int iInitCapacity = 0);
      virtual ~QueueSafe();
      
      bool PushFront(T* pItem);
      T* PopBack();

     private:
      std::queue<T*> m_Queue;
      pthread_mutex_t m_mutxQueue;
      int m_iCurSize;
    };

    ////
    template<typename T>
    QueueSafe<T>::QueueSafe(int iInitCapacity): m_iCurSize(iInitCapacity)
    {
        pthread_mutex_init(&m_mutxQueue, NULL);

        pthread_mutex_lock(&m_mutxQueue);
        for (int i = 0; i < m_iCurSize; ++i)
        {
            T* pNewItem = new T();
            m_Queue.push(pNewItem);
        }
        pthread_mutex_unlock(&m_mutxQueue);
    }

    template<typename T>
    QueueSafe<T>::~QueueSafe()
    {
        pthread_mutex_lock(&m_mutxQueue);
        while (m_iCurSize > 0)
        {
            T* pItem = m_Queue.front();
            delete pItem;
            m_iCurSize--;
            m_Queue.pop();
        }
        pthread_mutex_unlock(&m_mutxQueue);
    }
    
    template<typename T>
    bool QueueSafe<T>::PushFront(T* pItem)
    {
        pthread_mutex_lock(&m_mutxQueue);
        m_Queue.push(pItem);
        m_iCurSize++;
        pthread_mutex_unlock(&m_mutxQueue);
        return true;
    }

    template<typename T>
    T* QueueSafe<T>::PopBack()
    {
        pthread_mutex_lock(&m_mutxQueue);
        if (m_iCurSize == 0)
        {
            T* pItem = new T();
            pthread_mutex_unlock(&m_mutxQueue);
            return pItem;
        }

        T* pItem = m_Queue.front();
        m_Queue.pop();
        m_iCurSize--;
        pthread_mutex_unlock(&m_mutxQueue);
        return pItem;
    }


    /** define item of queue ****/
    struct QueueItem
    {
        QueueItem():iConnFd(-1) { }
        int iConnFd;
    };
}

#endif

