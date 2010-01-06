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

#ifndef _robMeshTriangularODE_h
#define _robMeshTriangularODE_h

#include <ode/ode.h>

#include <cisstRobot/robMeshTriangular.h>

class CISST_EXPORT robMeshTriangularODE : public robMeshTriangular {
private:

  //! The indices of the facets
  /**
     This array contains the indices of each triangle. The first 3
     entries are the indices to the vertices of the first triangle. The next 3
     entries are the indices to the vertices of the second triangle and son on.
     The size of this array is 3*ntriangles.
  */
  dTriIndex* indices;

  //! The vertices of the facets
  /**
     This array contains the coordinates of each vertex. The first 3 dVector3
     contains the coordinates of the 1st vertex, the second dVector3 contains
     the coordinates of the second vertex and so on. The vertices are no in any
     specific order and should be used in combination with the indices array.
     The size of this array is nvertices.
  */
  dVector3*  vertices;

  //! The ODE mesh ID
  /**
     This is a structure used by ODE to represent a triangular mesh
  */
  dTriMeshDataID meshid;

  //! The ODE geom ID
  /**
     This is a structure used by ODE to process collisions
  */
  dGeomID        geom;

  //! The ODE space ID
  /**
    This is a structure used by ODE to represent collision spaces
  */
  dSpaceID space;

public:
  
  //! Default constructor
  /**
     Set the pointer to null
  */
  robMeshTriangularODE();
  
  // copy constructor
  /**
     Make a deep copy of the mesh. Calls the copy constructor of 
     robMeshTriangular and allocate and copy the vertices and the indices arrays.
     \param mesh A ODE mesh object
  */
  robMeshTriangularODE( const robMeshTriangularODE& mesh );

  //! Default destructor
  /**
     Deallocate the memory used by the vertices and the indices array
  */
  ~robMeshTriangularODE();

  //! Assignment operator (deep copy)
  /**
     Make a deep copy of the mesh. This invokes robMeshTriangular assignment 
     operator and allocate and copy the vertices and indices arrays
     \param mesh A ODE mesh object
     \return A reference to the mesh
  */
  robMeshTriangularODE& operator=( const robMeshTriangularODE& mesh );

  //! Configure the mesh for ODE
  /**
     This builds the ODE mesh structure and configure the geometry for 
     collisions
     \param space The space of the geometry
  */
  void ConfigureMeshTriangularODE( dSpaceID space );

  //! Load an OBJ file
  /**
     This load an .obj (wavefront) file into memory. It calls 
     robMeshTriangular::Load(const std::string&) to load the mesh then allocates
     and populate the vertices and indices arrays
     \param filename The full path to an .obj filename
     \return SUCCESS if the mesh was properly loaded. ERROR otherwise
  */
  robError Load( const std::string& filename );

};

#endif
