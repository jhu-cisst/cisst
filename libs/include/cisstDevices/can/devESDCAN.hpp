

#ifndef __ESDCAN_HPP__
#define __ESDCAN_HPP__

#ifdef ESDCAN

#include "canbus.hpp"
#include <ntcan.h>

namespace openman{

  class OPENMAN_DLL ESDCAN : public CANbus{

  private:

    HANDLE handle;    // nt-handle

    static const long TXQUEUESIZE = 32;  // nr of entries in message queue*
    static const long RXQUEUESIZE = 32;  // nr of entries in message queue
    static const long TXTIMEOUT   = 50;  // tx-timeout in miliseconds 
    static const long RXTIMEOUT   = 50;  // rx-timeout in miliseconds 

  public:

    ESDCAN( uint32_t baudrate );

    Retval open();
    Retval close();

    Retval recv( void* data, size_t& nbytes, bool block=true);
    Retval send( void* data, size_t  nbytes, bool block=true);

  };

}

#endif

#endif
