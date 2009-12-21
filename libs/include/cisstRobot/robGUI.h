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

#include <GL/glut.h>
#include <vector>

#include <cisstRobot/robGeom.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robGUI {
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
  
  std::vector<const robGeom*> geoms; // the geometries
  
public:
  
  //! The pointer to the main GUI object;
  static robGUI* gui;
  
  //! Default constructor
  robGUI( int argc, char** argv);
  
  //! Insert a body to display
  static void Insert( const robGeom* geom );

  //! 
  void Register( const robGeom* geom );

  //! Draw everything
  void Draw();
  
  //! Process the keboard
  void Keyboard( int k, int x, int y );

  static void Refresh();
};

#endif
