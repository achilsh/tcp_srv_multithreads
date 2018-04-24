/**
 * @file: t_event.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-23
 */
#ifndef _t_event_hpp_
#define _t_event_hpp_

#include "t_eventbase.hpp"

namespace T_TCP
{
    class TcpSrv;
    class ListenConn: public ConnBase
    {
     public:
      ListenConn(int ifd, void* pData);
      virtual ~ListenConn();
      virtual bool AddEvent(struct event_base *pEvBase, int iEvent, void *arg);
      //TODO:
      virtual bool DelEvent(struct event_base *pEvBase, int iEvent, void *arg);
     private:
      static void ReadCallBack(int iFd, short sEvent, void *pData); 
      static void WriteCallBack(int iFd, short sEvent, void *pData); 
      bool DoRead();
      TcpSrv* m_pData;
    };
}

#endif


