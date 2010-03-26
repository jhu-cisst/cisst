#include <cisstDevices/ode/devODEBody.h>
#include <cisstVector/vctMatrixRotation3.h>

devODEBody::devODEBody( dWorldID worldid, 
			dSpaceID spaceid,
			const vctFrame4x4<double>& Rt,
			double m,
			const vctFixedSizeVector<double,3>& tbcom,
			const vctFixedSizeMatrix<double,3,3>& moit,
			const std::string& geomfile ) :
  triangles(NULL),
  ntriangles(0),
  vertices(NULL),
  nvertices(0) {

  /**** Configure the body ****/
  // create the ode body
  bodyid = dBodyCreate( worldid );
  this->tbcom = tbcom;

  // the position of the body frame wrt world frame
  vctFixedSizeVector<double,3> twb( Rt[0][3], Rt[1][3], Rt[2][3] );

  // the orientation of body frame wrt world frame
  vctMatrixRotation3<double> Rwb( Rt[0][0], Rt[0][1], Rt[0][2],
				  Rt[1][0], Rt[1][1], Rt[1][2],
				  Rt[2][0], Rt[2][1], Rt[2][2] );
  
  // position of center of mass wrt world frame
  vctFixedSizeVector<double,3> twcom( twb + Rwb*tbcom );

  // set the body position
  dBodySetPosition( BodyID(), twcom[0], twcom[1], twcom[2] );

  // get the orientation of the body
  dMatrix3 r = { Rt[0][0], Rt[0][1], Rt[0][2], 0.0,
		 Rt[1][0], Rt[1][1], Rt[1][2], 0.0,
		 Rt[2][0], Rt[2][1], Rt[2][2], 0.0 };

  // set the orientation
  dBodySetRotation( bodyid, r );

  /**** Configure the mass ****/

  dMassSetParameters( &mass,                               // 
		      m,                                   // mass
		      0.0, 0.0, 0.0,                       // center of mass
		      moit[0][0], moit[1][1], moit[2][2],  // tensor
		      moit[0][1], moit[0][2], moit[1][2] );
  
  // set the mass
  dBodySetMass( BodyID(), &mass );

  /**** Configure the geometry ****/
  if( !geomfile.empty() ){

    // load the mesh
    LoadOBJ( geomfile );

    // create the ODE data object
    meshid = dGeomTriMeshDataCreate();

    // build the data
    dGeomTriMeshDataBuildSimple( meshid,
				 (const dReal*)vertices, nvertices,
				 (const dTriIndex*)triangles, ntriangles );
  
    // create the geom
    geom = dCreateTriMesh( spaceid, meshid, NULL, NULL, NULL);
    
  }

}

devODEBody::~devODEBody(){
  if( triangles != NULL ) delete[] triangles;
  if( vertices != NULL ) delete[] vertices;
}


int devODEBody::LoadOBJ( const std::string& objfilename ){

  std::string line;       // used to parse lines of the obj file
  std::ifstream ifs;      // 

  // we're going to accumulate the vertices and triangles in these vectors
  std::vector< vctFixedSizeVector<double,3> > tmpvertices;
  std::vector< vctFixedSizeVector<int,3> > tmptriangles;

  // open the file name
  ifs.open(objfilename.data());
  if(!ifs){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Couldn't open file " << objfilename
		      << std::endl;
    return 1;
  }

  // copy the first line in the string
  getline( ifs, line );

  // until we reach the end of the file
  while(!ifs.eof()){

    // get the 1st (non-space) character of the line
    size_t first = line.find_first_not_of(" ");

    // ensure the 1st character is found (not an empty line)
    if( first != std::string::npos ){
      
      // ensure the line isn't a comment
      if( line.at( first ) != '#' ){

	// we'll tokenize the string
	char* array = new char[ line.size()+1 ];  // create an array
	strcpy( array, line.data() );             // copy the string
	
	char *token = strtok(array, " ");

	if( token != NULL ){
	  
	  // Is the token a vertex?
	  if( strcmp( token, "v" ) == 0 ){
	    std::istringstream linestream(line);
	    double x, y, z, w=1.0;   // the coordinates
	    char v;                  // the "v" character
	    linestream >> v >> x >> y >> z >> w;
	    tmpvertices.push_back( vctFixedSizeVector<double,3>(x, y, z) );
	  }
	  
	  // Is the token a facet?
	  if( strcmp( token, "f" ) == 0 ){

	    vctFixedSizeVector<int,3> triangle;   // holds the vertex indices
	    size_t n=0;                           // the number of vertices

	    char* triplet = strtok(NULL, " ");
	    while( triplet ){
	      int v, vt, vn;

	      // vertex + texture + normal
	      if( sscanf( triplet, "%d/%d/%d", &v, &vt, &vn  ) == 3 ){
		if( n < 3 ){
		  triangle[n] = v-1;
		  n++;
		}
	      }

	      // vertex + normal
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

	      triplet = strtok(NULL, " ");  // go to the next triplet
	    }

	    // did we got 3 vertices 
	    if( n == 3 ){
	      tmptriangles.push_back( triangle );
	    }
	    else{ /** error **/ }

	  }

	}
	delete[] array;
	
      }
    }
    getline( ifs, line );
  }

  nvertices = tmpvertices.size();
  ntriangles = tmptriangles.size();

  vertices = new dVector3[ nvertices ];
  triangles = new size_t[ ntriangles*3 ];

  // copy the vertex coordinates from the vectors
  for(size_t i=0; i<nvertices; i++){
    vertices[i][0] = tmpvertices[i][0];
    vertices[i][1] = tmpvertices[i][1];
    vertices[i][2] = tmpvertices[i][2];
  }

  // copy the indices (3 indices/triangle)
  for( size_t i=0; i<ntriangles; i++ ){
    triangles[i*3+0] = tmptriangles[i][0];
    triangles[i*3+1] = tmptriangles[i][1];
    triangles[i*3+2] = tmptriangles[i][2];
  }

  return 0;
}

/*
vctMatrixRotation3<double> devODEBody::GetOrientation() const{
  const dReal* R = dBodyGetRotation( BodyID() );
  return vctMatrixRotation3<double> ( R[0], R[1], R[2], // R[3],
				      R[4], R[5], R[6], // R[7],
				      R[8], R[9], R[10], VCT_NORMALIZE );
}

vctFixedSizeVector<double,3> devODEBody::GetPosition() const{
  // The position of the center of mass in the wrt world
  const dReal* t = dBodyGetPosition( BodyID() );
  vctFixedSizeVector<double,3> twcom( t[0], t[1], t[2] );

  // The orientation of the body wrt world
  vctMatrixRotation3<double> Rwb = GetOrientation();

  // return the position of the body: position of the com - (com in body frame) 
  return twcom - Rwb*tbcom;

}
*/
