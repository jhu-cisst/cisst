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

#ifndef _robMeshODE_h
#define _robMeshODE_h

#ifdef CISST_ODE_SUPPORT

#include <ode/ode.h>

#include <cisstRobot/robMeshTriangular.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robMeshODE : public robMeshTriangular {
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

protected:

  //! Configure the mesh for ODE
  /**
     This builds the ODE mesh structure and configure the geometry for 
     collisions
     \param space The space of the geometry
  */
  void Configure( dSpaceID space );

public:
  
  //! Default constructor
  /**
     Set the pointer to null
  */
  robMeshODE();
  
  // copy constructor
  /**
     Make a deep copy of the mesh. Calls the copy constructor of 
     robMeshTriangular and allocate and copy the vertices and the indices arrays.
     \param mesh A ODE mesh object
  */
  robMeshODE( const robMeshODE& mesh );

  //! Default destructor
  /**
     Deallocate the memory used by the vertices and the indices array
  */
  ~robMeshODE();

  //! Assignment operator (deep copy)
  /**
     Make a deep copy of the mesh. This invokes robMeshTriangular assignment 
     operator and allocate and copy the vertices and indices arrays
     \param mesh A ODE mesh object
     \return A reference to the mesh
  */
  robMeshODE& operator=( const robMeshODE& mesh );

  //! Assignment the position/orientation
  /**
     Assign a position and orientation to the mesh. This only sets the 4x4 
     matrix of the mesh.
     \param Rt The position and orientation
     \return A reference to the mesh
  */
  robMeshODE& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt );

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

#endif  // ODE


#endif
