
#include <cisstDevices/controllers/devController.h>
#include <cisstDevices/devExport.h>

#ifndef _devControllerJoints_h
#define _devControllerJoints_h

class CISST_EXPORT devControllerJoints : public devController {
    
protected:
  
  //! MTS vector of input variables
  mtsVector<double> mtsqs;
  mtsVector<double> mtsqsd;
  mtsVector<double> mtsqsdd;

public:
  
  devControllerJoints( const std::string& taskname, 
		       double period, 
		       const std::string& robfile,
		       const vctFrame4x4<double>& Rtw0,
		       bool enabled = false );

  ~devControllerJoints(){}

};

#endif
