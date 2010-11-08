

#ifndef _robBHF3_h
#define _robBHF3_h

#include <cisstRobot/BH/robBHFinger.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBHF3 : public robBHFinger {

 public:

  robBHF3();
  vctFrame4x4<double> ForwardKinematics( const vctDynamicVector<double>& q, 
					 robBHFinger::Phalanx phalanx );

};

#endif
