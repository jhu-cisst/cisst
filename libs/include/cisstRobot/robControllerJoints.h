

#ifndef _robControllerJoints_h
#define _robControllerJoints_h

#include <cisstVector/vctDynamicVector.h>

#include <cisstRobot/robExport.h>

class CISST_EXPORT robControllerJoints{

 public:

  virtual 
    vctDynamicVector<double> Control( double dt,
				      const vctDynamicVector<double>& qs,
				      const vctDynamicVector<double>& q, 
				      const vctDynamicVector<double>& qds,
				      const vctDynamicVector<double>& qd,
				      const vctDynamicVector<double>& qdds,
				      const vctDynamicVector<double>& qdd ) = 0;
};

#endif
