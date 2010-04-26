
#ifndef _devManipulator_h
#define _devManipulator_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devManipulator : public mtsTaskPeriodic {

 private:

  mtsVector<double> output;
  mtsVector<double> input;

 public:

  devManipulator( const std::string& taskname, double period, size_t N );
  
  void Configure(){}
  void Startup(){}
  void Run();
  void Cleanup(){}

  virtual vctDynamicVector<double> Read() = 0;
  virtual void Write( const vctDynamicVector<double>& values ) = 0;

  static const std::string InputInterface;
  static const std::string Input;

  static const std::string OutputInterface;
  static const std::string Output;

};

#endif
