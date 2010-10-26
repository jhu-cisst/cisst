

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearR3.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <fstream>

using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  R3IO* input;

public:

  File( const std::string& fname ): 
    devRobotComponent( "file", 0.001, true ){
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

  devKeyboard keyboard;
  keyboard.SetQuitKey('q');
  keyboard.AddKeyWriteFunction('n', "next", devSetPoints::NextSetPoint, true );
  taskManager->AddComponent( &keyboard );

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
			  0.001,
			  true,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  p1,
			  v );
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

