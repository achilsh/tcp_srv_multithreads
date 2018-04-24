/**
 * @file: t_tcpsrv.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-23
 */

#ifndef _t_tcpsrv_hpp_
#define _t_tcpsrv_hpp_

#include  <string>
#include  "t_socket.hpp"
#include  "t_worktask.hpp"
#include  "t_queue.hpp"
#include  "t_pools.hpp"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event.h>

namespace T_TCP
{
    class ListenConn;
    class TcpSrv
    {
     public:
      TcpSrv(const std::string& sIp, int iPort,int ithreadNums);
      virtual ~TcpSrv();
      bool Run();
      
      bool Accept();
     private:
      bool RegisteAcceptConn();
      bool Init();
     private:
      bool m_bInit;
      Sock* p_mListenSock;
      
      std::string  m_sLocalIp;
      int m_iLocalPort;
      struct event_base* m_pEventBase;
      //thread pools
      PthreadPools<WorkerTask>* m_pThreadPool;
      int m_iThreadPoolNums;
      QueueSafe<QueueItem> m_AcceptConnListWorking;
      QueueSafe<QueueItem> *m_pAcceptConnListFree;
      ListenConn* p_mListenConn; //in future, it can been allocated by conn_pools
    };
}
#endif


