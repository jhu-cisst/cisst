#include <cisstDevices/robotcomponents/osg/devOSGText.h>

devOSGText::devOSGText( const std::string& text, 
			const std::string& font, 
			devOSGWorld* world,
			float size ){

  osgfont = osgText::readFontFile( font.data() );

  osgtext = new osgText::Text;
  osgtext->setFont( osgfont.get() );
  osgtext->setText( text );
  osgtext->setAxisAlignment( osgText::Text::SCREEN );
  osgtext->setCharacterSize( 32.0f );
  osgtext->setColor( osg::Vec4( 0.f, 0.f, 1.f, 1.f ) );
  osgtext->setLayout(osgText::Text::LEFT_TO_RIGHT);

  osggeode = new osg::Geode();
  osggeode->addDrawable( osgtext );

  addChild( osggeode );

  //osg::ShapeDrawable* shape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),0.00001f));
  //text01Geode->addDrawable(shape); 

  world->addChild( this );

}
