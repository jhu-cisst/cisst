
#include <cisstCAN/osaBitCtrl.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <can.h>
#include <fcntl.h> // for O_RDWR

CMN_IMPLEMENT_SERVICES( osaBitCtrl );

osaBitCtrl::osaBitCtrl( const std::string& candevname, cisstCAN::Rate rate ) : 
  cisstCAN( rate ),
  candevname( candevname ),
  canfd( -1 ){}

osaBitCtrl::~osaBitCtrl(){
  
  // ensure the device is closed
  if( Close() == cisstCAN::EFAILURE ){
    CMN_LOG_RUN_ERROR << " Failed to close device " << candevname
		      << std::endl;
  }
  
}

cisstCAN::Errno osaBitCtrl::Open(){

#if (CISST_OS == CISST_QNX )

  // ensure the device is not already opened
  if( IsClosed() ){
    
    // open the device
    canfd = open( candevname.data(), O_RDWR );
    
    // check the file descriptor
    if( IsClosed() ){
      CMN_LOG_INIT_ERROR << " Failed to open the CAN device " << candevname
			 << std::endl;
      return cisstCAN::EFAILURE;
    }
    
    // SL: duno what this is supposed to do
    ioctl( canfd, CNFLUSH );
    return cisstCAN::ESUCCESS;
    
  }
  
  else{
    CMN_LOG_RUN_ERROR << "The CAN device has already been opened?"
		      << std::endl;
    return cisstCAN::EFAILURE;
  }
#endif

  return cisstCAN::EFAILURE;
}

cisstCAN::Errno osaBitCtrl::Close(){
  
#if (CISST_OS == CISST_QNX )

  // ensure the device is opened
  if( IsOpened() ){
    
    // close the device
    if( close( canfd ) == -1 ){
      CMN_LOG_RUN_ERROR << " Failed to close the device " << candevname 
			<< std::endl;
      return cisstCAN::EFAILURE;
    }
    // reset the file descriptor
    canfd = -1;
    
    return cisstCAN::ESUCCESS;
  }
#endif

  return cisstCAN::EFAILURE;

}

cisstCAN::Errno osaBitCtrl::Recv( cisstCAN::Frame& frame, cisstCAN::Flags ){

#if (CISST_OS == CISST_QNX )

  // ensure the device is opened
  if( IsOpened() ){
    
    // the can message
    canmsg_t canmsg;
    
    // read the message
    int nbytesread;

    nbytesread = read( canfd, &canmsg, sizeof(canmsg_t) );

    // check the nuber of bytes
    if( nbytesread != sizeof( canmsg_t ) ){
      CMN_LOG_RUN_ERROR << " Expected to read " << sizeof( canmsg_t ) << " bytes."
			<< " Got " << nbytesread
			<< std::endl;
      return cisstCAN::EFAILURE;	
    }
    
    // build and return a CAN frame
    frame = cisstCAN::Frame( canmsg.id, canmsg.data, canmsg.length );
    return cisstCAN::ESUCCESS;
    
  }

  else{
    CMN_LOG_RUN_ERROR << "Invalid file descriptor. Is the CAN deviced opened?"
		      << std::endl;
    return cisstCAN::EFAILURE;
  }

#endif

  return cisstCAN::EFAILURE;
  
}

cisstCAN::Errno osaBitCtrl::Send( const cisstCAN::Frame& frame, cisstCAN::Flags ){
  
#if (CISST_OS == CISST_QNX )

  // ensure the device is opened
  if( IsOpened() ){
    
    // the can message
    canmsg_t canmsg;
    
    // copy the values
    canmsg.flags = 0;
    canmsg.id = frame.GetID();
    canmsg.length = frame.GetLength();
    const cisstCAN::Frame::Data* data = frame.GetData();
    for( cisstCAN::Frame::DataLength i=0; i<frame.GetLength(); i++ )
      { canmsg.data[i] = data[i]; }
    
    // write the message
    int nbyteswrite;
    nbyteswrite = write( canfd, &canmsg, sizeof(canmsg_t) );
    
    // check the number of bytes
    if( nbyteswrite != sizeof( canmsg_t ) ){
      CMN_LOG_RUN_ERROR << " Expected to write " << sizeof( canmsg_t ) << " bytes."
			<< " Wrote " << nbyteswrite
			<< std::endl;
      return cisstCAN::EFAILURE;	
    }
    
    return cisstCAN::ESUCCESS;
    
  }
  
  else{
    CMN_LOG_RUN_ERROR << "Invalid file descriptor. Is the CAN deviced opened?"
		      << std::endl;
    return cisstCAN::EFAILURE;
  } 

#endif

  return cisstCAN::EFAILURE;
  
}

