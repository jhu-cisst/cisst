
#ifndef _robSAH_h
#define _robSAH_h

#include <cisstRobot/SAH/robSAHFinger.h>
#include <cisstRobot/SAH/robSAHThumb.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robSAH : public robManipulator {

 public:

  enum Finger{ THUMB, FIRST, MIDDLE, RING };

  robSAH( const std::string& palmfilename,
	  const std::string& fingerfilename,
	  const vctFrame4x4<double>& Rt );

  vctFrame4x4<double> ForwardKinematics() const ;

  vctFrame4x4<double>
    ForwardKinematics( const vctFixedSizeVector<double,3>& q, 
		       robSAH::Finger finger,
		       robSAHFinger::Phalanx phalanx) const;

  vctFrame4x4<double>
    ForwardKinematics( const vctFixedSizeVector<double,4>& q, 
		       robSAHThumb::Phalanx phalanx) const;

};

#endif
