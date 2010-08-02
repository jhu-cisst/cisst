

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devQLQRn.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <fstream>
using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  RnIO* input;

public:

  File( const std::string& fname, size_t N ): 
    devRobotComponent( "file", 0.01, true ){
    input = ProvideInputRn( "input", 
			    devRobotComponent::POSITION |
			    devRobotComponent::VELOCITY |
			    devRobotComponent::ACCELERATION,
			    N );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctDynamicVector<double> q;
    double t;
    input->GetPosition( q, t );
    cout << q << std::endl;
  }

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  vctDynamicVector<double> q1( 3,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0 );
  vctDynamicVector<double> q2( 3,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0 );
  vctDynamicVector<double> q3( 3,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0 );
  vctDynamicVector<double> q4( 3,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0,
                               rand()*10.0/RAND_MAX-5.0 );
  vctDynamicVector<double> qd( 3,
                               rand()*3.0/RAND_MAX,
                               rand()*3.0/RAND_MAX,
                               rand()*3.0/RAND_MAX );
  vctDynamicVector<double> qdd( 3, 
				rand()*10.0/RAND_MAX, 
				rand()*10.0/RAND_MAX, 
				rand()*10.0/RAND_MAX );

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddTask( &kb );

  std::vector< vctDynamicVector<double> > q;
  q.push_back( q1 );
  q.push_back( q2 );
  q.push_back( q3 );
  q.push_back( q4 );
  q.push_back( q1 );
  q.push_back( q3 );
  q.push_back( q2 );

  devSetPoints setpoints( "setpoints", q );
  taskManager->AddTask( &setpoints );

  File file( "trajrn", 3 );
  taskManager->AddTask( &file );

  devQLQRn trajectory( "trajectory",
		       0.01,
		       true,
		       devTrajectory::QUEUE,
		       devTrajectory::POSITION,
		       q1,
		       qd,
		       qdd );
  taskManager->AddTask( &trajectory );

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
  file.Start();
  trajectory.Start();

  pause();

  return 0;
}

