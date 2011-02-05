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


#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEJoint.h>
#include <cisstCommon/cmnLogger.h>


devODEWorld::devODEWorld( double period,
			  osaCPUMask mask,
			  const vctFixedSizeVector<double,3>& gravity ) : 
  //devRobotComponent( "devODEWorld", period, devODEWorld::ENABLED, mask ),
  mtsTaskPeriodic( "ODEWorld", period, true ),
  timestep(period) {

  dInitODE2(0);                             // initialize the engine

  worldid = dWorldCreate();                 // create new world for bodies
  spaceid = dSimpleSpaceCreate(0);          // create a new space for geometries
  contactsgid = dJointGroupCreate(0);       // create a new contact group

  floor = dCreatePlane( GetSpaceID(), 0.0, 0.0, 1.0, 0.0 );
  

  // Set the gravity
  dWorldSetGravity( GetWorldID(), gravity[0], gravity[1], gravity[2] );

  // The following values are tuned "to make ODE work" decently
  // you can change these values if you wish and results may vary
  // set the error reduction parameter
  //dWorldSetERP( GetWorldID(), 0.5 );

  // set the constraint force mixing
  //dWorldSetCFM( GetWorldID(), 0.0000001 );
  
  // set the surface layer depth
  //dWorldSetContactSurfaceLayer( GetWorldID(), 0.001 );
  //dWorldSetContactMaxCorrectingVel( GetWorldID(), 0.01 );
}

// destroy the world
devODEWorld::~devODEWorld(){ 
  dSpaceDestroy( GetSpaceID() );
  dWorldDestroy( GetWorldID() ); 
  dCloseODE();
}

// determine if two geoms belong to the same body (manipulator)
bool devODEWorld::SelfCollision( dGeomID o1, dGeomID o2 ){

  // if either parameters is the root space, then it's not self-collision
  // from here on we don't worry about the top level space anymore
  if( (((dSpaceID)o1)==GetSpaceID()) || (((dSpaceID)o2)==GetSpaceID()) )
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
  if( dGeomGetSpace(o1) == GetSpaceID() ||
      dGeomGetSpace(o2) == GetSpaceID() ) return false;

  if( dGeomGetSpace(o1) == dGeomGetSpace(o2) ) return true;

  return false;
}

void devODEWorld::Collision( dGeomID o1, dGeomID o2 ){

  dContact contacts[devODEWorld::NUM_CONTACTS];
  
  for(size_t i=0; i<devODEWorld::NUM_CONTACTS; i++){
    contacts[i].surface.mode = (
				dContactMu2     |
				dContactBounce  |
				dContactSoftERP |
				dContactSoftCFM 
				);
    contacts[i].surface.mu = 0.5;
    contacts[i].surface.mu2 = 0.5;
    contacts[i].surface.bounce = 0.1;
    contacts[i].surface.soft_cfm = 0.00001;
    contacts[i].surface.soft_erp = 0.3;
  }

  int N = dCollide( o1, o2,
                    devODEWorld::NUM_CONTACTS,
                    &(contacts[0].geom),
                    sizeof(dContact) );

  ContactsListMutex.Lock();
  for(int i=0; i<N; i++){

    dJointAttach( dJointCreateContact( GetWorldID(), 
				       GetGroupID(), 
				       &contacts[i] ),
		  dGeomGetBody(o1), 
		  dGeomGetBody(o2) );

    if( o1 != floor && o2 != floor ){
      devODEBody* body1 = (devODEBody*)dGeomGetData( o1 );
      devODEBody* body2 = (devODEBody*)dGeomGetData( o2 );

      dContactGeom geom = contacts[i].geom;

      devODEWorld::Contact contact(body1, 
				   body2,
				   vctFixedSizeVector<double,3>(geom.pos[0],
								geom.pos[1],
								geom.pos[2] ),
				   vctFixedSizeVector<double,3>(geom.normal[0],
								geom.normal[1],
								geom.normal[2]),
				   geom.depth);
      ContactsList.push_back( contact );
      
    }

  }
  ContactsListMutex.Unlock();

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
      dBodyID b1 = dGeomGetBody( o1 );
      dBodyID b2 = dGeomGetBody( o2 );

      // dAreConnected doesn't like if both bodies are "zero" (that is if both
      // geoms are non-placeable
      if( b1 != 0 && b2 != 0 ) {
	if( dAreConnected( b1, b2 ) == 0 )
	  { world->Collision( o1, o2 ); }
      }
      else{ world->Collision( o1, o2 ); }
    }
  }
}

void devODEWorld::Lock()
{ WorldMutex.Lock(); }

void devODEWorld::Unlock()
{ WorldMutex.Unlock(); }

//void devODEWorld::RunComponent() {
void devODEWorld::Run() {
  ProcessQueuedCommands();
  ProcessQueuedEvents();

  for( size_t i=0; i<joints.size(); i++ )
    { joints[i]->ApplyForceTorque(); }

  ContactsListMutex.Lock();
  ContactsList.clear();
  ContactsListMutex.Unlock();

  Lock();

  dSpaceCollide( GetSpaceID(), (void*)this, space_collision);
  dWorldStep( GetWorldID(), timestep );

  Unlock();

  dJointGroupEmpty( GetGroupID() );

}

void devODEWorld::Insert( devODEJoint* joint )
{ joints.push_back( joint ); }

std::list< devODEWorld::Contact >
devODEWorld::QueryContacts( const std::string& name ){

  std::list< devODEWorld::Contact > matches;

  ContactsListMutex.Lock();

  std::list< devODEWorld::Contact >::const_iterator contact;
  for( contact=ContactsList.begin(); contact!=ContactsList.end(); contact++ ){
    
    if( contact->body1->GetName() == name || contact->body2->GetName() == name )
      { matches.push_back( *contact ); }

  }

  ContactsListMutex.Unlock();

  return matches;

}
