#ifndef _robMeshBase_h
#define _robMeshBase_h

#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robDefines.h>

#include <string>

#include <GL/gl.h>

//! Base class for meshes
class robMeshBase : public vctFrame4x4<double,VCT_ROW_MAJOR> {
protected:

  //! Multiply the current matrix with the specified matrix
  /**
     For OpenGL. Call this to transform the matrix on top of the current stack
  */
  void glMultMatrix( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) const {
    double H[4*4];
    
    H[0] = Rt[0][0]; H[4] = Rt[0][1]; H[8] = Rt[0][2]; H[12] = Rt[0][3];
    H[1] = Rt[1][0]; H[5] = Rt[1][1]; H[9] = Rt[1][2]; H[13] = Rt[1][3];
    H[2] = Rt[2][0]; H[6] = Rt[2][1]; H[10] =Rt[2][2]; H[14] = Rt[2][3];
    H[3] = 0.0;      H[7] = 0.0;      H[11] =0.0;      H[15] = 1.0;
    
    // WARNING: This is for double precision
    glMultMatrixd(H);
  }

public:

  robMeshBase(){}
  virtual ~robMeshBase(){}

  //! Assignment for the frame subclass
  virtual robMeshBase& operator=(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt)=0;

  //! Read a file name from a input stream and load the file
  virtual void Read( std::istream& is ){
    std::string filename;
    is >> filename;
    Load( filename );
  }

  //! Load an .obj file
  virtual robError Load( const std::string& filename ) = 0;
  
  //! Render the mesh in OpenGL
  /**
     \warning Not thread safe: The transformation Rt can be overwritten
   */
  virtual void Draw() const = 0;

};

#endif
