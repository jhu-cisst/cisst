

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearR3.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <fstream>
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
  kb.AddKeyWriteCommand('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddTask( &kb );

  std::vector< vctFixedSizeVector<double,3> > p;
  p.push_back( p1 );
  p.push_back( p2 );
  p.push_back( p3 );
  p.push_back( p4 );
  p.push_back( p1 );
  p.push_back( p3 );
  p.push_back( p2 );

  devSetPoints setpoints( "setpoints", p );
  taskManager->AddTask( &setpoints );

  File file( "trajrn" );
  taskManager->AddTask( &file );

  devLinearR3 trajectory( "trajectory",
			  0.01,
			  true,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  p1,
			  v );
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
  usleep(1000000);
  file.Start();
  trajectory.Start();

  pause();

  return 0;
}

