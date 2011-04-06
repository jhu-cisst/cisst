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

#include <osgDB/ReadFile> 
#include <osg/PolygonMode>
#include <osg/Material>

#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// This is called at each update traversal
void devOSGBody::TransformCallback::operator()( osg::Node* node, 
						osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGBody::UserData* userdata;
  userdata = dynamic_cast<devOSGBody::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetBody()->Transform(); }
  traverse( node, nv );
}   

// This is called at each update traversal
void devOSGBody::SwitchCallback::operator()( osg::Node* node, 
					     osg::NodeVisitor* nv ){
  osg::Referenced* data = node->getUserData();
  devOSGBody::UserData* userdata;
  userdata = dynamic_cast<devOSGBody::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetBody()->Switch(); }
  traverse( node, nv );
}   


devOSGBody::devOSGBody(	const std::string& name,
			const vctFrame4x4<double>& Rt,
			const std::string& model, 
			devOSGWorld* world,
			const std::string& transformfn,
			const std::string& switchfn ) : 
  mtsComponent( name ),
  Rt_body( Rt ),
  switch_body( true ){
  
  setDataVariance( osg::Object::DYNAMIC );
  
  // Setup the user data for this body. This can be used to recover the body
  // from callbacks
  userdata = new devOSGBody::UserData( this );

  // Configure the (transform) node
  this->setUserData( userdata );
  this->setUpdateCallback( new devOSGBody::TransformCallback );

  /*
  osg::StateSet* state = this->getOrCreateStateSet();
  osg::ref_ptr<osg::Material> mat = new osg::Material;
  mat->setDiffuse( osg::Material::FRONT,
		   osg::Vec4( 0.f, 0.f, 0.f, 1.f ) );
  mat->setSpecular( osg::Material::FRONT,
		    osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );
  mat->setShininess( osg::Material::FRONT, 128.f );
  state->setAttribute( mat.get() );
  */

  // Create and configure the switch node
  osgswitch = new osg::Switch();
  osgswitch->setUserData( userdata );
  osgswitch->setUpdateCallback( new devOSGBody::SwitchCallback );

  // add the switch as the child of the transform node
  this->addChild( osgswitch );

  ReadModel( model );

  if( world != NULL )
    { world->addChild( this ); }
  
  CreateInterface( transformfn, switchfn );

  SetMatrix( Rt );

}

devOSGBody::devOSGBody(	const std::string& name,
			const vctFrm3& Rt,
			const std::string& model, 
			devOSGWorld* world,
			const std::string& transformfn,
			const std::string& switchfn ) : 
  mtsComponent( name ),
  Rt_body( Rt.Rotation(), Rt.Translation() ),
  switch_body( true ){
  
  setDataVariance( osg::Object::DYNAMIC );
  
  // Setup the user data for this body. This can be used to recover the body
  // from callbacks
  userdata = new devOSGBody::UserData( this );

  // Configure the (transform) node
  this->setUserData( userdata );
  this->setUpdateCallback( new devOSGBody::TransformCallback );

  // Create and configure the switch node
  osgswitch = new osg::Switch();
  osgswitch->setUserData( userdata );
  osgswitch->setUpdateCallback( new devOSGBody::SwitchCallback );

  // add the switch as the child of the transform node
  this->addChild( osgswitch );

  ReadModel( model );

  if( world != NULL )
    { world->addChild( this ); }

  CreateInterface( transformfn, switchfn );

  SetMatrix( vctFrame4x4<double>( Rt.Rotation(), Rt.Translation() ) );

}

devOSGBody::devOSGBody(	const std::string& name,
			const vctFrm3& Rt,
			const vctDynamicMatrix<double>& pc,
			devOSGWorld* world,
			unsigned char r, unsigned char g, unsigned char b,
			const std::string& transformfn,
			const std::string& switchfn ) :
  mtsComponent( name ),
  Rt_body( Rt.Rotation(), Rt.Translation() ),
  switch_body( true ){

  setDataVariance( osg::Object::DYNAMIC );
  
  // Setup the user data for this body. This can be used to recover the body
  // from callbacks
  userdata = new devOSGBody::UserData( this );

  // Configure the (transform) node
  this->setUserData( userdata );
  this->setUpdateCallback( new devOSGBody::TransformCallback );

  // Create and configure the switch node
  osgswitch = new osg::Switch();
  osgswitch->setUserData( userdata );
  osgswitch->setUpdateCallback( new devOSGBody::SwitchCallback );

  // add the switch as the child of the transform node
  this->addChild( osgswitch );

  Read3DData( pc, vctFixedSizeVector<unsigned char, 3>(r, g, b) );

  if( world != NULL )
    { world->addChild( this ); }
 
  CreateInterface( transformfn, switchfn );

  SetMatrix( vctFrame4x4<double>( Rt.Rotation(), Rt.Translation() ) );

}


devOSGBody::~devOSGBody(){}

void devOSGBody::CreateInterface( const std::string& transformfn,
				  const std::string& switchfn ){
  mtsInterfaceRequired* required;
  required = AddInterfaceRequired( "Transformation", MTS_OPTIONAL );
  if( required != NULL ){ 
    if( !transformfn.empty() )
      { required->AddFunction( transformfn, ReadTransformation ); }
    if( !switchfn.empty() )
      { required->AddFunction( transformfn, ReadSwitch ); }
  }
}

void devOSGBody::ReadModel( const std::string& model ){

  osg::ref_ptr< osgDB::ReaderWriter::Options > options;
  // "noRotation" is to cancel the default -X in .obj files
  options = new osgDB::ReaderWriter::Options("noRotation");

  std::string path;
  size_t found;
#if (CISST_OS == CISST_WINDOWS)
#else
  found = model.rfind( '/' );
#endif    

  if( found != std::string::npos )
    { path.assign( model, 0, found ); }
  options->setDatabasePath( path );

  osg::ref_ptr<osg::Node> node = osgDB::readNodeFile( model, options );

  if( node != NULL ){
    // Add the node to the transformation node
    osgswitch->addChild( node );
  }
  else{
    CMN_LOG_RUN_ERROR
      << CMN_LOG_DETAILS
      << "Failed to create node from file: " << model << std::endl;
  }

}

void devOSGBody::Read3DData( const vctDynamicMatrix<double>& pc,
			     const vctFixedSizeVector<unsigned char,3>& RGB ){

  size_t npoints= 0;
  if( pc.rows() == 3 )
    { npoints = pc.cols(); }
  else{
    // invalid data
  }
  
  // First, create a geode
  osg::ref_ptr<osg::Geode> geode;
  try{ geode = new osg::Geode; }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create a geode." 
		      << std::endl;
  }
  osgswitch->addChild( geode );

  // then create a geometry
  osg::ref_ptr<osg::Geometry> pointsGeom;
  try{ pointsGeom = new osg::Geometry; }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create a points geometry." 
		      << std::endl;
  }
  // add the geometry to the geode
  geode->addDrawable( pointsGeom );

  // Create an array primitive set 
  osg::ref_ptr<osg::DrawArrays> drawArrayPoints;
  try{ drawArrayPoints = new osg::DrawArrays( osg::PrimitiveSet::POINTS ); }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create array of points."
		      << std::endl;
  }
  // add the set to the geometry
  pointsGeom->addPrimitiveSet( drawArrayPoints );

  // add a vector of vertices
  osg::ref_ptr<osg::Vec3Array> vertexData;
  try{ vertexData = new osg::Vec3Array; }
  catch( std::bad_alloc& ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to create vertices array." 
		      << std::endl;
  }
  pointsGeom->setVertexArray( vertexData );
  
  for( size_t i=0; i<npoints; i++ )
    { vertexData->push_back( osg::Vec3( pc[0][i], pc[1][i], pc[2][i] ) ); }

  osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
  colorArray->push_back( osg::Vec4( RGB[0], RGB[1], RGB[2], 1.0f ) );
  
  osg::ref_ptr< osg::TemplateIndexArray< unsigned int, 
					 osg::Array::UIntArrayType,
					 4, 1 > > colorIndexArray;
  colorIndexArray = new osg::TemplateIndexArray< unsigned int, 
						 osg::Array::UIntArrayType,
						 4, 1>;
  colorIndexArray->push_back(0);

  pointsGeom->setColorArray( colorArray );
  pointsGeom->setColorIndices(colorIndexArray);
  pointsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

  drawArrayPoints->setFirst( 0 );
  drawArrayPoints->setCount( vertexData->size() );

}

// Set the OSG transformation
void devOSGBody::SetMatrix( const vctFrame4x4<double>& Rt ){
  this->setMatrix( osg::Matrix ( Rt[0][0], Rt[1][0], Rt[2][0], 0.0,
				 Rt[0][1], Rt[1][1], Rt[2][1], 0.0,
				 Rt[0][2], Rt[1][2], Rt[2][2], 0.0,
				 Rt[0][3], Rt[1][3], Rt[2][3], 1.0 ) );
}

// This is called from the body's callback
// This reads a transformation if the body is connected to an interface
void devOSGBody::Transform(){
  // Get the transformation if possible
  if( ReadTransformation.IsValid() ){
    mtsDoubleFrm4x4 Rt;
    ReadTransformation( Rt );
    SetMatrix( vctFrame4x4<double>(Rt) );
  }
  else
    { SetMatrix( Rt_body ); }
}

void devOSGBody::Switch(){
  // Get the transformation if possible
  if( ReadTransformation.IsValid() ){
    mtsBool mtsswitch;
    ReadTransformation( mtsswitch );
    osgswitch->setValue( 0, mtsswitch );
  }
  else
    { osgswitch->setValue( 0, switch_body ); }
}

void devOSGBody::SetTransform( const vctFrame4x4<double>& Rt )
{ this->Rt_body = Rt; }

void devOSGBody::SetTransform( const vctFrm3& Rt )
{ SetTransform( vctFrame4x4<double>( vctMatrixRotation3<double>( Rt.Rotation(),
								 VCT_NORMALIZE),
				     Rt.Translation() ) ); }

//! Set the switch of the body
void devOSGBody::SetSwitch( bool onoff )
{ this->switch_body = onoff; }
  
void devOSGBody::SetModeFill(){
  osg::ref_ptr<osg::StateSet> state = getOrCreateStateSet();
  osg::ref_ptr<osg::PolygonMode> pm;
  pm = dynamic_cast<osg::PolygonMode*>
    ( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
  if( !pm ){
    pm = new osg::PolygonMode;
    state->setAttribute( pm );
  }
  pm->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
}

void devOSGBody::SetModeLine(){
  osg::ref_ptr<osg::StateSet> state = getOrCreateStateSet();
  osg::ref_ptr<osg::PolygonMode> pm;
  pm = dynamic_cast<osg::PolygonMode*>
    ( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
  if( !pm ){
    pm = new osg::PolygonMode;
    state->setAttribute( pm );
  }
  pm->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
}

void devOSGBody::SetModePoint(){
  osg::ref_ptr<osg::StateSet> state = getOrCreateStateSet();
  osg::ref_ptr<osg::PolygonMode> pm;
  pm = dynamic_cast<osg::PolygonMode*>
    ( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
  if( !pm ){
    pm = new osg::PolygonMode;
    state->setAttribute( pm );
  }
  pm->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT );
}


    /*
    // This blocks gets the geometry out of the node
    // This is how it "should" work: First cast the node as a group
    osg::Group* group = node->asGroup();
    if( group != NULL ){

      for( size_t g = 0; g<group->getNumChildren(); g++ ){

	node = group->getChild( g );

	// This node should be a geode
	osg::Geode* geode = node->asGeode();
	if( geode != NULL ){

	  // Find if it has any drawables?
	  for( size_t d=0; d<geode->getNumDrawables(); d++ ){
	    // Get the first drawable
	    osg::Drawable* drawable = geode->getDrawable( d );
	  
	    // Cast the drawable as a geometry
	    osg::Geometry* geometry = drawable->asGeometry();
	    if( geometry == NULL ){
	      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
				<< "Failed to cast the drawable as a geometry."
				<< std::endl;
	    }
	    else { osggeometries.push_back( geometry ); }

	  }
	}
	else{
	  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			    << "Failed to cast node as a geode for : " << model 
			    << std::endl;
	}
      }
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< "Failed to cast node as a group for : " << model 
			<< std::endl;
    }
    */
