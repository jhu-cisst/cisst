
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

   // ensure the device is closed
   if( Close() == devCAN::EFAILURE ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                << " Failed to close device " << candevname
	                << std::endl;
   }

}
   
devCAN::Errno devBitCtrl::Open(){

   // ensure the device is not already opened
   if( canfd == -1 ){
   
      // open the device
      canfd = open( candevname.data(), O_RDWR );
   
      // check the file descriptor
      if( canfd == -1){
	 CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
	                    << " Failed to open the CAN device " << candevname
	                    << std::endl;
	 return devCAN::EFAILURE;
      }

      // SL: duno what this is supposed to do
      ioctl( canfd, CNFLUSH );
   
      return devCAN::ESUCCESS;
      
   }
   
   else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                << "The CAN device has already been opened?"
	                << std::endl;
      return devCAN::EFAILURE;
   }
   
}

devCAN::Errno devBitCtrl::Close(){

   // ensure the device is opened
   if( canfd != -1 ){
      
      // close the device
      if( close( canfd ) == -1 ){
	 CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                   << " Failed to close the device " << candevname 
	                   << std::endl;
	 return devCAN::EFAILURE;
      }

      // reset the file descriptor
      canfd = -1;

   }
   
   return devCAN::ESUCCESS;

}

devCAN::Errno devBitCtrl::Recv( devCAN::Frame& frame, devCAN::Flags ){


   // ensure the device is opened
   if( canfd != -1 ){
	
      // the can message
      canmsg_t canmsg;
   
      // read the message
      int nbytesread = read( canfd, &canmsg, sizeof(canmsg_t) );

      // check the nuber of bytes
      if( nbytesread != sizeof( canmsg_t ) ){
	 CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                   << " Expected to read " << sizeof( canmsg_t ) << " bytes."
	                   << " Got " << nbytesread
	                   << std::endl;
	 return devCAN::EFAILURE;	
      }

      // build and return a CAN frame
      frame = devCAN::Frame( canmsg.id, canmsg.data, canmsg.length );
      return devCAN::ESUCCESS;

   }
   
   else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                << "Invalid file descriptor. Is the CAN deviced opened?"
	                << std::endl;
      return devCAN::EFAILURE;
   }
   
}
   
devCAN::Errno devBitCtrl::Send( const devCAN::Frame& frame, devCAN::Flags ){
	

   // ensure the device is opened
   if( canfd != -1 ){
	
      // the can message
      canmsg_t canmsg;
   
      // copy the values
      canmsg.flags = 0;
      canmsg.id = frame.GetID();
      canmsg.length = frame.GetLength();
      const devCAN::Frame::Data* data = frame.GetData();
      for( devCAN::Frame::DataLength i=0; i<frame.GetLength(); i++ )
	{ canmsg.data[i] = data[i]; }
	
      // write the message
      int nbyteswrite = write( canfd, &canmsg, sizeof(canmsg_t) );
	
      // check the number of bytes
      if( nbyteswrite != sizeof( canmsg_t ) ){
	 CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                   << " Expected to write " << sizeof( canmsg_t ) << " bytes."
	                   << " Wrote " << nbyteswrite
	                   << std::endl;
	 return devCAN::EFAILURE;	
      }
	
      return devCAN::ESUCCESS;

   }
   
   else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
	                << "Invalid file descriptor. Is the CAN deviced opened?"
	                << std::endl;
      return devCAN::EFAILURE;
   } 

}
   
