

#include <cisstDevices/devKeyboard.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <fstream>
#include <cisstOSAbstraction/osaGetTime.h>

using namespace std;

class Source : public devRobotComponent {

private:

  std::ifstream ifs;
  RnIO* output;
  size_t N;

public:

  Source( const std::string& fname, size_t N ): 
    devRobotComponent( "Source", 0.2, false ),
    N( N ) {

    output = ProvideOutputRn( "Output", devRobotComponent::POSITION, N );
    ifs.open( fname.data() );

  }
  
  void RunComponent(){
    vctDynamicVector<double> q( N, 0.0 );
    for( size_t i=0; i<N; i++ ) { ifs >> q[i]; }
    if( !ifs.eof() ){
      output->SetPosition( q );
      std::cerr << q << std::endl;
    }
  }

};

class Sink : public devRobotComponent {

private:

  std::ofstream ofs;
  RnIO* input;
  double tstart;

public:

  Sink( const std::string& fname, size_t N ): 
    devRobotComponent( "Sink", 0.01, true ),
    tstart( osaGetTime()) {

    input = ProvideInputRn( "Input", 
			    devRobotComponent::POSITION |
			    devRobotComponent::VELOCITY |
			    devRobotComponent::ACCELERATION,
			    N );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctDynamicVector<double> q(3, 0.0), qd(3, 0.0), qdd(3, 0.0);
    double t;
    input->GetPosition( q, t );
    cout << osaGetTime() -tstart<< " " << q << endl;
  }

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  vctDynamicVector<double> qd( 3,
                               rand()*3.0/RAND_MAX,
                               rand()*3.0/RAND_MAX,
                               rand()*3.0/RAND_MAX );

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('e', "EnableSource",
			devRobotComponent::EnableCommand, true );
  kb.AddKeyWriteCommand('e', "EnableSink",  
			devRobotComponent::EnableCommand, true );
  kb.AddKeyWriteCommand('e', "EnableTraj",  
			devRobotComponent::EnableCommand, true );
  taskManager->AddTask( &kb );

  Source source( "sines", 3 );
  taskManager->AddTask( &source );

  Sink sink( "sink", 3 );
  taskManager->AddTask( &sink );

  devLinearRn trajectory( "Trajectory",
			  0.01,
			  false,
			  devTrajectory::TRACK,
			  devTrajectory::POSITION,
			  vctDynamicVector<double>(3, 0.0),
			  qd );
  taskManager->AddTask( &trajectory );

  taskManager->Connect( "keyboard",  "EnableSource",
			"Source",     Source::Control );

  taskManager->Connect( "keyboard",  "EnableSink",
			"Sink",       Sink::Control );

  taskManager->Connect( "keyboard",  "EnableTraj",
			"Trajectory", devTrajectory::Control );

  taskManager->Connect( "Trajectory", devTrajectory::Input,
                        "Source",     "Output" );

  taskManager->Connect( "Trajectory", devTrajectory::Output,
                        "Sink",       "Input" );

  taskManager->CreateAll();
  kb.Start();
  sink.Start();
  usleep(1000000);
  source.Start();
  trajectory.Start();

  pause();

  return 0;
}

