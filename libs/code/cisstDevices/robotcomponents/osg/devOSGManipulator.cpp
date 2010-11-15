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
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstVector/vctQuaternionRotation3.h>

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
  q( qinit ){

  input = ProvideInputRn( devManipulator::Input,
			  devRobotComponent::POSITION,
			  qinit.size() );

  output = ProvideOutputRn( devManipulator::Output,
			    devRobotComponent::POSITION,
			    qinit.size() );
  
  setName( devname );

  world->addChild( this );
  
  addChild( new devOSGBody( basemodel, world ) );

  for( size_t i=0; i<links.size(); i++ )
    { addChild( new devOSGBody( models[i], world ) ); }
  
  Write( qinit );
  
}

devOSGManipulator::~devOSGManipulator(){}

void devOSGManipulator::Read(){}

void devOSGManipulator::Write(){
  vctDynamicVector<double> q;
  double t;
  input->GetPosition( q, t );
  Write( q );
} 

void devOSGManipulator::Write( const vctDynamicVector<double>& q ){
  for( size_t i=0; i<getNumChildren(); i++ ){
    vctFrame4x4<double> Rtwi = ForwardKinematics( this->q , i );
    osg::Node* node = getChild( i );
    devOSGBody* body = dynamic_cast<devOSGBody*>( node );
    if( body != NULL )
      { body->SetTransformation( Rtwi ); }
  }
}
