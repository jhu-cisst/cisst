#include <cisstDevices/robotcomponents/osg/devOSGText.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>
#include <osg/ShapeDrawable>

int main(){
  osg::Group* root = new osg::Group;

  osgText::Font* font = osgText::readFontFile( "fonts/arial.ttf" );

  osgText::Text* text = new osgText::Text();
  text->setFont( font );
  text->setColor( osg::Vec4(1.0f,1.0f,1.0f,1.0f) );
  text->setCharacterSize(32.0f);
  text->setAxisAlignment(osgText::Text::SCREEN);
  text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
  //text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
  text->setText("CharacterSizeMode SCREEN_COORDS(size 32.0)");

  osg::Geode* geode = new osg::Geode();
  geode->addDrawable( text );
  geode->setInitialBound( osg::BoundingSphere( osg::Vec3(0.0, 0.0, 0.0), 10.0 ) );
  root->addChild( geode );

  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  viewer.setUpViewInWindow(50,50,800,600);
  viewer.run();

  return EXIT_SUCCESS;

} 
  /*

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG world
  devOSGWorld* world = new devOSGWorld;

  // Create a camera
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.1, Zfar = 10.0;
  devOSGMono* camera;
  camera = new devOSGMono( "camera",
			   world,
			   x, y, width, height,
			   55.0, ((double)width)/((double)height),
			   Znear, Zfar,
			   "", true, false );
  taskManager->AddComponent( camera );

  // Create text
  devOSGText text( "hello world", "/usr/share/fonts/msttcorefonts/cour.ttf", world, 32.0f );

  // Start the camera
  taskManager->CreateAll();
  taskManager->StartAll();


  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();


  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();
  */
