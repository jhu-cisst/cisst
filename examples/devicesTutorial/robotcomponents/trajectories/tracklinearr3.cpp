

#include <cisstDevices/devKeyboard.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearR3.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <fstream>

using namespace std;

class Source : public devRobotComponent {

private:

  std::ifstream ifs;
  devRobotComponent::R3IO* output;

public:

  Source( const std::string& fname ): 
    devRobotComponent( "Source", 0.2, false ) {

    output = ProvideOutputR3( "Output", devRobotComponent::POSITION );
    ifs.open( fname.data() );

  }
  
  void RunComponent(){
    vctFixedSizeVector<double,3> p( 0.0 );
    for( size_t i=0; i<3; i++ ) { ifs >> p[i]; }
    if( !ifs.eof() ){
      output->SetPosition( p );
      std::cerr << p << std::endl;
    }
  }

};

class Sink : public devRobotComponent {

private:

  std::ofstream ofs;
  R3IO* input;
  double tstart;

public:

  Sink( const std::string& fname ): 
    devRobotComponent( "Sink", 0.01, true ),
    tstart( osaGetTime()) {

    input = ProvideInputR3( "Input", 
			    devRobotComponent::POSITION |
			    devRobotComponent::VELOCITY |
			    devRobotComponent::ACCELERATION );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctFixedSizeVector<double,3> p(0.0);
    double t;
    input->GetPosition( p, t );
    cout << osaGetTime() -tstart << " " << p << endl;
  }

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  double vmax = rand()*3.0/RAND_MAX;

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteFunction( 'e', "EnableSource", 
			  devRobotComponent::EnableCommand, true );
  kb.AddKeyWriteFunction( 'e', "EnableSink",  
			  devRobotComponent::EnableCommand, true );
  kb.AddKeyWriteFunction( 'e', "EnableTraj",  
			  devRobotComponent::EnableCommand, true );
  taskManager->AddComponent( &kb );

  Source source( "sines" );
  taskManager->AddComponent( &source );

  Sink sink( "sink" );
  taskManager->AddComponent( &sink );

  devLinearR3 trajectory( "Trajectory",
			  0.01,
			  false,
			  devTrajectory::TRACK,
			  devTrajectory::POSITION,
			  vctDynamicVector<double>( 3, 0.0 ),
			  vmax );
  taskManager->AddComponent( &trajectory );

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
  osaSleep(1);
  source.Start();
  trajectory.Start();

  getchar();

  return 0;
}

