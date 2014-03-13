

#ifndef _robBH_h
#define _robBH_h

#include <cisstRobot/BH/robBHFinger.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBH : public robManipulator {

 public:

  enum Finger
  { 
    F1,
    F2,
    F3
  };

  robBH( const vctFrame4x4<double>& Rt );

  vctFrame4x4<double>
    ForwardKinematics( const vctDynamicVector<double>& q, 
		       robBH::Finger finger,
		       robBHFinger::Phalanx phalanx) const;

};

#endif
