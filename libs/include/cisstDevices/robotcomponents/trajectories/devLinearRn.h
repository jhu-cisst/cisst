#ifndef _devLinearRn_h
#define _devLinearRn_h

#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devLinearRn : public devTrajectory {

 private:

  RnIO* input;
  RnIO* output;

  vctDynamicVector<double> qold;
  vctDynamicVector<double> qdmax;

  vctDynamicVector<double> GetInput();

 protected:

  // Declare a pure virtual method 
  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double, robFunction*  );

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();

 public:

  devLinearRn( const std::string& name, 
	       double period, 
	       devTrajectory::State state,
	       osaCPUMask cpumask,
	       devTrajectory::Mode mode,
	       devTrajectory::Variables variables,
	       const vctDynamicVector<double>& qinit,
	       const vctDynamicVector<double>& qdmax );
  ~devLinearRn(){}

};

#endif
