/**
 * @file: t_tcpsrv.hpp
 * @brief:  tcp 服务的主进程(主线程), 期间listenconn的连接对象向其
 * 注册，由这些连接对象来监控网络事件，具体网络事件的逻辑处理交给主线程
 * 。 主进程也分配一个连接线程池，当有新tcp连接到达时，从线程池中分配一
 * 个线程用于数据的收发(一般通过管道方式转移新的连接,  acceptconn 连接对象向工作
 * 线程注册网络事件,由该对象来监控新线程上的网络事件，事件就绪后交给工作线程处理)。
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


