#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstVector/vctMatrixRotation3.h>

devODEBody::OSGCallback::OSGCallback(){}

void devODEBody::OSGCallback::operator()( osg::Node* node, 
					  osg::NodeVisitor* nv ){
      
  devODEBody* body = dynamic_cast<devODEBody*>(node);
  
  if( body != NULL ){
    // set the position and orientation of the OSG body
    vctFixedSizeVector<double,3> t = body->GetPosition();
    vctMatrixRotation3<double>   R = body->GetOrientation();
    body->SetTransformation( vctFrame4x4<double>( R, t ) );
  }
  
  traverse( node, nv );
}   

devODEBody::devODEBody( const std::string& name,
		        const vctFrame4x4<double>& Rtwb,
			const std::string& model,
			dSpaceID spaceid,
			devODEWorld* world ) :

  devOSGBody( model, world, Rtwb ),
  name( name ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( NULL ){

  if( world != NULL ){
    world->Lock();
    setUpdateCallback( new OSGCallback );
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
			double m,
			const vctFixedSizeVector<double,3>& tbcom,
			const vctFixedSizeMatrix<double,3,3>& moit,
			const std::string& model,
			dSpaceID spaceid,
			devODEWorld* world ) :

  devOSGBody( model, world, Rtwb ),
  name( name ),
  world( world ),
  bodyid( 0 ),
  mass( NULL ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( NULL ){

  setUpdateCallback( new OSGCallback );

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

  // Find if a geometry was loaded
  if( geometry != NULL ){
    
    // Get the OSG vertex array
    const osg::Array* vertexarray = geometry->getVertexArray();

    // This part copies the vertices in a temporary (double) buffer
    // Ensure we are dealing with an array of 3d points
    if( vertexarray->getType() == osg::Array::Vec3ArrayType ){
      // Ensure that the array is GL_FLOAT
      if( vertexarray->getDataType() == GL_FLOAT ){
	
	// Copy the array in the buffer
	GLfloat* fvertices = (GLfloat*)vertexarray->getDataPointer();
        Vertices  = new dVector3[ vertexarray->getNumElements() ];
	VertexCount = vertexarray->getNumElements();

	for( size_t i=0, idx=0; i<vertexarray->getNumElements(); i++){
	  Vertices[i][0] = fvertices[idx++] - com[0]; 
	  Vertices[i][1] = fvertices[idx++] - com[1]; 
	  Vertices[i][2] = fvertices[idx++] - com[2]; 
	}

      }

    }
    

    const osg::Geometry::PrimitiveSetList primitivesetlist = 
      geometry->getPrimitiveSetList();

    // Copy the indices in a temporary array
    // Ensure we only have triangle primitives
    if( primitivesetlist.size() == 1 ){

      const osg::PrimitiveSet* primitiveset;
      primitiveset = geometry->getPrimitiveSet( 0 );

      // thest that the primitive are triangles
      if( primitiveset->getMode() == GL_TRIANGLES ){
	Indices = new dTriIndex[ primitiveset->getNumIndices() ];
	IndexCount = primitiveset->getNumIndices();
	for( unsigned int i=0; i<primitiveset->getNumIndices(); i++ )
	  { Indices[i] = (dTriIndex) primitiveset->index( i ); }
      }

      else{
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Illegal primitive."
			  << std::endl;
      }
    }
    else{
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Model must be triangulated."
			  << std::endl;
    }

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

vctMatrixRotation3<double> devODEBody::GetOrientation() const{
  const dReal* Rwcom = dGeomGetRotation( GetGeomID() );
  return vctMatrixRotation3<double> ( Rwcom[0], Rwcom[1], Rwcom[2], // R[3],
				      Rwcom[4], Rwcom[5], Rwcom[6], // R[7],
				      Rwcom[8], Rwcom[9], Rwcom[10],// R[11],
				      VCT_NORMALIZE );
}

vctFixedSizeVector<double,3> devODEBody::GetPosition() const{

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

