/*

  Author(s): Simon Leonard
  Created on: Dec 17 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robLink.h>

robLink::robLink(){}
robLink::~robLink(){}

robLink& robLink::operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ){
  if( *this != Rt )
    robBody::operator=( Rt );
  return *this;
}

robError robLink::Read( std::istream& is ){ 
  if( robDH::Read( is ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to read the DH parameters."
		      << std::endl;
    return ERROR;
  }
  
  if( robBody::Read( is ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to read the body parameters."
		      << std::endl;
    return ERROR;
  }
  
  return SUCCESS;
}

robError robLink::Write( std::ostream& os ) const { 

  if( robDH::Write( os ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to write the DH parameters."
		      << std::endl;
    return ERROR;
  }

  if( robBody::Write( os ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to write the body parameters."
		      << std::endl;
    return ERROR;
  }
  
  return SUCCESS;

}

