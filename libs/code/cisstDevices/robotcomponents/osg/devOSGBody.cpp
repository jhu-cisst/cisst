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
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// This is called at each update traversal
void devOSGBody::UpdateCallback::operator()( osg::Node* node, 
					   osg::NodeVisitor* nv ){
      
  osg::Referenced* data = node->getUserData();
  devOSGBody::UserData* userdata;
  userdata = dynamic_cast<devOSGBody::UserData*>( data );

  if( userdata != NULL )
    { userdata->GetBody()->Update(); }

  traverse( node, nv );

}   


devOSGBody::devOSGBody(	const std::string& name,
			const vctFrame4x4<double>& Rt,
			const std::string& model, 
			devOSGWorld* world,
			const std::string& fnname ) : 
  mtsComponent( name ){
  
  setDataVariance( osg::Object::DYNAMIC );
  
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
  
  osg::Node* node = osgDB::readNodeFile( model, options );
  if( node != NULL ){
    
    // Add the node to the transformation node
    addChild( node );

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
	    else { geometries.push_back( geometry ); }

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

  }
  else{
    CMN_LOG_RUN_ERROR
      << CMN_LOG_DETAILS
      << "Failed to create node from file: " << model << std::endl;
  }

  if( world != NULL )
    { world->addChild( this ); }
  
  // Setup the callback
  // Set callback stuff
  userdata = new devOSGBody::UserData( this );
  setUserData( userdata );
  setUpdateCallback( new devOSGBody::UpdateCallback );

  // MTS stuff
  if( !fnname.empty() ){
    CMN_LOG_INIT_VERBOSE << name << " " << fnname << std::endl;
    mtsInterfaceRequired* required;
    required = AddInterfaceRequired( "Transformation", MTS_OPTIONAL );
    if( required != NULL )
      { required->AddFunction( fnname, ReadTransformation ); }

  }

  SetMatrix( Rt );

}

devOSGBody::~devOSGBody(){}

// Set the OSG transformation
void devOSGBody::SetMatrix( const vctFrame4x4<double>& Rt ){
  this->setMatrix( osg::Matrix ( Rt[0][0], Rt[1][0], Rt[2][0], 0.0,
				 Rt[0][1], Rt[1][1], Rt[2][1], 0.0,
				 Rt[0][2], Rt[1][2], Rt[2][2], 0.0,
				 Rt[0][3], Rt[1][3], Rt[2][3], 1.0 ) );
}

// This is called from the body's callback
// This reads a transformation if the body is connected to an interface
void devOSGBody::Update(){
  // Get the transformation if possible
  if( ReadTransformation.IsValid() ){
    mtsDoubleFrm4x4 Rt;
    ReadTransformation( Rt );
    SetMatrix( vctFrame4x4<double>(Rt) );
  }
}
