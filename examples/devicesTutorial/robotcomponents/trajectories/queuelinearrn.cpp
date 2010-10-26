

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <fstream>

using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  RnIO* input;

public:

  File( const std::string& fname, size_t N ): 
    devRobotComponent( "file", 0.001, true ){
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
                               rand()*10.0/RAND_MAX,
                               rand()*10.0/RAND_MAX,
                               rand()*10.0/RAND_MAX );
  vctDynamicVector<double> qdd( 3, 
				rand()*10.0/RAND_MAX, 
				rand()*10.0/RAND_MAX, 
				rand()*10.0/RAND_MAX );

  devKeyboard keyboard;
  keyboard.SetQuitKey('q');
  keyboard.AddKeyWriteFunction('n', "next", devSetPoints::NextSetPoint, true );
  taskManager->AddComponent( &keyboard );

  std::vector< vctDynamicVector<double> > q;
  q.push_back( q1 );
  q.push_back( q2 );
  q.push_back( q3 );
  q.push_back( q4 );
  q.push_back( q1 );
  q.push_back( q3 );
  q.push_back( q2 );

  devSetPoints setpoints( "setpoints", q );
  taskManager->AddComponent( &setpoints );

  File file( "trajrn", 3 );
  taskManager->AddComponent( &file );

  devLinearRn trajectory( "trajectory",
			  0.001,
			  true,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  q1,
			  qd );
  taskManager->AddComponent( &trajectory );

  taskManager->Connect( keyboard.GetName(),  "next",
			setpoints.GetName(), devSetPoints::Control );

  taskManager->Connect( trajectory.GetName(), devTrajectory::Input,
                        setpoints.GetName(),  devSetPoints::Output );

  taskManager->Connect( trajectory.GetName(), devTrajectory::Output,
                        file.GetName(),       "input" );


  taskManager->CreateAll();

  keyboard.Start();
  setpoints.Start();

  while( !setpoints.IsRunning() )  osaSleep(0.01);
  trajectory.Start();

  while( !trajectory.IsRunning() ) osaSleep(0.01);
  file.Start();

  pause();

  return 0;
}

