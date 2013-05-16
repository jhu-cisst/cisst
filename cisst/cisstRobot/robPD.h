

#ifndef _robPD_H
#define _robPD_H

#include <cisstRobot/robControllerJoints.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robPD : public robControllerJoints {

  double Kp;
  double Kd;

  double eold;

 public:

  robPD( double Kp, double Kd );

  vctDynamicVector<double> Control( double dt,
				    const vctDynamicVector<double>& qs,
				    const vctDynamicVector<double>& q, 
				    const vctDynamicVector<double>& qds,
				    const vctDynamicVector<double>& qd,
				    const vctDynamicVector<double>& qdds,
				    const vctDynamicVector<double>& qdd );

};


#endif
