

#ifndef _robBHF2_h
#define _robBHF2_h

#include <cisstRobot/BH/robBHFinger.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBHF2 : public robBHFinger {

 public:

  robBHF2();
  vctFrame4x4<double> ForwardKinematics( const vctDynamicVector<double>& q, 
					 robBHFinger::Phalanx phalanx );

};

#endif
