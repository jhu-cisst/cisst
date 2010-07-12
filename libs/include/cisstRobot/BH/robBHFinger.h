

#ifndef _robBHFinger_h
#define _robBHFinger_h

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBHFinger : public robManipulator {

 public:

  enum Phalanx 
  { 
    BASE,
    METACARP,
    PROXIMAL, 
    INTERMEDIATE, 
  };

  robBHFinger( const vctFrame4x4<double>& Rt ) : robManipulator( Rt ){}
  
  virtual vctFrame4x4<double> 
    ForwardKinematics( const vctDynamicVector<double>& q, 
		       robBHFinger::Phalanx phalanx ) = 0;
  
};

#endif
