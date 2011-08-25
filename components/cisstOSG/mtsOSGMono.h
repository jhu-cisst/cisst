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


#ifndef _mtsOSGMono_h
#define _mtsOSGMono_h


#include <cisstOSG/cisstOSGMono.h>
#include <cisstOSG/mtsOSGCamera.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT mtsOSGMono :

  public mtsOSGCamera,
  public cisstOSGMono{
  
 public:
  
  // Main constructor
  mtsOSGMono( const std::string& name,
	      cisstOSGWorld* world,
	      int x, int y, int width, int height,
	      double fovy, double aspectRatio,
	      double zNear, double zFar,
	      bool trackball = true,
	      bool offscreenrendering = false ) : 
    mtsOSGCamera( name, this ),
    cisstOSGMono( world,
		  x, y, 
		  width, height, 
		  fovy, aspectRatio, 
		  zNear, zFar, 
		  trackball, offscreenrendering ){}
  
  ~mtsOSGMono(){}

};

#endif
