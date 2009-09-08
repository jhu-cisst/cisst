#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSE3File.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robSE3File::robSE3File( const std::string& filename ){ this->filename=filename; }

robSE3File::~robSE3File(){ Close(); }

robError robSE3File::Open(){

  ifs.open( filename.data() );
  if( !ifs ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Could not open " << filename << endl;
    return FAILURE;
  }
  return SUCCESS;
}


robError robSE3File::Close(){
  ifs.close();
  return SUCCESS;
}

robError robSE3File::Write( const robDOF& dof ){ return SUCCESS; }

robError robSE3File::Read( robDOF& dof ){

  std::string line;

  SE3 Rt;
  for(size_t r=0; r<4; r++){         // read 4 lines
    getline(ifs, line);
    std::istringstream linestream(line);
    linestream >> Rt[r][0] >> Rt[r][1] >> Rt[r][2] >> Rt[r][3];
    if( linestream.fail() )
      { return FAILURE; }
  }
  dof.SetPos( robDOF::RT, Rt );
  return SUCCESS;
}
