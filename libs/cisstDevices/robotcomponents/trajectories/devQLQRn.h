#ifndef _devQLQRn_h
#define _devQLQRn_h

#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devQLQRn : public devTrajectory {

 private:

  RnIO* input;
  RnIO* output;

  vctDynamicVector<double> qold;
  vctDynamicVector<double> qdmax;
  vctDynamicVector<double> qddmax;

  double EvaluateTransitionTime( const vctDynamicVector<double>& q1d,
				 const vctDynamicVector<double>& q2d );

  vctDynamicVector<double> GetInput();

 protected:

  // Declare a pure virtual method 
  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double , robFunction*  ){return NULL;}

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();

 public:

  devQLQRn( const std::string& name, 
	    double period, 
	    devTrajectory::State state,
	    osaCPUMask mask,
	    devTrajectory::Mode mode,
	    devTrajectory::Variables variables,
	    const vctDynamicVector<double>& qinit,
	    const vctDynamicVector<double>& qdmax,
	    const vctDynamicVector<double>& qddmax );

  ~devQLQRn(){}


};

#endif
