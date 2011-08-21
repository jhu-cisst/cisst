
#ifndef _devInverseKinematics_h
#define _devInverseKinematics_h

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>


class CISST_EXPORT devInverseKinematics : public devTrajectory {

 private:

  SE3IO*  input;
  RnIO*  output;

  robManipulator* robot;

  double vmax;
  double wmax;

  vctFrame4x4<double>      oldinput;
  vctDynamicVector<double> oldoutput;

  vctFrame4x4<double> GetInput();
  bool IsInputNew();

 protected:

  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double t, robFunction* function );

  //! Evaluate a function
  void Evaluate( double t, robFunction* function );

 public:

  devInverseKinematics( const std::string& name, 
			double period, 
			devTrajectory::State state,
			osaCPUMask cpumask,
			devTrajectory::Mode mode,
			const vctDynamicVector<double>& qinit,
			double vmax, 
			double wmax,
			const std::string& robfile,
			const vctFrame4x4<double>& Rtw0 );


  devInverseKinematics( const std::string& name, 
			double period, 
			devTrajectory::State state,
			osaCPUMask cpumask,
			devTrajectory::Mode mode,
			const vctDynamicVector<double>& qinit,
			double vmax, 
			double wmax,
			const std::string& robfile,
			const vctFrm3& Rtw0 );

  void Reset( const vctDynamicVector<double>& q );

  ~devInverseKinematics();

};

#endif
