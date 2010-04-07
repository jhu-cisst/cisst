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

robLink::robLink( const robDH& dh, 
		  const robMass& mass ) :
  robDH( dh ), robMass( mass ) {}

robLink::Errno robLink::ReadLink( std::istream& is ){ 

  if( ReadDH( is ) != robDH::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to read the DH parameters."
		      << std::endl;
    return robLink::EFAILURE;
  }
  
  if( ReadMass( is ) != robMass::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to read the body parameters."
		      << std::endl;
    return robLink::EFAILURE;
  }
  
  return robLink::ESUCCESS;
}

robLink::Errno robLink::WriteLink( std::ostream& os ) const { 

  if( WriteDH( os ) != robDH::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to write the DH parameters."
		      << std::endl;
    return robLink::EFAILURE;
  }

  if( WriteMass( os ) != robMass::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to write the body parameters."
		      << std::endl;
    return robLink::EFAILURE;
  }
  
  return robLink::ESUCCESS;

}

