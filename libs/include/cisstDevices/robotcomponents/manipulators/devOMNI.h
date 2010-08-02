

#ifndef _devOMNI_h
#define _devOMNI_h

#include <HDU/hduVector.h>
#include <HD/hd.h>

#include <cisstDevices/robotcomponents/manipulators/devManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOMNI : public devManipulator {

 private:

  mtsVector<double> buttons;
  
  HHD hHD;
  HDSchedulerHandle hUpdateHandle;

 public:

  HDint asyncbuttons;
  HDint syncbuttons;

  hduVector3Dd asyncpositions;
  hduVector3Dd asyncgimbals;
  hduVector3Dd syncpositions;
  hduVector3Dd syncgimbals;

  hduVector3Dd asynctorques;
  hduVector3Dd synctorques;
  
  devOMNI( const std::string& devname, double period );
  ~devOMNI();

  void Configure();

  static const std::string ButtonsInterface;
  static const std::string Buttons;

  vctDynamicVector<double> Read();
  void Write( const vctDynamicVector<double>& ft );

};

#endif


