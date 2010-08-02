

#ifndef _devLinearIK_h
#define _devLinearIK_h


#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robLinearSE3.h>

#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devLinearIK : public devTrajectory, public robManipulator {

 private:

  SE3IO* input;
  RnIO* output;

  vctFrame4x4<double> Rtold;
  vctDynamicVector<double> qold;
  double vmax;
  double wmax;

  vctFrame4x4<double> GetInput();

 protected:

  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double, robFunction*  ){return NULL;}

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();

 public:

  devLinearIK( const std::string& TaskName, 
	       double period, 
	       bool enabled,
	       devTrajectory::Mode mode,
	       devTrajectory::Variables variables,
	       double vmax, double wmax, 
	       const std::string& robotfile,
	       const vctFrame4x4<double>& Rtw0,
	       const vctDynamicVector<double>& qinit );

  ~devLinearIK(){}

};

#endif

