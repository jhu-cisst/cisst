
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

#ifndef _devOSGVolume_h
#define _devOSGVolume_h

#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgVolume/Volume>

#include <cisstVector/vctFrame4x4.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGVolume : 
  //public mtsComponent,
  public osg::MatrixTransform {

 protected:

  osg::ref_ptr<osgVolume::ImageLayer> layer;
  osg::ref_ptr<osgVolume::VolumeTile> tile;

  osg::ref_ptr<osg::TransferFunction1D> transferFunction;
  osg::ref_ptr<osgVolume::SwitchProperty> sp;

  osg::ref_ptr<osgVolume::AlphaFuncProperty> ap;
  osg::ref_ptr<osgVolume::SampleDensityProperty> sd;
  osg::ref_ptr<osgVolume::SampleDensityWhenMovingProperty> sdwm;
  osg::ref_ptr<osgVolume::TransparencyProperty> tp;
  osg::ref_ptr<osgVolume::TransferFunctionProperty> tfp;

  osg::ref_ptr<osg::Image>        image;
  osg::ref_ptr<osgVolume::Volume> volume;

  void ReadVolume( const std::string& directory );


 public: 

  //! OSG Volume constructor
  /**
     Create a OSG body component. The body will add a required interface *if*
     a function name is passed. In this case the interface is called 
     "Transformation" and the body will read the function at each update 
     traversal.
     \param name The name of the body/component
     \param Rt The initial transformation of the body
     \param model The file name of a 3D model
     \param world The OSG world the body belongs to
     \param transformfn The name of a MTS read command the body will connect
  */
  devOSGVolume( const std::string& name, 
		const std::string& directory,
		devOSGWorld* world = NULL );
  
  ~devOSGVolume();
  
};

#endif
