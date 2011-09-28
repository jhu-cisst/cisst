


#ifndef _devOMNItoWAM_h
#define _devOMNItoWAM_h

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>


class CISST_EXPORT devOMNItoWAM : public devTrajectory {

 private:

  RnIO*  input;
  RnIO* output;

  robManipulator* omni;
  robManipulator* wam;

  vctDynamicVector<double> qomnioldsamp;     // old omni joints angles
  vctDynamicVector<double> qomnioldinterp;   // old omni joints angles
  vctDynamicVector<double> qwamoldsol;       // old wam joints angles

  double told;

  vctDynamicVector<double> GetInput();
  bool IsInputNew();

  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double t, robFunction* function );
  
 protected:

  //! Evaluate a function
  void Evaluate( double t, robFunction* function );

 public:

  devOMNItoWAM( const std::string& name, 
		double period, 
		devTrajectory::State state,
		osaCPUMask cpumask,
		const std::string& fileomni,
		const vctDynamicVector<double>& qomni,
		const std::string& filewam,
		const vctFrame4x4<double>& Rtw0wam,
		const vctDynamicVector<double>& qwam );

  void Startup();
  void Reset( double dt, const vctDynamicVector<double>& qomni );

  

};

#endif
