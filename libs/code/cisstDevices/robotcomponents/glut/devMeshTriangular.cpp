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

#include <stdlib.h>
#include <GL/glut.h>

#include <vector>
#include <string.h>

#include <cisstDevices/robotcomponents/glut/devMeshTriangular.h>
#include <cisstCommon/cmnLogger.h>


devMeshTriangular::devMeshTriangular() :
  vx(NULL),  vy(NULL),  vz(NULL), nvertices(0),
  p1(NULL),  p2(NULL),  p3(NULL), ntriangles(0),
  nx(NULL),  ny(NULL),  nz(NULL) {}

devMeshTriangular::~devMeshTriangular()
{ DeallocateMemory(); }

void devMeshTriangular::AllocateMemory( size_t nv, size_t nt ){

  nvertices = nv;
  ntriangles = nt;

  // allocate arrays to store the vertices
  vx = new double[nvertices];
  vy = new double[nvertices];
  vz = new double[nvertices];

  // allocate arrays to store the faces vertices index
  p1 = new size_t[ntriangles];
  p2 = new size_t[ntriangles];
  p3 = new size_t[ntriangles];

  // allocate arrays to store the faces normals
  nx = new double[ntriangles];
  ny = new double[ntriangles];
  nz = new double[ntriangles];

}

void devMeshTriangular::DeallocateMemory(){

  if( vx != NULL ) delete[] vx;
  if( vy != NULL ) delete[] vy;
  if( vz != NULL ) delete[] vz;

  if( p1 != NULL ) delete[] p1;
  if( p2 != NULL ) delete[] p2;
  if( p3 != NULL ) delete[] p3;

  if( nx != NULL ) delete[] nx;
  if( ny != NULL ) delete[] ny;
  if( nz != NULL ) delete[] nz;

}

void devMeshTriangular::Draw( ) const{

  vctFrame4x4<double> Rtcopy = Rt;

  glMultMatrix( Rtcopy );                // push the transformation

  glColor3f(0.6, 0.6, 0.6);              // set the color
  //glColor3f(RGB[0], RGB[1], RGB[2]);   // set the color
  
  glBegin(GL_TRIANGLES);               // draw a triangle
  for( size_t i=0; i<ntriangles; i++ ){  // draw the mesh

    glNormal3d( nx[i], ny[i], nz[i] );   // set the normal

    glVertex3d( vx[ p1[i] ], vy[ p1[i] ], vz[ p1[i] ] );  // set the vertices
    glVertex3d( vx[ p2[i] ], vy[ p2[i] ], vz[ p2[i] ] );
    glVertex3d( vx[ p3[i] ], vy[ p3[i] ], vz[ p3[i] ] );

  }
  glEnd();                             // done
  
  glMultMatrix( Rtcopy.InverseSelf() ); // push the transformation's inverse
}

devGeometry::Errno devMeshTriangular::LoadOBJ( const std::string& filename ){

  std::string line;
  std::ifstream ifs;

  std::vector< vctFixedSizeVector<double,3> > vertices;
  std::vector< vctFixedSizeVector<int,3> > triangles;

  ifs.open(filename.data());
  if(!ifs){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Couldn't open file " << filename
		      << std::endl;
    return devGeometry::EFAILURE;
  }

  getline( ifs, line );

  while(!ifs.eof()){

    // get the 1st char of the line
    size_t first = line.find_first_not_of(" ");

    // ensure the 1st char is found (not an empty line)
    if( first != std::string::npos ){
      
      // comment line? skip the rest
      if( line.at( first ) != '#' ){

	char* array = new char[ line.size()+1 ];  // create an array
	strcpy( array, line.data() );             // copy the string
	
	char *token = strtok(array, " ");

	if( token != NULL ){
	  
	  // Vertex data
	  if( strcmp( token, "v" ) == 0 ){
	    std::istringstream linestream(line);
	    double x, y, z, w=1.0;
	    char v;
	    linestream >> v >> x >> y >> z >> w;
	    vertices.push_back( vctFixedSizeVector<double,3>(x, y, z) );
	  }
	  
	  if( strcmp( token, "vt" ) == 0 ){}
	  if( strcmp( token, "vn" ) == 0 ){}
	  if( strcmp( token, "vp" ) == 0 ){}
	  if( strcmp( token, "cstype" ) == 0 ){}
	  if( strcmp( token, "deg" ) == 0 ){}
	  if( strcmp( token, "bmat" ) == 0 ){}
	  if( strcmp( token, "step" ) == 0 ){}
	  
	  // Elements
	  if( strcmp( token, "p" ) == 0 ){}
	  if( strcmp( token, "l" ) == 0 ){}
	  if( strcmp( token, "f" ) == 0 ){

	    vctFixedSizeVector<int,3> triangle;     // holds the vertex indices
	    size_t n=0;

	    char* triplet = strtok(NULL, " ");
	    while( triplet ){
	      int v, vt, vn;

	      // vertex, texture, normal
	      if( sscanf( triplet, "%d/%d/%d", &v, &vt, &vn  ) == 3 ){
		if( n < 3 ){
		  triangle[n] = v-1;
		  n++;
		}
	      }

	      // vertex, normal
	      else if( sscanf( triplet, "%d//%d", &v, &vn ) == 2 ){
		if( n < 3 ){
		  triangle[n] = v-1;
		  n++;
		}
	      }
	      // vertex
	      else if( sscanf( triplet, "%d", &v ) == 1 ){
		if( n < 3 ){
		  triangle[n] = v-1;
		  n++;
		}
	      }

	      triplet = strtok(NULL, " ");
	    }

	    // did we got 3 vertices 
	    if( n == 3 ){
	      triangles.push_back( triangle );
	    }
	    else{
	    }

	  }
	  if( strcmp( token, "curv" ) == 0 ){}
	  if( strcmp( token, "curv2" ) == 0 ){}
	  if( strcmp( token, "surf" ) == 0 ){}
	  
	  // Free-form curve/surface body statements
	  if( strcmp( token, "parm" ) == 0 ){}
	  if( strcmp( token, "trim" ) == 0 ){}
	  if( strcmp( token, "hole" ) == 0 ){}
	  if( strcmp( token, "scrv" ) == 0 ){}
	  if( strcmp( token, "sp" ) == 0 ){}
	  if( strcmp( token, "end" ) == 0 ){}
	  
	  // Connectivity between free-form surfaces
	  if( strcmp( token, "con" ) == 0 ){}
	  
	  // Grouping
	  if( strcmp( token, "g" ) == 0 ){}
	  if( strcmp( token, "s" ) == 0 ){}
	  if( strcmp( token, "mg" ) == 0 ){}
	  if( strcmp( token, "o" ) == 0 ){}
	  
	  // Display/render attributes
	  if( strcmp( token, "bevel" ) == 0 ){}
	  if( strcmp( token, "c_interp" ) == 0 ){}
	  if( strcmp( token, "d_interp" ) == 0 ){}
	  if( strcmp( token, "lod" ) == 0 ){}
	  if( strcmp( token, "usemtl" ) == 0 ){}
	  if( strcmp( token, "mtllib" ) == 0 ){}
	  if( strcmp( token, "shadow_obj" ) == 0 ){}
	  if( strcmp( token, "trace_obj" ) == 0 ){}
	  if( strcmp( token, "ctech" ) == 0 ){}
	  if( strcmp( token, "stech" ) == 0 ){}
	}
	delete[] array;
	
      }
    }
    getline( ifs, line );
  }
  
  // Allocate the memory
  AllocateMemory( vertices.size(), triangles.size() );

  // copy the vertex coordinates from the vectors
  for(size_t i=0; i<nvertices; i++){
    vx[i] = vertices[i][0];
    vy[i] = vertices[i][1];
    vz[i] = vertices[i][2];
  }

  // copty the facets vertices index and compute the normals
  for(size_t i=0; i<ntriangles; i++){
    // copy the index
    p1[i] = triangles[i][0];
    p2[i] = triangles[i][1];
    p3[i] = triangles[i][2];

    // compute the normal
    vctFixedSizeVector<double,3> v1 = vertices[ triangles[i][0] ];
    vctFixedSizeVector<double,3> v2 = vertices[ triangles[i][1] ];
    vctFixedSizeVector<double,3> v3 = vertices[ triangles[i][2] ];
    vctFixedSizeVector<double,3> n = (v2-v1)%(v3-v1);
    
    // watch out sometimes the facet degenerates to a line
    try{ n.NormalizedSelf(); }
    catch( std::exception )
      {n = vctFixedSizeVector<double,3>(0.0, 0.0, 1.0);}

    nx[i] = n[0];
    ny[i] = n[1];
    nz[i] = n[2];
  }

  return devGeometry::ESUCCESS;
}



