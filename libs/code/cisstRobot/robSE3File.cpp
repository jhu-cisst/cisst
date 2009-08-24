#include <cisstRobot/robSE3File.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robSE3File::robSE3File( const std::string& filename ){
  ifs.open( filename.data() );
  if( !ifs ){
    cout << "robSE3File::robSE3File: could not open " << filename << endl;
  }
}

robError robSE3File::Generate( robDOF& dof ){

  std::string line;

  SE3 Rt;
  for(size_t r=0; r<4; r++){
    getline(ifs, line);
    std::istringstream linestream(line);
    linestream >> Rt[r][0] >> Rt[r][1] >> Rt[r][2] >> Rt[r][3];
    if( linestream.fail() )
      { return FAILURE; }
  }
  dof.SetPos( robDOF::RT, Rt );
  return SUCCESS;
}
