#include <cisstOSG/cisstOSGBody.h>
#include <cisstOSG/cisstOSGPick.h>
#include <cisstOSG/cisstOSGMono.h>
#include <cisstOSG/cisstOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>

int main(){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  // Create the OSG world
  osg::ref_ptr<cisstOSGWorld> world = new cisstOSGWorld;

  // Create a camera
  int x = 0, y = 0;
  int width = 640, height = 480;
  double Znear = 0.1, Zfar = 10.0;
  osg::ref_ptr<cisstOSGMono> camera;
  camera = new cisstOSGMono( world,
			     x, y, width, height,
			     55.0, ((double)width)/((double)height),
			     Znear, Zfar );
  camera->Initialize();
  camera->addEventHandler( new cisstOSGPick() );

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );
  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );


  vctFrame4x4<double> eye;
  osg::ref_ptr<cisstOSGBody> background;
  background = new cisstOSGBody( data+"background.3ds", world, eye );
  background->setName( "background" );

  osg::ref_ptr<cisstOSGBody> hubble;
  hubble = new cisstOSGBody( data+"hst.3ds", world, Rt );
  hubble->setName( "hubble" );

  while( !camera->done() )
    camera->frame();


  return 0;

}
