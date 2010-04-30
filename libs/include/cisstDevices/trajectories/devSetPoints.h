

#include <cisstCommon.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstDevices/devExport.h>

#ifndef _devSetPoints_h
#define _devSetPoints_h

class CISST_EXPORT devSetPoints : public mtsTaskPeriodic {

 private:

  std::vector< vctDynamicVector<double> > rnsetpoints;
  std::vector< vctDynamicVector<double> > se3setpoints;
  size_t cnt;

  mtsVector<double> rnoutput;
  mtsVector<double> se3output;

  mtsBool state;
  bool    stateold;


 public:


  devSetPoints( const std::string& taskname,
		const std::vector< vctDynamicVector<double> >& setpoints );

  devSetPoints( const std::string& taskname,
		const std::vector< vctFrame4x4<double> >& setpoints );

  ~devSetPoints(void) {};

  void Configure(const std::string & CMN_UNUSED(filename) = "") {};
  void Startup(void){};
  void Run();
  void Cleanup(void) {};
  
  static const std::string OutputInterface;
  static const std::string GetRnSetPoint;
  static const std::string GetSE3SetPoint;

  static const std::string ControlInterface;
  static const std::string NextSetPoint;

};

  
#endif
