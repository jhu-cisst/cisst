
#ifndef _devOSGText_h
#define _devOSGText_h

#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgText/Text>
#include <osgText/Font>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGText : public osg::MatrixTransform {


 protected:

  osg::ref_ptr<osgText::Text> osgtext;
  osg::ref_ptr<osgText::Font> osgfont;
  osg::ref_ptr<osg::Geode> osggeode;
  osg::ref_ptr<osg::Switch> osgswitch;

 public:

  devOSGText( const std::string& text, 
	      const std::string& font, 
	      devOSGWorld* world,
	      float size );

};


#endif
