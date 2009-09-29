#ifndef _robBody_h
#define _robBody_h

#include <cisstRobot/robMutex.h>
#include <cisstRobot/robDefines.h>
#include <FL/gl.h>


namespace cisstRobot {

  class robBody{
  private:

    robMutex mutex;

    void glMultMatrix( const SE3& Rt ) const;

  protected:

    SE3 Rt;     // position and orientation

    Real mass;  // the mass of the link (0th order moment)
    R3    com;  // the center of mass of the link (1st order moment)
    MOIT moit;  // the inertia tensor of the link (2nd order moment)
    
  public:

    size_t nvertices;   // the number over mesh vertices
    R3*     vertices;   // the coordinates of vertices
    size_t ntriangles;  // the number of triangles
    N3*     triangles;  // the index of the vertices
    R3*     normals;    // the triangles normals

    robBody();
    robBody(const robBody& body);

    ~robBody();

    SE3  PositionOrientation( ) const { return Rt; } 
    void PositionOrientation( const SE3& Rt ) { this->Rt = Rt; } 

    void Draw() const;    

    robError LoadOBJ( const std::string& filename );
  };
}

#endif
