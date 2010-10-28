
#ifndef _devGravityCompensation_h
#define _devGravityCompensation_h

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/robotcomponents/controllers/devController.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devGravityCompensation : 

  public devController,
  public robManipulator {

 private:

  RnIO* output;
  RnIO* feedback;

 protected:

  void Evaluate();

 public:

  devGravityCompensation( const std::string& name, 
			  double period,
			  devController::State state,
			  osaCPUMask mask,
			  const std::string& robfile,
			  const vctFrame4x4<double>& Rtwb );


};

#endif
