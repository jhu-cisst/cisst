

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearIK.h>
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

  File( const std::string& fname ): 
    devRobotComponent( "file", 0.001, true ){
    input = ProvideInputRn( "input", 
			    devRobotComponent::POSITION |
			    devRobotComponent::VELOCITY |
			    devRobotComponent::ACCELERATION,
			    7 );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctDynamicVector<double> q;
    double t;
    input->GetPosition( q, t );
    cout << q << std::endl;
    cerr << q << std::endl;
  }

};

double myrand( double low, double high )
{ return (rand()/((double)RAND_MAX)) * fabs( high-low ) - low; }

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  std::string robfile( "libs/etc/cisstRobot/WAM/wam7.rob" );
  robManipulator wam( robfile, vctFrame4x4<double>() );

  vctDynamicVector<double> q1( 7, 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand(  0.0, 3.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -1.0, 2.0 ), 
			       myrand( -2.0, 2.0 ) );

  vctDynamicVector<double> q2( 7, 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand(  0.0, 3.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -1.0, 2.0 ), 
			       myrand( -2.0, 2.0 ) );

  vctDynamicVector<double> q3( 7, 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand(  0.0, 3.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -1.0, 2.0 ), 
			       myrand( -2.0, 2.0 ) );

  vctDynamicVector<double> q4( 7, 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand(  0.0, 3.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -1.0, 2.0 ), 
			       myrand( -2.0, 2.0 ) );

  vctDynamicVector<double> q5( 7, 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand(  0.0, 3.0 ), 
			       myrand( -2.0, 2.0 ), 
			       myrand( -1.0, 2.0 ), 
			       myrand( -2.0, 2.0 ) );
                               
  vctFrame4x4<double> Rt1( wam.ForwardKinematics( q1 ) );
  vctFrame4x4<double> Rt2( wam.ForwardKinematics( q2 ) );
  vctFrame4x4<double> Rt3( wam.ForwardKinematics( q3 ) );
  vctFrame4x4<double> Rt4( wam.ForwardKinematics( q4 ) );
  vctFrame4x4<double> Rt5( wam.ForwardKinematics( q5 ) );

  double v = rand()*1.0/RAND_MAX;
  double w = rand()*1.0/RAND_MAX;

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddComponent( &kb );

  std::vector< vctFrame4x4<double> > Rt;
  Rt.push_back( Rt1 );
  Rt.push_back( Rt2 );
  Rt.push_back( Rt3 );
  Rt.push_back( Rt4 );
  Rt.push_back( Rt1 );
  Rt.push_back( Rt3 );
  Rt.push_back( Rt2 );

  devSetPoints setpoints( "setpoints", Rt );
  taskManager->AddComponent( &setpoints );

  File file( "trajso3" );
  taskManager->AddComponent( &file );

  devLinearIK trajectory( "trajectory",
			  0.001,
			  true,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  v, w,
			  robfile,
			  vctFrame4x4<double>(),
			  q1 );
  taskManager->AddComponent( &trajectory );

  taskManager->Connect( "keyboard",  "next",
			"setpoints", devSetPoints::Control );

  taskManager->Connect( "trajectory", devTrajectory::Input,
                        "setpoints",  devSetPoints::Output );

  taskManager->Connect( "trajectory", devTrajectory::Output,
                        "file",       "input" );


  taskManager->CreateAll();
  //taskManager->StartAll();
  setpoints.Start();
  osaSleep(1);
  kb.Start();
  file.Start();
  trajectory.Start();

  getchar();

  return 0;
}

