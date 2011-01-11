#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>

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
  devODEWorld* world = new devODEWorld( 0.001, OSA_CPU1 );
  taskManager->AddComponent( world );

  // Create a camera
  int width = 640, height = 480;
  devOSGCamera* camera = new devOSGCamera( "stereo",
					   world,
					   0, 0, width, height,
					   55, 
					   ((double)width)/((double)height),
					   0.01, 10.0 );
  // Add+configure the trackball of the camera
  camera->setCameraManipulator( new osgGA::TrackballManipulator );
  camera->getCameraManipulator()->setHomePosition( osg::Vec3d( 1,0,1 ),
						   osg::Vec3d( 0,0,0 ),
						   osg::Vec3d( 0,0,1 ) );
  camera->home();

  // add a bit more light
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setAmbient( osg::Vec4( 1, 1, 1, 1 ) );
  camera->setLight( light );
  
  // add the camera task
  taskManager->AddComponent( camera );

  std::string path("libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( path+"l1.obj" );
  models.push_back( path+"l2.obj" );
  models.push_back( path+"l3.obj" );
  models.push_back( path+"l4.obj" );
  models.push_back( path+"l5.obj" );
  models.push_back( path+"l6.obj" );
  models.push_back( path+"l7.obj" );

  vctDynamicVector<double> qinit(7, 0.0);
  devODEManipulator* WAM = new devODEManipulator( "WAM",
						  0.002,
						  devManipulator::ENABLED,
						  OSA_CPU1,
						  world,
						  devManipulator::POSITION,
						  path+"wam7.rob",
						  vctFrame4x4<double>(),
						  qinit,
						  models,
						  path+"l0.obj" );
  taskManager->AddComponent( WAM );

  Trajectory trajectory;
  taskManager->AddComponent( &trajectory );

  taskManager->Connect( trajectory.GetName(), "Output",
			WAM->GetName(),       devOSGManipulator::Input );

  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();

  return 0;
}
