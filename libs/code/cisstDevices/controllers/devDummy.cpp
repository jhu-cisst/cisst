#include <cisstDevices/controllers/devDummy.h>

devDummy::devDummy( const std::string& taskname, 
		    double period,
		    const std::string& robfile,
		    const vctFrame4x4<double>& Rtw0,
		    bool enabled ) :
  devControllerJoints( taskname, period, robfile, Rtw0, enabled ){}

vctDynamicVector<double> devDummy::Control( const mtsVector<double>& ){
  //CMN_LOG_RUN_ERROR << "devDummy: q = " << q << std::endl;
  //CMN_LOG_RUN_ERROR << "devDummy: qs = " << mtsqs << std::endl;
  //CMN_LOG_RUN_ERROR << "devDummy: qsd = " << mtsqsd << std::endl;
  //CMN_LOG_RUN_ERROR << "devDummy: qsdd = " << mtsqsdd << std::endl;
  return mtsqs;
}
