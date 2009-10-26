#ifndef _robFileSE3_h
#define _robFileSE3_h

#include <cisstRobot/robSource.h>

#include <fstream>
#include <string>

class robFileSE3 : public robSource {
 protected:
  
  std::string filename;
  std::ifstream ifs;
  
 public:
  
  robFileSE3( const std::string& filename );
  ~robFileSE3();
  
  robError Read( robVariables& var );
  robError Write( const robVariables& var );
  
  robError Open();
  robError Close();
    
};

#endif
