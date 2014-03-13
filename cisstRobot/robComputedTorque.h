
#ifndef _robComputedTorque_H
#define _robComputedTorque_H

#include <cisstRobot/robControllerJoints.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robComputedTorque : 

  public robControllerJoints,
  public robManipulator {

 protected:

  vctDynamicVector<double> qold;
  vctDynamicVector<double> eold;

  vctDynamicMatrix<double> Kp;
  vctDynamicMatrix<double> Kd;

 public:

  robComputedTorque( const std::string& robfile,
		     const vctFrame4x4<double>& Rtw0,
		     const vctDynamicMatrix<double>& Kp,
                     const vctDynamicMatrix<double>& Kd );

  vctDynamicVector<double> Control( double dt,
				    const vctDynamicVector<double>& qs,
				    const vctDynamicVector<double>& q, 
				    const vctDynamicVector<double>& qds,
				    const vctDynamicVector<double>& qd,
				    const vctDynamicVector<double>& qdds,
				    const vctDynamicVector<double>& qdd );

};

#endif

