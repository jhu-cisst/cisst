#ifndef _robDevice_h
#define _robDevice_h

#include <cisstRobot/robDOF.h>

namespace cisstRobot{

  class robDevice {
  protected:
    
    // Mutex;
    
  public:
    
    virtual robError Generate( robDOF& dof ) = 0;
    
  };
}

#endif
