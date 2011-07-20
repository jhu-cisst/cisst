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
#include <osgVolume/Volume>
#include <osgDB/FileUtils>
#include <osgVolume/RayTracedTechnique>
#include <osgVolume/FixedFunctionTechnique>
#include <osg/ImageUtils>


#include <cisstDevices/robotcomponents/osg/devOSGVolume.h>

devOSGVolume::devOSGVolume( const std::string& name,
			    const std::string& directory, 
			    devOSGWorld* world ){
  
  setDataVariance( osg::Object::DYNAMIC );
  
  ReadVolume( directory );

  if( world != NULL ){ 
    if( volume.valid() ){
      world->addChild( volume.get() ); 
    }
    else
      std::cout << "INVALID" << std::endl;
  }

}

devOSGVolume::~devOSGVolume(){}

void devOSGVolume::ReadVolume( const std::string& directory ){

  double alphaFunc = 0.02;

  osgDB::FileType fileType = osgDB::fileType( directory );
  if( fileType == osgDB::DIRECTORY )
    { image = osgDB::readImageFile( directory+".dicom" ); }

  osg::ref_ptr<osgVolume::ImageDetails> details;
  details = dynamic_cast<osgVolume::ImageDetails*>( image->getUserData() );

  osg::Matrixd* matrix = new osg::Matrixd();
  matrix->makeIdentity();


  volume = new osgVolume::Volume;
  tile = new osgVolume::VolumeTile;
  volume->addChild( tile.get() );

  layer = new osgVolume::ImageLayer( image.get() );  
  layer->setTexelOffset( details->getTexelOffset() );
  layer->setTexelScale( details->getTexelScale() );
  layer->rescaleToZeroToOneRange();
  layer->setLocator( new osgVolume::Locator(*matrix) );

  tile->setLocator( new osgVolume::Locator(*matrix) );
  tile->setLayer( layer.get() );
  tile->setEventCallback( new osgVolume::PropertyAdjustmentCallback() );
 

  sp = new osgVolume::SwitchProperty;
  sp->setActiveProperty(0);

  ap = new osgVolume::AlphaFuncProperty( 0.02 );
  sd = new osgVolume::SampleDensityProperty( 0.005 );
  tp = new osgVolume::TransparencyProperty(1.0);

  {
    // Standard
    osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
    cp->addProperty(ap);
    cp->addProperty(sd);
    cp->addProperty(tp);
    sp->addProperty(cp);
  }

  {
    // Light
    osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
    cp->addProperty(ap);
    cp->addProperty(sd);
    cp->addProperty(tp);
    cp->addProperty( new osgVolume::LightingProperty );
    sp->addProperty(cp);
  }
  
  {
    // Isosurface
    osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
    cp->addProperty(sd);
    cp->addProperty(tp);
    cp->addProperty(new osgVolume::IsoSurfaceProperty( 0.02f ) );
    sp->addProperty(cp);
  }
  
  {
    // MaximumIntensityProjection
    osgVolume::CompositeProperty* cp = new osgVolume::CompositeProperty;
    cp->addProperty(ap);
    cp->addProperty(sd);
    cp->addProperty(tp);
    cp->addProperty(new osgVolume::MaximumIntensityProjectionProperty);
    sp->addProperty(cp);
  }

  sp->setActiveProperty(0);

  layer->addProperty(sp);
  
  tile->setVolumeTechnique(new osgVolume::RayTracedTechnique);

  /*
  layer->addProperty( new osgVolume::AlphaFuncProperty( alphaFunc ) );
  tile->setVolumeTechnique( new osgVolume::FixedFunctionTechnique );
  */

}
