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

#ifndef _devODEWorld_h
#define _devODEWorld_h

#include <ode/ode.h>

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstDevices/robotcomponents/ode/devODEJoint.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEWorld : public devRobotComponent {

private:

  //! The time step of the engine
  double timestep;
  
  //! The ODE world ID
  /**
     In ODE, a world contain all the bodies and the spaces containing the 
     geometries
  */
  dWorldID worldid;

  //! The ODE space ID
  /**
     In ODE, a space contains all the geometries (shape) of the bodies. Spaces
     are used to process collisions between geometries.
  */
  dSpaceID spaceid;

  //! The floor geom
  dGeomID floor;
  
  //! The ODE contact group
  /**
     Contact group is used to add contacts between geometries at each iteration.
     This contact group is the one used by the world
  */
  dJointGroupID contactsgid;

  //! Return the contacts group ID
  /**
     This is used internally for processing collisions. 
     \return The contacts group ID.
  */
  dJointGroupID GroupID() const { return contactsgid; }

  //! The maximum number of contacts
  /**
     This value determines the manimum number of contacts that can happen 
     between two bodies. Using several contact point slows down the simulation 
     while too few gives bad results (100 contacts points is actually quite 
     large)
  */
  static const size_t NUM_CONTACTS = 5;

  std::vector<devODEBody*> bodies;
  std::vector<devODEJoint*> joints;

public:

  //! Create a new world
  /**
     Create a new ODE world. This initializes the ODE engine and create a new
     world and a new top level space. It also sets simulation parameters such as
     error reduction parameter (ERP) and constraint force mixing (CFM)
     \param gravity A gravity vector. The defalt value is 
                    \$\begin{bmatrix} 0 & 0 & -9.81 \end{bmatrix}\$.
  */
  devODEWorld( double period,
	       osaCPUMask mask,
	       const vctFixedSizeVector<double,3>& gravity = 
	       vctFixedSizeVector<double,3>(0.0, 0.0, -9.81) );

  //! Destroy the world!
  ~devODEWorld();

  //! Return the world ID
  dWorldID WorldID() const { return worldid; }

  //! Return the space ID
  dSpaceID SpaceID() const { return spaceid; }

  //! Detect a manipulator "self collision"
  /**
     A self collision is a collision when the manipulator hits itself. This is
     called to avoid computing these self collision because they hog too much 
     CPU and often make the simulation unstable. The problem is that adjacent 
     links can touch each other at the joint, thus, creating permanent collision.
     This really mess things up and it's better to avoid them altogether. Sure 
     it less realistic but eventually it would be nice to have the manipulator 
     handle these collision by themselves (and permit some links to interfere)
     \param o1 The geometry of the first object
     \param o2 The geometry of the second object
     \return true if the two geometries belong to the same subspace. false
             otherwise
  */
  bool SelfCollision( dGeomID o1, dGeomID o2 );

  //! Process collisions between two geometries
  /**
     This is the main call to process collisions between geometries
     \param o1 The geometry of the first object
     \param o2 The geometry of the second object
  */
  void Collision( dGeomID o1, dGeomID o2 );

  void RunComponent();

  void Insert( devODEBody* body );
  void Insert( devODEJoint* joint );

};

#endif
