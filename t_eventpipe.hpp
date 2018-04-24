/**
 * @file: t_eventpipe.hpp
 * @brief: 
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

