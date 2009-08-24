#ifndef _robSE3Track_h
#define _robSE3Track_h

#include <cisstRobot/robFunction.h>

namespace cisstRobot{
  
  class robSE3Track : public robFunction{

  private:

    robFunction *txblender;
    robFunction *tyblender;
    robFunction *tzblender;
    robFunction *so3blender;
    
    SO3 Rw0;                   // the old orientation
    SO3 Rw1;                   // the current orientation
    SO3 Rw2;                   // the future orientation
    R3 tw0;                    // the old position
    R3 tw1;                    // the current position
    R3 tw2;                    // the future position
    R3 v1, v1d, v2, v2d;       // linear velocity/acceleration
    R3 w1, w1d, w2, w2d;       // angular velocity/acceleration

    real t0, t1, t2;// old, current and future time;
    
    real vmax;
    real vdmax;
    real wmax;
    real wdmax;

    //uint32_t numwp;
    unsigned long int numwp;

    robError Evaluate(const robDOF& input);

  public:

    robSE3Track( real vmax=-1, real wmax=-1, real vdmax=-1, real wdmax=-1 );

    robDomainAttribute IsDefinedFor( const robDOF& input ) const;

    robError Evaluate( const robDOF& input, robDOF& output );

  };

}

#endif

