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

#ifndef _robGUI_h
#define _robGUI_h

#include <stdlib.h>
#include <GL/glut.h>
#include <vector>

#include <cisstDevices/glut/devGeometry.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devGLUT {

 private:

  int x, y;           // X, Y positions (top left corner)
  int width, height;  // width and heiht
    
  double azimuth;       // rotation about the Z axis
  double elevation;     // elevation from the X-Y plane
  double distance;      // distance from the origin
  float perspective;  // camera FOV
  
  void DrawXYZ();     // draw the X-Y-Z axis
  void DrawGrid(double width, int subdivisions); // draw the floor
  
  // compute the camera XYZ coordinates
  vctFixedSizeVector<double,3> CameraPosition() const;
  
  std::vector<const devGeometry*> geoms; // the geometries
  
 public:

  static devGLUT* glut;
  
  //! Default constructor
  devGLUT( int argc, char** argv);
  
  //! 
  static void Register( const devGeometry* geom );

  //! Draw everything
  void Draw();
  
  //! Process the keboard
  void Keyboard( int k, int x, int y );

  devGeometry* LoadOBJ( const std::string& filename );

  static void Refresh();
};

#endif
