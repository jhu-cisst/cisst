
#ifndef _robSAHThumb_h
#define _robSAHThumb_h

#include <cisstRobot/SAH/robSAHFinger.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robSAHThumb : public robSAHFinger {
  
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
  enum Phalanx { BASE, METACARPUS, MCP, PROXIMAL, INTERMEDIATE, DISTAL };

  robSAHThumb( const vctFrame4x4<double>& Rtw0 );

  vctFrame4x4<double>
    ForwardKinematics( const vctFixedSizeVector<double,4>& q,
		       robSAHThumb::Phalanx phalanx ) const;

};

#endif
