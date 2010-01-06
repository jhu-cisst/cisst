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

#include <cisstRobot/ode/robMeshTriangularODE.h>

// Set the pointer to null
robMeshTriangularODE::robMeshTriangularODE() : robMeshTriangular(){
  vertices = NULL;
  indices = NULL;
}

// Deallocate the arrays
robMeshTriangularODE::~robMeshTriangularODE(){
  if( indices != NULL ) delete[] indices;
  if( vertices != NULL ) delete[] vertices;
}

// Deep copy
// Call the robMeshTriangular constructor
robMeshTriangularODE::robMeshTriangularODE( const robMeshTriangularODE& mesh ) :
  
  robMeshTriangular( mesh ){

  // create new arrays
  vertices = new dVector3[nvertices];  // hold the vertices
  indices  = new size_t[ntriangles*3]; // hold the indices (3indices/triangle)

  // copy the values. ensures that the source is there
  assert( !(0 < mesh.nvertices  && mesh.vertices == NULL) );
  assert( !(0 < mesh.ntriangles && mesh.indices  == NULL) );
  memcpy( (void*)vertices, (void*)mesh.vertices, nvertices*sizeof(dVector3) ); 
  memcpy( (void*)indices,  (void*)mesh.indices,  ntriangles*3*sizeof(size_t) );

}

// assignment operator
robMeshTriangularODE& 
robMeshTriangularODE::operator=(const robMeshTriangularODE& mesh){

  if( this != &mesh ){
    // call the base class operator
    robMeshTriangular::operator=(mesh);

    // delete the arrays if necessary
    if( indices != NULL ) delete[] indices;
    if( vertices != NULL ) delete[] vertices;
    
    // create the new arrays
    vertices = new dVector3[nvertices];  // hold the vertices
    indices  = new size_t[ntriangles*3]; // hold the indices 

    // copy the values. ensures that the source is there
    assert( !(0 < mesh.nvertices  && mesh.vertices == NULL) );
    assert( !(0 < mesh.ntriangles && mesh.indices  == NULL) );
    memcpy( (void*)vertices, (void*)mesh.vertices, nvertices*sizeof(dVector3) ); 
    memcpy( (void*)indices,  (void*)mesh.indices, ntriangles*3*sizeof(size_t) );
  }

  return *this;

}

robError 
robMeshTriangularODE::Load( const std::string& filename ){

  // call the base class to load the mesh
  if( robMeshTriangular::Load( filename ) == ERROR )
    return ERROR;

  // create the arrays
  if( vertices != NULL ) delete[] vertices;
  if( indices != NULL )  delete[] indices;

  vertices = new dVector3[nvertices];  // hold the vertices
  indices  = new size_t[ntriangles*3]; // hold the indices 

  // copy the vertices values
  for( size_t i=0; i<nvertices; i++ ){
    vertices[i][0] = vx[i];
    vertices[i][1] = vy[i];
    vertices[i][2] = vz[i];
  }

  // copy the indices (3 indices/triangle)
  for( size_t i=0; i<ntriangles; i++ ){
    indices[i*3+0] = p1[i];
    indices[i*3+1] = p2[i];
    indices[i*3+2] = p3[i];  }
  
  return SUCCESS;
}

void 
robMeshTriangularODE::ConfigureMeshTriangularODE( dSpaceID space ){

  this->space = space; 

  // create the ODE data object
  meshid = dGeomTriMeshDataCreate();

  // build the data
  dGeomTriMeshDataBuildSimple( meshid,
  			       (const dReal*)vertices, nvertices,
  			       (const dTriIndex*)indices, ntriangles );
  
  // create the geom
  geom = dCreateTriMesh( space, meshid, NULL, NULL, NULL);

}
