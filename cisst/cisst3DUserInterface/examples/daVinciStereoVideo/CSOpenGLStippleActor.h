/*=========================================================================



=========================================================================*/
// .NAME CSOpenGLStippleActor - OpenGL actor
// .SECTION Description
// vtkOpenGLActor is a concrete implementation of the abstract class vtkActor.
// vtkOpenGLActor interfaces to the OpenGL rendering library.

#ifndef __CSOpenGLStippleActor_h
#define __CSOpenGLStippleActor_h

#include "vtkOpenGLActor.h"

class vtkOpenGLRenderer;

class VTK_RENDERING_EXPORT CSOpenGLStippleActor : public vtkOpenGLActor
{
protected:

public:
	static CSOpenGLStippleActor *New();
	//  vtkTypeRevisionMacro(CSOpenGLStippleActor,vtkOpenGLActor);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Actual actor render method.
	void Render(vtkRenderer *ren, vtkMapper *mapper);
	void SetStipplePattern(int mode);
	void SetStipplePercentage(int percentage);

protected:
	CSOpenGLStippleActor();
	~CSOpenGLStippleActor();

	vtkRenderer     *ren1;
	vtkMapper       *map1;

	int pattern;
	int StipplePercentage;

	unsigned char StipplePattern[32*4];

	bool debug;

};

#endif


