
#include <cisstMultiTask/mtsVector.h>
#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstDevices/controllers/devController.h>
#include <cisstDevices/devExport.h>

#ifndef _devControllerSE3_h
#define _devControllerSE3_h

class CISST_EXPORT devControllerSE3 : public devController {
    
protected:
  
  //! MTS vector of input variables
  mtsVector<double> mtstws;
  mtsVector<double> mtsvs;
  mtsVector<double> mtsvsd;

  mtsDoubleQuatRot3 mtsqws;
  mtsVector<double> mtsws;
  mtsVector<double> mtswsd;

public:
  
  devControllerSE3( const std::string& taskname, 
		    double period, 
		    const std::string& robfile,
		    const vctFrame4x4<double>& Rtw0,
		    bool enabled );

  ~devControllerSE3(){}

};

#endif
