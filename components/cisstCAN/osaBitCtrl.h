

#ifndef _osaBitCtrl_h
#define _osaBitCtrl_h

#include <cisstCAN/cisstCAN.h>
#include <cisstCAN/cisstCANExport.h>

//! A BitCtrl CAN device
/**
   This implements a CAN device based on the BitCtrl driver. It's not really a 
   driver since it only provides an API and uses ISO C read/write.
*/

class CISST_EXPORT osaBitCtrl : public cisstCAN {
  
  CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 private:
  
  std::string candevname;
  int canfd;

  bool IsOpened() const { return canfd != -1 ? true : false; }
  bool IsClosed() const { return canfd == -1 ? true : false; }
   
 public:
  
  
  osaBitCtrl( const std::string& candevname, cisstCAN::Rate rate );
  ~osaBitCtrl();
  
  cisstCAN::Errno Open();
  cisstCAN::Errno Close();
  
  cisstCAN::Errno Send( const cisstCANFrame& frame, 
			cisstCAN::Flags flags = cisstCAN::MSG_NOFLAG );
  
  cisstCAN::Errno Recv( cisstCANFrame& frame, 
			cisstCAN::Flags flags = cisstCAN::MSG_NOFLAG );
  
  cisstCAN::Errno AddFilter( const cisstCAN::Filter& ) 
    { return cisstCAN::ESUCCESS; }
  
  
};

CMN_DECLARE_SERVICES_INSTANTIATION(osaBitCtrl)

#endif
