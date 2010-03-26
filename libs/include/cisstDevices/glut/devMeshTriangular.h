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

#ifndef _devMeshTriangular_h
#define _devMeshTriangular_h

#include <cisstDevices/glut/devGeometry.h>

class CISST_EXPORT devMeshTriangular : public devGeometry {

 protected:

  //! the coordinates of vertices
  double* vx;           // vertices x coordinates
  double* vy;           // vertices y coordinates
  double* vz;           // vertices z coordinates     
  size_t nvertices;
  
  //! the index of the vertices
  size_t* p1;           // index of the 1st vertex
  size_t* p2;           // index of the 2nd vertex
  size_t* p3;           // index of the 3rd vertex
  size_t ntriangles;

  //! the triangles normals
  double* nx;           // normal x
  double* ny;           // normal y
  double* nz;           // normal z

  //! The RGB color
  vctFixedSizeVector<double,3>  RGB;

  //! Allocate the arrays for the mesh
  /**
     Allocate the arrays to store the vertices, indices and normals
     \param nv The number of vertices
     \param nt The number of triangles
  */
  void AllocateMemory( size_t nv, size_t nt );

  //! Deallocate the arrays
  /**
     Free the memory of all the arrays
  */
  void DeallocateMemory();

public:
  
  //! Default constructor
  /**
     Set the number of vertices and triangles to zero and all the arrays to NULL
  */
  devMeshTriangular();

  //! Destructor
  /**
     Deallocate the mesh arrays
  */
  ~devMeshTriangular();

  //! Load an OBJ file
  /**
     Load an OBJ file. Currently the bare minimum is supported. Only simple mesh
     (no nurb, no curve) can be loaded.
     \param filename The filename of the OBJ file
     \param SUCCESS if the file was properly loaded. ERROR otherwise.
   */
  devGeometry::Errno LoadOBJ( const std::string& filename );

  //! Draw the mesh in OpenGL
  /**
     Draw the mesh in a OpenGL visual. The position and orientation of the mesh
     are pushed on the stack before drawing and poped after.     
  */
  void Draw() const;

};

#endif
