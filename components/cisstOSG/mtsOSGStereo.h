/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsOSGStereo_h
#define _mtsOSGStereo_h


#include <cisstOSG/cisstOSGStereo.h>
#include <cisstOSG/mtsOSGCameraTask.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT mtsOSGStereo : public mtsOSGCameraTask {
  
 public:
  
  // Main constructor
  /**
  */
  mtsOSGStereo( const std::string& name,
		cisstOSGWorld* world,
		int x, int y, int width, int height,
		double fovy, double aspectRatio,
		double zNear, double zFar,
		double baseline,
		bool trackball = true ) :
    mtsOSGCameraTask( name, 
		      new cisstOSGStereo( world,
					  x, y, 
					  width, height, 
					  fovy, aspectRatio, 
					  zNear, zFar,
					  baseline,
					  trackball ) ){}
  
  ~mtsOSGStereo(){}

};

#endif
