/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkOpenGLActor.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "CSOpenGLStippleActor.h"
//#include "vtkOpenGLActor.h"

#include "vtkMapper.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkProperty.h"

#include "vtkOpenGL.h"
#include <math.h>

#ifndef VTK_IMPLEMENT_MESA_CXX
//vtkCxxRevisionMacro(CSOpenGLStippleActor, "$Revision: ?? $");
vtkStandardNewMacro(CSOpenGLStippleActor);
#endif


CSOpenGLStippleActor::CSOpenGLStippleActor()
{
    this -> pattern= 1;

}


// Actual actor render method.
void CSOpenGLStippleActor::Render(vtkRenderer *ren, vtkMapper *mapper)
{
  float opacity;
//     vtkRenderer *ren1 = vtkRenderer::New();
//     ren1 = ren;
//     vtkMapper *map1 = vtkMapper::New();
//     map1 = mapper;

  // get opacity
  opacity = this->GetProperty()->GetOpacity();
  if (opacity == 1.0)
    {
    glDepthMask (GL_TRUE);
    }
  else
    {
    // add this check here for GL_SELECT mode
    // If we are not picking, then don't write to the zbuffer
    // because we probably haven't sorted the polygons. If we
    // are picking, then translucency doesn't matter - we want to
    // pick the thing closest to us.
    GLint param[1];
    glGetIntegerv(GL_RENDER_MODE, param);
    if(param[0] == GL_SELECT )
      {
      glDepthMask(GL_TRUE);
      }
    else
      {
      glDepthMask (GL_FALSE);
      }
    }

  // build transformation 
  if (!this->IsIdentity)
    {
    double *mat = this->GetMatrix()->Element[0];
    double mat2[16];
    mat2[0] = mat[0];
    mat2[1] = mat[4];
    mat2[2] = mat[8];
    mat2[3] = mat[12];
    mat2[4] = mat[1];
    mat2[5] = mat[5];
    mat2[6] = mat[9];
    mat2[7] = mat[13];
    mat2[8] = mat[2];
    mat2[9] = mat[6];
    mat2[10] = mat[10];
    mat2[11] = mat[14];
    mat2[12] = mat[3];
    mat2[13] = mat[7];
    mat2[14] = mat[11];
    mat2[15] = mat[15];
    
    // insert model transformation 
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glMultMatrixd(mat2);
    }

    SetStipplePattern(pattern);

  // send a render to the mapper; update pipeline
  mapper->Render(ren,this);

  // pop transformation matrix
  if (!this->IsIdentity)
    {
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    }
  
  if (opacity != 1.0)
    {
    glDepthMask (GL_TRUE);
    }

//--------------------------------------------------------------------------------------
    glPopAttrib();
}

//------------------------------
void CSOpenGLStippleActor::SetStipplePattern(int mode)
{
    pattern = mode;

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_POLYGON_STIPPLE);
    GLubyte kPattern0;
    GLubyte kPattern1;

//cout << mode << endl;

switch (mode)
{
case 1:

    kPattern0 = 0xAA;
    kPattern1 = 0x55;
// Load a fine 32x32 bit halftone pattern
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 4; j++)
        { 
            StipplePattern[i*4+j] = i % 2 == 0 ?  kPattern0 : kPattern1;
        }
    }
    break;
case 2:
    for (int i = 0; i < 32*4; i++)
    {
        //for (int j = 0; j < 4; j++)
        { 
            StipplePattern[i] = 0xFF;
        }
    }
    break;
case 3:
    kPattern0 = 0x00;
    kPattern1 = 0x55;
// Load a fine 32x32 bit halftone pattern
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 4; j++)
        { 
            StipplePattern[i*4+j] = i % 2 == 0 ?  kPattern0 : kPattern1;
        }
    }
    break;
default:
    break;

}

    

    glPolygonStipple(StipplePattern);

//cout << " send a render to the mapper; update pipeline" << endl;
//  map1->Render(ren1,this);
    this->Modified();

//cout << "after render" << endl;
//---------------------------------------------------------------------------------


}

//----------------------------------------------------------------------------
void CSOpenGLStippleActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
