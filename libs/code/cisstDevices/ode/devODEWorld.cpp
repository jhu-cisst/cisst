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


#include <cisstDevices/ode/devODEWorld.h>
#include <cisstCommon/cmnLogger.h>

devODEWorld::devODEWorld( double period,
			  const vctFixedSizeVector<double,3>& gravity ) : 
  mtsTaskPeriodic( "devODEWorld", period, true ),
  timestep(period) {
  
  dInitODE2(0);                             // initialize the engine

  worldid = dWorldCreate();                 // create new world for bodies
  spaceid = dSimpleSpaceCreate(0);          // create a new space for geometries
  contactsgid = dJointGroupCreate(0);       // create a new contact group

  //floor = dCreatePlane( SpaceID(), 0.0, 0.0, 1.0, 0.0 );

  // Set the gravity
  dWorldSetGravity( WorldID(), gravity[0], gravity[1], gravity[2] );

  // The following values are tuned "to make ODE work" decently
  // you can change these values if you wish and results may vary
  // set the error reduction parameter
  dWorldSetERP( WorldID(), 0.5 );

  // set the constraint force mixing
  dWorldSetCFM( WorldID(), 0.0000001 );

  // set the surface layer depth
  dWorldSetContactSurfaceLayer( WorldID(), 0.001 );

}

// destroy the world
devODEWorld::~devODEWorld()
{ dWorldDestroy( WorldID() ); }

// determine if two geoms belong to the same body (manipulator)
bool devODEWorld::SelfCollision( dGeomID o1, dGeomID o2 ){

  // if either parameters is the root space, then it's not self-collision
  // from here on we don't worry about the top level space anymore
  if( (((dSpaceID)o1)==SpaceID()) || (((dSpaceID)o2)==SpaceID()) )
    return false;

  // Deal with 2 spaces here
  if( dGeomIsSpace(o1) && dGeomIsSpace(o2) ){
    // if o1 and o2 represents the same space, then it's a self collision
    if( o1==o2 ) return true;
    else         return false;
  }

  // here o1 is a space and o2 is a geom
  if( dGeomIsSpace(o1) ){
    // if the space of o2 is o1 then it's a self collision
    if( ((dSpaceID)o1)==dGeomGetSpace(o2) ) return true;
    else                                    return false;
  }

  // here o2 is a space and o1 is a geom
  if( dGeomIsSpace(o2) ){
    // if the space of o1 is o2 then it's a self collision
    if( ((dSpaceID)o2)==dGeomGetSpace(o1) ) return true;
    else                                    return false;
  }

  // here both parameters are geoms. if one of them is in the top level space
  // no self-collision
  if( dGeomGetSpace(o1) == SpaceID() ||
      dGeomGetSpace(o2) == SpaceID() ) return false;

  if( dGeomGetSpace(o1) == dGeomGetSpace(o2) ) return true;

  return false;
}

void devODEWorld::Collision( dGeomID o1, dGeomID o2 ){
  dContact contacts[devODEWorld::NUM_CONTACTS];
  
  for(size_t i=0; i<devODEWorld::NUM_CONTACTS; i++){
    contacts[i].surface.mode = ( dContactBounce  |
				 dContactSoftERP |
				 dContactSoftCFM );
    contacts[i].surface.mu = 2;
    contacts[i].surface.mu2 = 2;
    contacts[i].surface.bounce = 0.0;
    contacts[i].surface.soft_erp = 0.3;
    contacts[i].surface.soft_cfm = 0.0001;
  }

  int N = dCollide( o1, o2,
                    devODEWorld::NUM_CONTACTS,
                    &(contacts[0].geom),
                    sizeof(dContact) );

  for(int i=0; i<N; i++){
    dBodyID body1 = dGeomGetBody(o1);
    dBodyID body2 = dGeomGetBody(o2);
    dJointID contact = dJointCreateContact(WorldID(), GroupID(), &contacts[i]);

    dJointAttach (contact, body1, body2);
  }
}

static void space_collision(void *argv, dGeomID o1, dGeomID o2){
  devODEWorld* world = (devODEWorld*)argv;

  if(!world->SelfCollision(o1, o2)){
    
    if (dGeomIsSpace(o1) || dGeomIsSpace(o2)) {
      // colliding a space with something
      dSpaceCollide2(o1, o2, argv, &space_collision);
      
      // collide all geoms internal to the space(s)
      if(dGeomIsSpace (o1)) dSpaceCollide((dSpaceID)o1, argv, &space_collision);
      if(dGeomIsSpace (o2)) dSpaceCollide((dSpaceID)o2, argv, &space_collision);
    }
    else {
      // colliding two non-space geoms, so generate contacts
      world->Collision( o1, o2 );
    }
  }
}

void devODEWorld::Run() {
  
  for( size_t i=0; i<joints.size(); i++ )
    { joints[i]->ApplyForceTorque(); }

  dSpaceCollide( SpaceID(), (void*)this, space_collision);
  dWorldStep( WorldID(), timestep );

  for( size_t i=0; i<bodies.size(); i++ )
    { bodies[i]->Update(); }

  dJointGroupEmpty( GroupID() );

}

void devODEWorld::Insert( devODEBody* body )
{  bodies.push_back( body );  }

void devODEWorld::Insert( devODEJoint* joint )
{ joints.push_back( joint ); }
