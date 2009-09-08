#ifndef _robDevice_h
#define _robDevice_h

#include <cisstRobot/robDOF.h>

namespace cisstRobot{

  class robDevice {
  protected:
    
    // Mutex;
    
  public:
    
    virtual robError Open() = 0;
    virtual robError Close() = 0;

    virtual robError Read( robDOF& dof ) = 0;
    virtual robError Write( const robDOF& dof ) = 0;
    
  };
}

#endif
