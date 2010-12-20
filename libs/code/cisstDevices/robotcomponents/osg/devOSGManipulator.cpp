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
				      devManipulator::Mode mode ):
  devManipulator( devname, period, state, mask, mode ){}

devOSGManipulator::devOSGManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devManipulator::Mode mode,
				      const std::string& robotfile,
				      const vctFrame4x4<double>& Rtw0 ) :
  devManipulator( devname, period, state, mask, mode ),
  robManipulator( robotfile, Rtw0 ) {}

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

  // MTS transformations. One per link
  mtsRtw.resize( this->links.size() + 1 );

  // We need to add ourselves since we'll connect the links
  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();
  taskManager->AddComponent( this );

  // Create a transformation interface to communicate with the links
  mtsInterfaceProvided* provided;
  provided = AddInterfaceProvided( "Transformation" );

  devOSGBody* link = NULL;

  if( provided != NULL ){

    std::ostringstream linkname;           // name of the link component
    std::ostringstream dataname;           // name of the state table data
    std::ostringstream cmndname;           // name of the command
    linkname << devname << "link" << 0;    // name of link 0
    dataname << "Rtw" << 0;                // name of link 0 transformation
    cmndname << "GetRtw" << 0;             // name of link 0 command

    // Create the MTS stuff
    StateTable.AddData( mtsRtw[0], dataname.str() );
    provided->AddCommandReadState( StateTable, mtsRtw[0], cmndname.str() );

    // Create the link for the base
    link=new devOSGBody(linkname.str(), Rtw0, basemodel, world, cmndname.str());
    // Add the base
    taskManager->AddComponent( link );
    // Add the base to the OSG group
    addChild( link );

    // connect the manipulator to the base
    taskManager->Connect( link->GetName(), "Transformation",
    			  this->GetName(), "Transformation" );

    // Add the remaining links
    for( size_t i=1; i<=links.size(); i++ ){

      linkname.clear();      linkname.str("");  // clear the string streams
      dataname.clear();      dataname.str("");
      cmndname.clear();      cmndname.str("");

      linkname << devname << "link" << i;       // name of link i
      dataname << "Rtw" << i;                   // name of link i transformation
      cmndname << "GetRtw" << i;                // name of link i command
      
      // Create the MTS stuff
      StateTable.AddData( mtsRtw[i], dataname.str() );
      provided->AddCommandReadState( StateTable, mtsRtw[i], cmndname.str() );

      // Create the link
      link = new devOSGBody( linkname.str(), mtsRtw[i], models[i-1], 
			     world, cmndname.str() );
      // Add the link
      taskManager->AddComponent( link );      

      // Add the body to the OSG group
      addChild( link );

      // connect the manipulator to the link
      taskManager->Connect( link->GetName(), "Transformation",
			    this->GetName(), "Transformation" );

    }

  }
  
  // Update the transformations
  UpdateKinematics();

  // Add this manipulator to the world
  world->addChild( this );

}

devOSGManipulator::~devOSGManipulator(){
  if( input != NULL )  delete input;
  if( output != NULL ) delete output;
}

void devOSGManipulator::Read()
{ output->SetPosition( q ); }

void devOSGManipulator::Write(){
  double t;
  // Fetch the joint positions
  input->GetPosition( q, t );
  UpdateKinematics();
} 

void devOSGManipulator::UpdateKinematics(){
  for( size_t i=0; i<getNumChildren(); i++ )
    { mtsRtw[i] = ForwardKinematics( q , i ); }
}
