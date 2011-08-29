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
  base( NULL ) {

  if( !basemodel.empty() ){ 
    // create the base and add it to the manipulator
    base =  new cisstOSGBody( basemodel, (cisstOSGWorld*)this, Rtw0 ); 
  }

  // create a group for the links
  osglinks = new osg::Group();

  // create the links and add them to the link group
  for( size_t i=0; i<links.size(); i++ ){
    osg::ref_ptr<cisstOSGBody> li;
    li = new cisstOSGBody( models[i], 
			   (cisstOSGWorld*)(osglinks.get()), 
			   vctFrame4x4<double>() );
  }

  // add the links to the manipulator
  addChild( osglinks );

  // add the manipulator to the world
  world->addChild( this );
  
}

cisstOSGManipulator::cisstOSGManipulator(const std::vector<std::string>& models,
					 cisstOSGWorld* world,
					 const vctFrm3& Rtw0,
					 const std::string& robotfile,
					 const std::string& basemodel ) :
  robManipulator( robotfile, 
		  vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation() ) ),
  base( NULL ){
  
  if( !basemodel.empty() ){ 
    // create the base and add it to the manipulator
    base =  new cisstOSGBody( basemodel, (cisstOSGWorld*)this, Rtw0 ); 
  }

  // create a group for the links
  osglinks = new osg::Group();

  // create the links and add them to the link group
  for( size_t i=0; i<links.size(); i++ ){
    osg::ref_ptr<cisstOSGBody> li;
    li = new cisstOSGBody( models[i], 
			   (cisstOSGWorld*)(osglinks.get()),
			   vctFrame4x4<double>() );
  }

  // add the links to the manipulator
  addChild( osglinks );

  // add the manipulator to the world
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
    for( unsigned int i=0; i<GetNumLinks(); i++ ){
      osg::ref_ptr<cisstOSGBody> li = GetLink( i );
      if( li.get() != NULL )
	{ li->SetTransform( ForwardKinematics( q , i+1 ) ); }
    }

    return cisstOSGManipulator::ESUCCESS;
  }
  
  CMN_LOG_RUN_ERROR << "Expected " << links.size() << " values. "
		    << "size(qs)=" << qs.size()
		    << std::endl;
  
  return cisstOSGManipulator::EFAILURE;
  
}

unsigned int cisstOSGManipulator::GetNumLinks()
{ return osglinks->getNumChildren(); }

cisstOSGBody* cisstOSGManipulator::GetLink( size_t i ){
  if( i < GetNumLinks() )
    { return dynamic_cast<cisstOSGBody*>( osglinks->getChild(i) ); }
  return NULL;
}

