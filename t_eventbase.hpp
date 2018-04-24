/**
 * @file: t_eventbase.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: 
 * @date: 2018-04-23
 */

#ifndef _t_eventbase_hpp_
#define _t_eventbase_hpp_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event.h>

#include <sys/types.h>
#include <sys/socket.h>

namespace T_TCP
{
    class ConnBase
    {
     public:
      ConnBase(int iFd);
      virtual ~ConnBase();
      //
      virtual bool AddEvent(struct event_base *pEvBase, int iEvent, void *arg) = 0;
      virtual bool DelEvent(struct event_base *pEvBase, int iEvent, void *arg) = 0;
      //
     protected:
      int m_iFd;
      //
      struct event  m_stREvent;
      struct event  m_stWEvent;
    };
}

#endif
