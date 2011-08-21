#include <cisstDevices/robotcomponents/osg/devOSGVolume.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>

int main( int argc, char** argv ){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG world
  devOSGWorld* world = new devOSGWorld;

  // Create a camera
  int x = 0, y = 0;
  int width = 640, height = 480;
  double Znear = 0.01, Zfar = 1.0;
  devOSGMono* camera;
  camera = new devOSGMono( "camera",
			   world,
			   x, y, width, height,
			   55.0, ((double)width)/((double)height),
			   Znear, Zfar,
			   "", true, false );
  taskManager->AddComponent( camera );

  osg::ref_ptr<devOSGVolume> volume;
  volume = new devOSGVolume( "volume", argv[1], world );


  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();


  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
