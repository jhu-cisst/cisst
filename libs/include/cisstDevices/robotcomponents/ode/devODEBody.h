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

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
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

  // This is used to update the position of the OSG body from the 
  // ODE position
  class OSGCallback : public osg::NodeCallback {    
  public:
    OSGCallback();
    void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };

  //! The name of the body
  std::string name;
  
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

  vctFrame4x4<double> Rtcomb;

  void BuildODETriMesh( dSpaceID spaceid, 
			const vctFixedSizeVector<double,3>& com );

 public:

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
	      double m,
	      const vctFixedSizeVector<double,3>& com,
	      const vctFixedSizeMatrix<double,3,3>& moit,
	      const std::string& model,
	      dSpaceID spaceid,
	      devODEWorld* odeWorld );

  devODEBody( const std::string& name,
	      const vctFrame4x4<double>& Rtwb,
	      const std::string& model,
	      dSpaceID spaceid,
	      devODEWorld* odeWorld );

  //! Default destructor
  ~devODEBody();
  
  void Enable();
  void Disable();

  //! Query the ID of the body
  dBodyID GetBodyID() const { return bodyid; }

  //! Get the body name
  std::string GetName() const { return name; }

  //! Query the ID of the body
  dGeomID GetGeomID() const { return geomid; }

  void Update();

  vctMatrixRotation3<double> GetOrientation() const;

  vctFixedSizeVector<double,3> GetPosition() const;

};

#endif
