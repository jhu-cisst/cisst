#ifndef _robGUI_h
#define _robGUI_h

#include <FL/glut.h>

#include <cisstRobot/robBody.h>
#include <cisstRobot/robManipulator.h>

#include <vector>

namespace cisstRobot{

  class robGUI : public Fl_Glut_Window{
  private:
    int x, y;
    int width, height;
    
    Real azimuth;
    Real elevation;
    Real distance;
    float  perspective;

    void draw();
    void DrawXYZ();
    void DrawGrid(Real width, int subdivisions);

    R3 CameraPosition() const;

    std::vector<const robBody*> bodies;

  public:

    static robGUI* gui;

    robGUI(int X=0, int Y=0, int W=640, int H=480, const char*L=NULL);
    
    void Insert( const robBody* body );
    void Insert( const robManipulator* manipulator );

    void Keyboard( int k, int x, int y );
  };
}

#endif
