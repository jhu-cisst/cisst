

#ifndef _robSAHFinger_h
#define _robSAHFinger_h

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robSAHFinger : public robManipulator{
  
 public:

  //! The links of the finger
  /**
     MCP: Metacarpophalangeal joint. Ok, technically those aren't a phalanx
          but they do represent a link on the SAH (the round knuckles)
     PROXIMAL: The first phalanx
     INTERMETIATE: The second phalanx
     DISTAL: The third phalanx
     NAIL: The finger tip
  */
  enum Phalanx 
  { 
    BASE,
    MCP, 
    PROXIMAL, 
    INTERMEDIATE, 
    DISTAL 
  };

  robSAHFinger( const vctFrame4x4<double>& Rtb0 );

  vctFrame4x4<double>
    ForwardKinematics( const vctFixedSizeVector<double,3>& q, 
		       robSAHFinger::Phalanx phalanx ) const;

};

#endif
