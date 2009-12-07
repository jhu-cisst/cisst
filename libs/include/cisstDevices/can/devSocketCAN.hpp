

#ifndef __SOCKETCAN_HPP__
#define __SOCKETCAN_HPP__

#include "canbus.hpp"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

namespace openman {

  class SocketCAN : public CANbus{

  private:

    std::string device;

    int canfd;
    struct sockaddr to_addr;

  public:

    SocketCAN( const std::string& device, uint32_t baudrate );

    Retval open();
    Retval close();

    Retval send( void* data, size_t  nbytes, bool block=true);
    Retval recv( void* data, size_t& nbytes, bool block=true);

  };

}

#endif
