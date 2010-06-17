#include <cisstDevices/glut/devGLUT.h>
#include <cisstDevices/glut/devGLUTManipulator.h>
#include <cisstMultiTask/mtsTaskManager.h>

class Trajectory : public mtsTaskPeriodic{
  
private:

  mtsFunctionWrite mtsFnSetJointsPosition;

  mtsVector<double> q;       // position of the trajectory

public:

  Trajectory() : 
    //mtsTaskPeriodic( "trajectory", 0.01, true ){
      mtsTaskPeriodic( "trajectory", 0.05, true ){

    q.SetSize( 7 );
    q.SetAll( 0.0 );

    mtsRequiredInterface* interface;
    interface = this->AddRequiredInterface( "Trajectory::OutputInterface" );
    if( interface ){ 
      interface->AddFunction
	( devManipulator::Input, mtsFnSetJointsPosition ); 
    }

  }

  void Configure( const std::string& = "" ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();

    for( size_t i=0; i<q.size(); i++ ){ 
      q[i] += 0.01; 
    }

    mtsFnSetJointsPosition( q );

  }
  void Cleanup(){}



};

int main( int argc, char** argv ){

  // log configuration
  cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
  cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devGLUT glut(argc, argv);


  std::string path("libs/etc/cisstRobot/WAM/");
  std::vector<std::string> links;
  links.push_back( path + "l1.obj" );
  links.push_back( path + "l2.obj" );
  links.push_back( path + "l3.obj" );
  links.push_back( path + "l4.obj" );
  links.push_back( path + "l5.obj" );
  links.push_back( path + "l6.obj" );
  links.push_back( path + "l7.obj" );

  Trajectory trajectory;
  taskManager->AddTask(&trajectory);

  devGLUTManipulator WAM("WAM", 
			 0.03,
			 path + "wam7.rob",
			 vctFrame4x4<double>(),
			 vctDynamicVector<double>( 7, 0.0 ),
			 links,
			 path + "l0.obj" );
  taskManager->AddTask(&WAM);

  if (!taskManager->Connect( trajectory.GetName(), "Trajectory::OutputInterface",
                             WAM.GetName(),    devManipulator::InputInterface ))
  {
      std::cerr << "Connect failed: " 
          << trajectory.GetName() << ":" << "Trajectory::OutputInterface"
          << " - "
          << WAM.GetName() << ":" << devManipulator::InputInterface
          << std::endl;
      return 1;
  }

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;

}
