#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstCommon/cmnGetChar.h>

class Rotate : public mtsTaskPeriodic {
private:
  mtsDoubleFrm4x4 Rt;
  double theta;
public:

  Rotate() : mtsTaskPeriodic( "Rotate", 0.001, true ), theta(0.0){
    mtsInterfaceProvided* interface = AddInterfaceProvided( "Transformation" );
    StateTable.AddData( Rt, "Transformation" );
    interface->AddCommandReadState( StateTable, Rt, "Transform" );
  }

  void Configure( const std::string& = "" ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    theta += 0.001;
    vctFixedSizeVector<double,3> u( 0.0, 0.0, 1.0 );
    vctAxisAngleRotation3<double> ut( u, theta );
    Rt = vctFrame4x4<double>( ut, vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  }
  void Cleanup(){}
};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devOSGWorld* world = new devOSGWorld;

  // Create a stereo rig
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.01, Zfar = 10.0;
  double baseline = 0.1;

  devOSGStereo* stereo1=new devOSGStereo( "stereo1",
					  world,
					  x, y, width, height,
					  55, ((double)width)/((double)height),
					  Znear, Zfar,
					  baseline );
  // define the culling mask of the stereo rig
  osg::Node::NodeMask mask1 = 0x0001;
  stereo1->setCullMask( mask1 );
  // Add the camera component
  taskManager->AddComponent( stereo1 );

  devOSGStereo* stereo2=new devOSGStereo( "stereo2",
					  world,
					  x, y+300, width, height,
					  55, ((double)width)/((double)height),
					  Znear, Zfar,
					  baseline );
  osg::Node::NodeMask mask2 = 0x0002;
  stereo2->setCullMask( mask2 );
  // Add the camera component
  taskManager->AddComponent( stereo2 );

  // Create the objects

  // path to the models
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> eye;
  
  // Create the telescope
  devOSGBody* hubble;
  hubble = new devOSGBody( "hubble", eye, data+"hst.3ds", world, "Transform" );
  // make the hubble only visible to the camera with mask 0x0001
  hubble->setNodeMask( mask1 );
  taskManager->AddComponent( hubble );

  // Create the background
  devOSGBody* background;
  background = new devOSGBody( "background", eye, data+"background.3ds", world);

  Rotate rotate;
  taskManager->AddComponent( &rotate );
  
  taskManager->Connect( "hubble", "Transformation","Rotate", "Transformation" );
  
  // Start the cameras
  taskManager->CreateAll();
  cmnGetChar();
  taskManager->StartAll();

  cmnGetChar();

  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
