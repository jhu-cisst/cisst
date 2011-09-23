#ifndef _cisstOSGPick_h
#define _cisstOSGPick_h

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

#include <cisstOSG/cisstOSGBody.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT cisstOSGPick : public osgGA::GUIEventHandler{

 protected:

  double mousex;
  double mousey;

  bool Pick( double x, double y, osgViewer::Viewer* viewer );

  virtual void PickHandler( cisstOSGBody* body ){}

 public:

  cisstOSGPick();

  // OSG virtual method
  bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

};

#endif
