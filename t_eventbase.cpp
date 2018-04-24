#include "t_eventbase.hpp"
#include <unistd.h>
#include <iostream>
namespace T_TCP
{
    ConnBase::ConnBase(int iFd):m_iFd(iFd) 
    {
    }

    //
    ConnBase::~ConnBase()
    {
        event_del(&m_stREvent);
        event_del(&m_stWEvent);
        std::cout << "close this conn, fd: " << m_iFd << std::endl;
        ::close(m_iFd);
    }
    //
}
