/**
 * @file: t_eventconn.hpp
 * @brief: 
 *      client 已连接对象的实现，线程或者工作线程用该对象进行数据的收发，
 *      业务逻辑的处理等。
 *      目前是采用一个线程一个连接。后续可优化为: 业务用独立的线程来完成，
 *      处理完后再用接收数据的线程发送出去。
 * @author:  wusheng Hu
 * @version: v0x00001
 * @date: 2018-04-24
 */

#ifndef _t_eventconn_hpp_
#define _t_eventconn_hpp_

#include "t_eventbase.hpp"

namespace T_TCP
{
    class BusiCodec;
    class WorkerTask;
    class AcceptConn: public ConnBase
    {
     public:
      AcceptConn(int ifd, void *pData);
      virtual ~AcceptConn();
      virtual bool AddEvent(struct event_base *pEvBase, int iEvent, void *arg);
      virtual bool DelEvent(struct event_base *pEvBase, int iEvent, void *arg);
     private:
      static void ReadCallBack(int iFd, short sEvent, void *pData); 
      static void WriteCallBack(int iFd, short sEvent, void *pData);

      bool DoRead();
      bool DoWrite();
     private:
      WorkerTask* m_pData;
      BusiCodec* m_pCodec;
     private:
      char* m_pRecvBuf;  //接收缓冲区
      int m_iCurrRecvOffSet;
      //
      char* m_pSendBuf; //发送缓冲区
      int m_iCurrSendOffSet;
      static  const int CNT_STATIC_BUF_MAX_LEN = 1024;
    };
}
#endif
