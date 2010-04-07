#include <cisstDevices/glut/devMeshTriangular.h>
#include <cisstDevices/ode/devODEBody.h>
#include <cisstVector/vctMatrixRotation3.h>

devODEBody::devODEBody( dWorldID worldid, 
			dSpaceID spaceid,
			const vctFrame4x4<double>& Rtwb,
			double m,
			const vctFixedSizeVector<double,3>& tbcom,
			const vctFixedSizeMatrix<double,3,3>& moit,
			const std::string& geomfile,
			bool glutgeom ) :
  mass(NULL),
  triangles(NULL),
  ntriangles(0),
  vertices(NULL),
  nvertices(0),
  geometry(NULL) {

  // Create the ode body
  bodyid = dBodyCreate( worldid );

  // Create and configure the mass 
  this->mass = new dMass;
  dMassSetParameters( mass,                                // 
		      m,                                   // mass
		      0.0, 0.0, 0.0,                       // center of mass
		      moit[0][0], moit[1][1], moit[2][2],  // tensor
		      moit[0][1], moit[0][2], moit[1][2] );
  // set the mass of the body
  dBodySetMass( BodyID(), this->mass );

  // Configure the geometry
  // Ensure an .obj file is given
  if( !geomfile.empty() ){

    // Load the mesh
    LoadOBJ( geomfile, tbcom );

    // Create the ODE data object
    this->meshid = dGeomTriMeshDataCreate();

    // Build the mesh data for ODE
    dGeomTriMeshDataBuildSimple( this->meshid,
				 (const dReal*)this->vertices,
				 this->nvertices,
				 this->triangles,
				 this->ntriangles );
    // Create the geom
    this->geomid = dCreateTriMesh( spaceid, this->meshid, NULL, NULL, NULL);

    // dBodySetPosition( BodyID(), tbcom[0], tbcom[1], tbcom[2] );
    // Attach the geom to the body
    dGeomSetBody( GeomID(), BodyID() );

    // Add a mesh for GLUT if we need one
    if( glutgeom ){
      this->geometry = new devMeshTriangular;
      this->geometry->LoadOBJ( geomfile );
      devGLUT::Register( (devGeometry*)(this->geometry) );
    }
  }

  // This is the center of mass. ODE requires that the coordinate frame of the
  // body be a its center of mass. Thus, we must shift the position of the body
  // to its center of mass.

  // Center of mass with respect to the body coordinate frame...
  vctFrame4x4<double> Rtbcom( vctMatrixRotation3<double>(), tbcom );
  this->Rtcomb = Rtbcom;
  
  // ...We actually want to remember its inverse...
  this->Rtcomb.InverseSelf();

  // Center of mass wrt to the world frame
  vctFrame4x4<double> Rtwcom = Rtwb * Rtbcom;

  // set the body position
  dBodySetPosition( BodyID(), Rtwcom[0][3], Rtwcom[1][3], Rtwcom[2][3] );

  // get the orientation of the body
  dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		 Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		 Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };

  // set the orientation
  dBodySetRotation( BodyID(), R );

}

devODEBody::~devODEBody(){
  if( triangles != NULL ) delete[] triangles;
  if( vertices != NULL )  delete[] vertices;
}

void devODEBody::Update(){
  if( geometry != NULL ){
    vctMatrixRotation3<double> Rwb = GetOrientation();
    vctFixedSizeVector<double,3> twb = GetPosition();
    geometry->devGeometry::SetPositionOrientation(vctFrame4x4<double>(Rwb,twb));
  }
}

vctMatrixRotation3<double> devODEBody::GetOrientation() const{
  const dReal* Rwcom = dBodyGetRotation( BodyID() );
  return vctMatrixRotation3<double> ( Rwcom[0], Rwcom[1], Rwcom[2], // R[3],
				      Rwcom[4], Rwcom[5], Rwcom[6], // R[7],
				      Rwcom[8], Rwcom[9], Rwcom[10],// R[11],
				      VCT_NORMALIZE );
}

vctFixedSizeVector<double,3> devODEBody::GetPosition() const{
  const dReal* t = dBodyGetPosition( BodyID() );
  //return vctFixedSizeVector<double,3>( t[0], t[1], t[2] );
  // The position of the center of mass in the wrt world
  vctFixedSizeVector<double,3> twcom( t[0], t[1], t[2] );

  // The orientation of the center of mass
  vctMatrixRotation3<double> Rwcom = GetOrientation();

  // The position and orientation of the com wrt to the world frame
  vctFrame4x4<double> Rtwcom( Rwcom, twcom );

  vctFrame4x4<double> Rtwb = Rtwcom * this->Rtcomb;

  // return the position of the body: position of the com - (com in body frame) 
  return vctFixedSizeVector<double,3>( Rtwb[0][3], Rtwb[1][3], Rtwb[2][3] );

}

int devODEBody::LoadOBJ( const std::string& objfilename,
			 const vctFixedSizeVector<double,3>& com ){

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
	    else{ }

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
  triangles = new dTriIndex[ ntriangles*3 ];

  // copy the vertex coordinates from the vectors
  for(size_t i=0; i<nvertices; i++){
    vertices[i][0] = tmpvertices[i][0] - com[0];
    vertices[i][1] = tmpvertices[i][1] - com[1];
    vertices[i][2] = tmpvertices[i][2] - com[2];
  }

  // copy the indices (3 indices/triangle)
  for( size_t i=0; i<ntriangles; i++ ){
    triangles[i*3+0] = (dTriIndex)tmptriangles[i][2];
    triangles[i*3+1] = (dTriIndex)tmptriangles[i][1];
    triangles[i*3+2] = (dTriIndex)tmptriangles[i][0];
  }

  return 0;
}

