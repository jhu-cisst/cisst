#ifndef _robSE3File_h
#define _robSE3File_h

#include <cisstRobot/robDevice.h>

#include <fstream>
#include <string>

namespace cisstRobot{

  class robSE3File : public robDevice {
  protected:
    
    std::string filename;
    std::ifstream ifs;
    
  public:
    
    robSE3File( const std::string& filename );
    ~robSE3File();

    robError Read( robDOF& dof );
    robError Write( const robDOF& dof );

    robError Open();
    robError Close();
    
  };
}

#endif
