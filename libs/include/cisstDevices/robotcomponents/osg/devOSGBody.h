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

#ifndef _devOSGBody_h
#define _devOSGBody_h

#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <cisstVector/vctFrame4x4.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGBody : public osg::MatrixTransform {

 protected:

  osg::Geometry* geometry;

 public: 

  devOSGBody( const std::string& model, 
	      devOSGWorld* world = NULL,
	      const vctFrame4x4<double>& = vctFrame4x4<double>() );
  ~devOSGBody();

  void SetTransformation( const vctFrame4x4<double>& Rt );

};

#endif
