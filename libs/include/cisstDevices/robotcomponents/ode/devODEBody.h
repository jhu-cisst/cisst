/*

  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devODEBody_h
#define _devODEBody_h

#include <ode/ode.h>

#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstDevices/devExport.h>

class devODEWorld;

class CISST_EXPORT devODEBody : public devOSGBody {

 public:

  struct State{
    vctMatrixRotation3<double>   R;
    vctFixedSizeVector<double,3> t;
    vctFixedSizeVector<double,3> v;
    vctFixedSizeVector<double,3> w;
  };


 private:

  //! The World ID
  devODEWorld* world;

  //! The ODE body ID
  dBodyID bodyid;

  //! The ODE mass
  dMass* mass;

  //! The ODE mesh ID
  /**
     This is a structure used by ODE to represent a triangular mesh
  */
  dTriMeshDataID meshid;

  //! The ODE geom ID
  /**
     This is a structure used by ODE to process collisions
  */
  dGeomID geomid;

  //! Data used by ODE Tri Mesh
  dVector3* Vertices;
  int       VertexCount;
  dTriIndex* Indices;
  int        IndexCount;

  //! The ODE space ID
  /**
    This is a structure used by ODE to represent collision spaces
  */
  dSpaceID space;

  // Transformation from the center of mass to the intertial frame (base)
  vctFrame4x4<double> Rtcomb;

  void BuildODETriMesh( dSpaceID spaceid, 
			const vctFixedSizeVector<double,3>& com );

  // This is called from each OSG update traversal
  void Update();

  vctMatrixRotation3<double> GetOrientation() const;
  vctFixedSizeVector<double,3> GetPosition() const;

 public:

  devODEBody( const std::string& name,
	      const vctFrame4x4<double>& Rtwb,
	      const std::string& model,
	      devODEWorld* odeWorld,
	      dSpaceID spaceid );

  //! Main constructor
  /**
     This create an ODE body that can be inserted in an ODE world.
     \param worldID The ID of the world used by the body
     \param spaceID The ID of the space used by the body
     \param Rt The position and orientation of the body wrt to the world frame
     \param m The mass of the body
     \param com The center of mass of the body wrt to the body's coordinate 
                frame
     \param moit The moment of inertia tensor of the body about the COM
                 center of mass
     \param geomfile The name of the Wavefront file used by the body
  */
  devODEBody( const std::string& name,
	      const vctFrame4x4<double>& Rt,
	      const std::string& model,
	      devODEWorld* odeWorld,
	      double m,
	      const vctFixedSizeVector<double,3>& com,
	      const vctFixedSizeMatrix<double,3,3>& moit,
	      dSpaceID spaceid );


  //! Default destructor
  ~devODEBody();
  
  void Enable();
  void Disable();

  //! Query the ID of the body
  dBodyID GetBodyID() const { return bodyid; }

  //! Query the ID of the body
  dGeomID GetGeomID() const { return geomid; }

};

#endif
