#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robFileSE3.h>
#include <typeinfo>
#include <iostream>

robFileSE3::robFileSE3( const std::string& filename )
{ this->filename=filename; }

robFileSE3::~robFileSE3(){ Close(); }

robError robFileSE3::Open(){

  ifs.open( filename.data() );
  if( !ifs ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Could not open " << filename << std::endl;
    return ERROR;
  }
  return SUCCESS;
}


robError robFileSE3::Close(){
  ifs.close();
  return SUCCESS;
}

robError robFileSE3::Write( const robVariables& var ){ return SUCCESS; }

robError robFileSE3::Read( robVariables& var ){

  std::string line;

  vctFrame4x4<double,VCT_ROW_MAJOR> Rt;
  for(size_t r=0; r<4; r++){         // read 4 lines
    getline(ifs, line);
    std::istringstream linestream(line);
    linestream >> Rt[r][0] >> Rt[r][1] >> Rt[r][2] >> Rt[r][3];
    if( linestream.fail() )
      { return ERROR; }
  }
  
  var.Set( robVariables::CARTESIAN_POS, Rt );
  return SUCCESS;
}
