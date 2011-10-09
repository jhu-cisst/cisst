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

#include <osg/Geode>
#include <osg/TriangleFunctor>

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

  // This class is used to extract the triangle mesh out of the OSG
  // classes/structures. It is a geode visitor that traverse the drawable 
  // objects and extract all the triangles from all the drawables
  class GeodeVisitor : public osg::NodeVisitor {

  private:

    // Create a structure to hold a triangle
    struct Triangle
    { dVector3 v1, v2, v3; };

    // For each drawable, a TriangleExtractor object is created. The operator() 
    // is called for each triangle of the drawable
    struct TriangleExtractor {

      // The list of triangles for a drawable
      std::vector< devODEBody::GeodeVisitor::Triangle > drawabletriangles;

      // This method is called for each triangle of the drawable. All it does
      // is to copy the vertices to the vector
      inline void operator ()( const osg::Vec3& v1, 
			       const osg::Vec3& v2, 
			       const osg::Vec3& v3, 
			       bool treatVertexDataAsTemporary );
    };

  public:

    // the list of triangles for the geode (composed of several drawables)
    std::vector< devODEBody::GeodeVisitor::Triangle > geodetriangles;
    
    // Default constructor
    GeodeVisitor();

    // This method is called for each geode. It scans all the drawable of the 
    // geode and extract/copy the triangles to the triangle vector
    virtual void apply( osg::Geode& geode );
    
  };

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
