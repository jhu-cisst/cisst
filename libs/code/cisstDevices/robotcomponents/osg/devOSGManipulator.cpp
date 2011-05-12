/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <sstream>
#include <osgDB/ReadFile> 
#include <osg/ref_ptr>
#include <cisstDevices/robotcomponents/osg/devOSGManipulator.h>
#include <cisstVector/vctQuaternionRotation3.h>

devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devManipulator::Mode inputmode ) :
  devManipulator( devname, period, state, mask, inputmode ),
  robManipulator(),
  input( NULL ),
  output( NULL ){}


devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devManipulator::Mode inputmode,
				      const std::string& robotfile,
				      const vctFrame4x4<double>& Rtw0 ) :
  devManipulator( devname, period, state, mask, inputmode ),
  robManipulator( robotfile, Rtw0 ),
  input( NULL ),
  output( NULL ){}

devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devOSGWorld* world,
				      const std::string& robotfile,
				      const vctFrame4x4<double>& Rtw0,
				      const vctDynamicVector<double>& qinit,
				      const std::vector<std::string>& models,
				      const std::string& basemodel ) :

  devManipulator( devname, period, state, mask, devManipulator::POSITION ),
  robManipulator( robotfile, Rtw0 ),
  q( qinit ),
  input( NULL ),
  output( NULL ){


  // Create a Rn position input
  input = ProvideInputRn( devManipulator::Input,
			  devRobotComponent::POSITION,
			  qinit.size() );

  // Create a Rn position output (which is the same as the input)
  output = ProvideOutputRn( devManipulator::Output,
			    devRobotComponent::POSITION,
			    qinit.size() );


  // If the base is not empty, add a body called "link0"
  if( !basemodel.empty() ){

    std::ostringstream linkname;           // name of the link component
    linkname << devname << "link" << 0;    // name of link 0

    // Add the base to the group
    vctFrame4x4<double> Rtw0 = ForwardKinematics( qinit, 0 );

    // add the body as a child to the group
    try{ 
      devOSGBody* body = new devOSGBody( linkname.str(), Rtw0, basemodel );
      addChild( body );
    }
    catch( std::bad_exception& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate body " << linkname.str()
			<< std::endl;
    }
    
  }

  // Add the remaining links called "linki"
  for( size_t i=1; i<=links.size(); i++ ){

    std::ostringstream linkname;           // name of the link component
    linkname << devname << "link" << i;    // name of link i

    // Add the body to the OSG group
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i );
    try{ 
      devOSGBody* body = new devOSGBody( linkname.str(), Rtwi, models[i-1] );
      addChild( body );
    }
    catch( std::bad_exception& ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to allocate body " << linkname.str()
			<< std::endl;
    }

  }
  
  // Add this manipulator to the world
  world->addChild( this );

}

devOSGManipulator::~devOSGManipulator(){
  if( input != NULL )  delete input;
  if( output != NULL ) delete output;
}

void devOSGManipulator::Read(){
  CMN_ASSERT( output );
  output->SetPosition( this->q );
}

vctDynamicVector<double> devOSGManipulator::GetJointsPositions() const
{ return this->q; }

vctDynamicVector<double> devOSGManipulator::GetJointsVelocities() const
{ return this->qd; }

void devOSGManipulator::Write(){

  switch( GetInputMode() ){

  case devManipulator::POSITION: 
    {
      double t;
      CMN_ASSERT( input );
      input->GetPosition( this->q, t ); 
      if( SetPositions( this->q ) != devOSGManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set the joint positions q: "
			  << this->q << std::endl;
      }
    }
    break;

  case devManipulator::VELOCITY: 
    break;

  case devManipulator::FORCETORQUE:
    break;

  default: 
    break;

  }

}


devOSGManipulator::Errno 
devOSGManipulator::SetPositions
( const vctDynamicVector<double>& qs ){

  // Ensure one joint value per link
  if( qs.size() == links.size() ){

    // this is to skip the base if no model for the base is used in OSG
    int startlink = 0;       // assume that the base has a model
    if( links.size() == getNumChildren() )
      { startlink = 1; }     // no, base means the first child is link 1
    
    // For each children
    for( unsigned int i=0; i<getNumChildren(); i++ ){ 
      
      // Compute the forward kinematics for that children
      devOSGBody* body = dynamic_cast<devOSGBody*>( getChild( i ) );
      if( body != NULL )
	{ body->SetTransform( ForwardKinematics( q , i+startlink ) ); }

      else{
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to cast child as devOSGBody."
			  << std::endl;
	return devOSGManipulator::EFAILURE;
      }

    }
    
    return devOSGManipulator::ESUCCESS;
  }

  return devOSGManipulator::EFAILURE;

}

devOSGManipulator::Errno 
devOSGManipulator::SetVelocities
( const vctDynamicVector<double>& CMN_UNUSED( qds ) )
{ return devOSGManipulator::EFAILURE; }

devOSGManipulator::Errno 
devOSGManipulator::SetForcesTorques
( const vctDynamicVector<double>& CMN_UNUSED( ft ) )
{ return devOSGManipulator::EFAILURE; }
