
#include <cisstDevices/can/devBitCtrl.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <can.h>
#include <fcntl.h> // for O_RDWR

devBitCtrl::devBitCtrl( const std::string& candevname, devCAN::Rate rate ) : 
   devCAN( rate ),
   candevname( candevname ),
   canfd( -1 ){}

devBitCtrl::~devBitCtrl(){
   if( canfd != -1 ){
      if( Close() == devCAN::EFAILURE ){
	 CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	   << " Failed to close device " << candevname
	   << std::endl;
      }
   }
}
   
devCAN::Errno devBitCtrl::Open(){
   
   // open the device
   canfd = open( candevname.data(), O_RDWR );
   
   // check
   if( canfd == -1){
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
	                 << " Failed to open the CAN device " << candevname
	<< std::endl;
      return devCAN::EFAILURE;
   }

   CMN_LOG_INIT_VERBOSE << "CAN device " << candevname << " opened."
     << std::endl;

   // SL: duno what this is supposed to do
   ioctl( canfd, CNFLUSH );
   
   return devCAN::ESUCCESS;
   
}

devCAN::Errno devBitCtrl::Close(){
   // if canfd is valid
   if( canfd != -1 ){
      // close the device
      if( close( canfd ) == -1 ){
	 CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	   << " Failed to close the device " << candevname 
	   << std::endl;
	 return devCAN::EFAILURE;
      }
   }
   return devCAN::ESUCCESS;
}

devCAN::Errno devBitCtrl::Recv( devCAN::Frame& frame, devCAN::Flags ){

   canmsg_t canmsg;
   
   int nbytesread = read( canfd, &canmsg, sizeof(canmsg_t) );
   
   if( nbytesread != sizeof( canmsg_t ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	<< " Expected to read " << sizeof( canmsg_t ) << " bytes."
	<< " Got " << nbytesread
	<< std::endl;
      return devCAN::EFAILURE;	
   }
   
   frame = devCAN::Frame( canmsg.id, canmsg.data, canmsg.length );
   return devCAN::ESUCCESS;
   
}
   
devCAN::Errno devBitCtrl::Send( const devCAN::Frame& frame, devCAN::Flags ){
	
   canmsg_t canmsg;
   
   canmsg.flags = 0;
   canmsg.id = frame.GetID();
   canmsg.length = frame.GetLength();
   const uint8_t* data = (const uint8_t*)frame.GetData();
   for( size_t i=0; i<frame.GetLength(); i++ )
     { canmsg.data[i] = data[i]; }
   
   int nbyteswrite = write( canfd, &canmsg, sizeof(canmsg_t) );
   
   if( nbyteswrite != sizeof( canmsg_t ) ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	<< " Expected to write " << sizeof( canmsg_t ) << " bytes."
	<< " Wrote " << nbyteswrite
	<< std::endl;
      return devCAN::EFAILURE;	
   }
   
   return devCAN::ESUCCESS;
      
}
   
