#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEBH.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>

#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTaskManager.h>

// This is an example of a simple trajectory
class BHTrajectory : public devRobotComponent{
private:
  RnIO* output;
  vctDynamicVector<double> q;
public:
  BHTrajectory() :
    devRobotComponent( "trajectory", 0.1, BHTrajectory::ENABLED, OSA_CPU1 ),
    q( 4, 0.0 ){    
    output = RequireOutputRn( "Output", BHTrajectory::POSITION, 4 );
  }
  void Configure( const std::string& CMN_UNUSED( argv ) = "" ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    for( size_t i=0; i<4; i++ ) q[i] += 0.01;
    output->SetPosition( q );
  }
  void Cleanup(){}
};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devODEWorld* world = NULL;
  world = new devODEWorld( 0.001, OSA_CPU1,vctFixedSizeVector<double,3>( 0.0 ) );
  taskManager->AddComponent( world );

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

  // BH stuff
  std::string path(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/BH/");

  vctDynamicVector<double> qinit(4, 0.0);
  devODEBH* BH = new devODEBH( "BH",
			       0.002,
			       devManipulator::ENABLED,
			       OSA_CPU1,
			       world,
			       devManipulator::POSITION,
			       path+"f1f2.rob",
			       path+"f3.rob",
			       vctFrame4x4<double>(),
			       path+"l0.obj",
			       path+"l1.obj",
			       path+"l2.obj",
			       path+"l3.obj" );
  taskManager->AddComponent( BH );

  // Trajectory
  BHTrajectory trajectory;
  taskManager->AddComponent( &trajectory );

  // Connect trajectory to robot
  taskManager->Connect( trajectory.GetName(), "Output",
			BH->GetName(),       devOSGManipulator::Input );


  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();
  
  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
