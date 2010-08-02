#ifndef _devLinearR3_h
#define _devLinearR3_h

#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devLinearR3 : public devTrajectory {

 private:

  R3IO* input;
  R3IO* output;

  vctFixedSizeVector<double,3> pold;
  double vmax;

  vctFixedSizeVector<double,3> GetInput();

 protected:

  // Declare a pure virtual method 
  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double, robFunction*  );

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();

 public:

  devLinearR3( const std::string& TaskName, 
	       double period, 
	       bool enabled,
	       devTrajectory::Mode mode,
	       devTrajectory::Variables variables,
	       const vctFixedSizeVector<double,3>& pinit,
	       double vmax );
  ~devLinearR3(){}

};

#endif
