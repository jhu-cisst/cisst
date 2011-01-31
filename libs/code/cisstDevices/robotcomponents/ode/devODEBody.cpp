#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstVector/vctMatrixRotation3.h>


devODEBody::devODEBody( const std::string& name,
			const vctFrm3& Rt,
			const std::string& model,
			devODEWorld* world ) :
  devOSGBody( name, Rt, model, world ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  if( world != NULL ){

    vctFrame4x4<double> Rtwb( Rt.Rotation(), Rt.Translation() );

    world->Lock();

    BuildODETriMesh( world->GetSpaceID(), vctFixedSizeVector<double,3>( 0.0 ) );

    // set the body position
    dGeomSetPosition( GetGeomID(), Rtwb[0][3], Rtwb[1][3], Rtwb[2][3] );
    
    // get the orientation of the body
    dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		   Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		   Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };
    
    // set the orientation
    dGeomSetRotation( GetGeomID(), R );
    
    world->Unlock();
  }
}

devODEBody::devODEBody( const std::string& name,
		        const vctFrame4x4<double>& Rtwb,
			const std::string& model,
			devODEWorld* world,
			dSpaceID spaceid ) : 
  devOSGBody( name, Rtwb, model, world ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  if( world != NULL ){

    world->Lock();

    BuildODETriMesh( spaceid, vctFixedSizeVector<double,3>( 0.0 ) );

    // set the body position
    dGeomSetPosition( GetGeomID(), Rtwb[0][3], Rtwb[1][3], Rtwb[2][3] );
    
    // get the orientation of the body
    dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		   Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		   Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };
    
    // set the orientation
    dGeomSetRotation( GetGeomID(), R );
    
    world->Unlock();
  }
}

devODEBody::devODEBody( const std::string& name,
		        const vctFrame4x4<double>& Rtwb,
			const std::string& model,
			devODEWorld* world, 
			double m,
			const vctFixedSizeVector<double,3>& tbcom,
			const vctFixedSizeMatrix<double,3,3>& moit,
			dSpaceID spaceid ) :

  devOSGBody( name, Rtwb, model, world ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  if( world != NULL ){

    // Ensure that the world isn't turning
    world->Lock();

    // Create the ode body
    bodyid = dBodyCreate( world->GetWorldID() );

    dBodySetData( GetBodyID(), (void*)this );

    // Create and configure the mass 
    this->mass = new dMass;
    //dMassSetBoxTotal (mass, 1, 0.1, 0.1, 0.1 );
    dMassSetParameters( mass,                                // 
			m,                                   // mass
			0.0, 0.0, 0.0,                       // center of mass
			moit[0][0], moit[1][1], moit[2][2],  // tensor
			moit[0][1], moit[0][2], moit[1][2] );

    // set the mass of the body
    dBodySetMass( GetBodyID(), this->mass );

    // Build the mesh data
    BuildODETriMesh( spaceid, tbcom );

    // This is the center of mass. ODE requires that the coordinate frame of the
    // body be a its center of mass. Thus, we must shift the position of the
    // body to its center of mass.
    
    // Center of mass with respect to the body coordinate frame...
    vctFrame4x4<double> Rtbcom( vctMatrixRotation3<double>(), tbcom );
    this->Rtcomb = Rtbcom;
    
    // ...We actually want to remember its inverse...
    this->Rtcomb.InverseSelf();
    
    // Center of mass wrt to the world frame
    vctFrame4x4<double> Rtwcom = Rtwb * Rtbcom;
    
    // set the body position
    dBodySetPosition( GetBodyID(), Rtwcom[0][3], Rtwcom[1][3], Rtwcom[2][3] );
    
    // get the orientation of the body
    dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		   Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		   Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };
    
    // set the orientation
    dBodySetRotation( GetBodyID(), R );
    
    // Ensure that the world isn't turning
    world->Unlock();
  }

}

devODEBody::devODEBody( const std::string& name,
			const vctFrm3& Rt,
			const std::string& model,
			devODEWorld* world, 
			double m ) :

  devOSGBody( name, Rt, model, world ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  if( world != NULL ){

    vctFrame4x4<double> Rtwb( Rt.Rotation(), Rt.Translation() );
    vctFixedSizeVector<double,3> tbcom( 0.0 );
    vctFixedSizeMatrix<double,3,3> moit = vctFixedSizeMatrix<double,3,3>::Eye();

    // Ensure that the world isn't turning
    world->Lock();

    // Create the ode body
    bodyid = dBodyCreate( world->GetWorldID() );

    dBodySetData( GetBodyID(), (void*)this );

    // Create and configure the mass 
    this->mass = new dMass;
    //dMassSetBoxTotal (mass, 1, 0.1, 0.1, 0.1 );
    dMassSetParameters( mass,                                // 
			m,                                   // mass
			0.0, 0.0, 0.0,                       // center of mass
			moit[0][0], moit[1][1], moit[2][2],  // tensor
			moit[0][1], moit[0][2], moit[1][2] );

    // set the mass of the body
    dBodySetMass( GetBodyID(), this->mass );

    // Build the mesh data
    BuildODETriMesh( world->GetSpaceID(), tbcom );

    // This is the center of mass. ODE requires that the coordinate frame of the
    // body be a its center of mass. Thus, we must shift the position of the
    // body to its center of mass.
    
    // Center of mass with respect to the body coordinate frame...
    vctFrame4x4<double> Rtbcom( vctMatrixRotation3<double>(), tbcom );
    this->Rtcomb = Rtbcom;
    
    // ...We actually want to remember its inverse...
    this->Rtcomb.InverseSelf();
    
    // Center of mass wrt to the world frame
    vctFrame4x4<double> Rtwcom = Rtwb * Rtbcom;
    
    // set the body position
    dBodySetPosition( GetBodyID(), Rtwcom[0][3], Rtwcom[1][3], Rtwcom[2][3] );

    // get the orientation of the body
    dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		   Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		   Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };
    
    // set the orientation
    dBodySetRotation( GetBodyID(), R );
    
    // Ensure that the world isn't turning
    world->Unlock();
  }

}

devODEBody::~devODEBody(){
  world->Lock();

  /* 
  if( Vertices != NULL ){ delete[] Vertices; }
  if( Indices != NULL ) { delete[] Indices;  }
  if( mass != NULL )    { delete   mass;     }

  
  if( GetBodyID() != 0 ){ dBodyDestroy( GetBodyID() ); }
  dGeomTriMeshDataDestroy( this->meshid );
  dGeomDestroy( GetGeomID() );
  */

  world->Unlock();

}

void devODEBody::BuildODETriMesh( dSpaceID spaceid,
				  const vctFixedSizeVector<double,3>& com ){

  // used to accumulate the vertices of all geometries
  std::vector< vctFixedSizeVector<double,3> > geometriesvertices;
  // used to accumulate the vertices per geometry
  std::vector< int > geometriesverticescount;
  // used to accumulate the indices
  std::vector<dTriIndex> geometriesindices;

  // start a 0 vertex
  geometriesverticescount.push_back( 0 );

  for( size_t i=0; i<geometries.size(); i++ ){
    
    // Get the OSG vertex array of the geometry
    const osg::Array* vertexarray = geometries[i]->getVertexArray();

    // This part copies the vertices of a geometry in a temporary buffer
    // Ensure we are dealing with an array of 3d points
    if( vertexarray->getType() == osg::Array::Vec3ArrayType ){

      // Ensure that the array is GL_FLOAT
      if( vertexarray->getDataType() == GL_FLOAT ){
	
	// Copy the vertices
	GLfloat* fvertices = (GLfloat*)vertexarray->getDataPointer();
	for( size_t j=0, idx=0; j<vertexarray->getNumElements(); j++){
	  vctFixedSizeVector<double,3> vertex;
	  vertex[0] = fvertices[idx++] - com[0]; 
	  vertex[1] = fvertices[idx++] - com[1]; 
	  vertex[2] = fvertices[idx++] - com[2]; 
	  geometriesvertices.push_back( vertex );
	}
	// Copy the vertex count for this geometry
	geometriesverticescount.push_back( vertexarray->getNumElements()/3 );
      }

    }
    
    // Copy the indices in a temporary array
    // Process all the primitives in the geometry
    for( size_t j=0; j<geometries[i]->getNumPrimitiveSets(); j++ ){
      
      // get the ith primitive set
      const osg::PrimitiveSet* primitiveset = geometries[i]->getPrimitiveSet( j );

      // test that the primitive are triangles
      if( primitiveset->getMode() == osg::PrimitiveSet::TRIANGLES || 
	  primitiveset->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP ){

	// copy the indices. Add the vertex count of the previous geometry. This
	// is because the indices are numbered wrt to vertices of the geometry
	for( unsigned int k=0; k<primitiveset->getNumIndices(); k++ ){ 
	  dTriIndex idx = (dTriIndex)primitiveset->index( k );// vertex index wrt geometry
	  idx += geometriesverticescount[ i ];                // vertex absolute index
	  geometriesindices.push_back( idx );
	}
      }
    }
  }

  // Create the array for ODE and copy the data
  VertexCount =  geometriesvertices.size();
  Vertices  = new dVector3[ VertexCount ];
  IndexCount = geometriesindices.size();
  Indices = new dTriIndex[ IndexCount ];

  for( int i=0; i<VertexCount; i++ ){
    Vertices[i][0] = geometriesvertices[i][0];
    Vertices[i][1] = geometriesvertices[i][1];
    Vertices[i][2] = geometriesvertices[i][2];
  }

  for( int i=0; i<IndexCount; i++ ){
    Indices[i] = geometriesindices[i];
  }

  if( 0 < IndexCount && 0 < VertexCount ){

    // Build the mesh data for ODE
    this->meshid = dGeomTriMeshDataCreate();

    // Build the mesh
    dGeomTriMeshDataBuildSimple( this->meshid, 
				 (const dReal*)Vertices, VertexCount,
				 Indices, IndexCount );
  
    // Create the geom
    this->geomid = dCreateTriMesh( spaceid, this->meshid, NULL, NULL, NULL);
    dGeomSetData( GetGeomID(), (void*)this );
  
    // Attach the geom to the body
    dGeomSetBody( GetGeomID(), GetBodyID() );
  
  }

}

void devODEBody::Enable(){ 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
  dBodyEnable( GetBodyID() ); 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
}
void devODEBody::Disable(){ 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
  dBodyDisable( GetBodyID() ); 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
}

// This is different from the devOSGBody::Update since it gets the position/orientation
// from ODE world and not from MTS
void devODEBody::Update(){
  vctMatrixRotation3<double> R = GetOrientation();
  vctFixedSizeVector<double,3> t = GetPosition();
  SetMatrix( vctFrame4x4<double>(R,t) );
}

vctMatrixRotation3<double> devODEBody::GetOrientation() const{
  if( GetGeomID() != 0 ){
    const dReal* Rwcom = dGeomGetRotation( GetGeomID() );
    return vctMatrixRotation3<double> ( Rwcom[0], Rwcom[1], Rwcom[2], // R[3],
					Rwcom[4], Rwcom[5], Rwcom[6], // R[7],
					Rwcom[8], Rwcom[9], Rwcom[10],// R[11],
					VCT_NORMALIZE );
  }
  return vctMatrixRotation3<double>();
}

vctFixedSizeVector<double,3> devODEBody::GetPosition() const{
  if( GetGeomID() != 0 ){

    const dReal* t = dGeomGetPosition( GetGeomID() );
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
  return vctFixedSizeVector<double,3>( 0.0 );
}

