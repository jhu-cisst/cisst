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

devOSGBody::devOSGBody( const std::string& model, 
			devOSGWorld* world,
			const vctFrame4x4<double>& Rt ) :
  geometry( NULL ){
  
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

      // Then get the first child (there could be more!)
      node = group->getChild( 0 );
      // This node should be a geode
      osg::Geode* geode = node->asGeode();
      if( geode != NULL ){

	// Find if it has any drawables?
	if( 0 < geode->getNumDrawables() ){
	  
	  // Get the first drawable
	  osg::Drawable* drawable = geode->getDrawable( 0 );
	  
	  // Cast the drawable as a geometry
	  geometry = drawable->asGeometry();
	  if( geometry == NULL ){
	    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			      << "Failed to cast the drawable as a geometry."
			      << std::endl;
	  }

	}
      }
      else{
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to cast node as a geode for : " << model 
			  << std::endl;
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
  
  SetTransformation( Rt );
  
}

devOSGBody::~devOSGBody(){}

void devOSGBody::SetTransformation( const vctFrame4x4<double>& Rt ){
  setMatrix( osg::Matrix ( Rt[0][0], Rt[1][0], Rt[2][0], 0.0,
			   Rt[0][1], Rt[1][1], Rt[2][1], 0.0,
			   Rt[0][2], Rt[1][2], Rt[2][2], 0.0,
			   Rt[0][3], Rt[1][3], Rt[2][3], 1.0 ) );
}
  
