#ifndef _robSource_h
#define _robSource_h

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robVariables.h>

class robSource{
    
public:
    
  virtual robError Open() = 0;
  virtual robError Close() = 0;
  
  virtual robError Read( robVariables& variables ) = 0;
  virtual robError Write( const robVariables& variables ) = 0;
};

#endif
