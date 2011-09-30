#ifndef _devOSGPick_h
#define _devOSGPick_h

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGPick : public osgGA::GUIEventHandler{

 protected:

  double mousex;
  double mousey;

  bool Pick( double x, double y, osgViewer::Viewer* viewer );

 public:

  devOSGPick();

  // OSG virtual method
  bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );


};

#endif
