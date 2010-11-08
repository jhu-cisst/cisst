
#ifndef _devOMNItoBH_h
#define _devOMNItoBH_h

#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOMNItoBH : public devTrajectory{


 private:

  RnIO* input;
  RnIO* output;

  int action;
  vctDynamicVector<double> qold;

  vctDynamicVector<double> GetInput();

 protected:

  //! Test if a new input has been latched on the input interface
  bool IsInputNew();

  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double t, robFunction* function );

  //! Evaluate a function
  void Evaluate( double t, robFunction* function );

 public:

  devOMNItoBH( const std::string& name,
	       double period,
	       devTrajectory::State state,
	       osaCPUMask mask );

};


#endif
