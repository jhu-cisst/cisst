#include <cisstODE/cisstODEBody.h>
#include <cisstODE/cisstODEWorld.h>
#include <osgUtil/Simplifier>

#include <cisstVector/vctMatrixRotation3.h>


cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld,
			    const vctFrame4x4<double>& Rtwb,
			    cisstOSGWorld* osgworld ) :

  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  Initialize( Rtwb );
    
}

cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld,
			    const vctFrm3& Rtwb,
			    cisstOSGWorld* osgworld ) :
			    
  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& Rwb = Rtwb.Rotation();
  vctQuaternionRotation3<double> qwb( Rwb, VCT_NORMALIZE );
  vctFixedSizeVector<double,3> twb( Rtwb.Translation() );
  Initialize( vctFrame4x4<double>( qwb, twb ) );

}





cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld, 
			    const vctFrame4x4<double>& Rtwb,
			    double m,
			    cisstOSGWorld* osgworld ) :

  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  vctFixedSizeVector<double,3> tbcom( 0.0 );
  vctFixedSizeMatrix<double,3,3> moit = vctFixedSizeMatrix<double,3,3>::Eye();
  Initialize( Rtwb, m, tbcom, moit );

}

cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld, 
			    const vctFrm3& Rtwb,
			    double m,
			    cisstOSGWorld* osgworld ) :

  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  vctFixedSizeVector<double,3> tbcom( 0.0 );
  vctFixedSizeMatrix<double,3,3> moit = vctFixedSizeMatrix<double,3,3>::Eye();

  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& Rwb = Rtwb.Rotation();
  vctQuaternionRotation3<double> qwb( Rwb, VCT_NORMALIZE );
  vctFixedSizeVector<double,3> twb( Rtwb.Translation() );
  Initialize( vctFrame4x4<double>( qwb, twb ), m, tbcom, moit );

}




cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld, 
			    const vctFrame4x4<double>& Rtwb,
			    double m,
			    const vctFixedSizeVector<double,3>& tbcom,
			    const vctFixedSizeMatrix<double,3,3>& moit,
			    cisstOSGWorld* osgworld ) :

  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){

  Initialize( Rtwb, m, tbcom, moit );

}


cisstODEBody::cisstODEBody( const std::string& model,
			    cisstODEWorld* odeworld, 
			    const vctFrm3& Rtwb,
			    double m,
			    const vctFixedSizeVector<double,3>& tbcom,
			    const vctFixedSizeMatrix<double,3,3>& moit,
			    cisstOSGWorld* osgworld ) :

  cisstOSGBody( model, (osgworld == NULL) ? odeworld : osgworld, Rtwb ),
  odeworld( odeworld ),
  bodyid( 0 ),
  mass( NULL ),
  geomid( 0 ),
  Vertices( NULL ),
  VertexCount( 0 ),
  Indices( NULL ),
  IndexCount( 0 ){
  
  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& Rwb = Rtwb.Rotation();
  vctQuaternionRotation3<double> qwb( Rwb, VCT_NORMALIZE );
  vctFixedSizeVector<double,3> twb( Rtwb.Translation() );
  Initialize( vctFrame4x4<double>( qwb, twb ), m, tbcom, moit );
  
}


cisstODEBody::~cisstODEBody(){

  if( Vertices != NULL ){ delete[] Vertices; }
  if( Indices != NULL ) { delete[] Indices;  }
  if( mass != NULL )    { delete   mass;     }
  
  if( GetBodyID() != 0 ){ dBodyDestroy( GetBodyID() ); }
  dGeomTriMeshDataDestroy( this->meshid );
  dGeomDestroy( GetGeomID() );

}

void cisstODEBody::Initialize( const vctFrame4x4<double>& Rtwb ){
  if( odeworld != NULL ){
    
    BuildODETriMesh( vctFixedSizeVector<double,3>( 0.0 ) );

    // set the body position
    dGeomSetPosition( GetGeomID(), Rtwb[0][3], Rtwb[1][3], Rtwb[2][3] );
    
    // get the orientation of the body
    dMatrix3 R = { Rtwb[0][0], Rtwb[0][1], Rtwb[0][2], 0.0,
		   Rtwb[1][0], Rtwb[1][1], Rtwb[1][2], 0.0,
		   Rtwb[2][0], Rtwb[2][1], Rtwb[2][2], 0.0 };
    
    // set the orientation
    dGeomSetRotation( GetGeomID(), R );

  }

}

void cisstODEBody::Initialize( const vctFrame4x4<double>& Rtwb,
			       double m,
			       const vctFixedSizeVector<double,3>& tbcom,
			       const vctFixedSizeMatrix<double,3,3>& moit ){

  if( odeworld != NULL ){

    // Create the ode body
    bodyid = dBodyCreate( odeworld->GetWorldID() );

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
    BuildODETriMesh( tbcom );

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

  }

}

void cisstODEBody::BuildODETriMesh( const vctFixedSizeVector<double,3>& com ){

  cisstOSGBody::GeodeVisitor gvtmp;
  this->accept( gvtmp );

  if( 1000 < gvtmp.geodetriangles.size() ){
    double ratio = 1000.0 / ( (double) gvtmp.geodetriangles.size() );
    osgUtil::Simplifier simplifier( ratio, 4.0 );
    this->accept( simplifier );
  }

  cisstOSGBody::GeodeVisitor gv;
  this->accept( gv );

  // Create the array for ODE and copy the data
  VertexCount =  gv.geodetriangles.size()*3;  // 3 vertex per triangle
  Vertices  = new dVector3[ VertexCount ];    // create the vertices vector
  IndexCount = gv.geodetriangles.size()*3;    // 3 vertex index per ttriangle
  Indices = new dTriIndex[ IndexCount ];

  // copy the data
  // ti: triangle index
  // vi: vertex index
  for( size_t ti=0, vi=0; ti<gv.geodetriangles.size(); ti++ ){

    // copy the vertices
    Vertices[vi][0] = gv.geodetriangles[ti].p1[0] - com[0];
    Vertices[vi][1] = gv.geodetriangles[ti].p1[1] - com[1];
    Vertices[vi][2] = gv.geodetriangles[ti].p1[2] - com[2];
    Indices[vi] = vi;
    vi++;

    Vertices[vi][0] = gv.geodetriangles[ti].p2[0] - com[0];
    Vertices[vi][1] = gv.geodetriangles[ti].p2[1] - com[1];
    Vertices[vi][2] = gv.geodetriangles[ti].p2[2] - com[2];
    Indices[vi] = vi;
    vi++;

    Vertices[vi][0] = gv.geodetriangles[ti].p3[0] - com[0];
    Vertices[vi][1] = gv.geodetriangles[ti].p3[1] - com[1];
    Vertices[vi][2] = gv.geodetriangles[ti].p3[2] - com[2];
    Indices[vi] = vi;
    vi++;

  }

  // Create the cisst vector matrix of vertices
  vctVertices.SetSize( 3, VertexCount );
  // copy the data
  for( size_t i=0; i<gv.geodetriangles.size(); i++ ){
    vctVertices[0][i] = Vertices[i][0];
    vctVertices[1][i] = Vertices[i][1];
    vctVertices[2][i] = Vertices[i][2];
  }

  if( 0 < IndexCount && 0 < VertexCount ){

    // Build the mesh data for ODE
    this->meshid = dGeomTriMeshDataCreate();

    // Build the mesh
    dGeomTriMeshDataBuildSimple( this->meshid, 
				 (const dReal*)Vertices, VertexCount,
				 Indices, IndexCount );
  
    // Create the geom
    this->geomid = dCreateTriMesh( odeworld->GetSpaceID(), 
				   this->meshid, 
				   NULL, NULL, NULL);
    dGeomSetData( GetGeomID(), (void*)this );
  
    // Attach the geom to the body
    dGeomSetBody( GetGeomID(), GetBodyID() );
  
  }

}

void cisstODEBody::Enable(){ 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
  dBodyEnable( GetBodyID() ); 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
}
void cisstODEBody::Disable(){ 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
  dBodyDisable( GetBodyID() ); 
  dBodySetLinearVel( GetBodyID(), 0, 0, 0);
  dBodySetAngularVel( GetBodyID(), 0, 0, 0);
}

// This is different from the cisstOSGBody::Update since it gets the 
// position/orientation from ODE world and not from MTS
void cisstODEBody::UpdateTransform(){
  vctMatrixRotation3<double> Rwb = GetOrientation();
  vctFixedSizeVector<double,3> twb = GetPosition();
  cisstOSGBody::SetTransform( vctFrame4x4<double>(Rwb,twb) );
  cisstOSGBody::UpdateTransform();
}

vctMatrixRotation3<double> cisstODEBody::GetOrientation() const{
  if( GetGeomID() != 0 ){
    const dReal* Rwcom = dGeomGetRotation( GetGeomID() );
    return vctMatrixRotation3<double> ( Rwcom[0], Rwcom[1], Rwcom[2], // R[3],
					Rwcom[4], Rwcom[5], Rwcom[6], // R[7],
					Rwcom[8], Rwcom[9], Rwcom[10],// R[11],
					VCT_NORMALIZE );
  }
  return vctMatrixRotation3<double>();
}

vctFixedSizeVector<double,3> cisstODEBody::GetPosition() const{
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


vctFrm3 cisstODEBody::GetTransform() const{
  return vctFrm3( GetOrientation(), GetPosition() );
}

/*
vctDynamicMatrix<double> cisstODEBody::GetVertices() const
{ return vctVertices; }
*/

/*
  // used to accumulate the vertices of all geometries
  std::vector< vctFixedSizeVector<double,3> > geometriesvertices;
  // used to accumulate the vertices per geometry
  std::vector< int > geometriesverticescount;
  // used to accumulate the indices
  std::vector<dTriIndex> geometriesindices;

  // start a 0 vertex
  geometriesverticescount.push_back( 0 );

  for( size_t i=0; i<osggeometries.size(); i++ ){

    // Get the OSG vertex array of the geometry
    const osg::Array* vertexarray = osggeometries[i]->getVertexArray();
    
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
	//geometriesverticescount.push_back( vertexarray->getNumElements()/3 );
	geometriesverticescount.push_back( vertexarray->getNumElements() );
      }

    }

    // Copy the indices in a temporary array
    // Process all the primitives in the geometry
    for( size_t j=0; j<osggeometries[i]->getNumPrimitiveSets(); j++ ){

      // get the ith primitive set
      const osg::PrimitiveSet* primitiveset=osggeometries[i]->getPrimitiveSet(j);
      std::cout << "mode: " << primitiveset->getMode() << std::endl;
      // test that the primitive are triangles
      if( primitiveset->getMode() == osg::PrimitiveSet::TRIANGLES || 
	  primitiveset->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP ){

	// copy the indices. Add the vertex count of the previous geometry. This
	// is because the indices are numbered wrt to vertices of the geometry
	for( unsigned int k=0; k<primitiveset->getNumIndices(); k++ ){

	  dTriIndex idx = (dTriIndex)primitiveset->index( k );// vertex index wrt geometry
	  std::cout << "idx: " << idx << std::endl;
	  idx += geometriesverticescount[ i ];                // vertex absolute index
	  geometriesindices.push_back( idx );
	}
      }
    }
  }


  for( int i=0; i<VertexCount; i++ ){
    Vertices[i][0] = geometriesvertices[i][0];
    Vertices[i][1] = geometriesvertices[i][1];
    Vertices[i][2] = geometriesvertices[i][2];
  }

  vctVertices.SetSize( 3, VertexCount );

  for( size_t i=0; i<VertexCount; i++ ){
    vctVertices[0][i] = Vertices[i][0];
    vctVertices[1][i] = Vertices[i][1];
    vctVertices[2][i] = Vertices[i][2];
  }

  for( int i=0; i<IndexCount; i++ )
    { Indices[i] = geometriesindices[i]; }


*/
