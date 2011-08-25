/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstOSG/mtsOSGBody.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstCommon/cmnLogger.h>

mtsOSGBody::mtsOSGBody( const std::string& name,
			const std::string& model,
			cisstOSGWorld* world,
			const vctFrame4x4<double>& Rt ) :
  mtsComponent( name ),
  cisstOSGBody( model, world, Rt ),
  input( NULL ){

  // Create the IO interface and add read/write commands
  input = AddInterfaceRequired( "Input", MTS_OPTIONAL );
  if( input )
    { input->AddFunction( "GetPositionOrientation", GetPosition ); }
  else{
    CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Input" 
			    << std::endl;
  }

}

void mtsOSGBody::UpdateTransform(){

  if( GetPosition.IsValid() ){

    // Get the position of the camera
    prmPositionCartesianGet Rt;
    GetPosition( Rt );

    // Set the transformation
    cisstOSGBody::SetTransform( Rt.Position() );
    
  }

  // Update the transformation
  cisstOSGBody::UpdateTransform();

}
