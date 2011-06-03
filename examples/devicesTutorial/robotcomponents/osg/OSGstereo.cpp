#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devOSGWorld* world = new devOSGWorld;

  // Create a stereo rig
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.01, Zfar = 10.0;
  double baseline = 0.1;
  devOSGStereo* stereocam=new devOSGStereo( "stereo",
					    world,
					    x, y, width, height,
					    55,((double)width)/((double)height),
					    Znear, Zfar,
					    baseline );
  // Add the camera component
  taskManager->AddComponent( stereocam );

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  devOSGBody* hubble;
  hubble = new devOSGBody( "hubble", Rt, data+"hst.3ds", world );

  vctFrame4x4<double> eye;
  devOSGBody* background;
  background = new devOSGBody("background", eye, data+"background.3ds", world);

  // Start the cameras
  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();

  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
