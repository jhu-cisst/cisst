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
#include <cisstVector/vctFrame4x4.h>

#include <cisstDevices/glut/devMeshTriangular.h>
#include <cisstDevices/glut/devGLUT.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEBody {

private:
  
  //! The ODE body ID
  dBodyID bodyid;

  //! The ODE mass
  dMass* mass;

  //! The indices of the triangles
  /**
     This array contains the indices of each triangle. The first 3
     entries are the indices to the vertices of the first triangle. The next 3
     entries are the indices to the vertices of the second triangle and so on.
     The size of this array is 3*ntriangles.
  */
  dTriIndex* triangles;
  size_t ntriangles;

  //! The vertices of the facets
  /**
     This array contains the coordinates of each vertex. The first 3 dVector3
     contains the coordinates of the 1st vertex, the second dVector3 contains
     the coordinates of the second vertex and so on. The vertices are no in any
     specific order and should be used in combination with the indices array.
     The size of this array is nvertices.
  */
  dVector3* vertices;
  size_t nvertices;

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

  //! The ODE space ID
  /**
    This is a structure used by ODE to represent collision spaces
  */
  dSpaceID space;

  //! Load an OBJ file
  /**
     This load an .obj (wavefront) file into memory. It calls 
     robMeshTriangular::Load(const std::string&) to load the mesh then allocates
     and populate the vertices and indices arrays
     \param filename The full path to an .obj filename
     \return SUCCESS if the mesh was properly loaded. ERROR otherwise
  */
  int LoadOBJ( const std::string& filename,
	       const vctFixedSizeVector<double,3>& com );

  //!
  devMeshTriangular* geometry;

  vctFrame4x4<double> Rtcomb;

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
  devODEBody( dWorldID worldid, 
	      dSpaceID spaceid,
	      const vctFrame4x4<double>& Rt,
	      double m,
	      const vctFixedSizeVector<double,3>& com,
	      const vctFixedSizeMatrix<double,3,3>& moit,
	      const std::string& geomfile,
	      bool glutgeom = true );

  //! Default destructor
  ~devODEBody();
  
  //! Query the ID of the body
  dBodyID BodyID() const { return bodyid; }

  //! Query the ID of the body
  dGeomID GeomID() const { return geomid; }

  void Update();

  vctMatrixRotation3<double> GetOrientation() const;
  vctFixedSizeVector<double,3> GetPosition() const;

};

#endif
