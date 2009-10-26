#ifndef _robGUI_h
#define _robGUI_h

#include <cisstRobot/robBody.h>
#include <cisstRobot/robManipulator.h>

#include <GL/glut.h>

#include <vector>

namespace cisstRobot{

  class robGUI {
  private:

    int x, y;           // X, Y positions (top left corner)
    int width, height;  // width and heiht
    
    double azimuth;       // rotation about the Z axis
    double elevation;     // elevation from the X-Y plane
    double distance;      // distance from the origin
    float perspective;  // camera FOV

    void DrawXYZ();     // draw the X-Y-Z axis
    void DrawGrid(double width, int subdivisions); // draw the floor

    vctFixedSizeVector<double,3> CameraPosition() const; // compute the camera XYZ coordinates

    std::vector<const robBody*> bodies; // the bodies to be rendered

  public:

    //! The pointer to the main GUI object;
    static robGUI* gui;

    //! Default constructor
    robGUI( int argc, char** argv);
    
    //! Insert a body to display
    void Insert( const robBody* body );
    //! Insert a manipulator to display
    void Insert( const robManipulator* manipulator );

    //! Draw everything
    void Draw();

    //! Process the keboard
    void Keyboard( int k, int x, int y );
  };
}

#endif
