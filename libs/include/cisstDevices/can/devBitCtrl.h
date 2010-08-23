

#ifndef _devBitCtrl_h
#define _devBitCtrl_h

#include <cisstDevices/can/devCAN.h>
//#include <cisstDevices/devExport.h>

class devBitCtrl : public devCAN {

 private:
   
   std::string candevname;
   
   int canfd;
   
 public:
   
   
   devBitCtrl( const std::string& candevname, devCAN::Rate rate );
   ~devBitCtrl();

   devCAN::Errno Open();
   devCAN::Errno Close();
   
   devCAN::Errno Send( const devCAN::Frame& frame, 
		       devCAN::Flags flags = devCAN::MSG_NOFLAG );
   
   devCAN::Errno Recv( devCAN::Frame& frame, 
		       devCAN::Flags flags = devCAN::MSG_NOFLAG );
   
   devCAN::Errno AddFilter( const devCAN::Filter& ) { return devCAN::ESUCCESS; }
	 

};

#endif
