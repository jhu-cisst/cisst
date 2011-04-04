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
  vctDynamicMatrix<double> vctVertices;
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
  void Transform();

  vctMatrixRotation3<double> GetOrientation() const;
  vctFixedSizeVector<double,3> GetPosition() const;
  
 public:

  //! Main constructor
  /**
     This create an ODE body that can be inserted in an ODE world. This body
     does not have mass such that it cannot be moved (static object).
     \param name The name of the body
     \param Rtwb The position/orientation of the body
     \param model The file name of the body's geometry
     \param odeWorld The world the body belongs to
     \param spaceid The space the body belongs to
  */
  devODEBody( const std::string& name,
	      const vctFrame4x4<double>& Rtwb,
	      const std::string& model,
	      devODEWorld* odeWorld,
	      dSpaceID spaceid );

  //! Main constructor
  /**
     This create an ODE body that can be inserted in an ODE world. This body 
     has a mass such that it can be moved.
     \param name The name of the body
     \param Rtwb The position/orientation of the body
     \param model The file name of the body's geometry
     \param odeWorld The world the body belongs to
     \param m The mass of the body
     \param com The center of mass of the body
     \param moit The moment of inertia tensor
     \param spaceid The space the body belongs to
  */
  devODEBody( const std::string& name,
	      const vctFrame4x4<double>& Rt,
	      const std::string& model,
	      devODEWorld* odeWorld,
	      double m,
	      const vctFixedSizeVector<double,3>& com,
	      const vctFixedSizeMatrix<double,3,3>& moit,
	      dSpaceID spaceid );

  //! Constructor
  /**
     This create an ODE body that can be inserted in an ODE world. This body
     does not have mass such that it cannot be moved (static object). The space
     ID is defined to the world's space.
     \param name The name of the body
     \param Rtwb The position/orientation of the body
     \param model The file name of the body's geometry
     \param odeWorld The world the body belongs to
  */
  devODEBody( const std::string& name,
	      const vctFrm3& Rt,
	      const std::string& model,
	      devODEWorld* odeWorld );

  //! Main constructor
  /**
     This create an ODE body that can be inserted in an ODE world. This body 
     has a mass such that it can be moved. The center of mass is set to zero
     and the tensor to identity and space ID set to the world's space.
     \param name The name of the body
     \param Rtwb The position/orientation of the body
     \param model The file name of the body's geometry
     \param odeWorld The world the body belongs to
     \param m The mass of the body
  */
  devODEBody( const std::string& name,
	      const vctFrm3& Rt,
	      const std::string& model,
	      devODEWorld* world, 
	      double m );

  devODEBody( const std::string& name,
	      const vctFrm3& Rt,
	      const std::string& model,
	      devODEWorld* world, 
	      double m,
	      const vctFixedSizeVector<double,3>& tbcom,
	      const vctFixedSizeMatrix<double,3,3>& moit );

  //! Default destructor
  ~devODEBody();
  
  void Enable();
  void Disable();

  //! Query the ID of the body
  dBodyID GetBodyID() const { return bodyid; }

  //! Query the ID of the body
  dGeomID GetGeomID() const { return geomid; }

  vctFrm3 GetTransform() const;

  vctDynamicMatrix<double> GetVertices() const;
  
};

#endif
