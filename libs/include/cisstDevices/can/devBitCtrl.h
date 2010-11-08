

#ifndef _devBitCtrl_h
#define _devBitCtrl_h

#include <cisstDevices/can/devCAN.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devBitCtrl : public devCAN {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

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

CMN_DECLARE_SERVICES_INSTANTIATION(devBitCtrl)

#endif
