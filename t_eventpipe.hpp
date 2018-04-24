/**
 * @file: t_eventpipe.hpp
 * @brief:  
 *      管道连接对象主要是用于响应业务线程上的读数据就绪。当就绪后管道连接对象
 *      就会调用业务线程来处理. 而管道的另一写端将在 向业务线程发送管道命令信息
 *      的其他线程中调用. 而此处的管道连接对象主要是用于检测管道读端的网络事件是
 *      否就绪.
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-23
 */

#ifndef _t_eventpipe_hpp_
#define _t_eventpipe_hpp_

#include "t_eventbase.hpp"

namespace T_TCP
{
    class PipeConn: public ConnBase
    {
     public:
      PipeConn(int ifd, void* pData);
      virtual ~PipeConn();
      bool AddEvent(struct event_base *pEvBase, int iEvent, void *arg);
      //TODO:
      bool DelEvent(struct event_base *pEvBase, int iEvent, void *arg);
     private:
      static void ReadCallBack(int iFd, short sEvent, void *pData); 
      static void WriteCallBack(int iFd, short sEvent, void *pData);
      void DoRead(char *pData, int iLen);
      void *m_pData;
    };
}
#endif

