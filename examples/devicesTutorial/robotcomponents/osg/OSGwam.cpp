#include <cisstDevices/robotcomponents/osg/devOSGManipulator.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTaskManager.h>

// This is an example of a simple trajectory
class Trajectory : public devRobotComponent{
private:
  RnIO* output;
  vctDynamicVector<double> q;
public:
  Trajectory() :
    devRobotComponent( "trajectory", 0.1, Trajectory::ENABLED, OSA_CPU1 ),
    q( 7, 0.0 ){    
    output = RequireOutputRn( "Output", Trajectory::POSITION, 7 );
  }
  void Configure( const std::string& CMN_UNUSED( argv ) = "" ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    for( size_t i=0; i<7; i++ ) q[i] += 0.01;
    output->SetPosition( q );
  }
  void Cleanup(){}
};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devOSGWorld* world = new devOSGWorld;

  // Create a camera
  int x = 0, y = 0;
  int width = 640, height = 480;
  double Znear = 0.01, Zfar = 10.0;
  devOSGMono* camera;
  camera = new devOSGMono( "camera",
			   world,
			   x, y, width, height,
			   55, ((double)width)/((double)height),
			   Znear, Zfar );
  // Add the camera component
  taskManager->AddComponent( camera );
  
  // WAM stuff
  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( path+"l1.obj" );
  models.push_back( path+"l2.obj" );
  models.push_back( path+"l3.obj" );
  models.push_back( path+"l4.obj" );
  models.push_back( path+"l5.obj" );
  models.push_back( path+"l6.obj" );
  models.push_back( path+"l7.obj" );

  vctDynamicVector<double> qinit(7, 0.0);
  devOSGManipulator* WAM = new devOSGManipulator( "WAM",
						  0.1,
						  devManipulator::ENABLED,
						  OSA_CPU1,
						  world,
						  path+"wam7.rob",
						  vctFrame4x4<double>(),
						  qinit,
						  models,
						  path+"l0.obj" );
  taskManager->AddComponent( WAM );

  // Trajectory
  Trajectory trajectory;
  taskManager->AddComponent( &trajectory );

  // Connect trajectory to robot
  taskManager->Connect( trajectory.GetName(), "Output",
			WAM->GetName(),       devOSGManipulator::Input );


  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;
}
