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

#include <cisstODE/cisstODEManipulator.h>

cisstODEManipulator::cisstODEManipulator(const std::vector<std::string>& models,
					 cisstODEWorld* odeworld,
					 const vctFrame4x4<double>& Rtw0,
					 const std::string& robotfilename,
					 const std::string& basemodel,
					 const vctDynamicVector<double>& qinit):
  cisstOSGManipulator( Rtw0, robotfilename ),
  odeworld( odeworld ),
  qinit( qinit ){

  CreateManipulator( models, basemodel );

  addChild( osglinks );
  odeworld->addChild( this );

}


cisstODEManipulator::cisstODEManipulator(const std::vector<std::string>& models,
					 cisstODEWorld* odeworld,
					 const vctFrm3& Rtw0,
					 const std::string& robotfilename,
					 const std::string& basemodel,
					 const vctDynamicVector<double>& qinit):
  cisstOSGManipulator( Rtw0, robotfilename ),
  odeworld( odeworld ),
  qinit( qinit ){

  CreateManipulator( models, basemodel );

  addChild( osglinks );
  odeworld->addChild( this );

}

cisstODEManipulator::cisstODEManipulator(const std::vector<std::string>& models,
					 cisstODEWorld* odeworld,
					 const vctFrame4x4<double>& Rtw0,
					 const std::string& robotfilename,
					 cisstODEBody* base,
					 const vctDynamicVector<double>& qinit):
  cisstOSGManipulator( Rtw0, robotfilename ),
  odeworld( odeworld ),
  qinit( qinit ){

  this->base = base;
  CreateManipulator( models );

  addChild( osglinks );
  odeworld->addChild( this );

}


cisstODEManipulator::cisstODEManipulator(const std::vector<std::string>& models,
					 cisstODEWorld* odeworld,
					 const vctFrm3& Rtw0,
					 const std::string& robotfilename,
					 cisstODEBody* base,
					 const vctDynamicVector<double>& qinit):
  cisstOSGManipulator( Rtw0, robotfilename ),
  odeworld( odeworld ),
  qinit( qinit ){

  this->base = base;
  CreateManipulator( models );

  addChild( osglinks );
  odeworld->addChild( this );

}


// This "flattens" the bodies in the world. There are all added to the 
// same OSG world group

void 
cisstODEManipulator::CreateManipulator( const std::vector<std::string>& models,
					const std::string& basemodel ){


  if( !basemodel.empty() ){ 
    osg::ref_ptr<cisstODEBody> odebase = NULL;

    // create the base and add it to the manipulator
    odebase = new cisstODEBody( basemodel, 
				GetWorld(),
				Rtw0, 
				1.0,                                    // m   
				vctFixedSizeVector<double,3>(0.0),      // com
				vctFixedSizeMatrix<double,3,3>::Eye(),  // moit
				(cisstOSGWorld*)this );
    
    // attach the base
    dJointID jid = dJointCreateFixed( GetWorldID(), 0 );
    dJointAttach( jid, NULL, odebase->GetBodyID() );
    dJointSetFixed( jid );

    // keep a copy
    this->base = odebase;

  }
  
  // create a group for the links
  osglinks = new osg::Group();

  // Create the links
  for( size_t i=1; i<=links.size(); i++ ){

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );

    // create the links and add them to the link group       
    osg::ref_ptr<cisstOSGBody> odeli;
    odeli = new cisstODEBody( models[i-1],
			      GetWorld(),
			      Rtwi,
			      links[i-1].Mass(),                       // m   
			      links[i-1].CenterOfMass(),               // com
			      links[i-1].MomentOfInertiaAtCOM(),       // I 
			      (cisstOSGWorld*)(osglinks.get()) );;
  }

  // Initialize the joints
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  osg::ref_ptr<cisstODEBody> odeb1 = dynamic_cast<cisstODEBody*>( base.get() );
  dBodyID b1 = NULL;

  if( odeb1.get() != NULL )
    { b1 = odeb1->GetBodyID(); }

  for( unsigned int i=0; i<GetNumLinks(); i++ ){

    // obtain the ID of the distal link 
    osg::ref_ptr<cisstODEBody> odeb2=dynamic_cast<cisstODEBody*>( GetLink(i) );
    dBodyID b2 = odeb2->GetBodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    dJointType type = dJointTypeHinge;
    if( links[i].GetType() == robKinematics::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    cisstODEJoint* joint;
    joint =  new cisstODEJoint( odeworld->GetWorldID(), // the world ID
				b1,                     // the proximal body
				b2,                     // the distal body
				type,                   // the joint type
				anchor,                 // the XYZ position
				axis,                   // the Z axis 
				qmin,                   // the lower limit
				qmax );                 // the upper limit
    Insert( joint );

    // for some all joints, save the first one must be inverted axis
    double sign = -1.0;
    if( base == NULL && i == 0 )
      sign = 1.0;
    
    Insert( new cisstODEServoMotor( odeworld->GetWorldID(), 
				    b1,             // the first body
				    b2,             // the second body
				    axis*sign,      // the Z axis 
				    1,             // fudged values
				    links[i].ForceTorqueMax(),
				    type ) );

    b1 = b2;  // proximal is now distal

  }

}

void 
cisstODEManipulator::CreateManipulator(const std::vector<std::string>& models){

  // create a group for the links
  osglinks = new osg::Group();

  // Create the links
  for( size_t i=1; i<=links.size(); i++ ){

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );

    // create the links and add them to the link group       
    osg::ref_ptr<cisstOSGBody> odeli;
    odeli = new cisstODEBody( models[i-1],
			      GetWorld(),
			      Rtwi,
			      links[i-1].Mass(),                       // m   
			      links[i-1].CenterOfMass(),               // com
			      links[i-1].MomentOfInertiaAtCOM(),       // I 
			      (cisstOSGWorld*)(osglinks.get()) );;
  }

  // Initialize the joints
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  osg::ref_ptr<cisstODEBody> odeb1 = dynamic_cast<cisstODEBody*>( base.get() );
  dBodyID b1 = NULL;

  if( odeb1.get() != NULL )
    { b1 = odeb1->GetBodyID(); }

  for( unsigned int i=0; i<GetNumLinks(); i++ ){

    // obtain the ID of the distal link 
    osg::ref_ptr<cisstODEBody> odeb2=dynamic_cast<cisstODEBody*>( GetLink(i) );
    dBodyID b2 = odeb2->GetBodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    dJointType type = dJointTypeHinge;
    if( links[i].GetType() == robKinematics::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    cisstODEJoint* joint;
    joint =  new cisstODEJoint( odeworld->GetWorldID(), // the world ID
				b1,                  // the proximal body
				b2,                  // the distal body
				type,                // the joint type
				anchor,              // the XYZ position
				axis,                // the Z axis 
				qmin,                // the lower limit
				qmax );              // the upper limit
    Insert( joint );

    // for some all joints, save the first one must be inverted axis
    double sign = -1.0;
    if( base == NULL && i == 0 )
      sign = 1.0;
    
    Insert( new cisstODEServoMotor( odeworld->GetWorldID(), 
				    b1,             // the first body
				    b2,             // the second body
				    axis*sign,      // the Z axis 
				    1,              // fudged values
				    links[i].ForceTorqueMax(),
				    type ) );

    b1 = b2;  // proximal is now distal

  }

}



void cisstODEManipulator::Attach( cisstOSGManipulator* osgtool ){

  cisstODEBody* odebase = dynamic_cast<cisstODEBody*>( osgtool->GetBase() );
  cisstODEBody* lastlink = dynamic_cast<cisstODEBody*>( GetLink( GetNumLinks()-1 ) );

  if( odebase != NULL && lastlink != NULL){
    // Create a fix joint between the last link and the tool
    dJointID jid = dJointCreateFixed( GetWorldID(), 0 );
    dJointAttach( jid, lastlink->GetBodyID(), odebase->GetBodyID() );
    dJointSetFixed( jid );
  }

  //robManipulator::Attach( odetool );

}

void cisstODEManipulator::Disable(){

  for( size_t i=0; i<odebodies.size(); i++ )
    { odebodies[i]->Disable(); }

  for( size_t i=0; i<tools.size(); i++ ){
    cisstODEManipulator* odetool=dynamic_cast<cisstODEManipulator*>( tools[i] );
    if( odetool != NULL )
      { odetool->Disable(); }
  }

}

void cisstODEManipulator::Enable(){

  for( size_t i=0; i<odebodies.size(); i++ )
    { odebodies[i]->Enable(); }

  for( size_t i=0; i<tools.size(); i++ ){
    cisstODEManipulator* odetool=dynamic_cast<cisstODEManipulator*>( tools[i] );
    if( odetool != NULL )
      { odetool->Enable(); }
  }

}

dBodyID cisstODEManipulator::GetBaseID() const {
  osg::ref_ptr<cisstODEBody> odebase=dynamic_cast<cisstODEBody*>( base.get() );

  if( odebase == NULL ) return NULL;
  else                  return odebase->GetBodyID();
}

void cisstODEManipulator::Insert( cisstODEJoint* joint )
{ odejoints.push_back( joint ); }

void cisstODEManipulator::Insert( cisstODEServoMotor* servo )
{ odeservos.push_back( servo ); }

cisstODEManipulator::Errno
cisstODEManipulator::SetPositions( const vctDynamicVector<double>& qs ){

  vctDynamicVector<double> q;
  GetPositions( q );

  if( qs.size() == odeservos.size() && q.size() == odeservos.size() ){
    for( size_t i = 0; i<qs.size(); i++ )
      { odeservos[i]->SetPosition(  qs[i], q[i], odeworld->GetTimeStep() ); }
    return cisstODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << " Expected " << odeservos.size() 
		      << " velocities. Got " << qs.size() 
		      << std::endl;
    return cisstODEManipulator::EFAILURE;
  }

}

cisstODEManipulator::Errno
cisstODEManipulator::SetVelocities( const vctDynamicVector<double>& qds ){

  if( qds.size() == odeservos.size() ){
    for( size_t i = 0; i<qds.size(); i++ )
      { odeservos[i]->SetVelocity(  qds[i] ); }
    return cisstODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << " Expected " << odeservos.size() 
		      << " velocities. Got " << qds.size() 
		      << std::endl;
    return cisstODEManipulator::EFAILURE;
  }

}

cisstODEManipulator::Errno
cisstODEManipulator::SetForcesTorques( const vctDynamicVector<double>& ft) {

  if( ft.size() == odejoints.size() ){
    for(size_t i=0; i<odejoints.size() && i<ft.size(); i++ )
      { odejoints[i]->SetForceTorque( ft[i] ); }
    return cisstODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << " Expected " << odejoints.size() 
		      << " forces/torques. Got " << ft.size() 
		      << std::endl;
    return cisstODEManipulator::EFAILURE;
  }

}

cisstODEManipulator::Errno
cisstODEManipulator::GetPositions( vctDynamicVector<double>& q ) const {
  q.SetSize( odejoints.size());
  for(size_t i=0; i<odejoints.size(); i++)
    { q[i] =  odejoints[i]->GetPosition(); }
  q = q + qinit;
  return cisstODEManipulator::ESUCCESS;
}

cisstODEManipulator::Errno
cisstODEManipulator::GetVelocities( vctDynamicVector<double>& qd ) const {
  qd.SetSize( odejoints.size());
  for(size_t i=0; i<odejoints.size(); i++)
    { qd[i] = odejoints[i]->GetVelocity(); }
  return cisstODEManipulator::ESUCCESS;
}

#ifndef SWIG

cisstODEManipulator::State cisstODEManipulator::GetState() const {

  cisstODEManipulator::State state;
  
  for( size_t i=0; i<odejoints.size(); i++ ){
    
    dBodyID bid = odejoints[i]->GetDistalBody();

    cisstODEBody::State si;

    const dReal* R = dBodyGetRotation( bid );
    const dReal* t = dBodyGetPosition( bid );
    const dReal* v = dBodyGetLinearVel( bid );
    const dReal* w = dBodyGetAngularVel( bid );

    si.R = vctMatrixRotation3<double> ( R[0], R[1],  R[2], // R[3], 
					R[4], R[5],  R[6], // R[7], 
					R[8], R[9], R[10], // R[11], 
					VCT_NORMALIZE );
    si.t = vctFixedSizeVector<double,3>( t[0], t[1], t[2] );
    si.v = vctFixedSizeVector<double,3>( v[0], v[1], v[2] );
    si.w = vctFixedSizeVector<double,3>( w[0], w[1], w[2] );

    state.push_back( si );

  }

  return state;
}


void cisstODEManipulator::SetState( const cisstODEManipulator::State& state ){

  for( size_t i=0; i<state.size(); i++ ){
    
    dBodyID bid = odejoints[i]->GetDistalBody();

    dMatrix3 R = { state[i].R[0][0], state[i].R[0][1],  state[i].R[0][2], 0.0,
		   state[i].R[1][0], state[i].R[1][1],  state[i].R[1][2], 0.0,
		   state[i].R[2][0], state[i].R[2][1],  state[i].R[2][2], 0.0 };

    dBodySetRotation( bid, R );
    dBodySetPosition( bid, state[i].t[0], state[i].t[1], state[i].t[2] );
    dBodySetLinearVel(  bid, state[i].v[0], state[i].v[1], state[i].v[2] );
    dBodySetAngularVel( bid, state[i].w[0], state[i].w[1], state[i].w[2] );

  }

}

#endif

vctFrame4x4<double> 
cisstODEManipulator::ForwardKinematics( const vctDynamicVector<double>& q,int N )
const { return robManipulator::ForwardKinematics( q, N ); }

void cisstODEManipulator::ForwardKinematics( const vctDynamicVector<double>& q, 
					   vctFrm3& Rt,
					   int N ) const {

  vctFrame4x4<double> Rt4x4 = robManipulator::ForwardKinematics( q, N );
  vctMatrixRotation3<double> R( Rt4x4[0][0], Rt4x4[0][1], Rt4x4[0][2],
				Rt4x4[1][0], Rt4x4[1][1], Rt4x4[1][2],
				Rt4x4[2][0], Rt4x4[2][1], Rt4x4[2][2] );
  Rt = vctFrm3( R, Rt4x4.Translation() );
}
    
