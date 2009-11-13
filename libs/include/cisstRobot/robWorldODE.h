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

#ifndef _robWorldODE_h
#define _robWorldODE_h

#ifdef CISST_ODE_SUPPORT

#include <cisstRobot/robManipulatorODE.h>
#include <cisstRobot/robBodyODE.h>
#include <vector>

#include <cisstRobot/robExport.h>

class CISST_EXPORT robWorldODE {

private:
  
  //! A vector of ODE manipulators to be used in the simulation
  std::vector<robManipulatorODE*> manipulators;

  //! A vector of ODE bodies to be used in the simulation
  std::vector<robBodyODE*>  bodies;

  //! The ODE world ID
  /**
     In ODE, a world contain all the bodies and the spaces containing the 
     geometries
  */
  dWorldID          worldid;

  //! The ODE space ID
  /**
     In ODE, a space contains all the geometries (shape) of the bodies. Spaces
     are used to process collisions between geometries.
  */
  dSpaceID          spaceid;

  //! The ODE contact group
  /**
     Contact group is used to add contacts between geometries at each iteration.
     This contact group is the one used by the world
  */
  dJointGroupID contactsgid;

  //! The maximum number of contacts
  /**
     This value determines the manimum number of contacts that can happen between
     two bodies. Using several contact point slows down the simulation while too
     few gives bad results (100 contacts points is actually quite large)
  */
  static const size_t NUM_CONTACTS = 100;

  //! Return the contacts group ID
  /**
     This is used internally for processing collisions. 
     \return The contacts group ID.
  */
  dJointGroupID GroupID() const { return contactsgid; }

public:

  //! Create a new world
  /**
     Create a new ODE world. This initializes the ODE engine and create a new
     world and a new top level space. It also sets simulation parameters such as
     error reduction parameter (ERP) and constraint force mixing (CFM)
     \param gravity A gravity vector. The defalt value is 
                    \$\begin{bmatrix} 0 & 0 & -9.81 \end{bmatrix}\$.
   */
  robWorldODE( const vctFixedSizeVector<double,3>& gravity = 
	       vctFixedSizeVector<double,3>(0.0, 0.0, -9.81) );

  //! Destroy the world!
  ~robWorldODE();

  //! Return the ID of the ODE world
  /**
     In ODE you will need the world ID to create bodies and spaces.
     \return The ID of the world.
  */
  dWorldID WorldID() const { return worldid; }
  //! Return the ID of the top level space.
  /**
     In ODE you will need the space ID to add geometries and sub-spaces.
     \return The ID of the top level space.
  */
  dSpaceID SpaceID() const { return spaceid; }

  //! Detect a manipulator "self collision"
  /**
     A self collision is a collision when the manipulator hits itself. This is
     called to avoid computing these self collision because they hog to much 
     CPU and often make the simulation unstable. The problem is that the geometry
     of each might not be perfect and it is possible that adjacent links indeed
     touch each other. Thus, creating permanent collision.This really mess things
     up and it's better to avoid them altogether. Sure it less realistic but
     eventually it would be nice to have the manipulator handle these collision
     by themselves (and permit some links to interfere)
     \param o1 The geometry of the first object
     \param o2 The geometry of the second object
     \return true if the two geometries belong to the same manipulator. false
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

  //! Insert a body in the world
  /**
     Insert an ODE body in the world. The body will be enabled for simulations
     \param body A pointer to an ODE body
  */
  void Insert( robBodyODE* body );


  //! Insert a manipulator in the world
  /**
     Insert an ODE manipulator in the world. The body will be enabled for 
     simulations.
     \param body A pointer to an ODE body
  */
  void Insert( robManipulatorODE* manipulator );

  //! Take a simulation step
  /**
     Run the ODE engine for the specified amount of time. Note that changing the
     amount of time between steps is not a good idea. So once you have a step
     size, stick with it.
     \param dt The step size in seconds
   */
  void Step( double dt );

};

#endif // ODE

#endif
