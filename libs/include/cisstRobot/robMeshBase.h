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

#ifndef _robMeshBase_h
#define _robMeshBase_h

#include <string>
#include <iostream>
#include <GL/gl.h>

#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctMatrixRotation3.h>

#include <cisstRobot/robDefines.h>

//! Base class for meshes
/**
   robMeshBase defines the interface for all meshes. The primary purpose of this
   interface is graphics rendering in OpenGL. Thus, any subclass, must implement
   a Draw() method to draw the mesh in an OpenGL visual. robMeshBase derives
   from vctFrame4x4 and this class is used to store the position and orientation
   of the mesh. Any data specific to the mesh representation must be implemented
   in the subclass.
 */
class robMeshBase : public vctFrame4x4<double,VCT_ROW_MAJOR>{

protected:

  //! Multiply the OpenGL current matrix with the specified matrix
  /**
     For OpenGL. Call this before drawing your mesh to push the mesh position
     and orientation on the stack and call this (with the inverse) to remove
     the orientation and position.
     \param Rt The position and orientation of the mesh
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

  //! Default constructor
  robMeshBase(){}
  //! Default destructor
  ~robMeshBase(){}

  //! Assignment for the mesh position and orientation
  /**
     This operator enables to assign a vctFrame4x4 to the mesh to assign its
     position and orientation.
     \param Rt The position and orientation of the mesh
  */
  virtual robMeshBase& operator=(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt){
    if( *this != Rt )
      vctFrame4x4<double,VCT_ROW_MAJOR>::operator=(Rt);
    return *this;
  }

  //! Read a file name from a input stream and load the file
  /**
     Read a file name from the input stream and then call Load to load the mesh
     into memory. The implementation of Load is left to a subclass.
  */
  virtual robError Read( std::istream& is ){
    std::string filename;
    is >> filename;
    return Load( filename );
  }

  //! Load a file
  /**
     Pure virtual function. This method is called to load a mesh into memory.
     The type of file and mesh representation is implementation specific.
     \param filename The file name of the file to load
     \return SUCCESS if the file was loaded properly. ERROR otherwise.
   */
  virtual robError Load( const std::string& filename ) = 0;
  
  //! Render the mesh
  /**
     This method is called automatically to draw the mesh in OpenGL. If you want
     to render your mesh in OpenGL, then you need to make a subclass and 
     overload Draw(). If you don't need to draw the mesh, then leave this 
     method empty.
     \warning Not thread safe: The transformation Rt can be overwritten during
              rendering.
  */
  virtual void Draw() const{}

  //! Set the position of the mesh
  /**
     Set the position of the vctFrame4x4 subclass
     \param t A 3D position vector
  */
  void SetPosition( const vctFixedSizeVector<double,3>& t){
    TranslationRefType p = Translation();
    p[0] = t[0];    p[1] = t[1];    p[2] = t[2];
  }

  //! Set the orientation of the mesh
  /**
     Set the orientation of the vctFrame4x4 subclass
     \param t A 3D rotation matrix
  */
  void SetOrientation(const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R){
    RotationRefType r = Rotation();
    r[0][0] = R[0][0]; r[0][1] = R[0][1]; r[0][2] = R[0][2];
    r[1][0] = R[1][0]; r[1][1] = R[1][1]; r[1][2] = R[1][2];
    r[2][0] = R[2][0]; r[2][1] = R[2][1]; r[2][2] = R[2][2];
  }
  
  //! Return the position of the mesh
  /**
     \return A 3D position vector
  */
  vctFixedSizeVector<double,3> GetPosition() const
  { return Translation(); }
  
  //! Return the orientation of the mesh
  /**
     \return A 3D rotation matrix
  */
  vctMatrixRotation3<double,VCT_ROW_MAJOR> GetOrientation() const{ 
    ConstRotationRefType R = Rotation();
    return vctMatrixRotation3<double,VCT_ROW_MAJOR>(R[0][0], R[0][1], R[0][2],
						    R[1][0], R[1][1], R[1][2],
						    R[2][0], R[2][1], R[2][2] );
  }

};

#endif
