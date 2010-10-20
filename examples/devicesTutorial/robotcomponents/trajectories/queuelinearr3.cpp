

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearR3.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <fstream>

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#include <native/task.h>
#endif

using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  R3IO* input;

public:

  File( const std::string& fname ): 
    devRobotComponent( "file", 0.01, true ){
    input = ProvideInputR3( "input", 
			    devRobotComponent::POSITION |
			    devRobotComponent::VELOCITY |
			    devRobotComponent::ACCELERATION );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctFixedSizeVector<double,3> p;
    double t;
    input->GetPosition( p, t );
    cout << p << std::endl;
  }

};

int main(){

#if (CISST_OS == CISST_LINUX_XENOMAI)
  RT_TASK main;
  mlockall( MCL_CURRENT | MCL_FUTURE );
  rt_task_shadow( &main, "main", 30, 0 );
#endif

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  vctFixedSizeVector<double,3> p1( rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> p2( rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> p3( rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> p4( rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0,
				   rand()*10.0/RAND_MAX-5.0 );
  double v = rand()*10.0/RAND_MAX;

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteFunction('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddComponent( &kb );

  std::vector< vctFixedSizeVector<double,3> > p;
  p.push_back( p1 );
  p.push_back( p2 );
  p.push_back( p3 );
  p.push_back( p4 );
  p.push_back( p1 );
  p.push_back( p3 );
  p.push_back( p2 );

  devSetPoints setpoints( "setpoints", p );
  taskManager->AddComponent( &setpoints );

  File file( "trajrn" );
  taskManager->AddComponent( &file );

  devLinearR3 trajectory( "trajectory",
			  0.01,
			  true,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  p1,
			  v );
  taskManager->AddComponent( &trajectory );

  taskManager->Connect( "keyboard",  "next",
			"setpoints", devSetPoints::Control );

  taskManager->Connect( "trajectory", devTrajectory::Input,
                        "setpoints",  devSetPoints::Output );

  taskManager->Connect( "trajectory", devTrajectory::Output,
                        "file",       "input" );


  taskManager->CreateAll();
  //taskManager->StartAll();
  kb.Start();
  setpoints.Start();
  osaSleep(1);
  file.Start();
  trajectory.Start();

  getchar();

  return 0;
}

