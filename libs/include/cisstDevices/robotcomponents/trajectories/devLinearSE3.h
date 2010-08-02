

#ifndef _devLinearSE3_h
#define _devLinearSE3_h

#include <cisstVector/vctFrame4x4.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devLinearSE3 : public devTrajectory {

 private:
  
  SE3IO* input;
  SE3IO* output;

  vctFrame4x4<double> Rtold;
  double vmax;
  double wmax;

  vctFrame4x4<double> GetInput();

 protected:

  // Declare a pure virtual method 
  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double, robFunction*  );

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();
  
 public:
  
  devLinearSE3( const std::string& TaskName,
		double period,
		bool enabled,
		devTrajectory::Mode mode,
		devTrajectory::Variables variables,
		const vctFrame4x4<double>& Rtinit , 
		double vmax,
		double wmax );
  ~devLinearSE3(){}

};

#endif
