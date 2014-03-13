

#ifndef _robBHF1_h
#define _robBHF1_h

#include <cisstRobot/BH/robBHFinger.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBHF1 : public robBHFinger {

 public:

  robBHF1();
  vctFrame4x4<double> ForwardKinematics( const vctDynamicVector<double>& q, 
					 robBHFinger::Phalanx phalanx );

};

#endif
