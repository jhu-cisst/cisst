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

#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsTaskManager.h>

devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devManipulator::Mode mode ) :
  devManipulator( devname, period, state, mask, mode ),
  robManipulator(),
  input( NULL ),
  output( NULL ){}


devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devManipulator::Mode mode,
				      const std::string& robotfile,
				      const vctFrame4x4<double>& Rtw0 ) :
  devManipulator( devname, period, state, mask, mode ),
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
  input( NULL ),
  output( NULL ),
  q( qinit ){

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
    addChild( new devOSGBody( linkname.str(), Rtw0, basemodel ) );

  }

  // Add the remaining links called "linki"
  for( size_t i=1; i<=links.size(); i++ ){

    std::ostringstream linkname;           // name of the link component
    linkname << devname << "link" << i;    // name of link i

    // Add the body to the OSG group
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i );
    addChild( new devOSGBody( linkname.str(), Rtwi, models[i-1] ) );

  }
  
  // Add this manipulator to the world
  world->addChild( this );

}

devOSGManipulator::~devOSGManipulator(){
  if( input != NULL )  delete input;
  if( output != NULL ) delete output;
}

void devOSGManipulator::Read()
{ output->SetPosition( this->q ); }

void devOSGManipulator::Write(){
  double t;

  // Fetch the joint positions
  input->GetPosition( this->q, t );

  // this is to skip the base if no model for the base is used
  int startlink = 0;
  if( links.size() == getNumChildren() )
    { startlink = 1; }

  for( unsigned int i=0; i<getNumChildren(); i++ ){ 

    devOSGBody* body = dynamic_cast<devOSGBody*>( getChild( i ) );
    if( body != NULL )
      { body->SetTransform( ForwardKinematics( q , i+startlink ) ); }

  }

} 

