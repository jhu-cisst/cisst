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

#include <cisstOSG/cisstOSGManipulator.h>

cisstOSGManipulator::cisstOSGManipulator(const std::vector<std::string>& models,
					 cisstOSGWorld* world,
					 const vctFrame4x4<double>& Rtw0,
					 const std::string& robotfile,
					 const std::string& basemodel ) :
  robManipulator( robotfile, Rtw0 ),
  cisstOSGBody( basemodel, world, Rtw0 ){

  // create a group for the links
  osglinks = new osg::Group();

  // add the link group to the base transform
  osgtransform->addChild( osglinks );

  // Add the remaining links called "linki"
  for( size_t i=0; i<links.size(); i++ ){

    try{ 
      osg::ref_ptr<cisstOSGBody> li;
      li = new cisstOSGBody( models[i], 
			     (cisstOSGWorld*)(osglinks.get()), 
			     vctFrame4x4<double>() );
    }
    catch( std::bad_exception& ){
      CMN_LOG_RUN_ERROR << "Failed to allocate body " << models[i] << std::endl;
    }

  }
  
  // Add the base to the world
  world->addChild( this );

}

cisstOSGManipulator::cisstOSGManipulator(const std::vector<std::string>& models,
					 cisstOSGWorld* world,
					 const vctFrm3& Rtw0,
					 const std::string& robotfile,
					 const std::string& basemodel ) :
  robManipulator( robotfile, 
		  vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation() ) ),
  cisstOSGBody( basemodel, world, Rtw0 ){
  
  // create a group for the links
  osglinks = new osg::Group();
  
  // add the link group to the base transform
  osgtransform->addChild( osglinks );
  
  // Add the remaining links called "linki"
  for( size_t i=0; i<links.size(); i++ ){

    try{ 
      osg::ref_ptr<cisstOSGBody> li;
      li = new cisstOSGBody( models[i], 
			     (cisstOSGWorld*)(osglinks.get()), 
			     vctFrame4x4<double>() );
    }
    catch( std::bad_exception& ){
      CMN_LOG_RUN_ERROR << "Failed to allocate body " << models[i] << std::endl;
    }

  }
  
  // Add the base to the world
  world->addChild( this );

}

cisstOSGManipulator::~cisstOSGManipulator(){}

cisstOSGManipulator::Errno
cisstOSGManipulator::GetPositions( vctDynamicVector<double>& q ) const {
  q = this->q; 
  return cisstOSGManipulator::ESUCCESS;
}

cisstOSGManipulator::Errno 
cisstOSGManipulator::SetPositions( const vctDynamicVector<double>& qs ){
  
  // Ensure one joint value per link
  if( qs.size() == links.size() ){

    q = qs;
    
    // For each children
    for( unsigned int i=0; i<osglinks->getNumChildren(); i++ ){ 
      
      // Compute the forward kinematics for that children
      osg::ref_ptr<cisstOSGBody> li;
      li = dynamic_cast<cisstOSGBody*>( osglinks->getChild(i) );
      if( li.get() != NULL ){
	li->SetTransform( ForwardKinematics( q , i+1 ) ); 
      }
    }

    return cisstOSGManipulator::ESUCCESS;
  }
  
  CMN_LOG_RUN_ERROR << "Expected " << links.size() << " values. "
		    << "size(qs)=" << qs.size()
		    << std::endl;

  return cisstOSGManipulator::EFAILURE;

}

