

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearSE3.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <fstream>
using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  SE3IO* input;

public:

  File( const std::string& fname ): 
    devRobotComponent( "file", 0.01, true ){
    input = ProvideInputSE3( "input", 
			     devRobotComponent::POSITION |
			     devRobotComponent::VELOCITY |
			     devRobotComponent::ACCELERATION );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctFrame4x4<double> Rt;
    double t;
    input->GetPosition( Rt, t );
    cout << Rt << std::endl;
  }

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  vctFixedSizeVector<double,3> t1( rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> t2( rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> t3( rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> t4( rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0 );
  vctFixedSizeVector<double,3> t5( rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0, 
				   rand()*10.0/RAND_MAX-5.0 ); 

 vctFixedSizeVector<double,4> v1, v2, v3, v4, v5;
  for( int i=0; i<4; i++ ){
    v1[i] = rand()*10.0/RAND_MAX - 5.0;
    v2[i] = rand()*10.0/RAND_MAX - 5.0;
    v3[i] = rand()*10.0/RAND_MAX - 5.0;
    v4[i] = rand()*10.0/RAND_MAX - 5.0;
    v5[i] = rand()*10.0/RAND_MAX - 5.0;
  }

  try{ v1.NormalizedSelf(); } catch( std::runtime_error ){}
  try{ v2.NormalizedSelf(); } catch( std::runtime_error ){}
  try{ v3.NormalizedSelf(); } catch( std::runtime_error ){}
  try{ v4.NormalizedSelf(); } catch( std::runtime_error ){}
  try{ v5.NormalizedSelf(); } catch( std::runtime_error ){}

  vctQuaternionRotation3<double> q1( v1[0], v1[1], v1[2], v1[3],VCT_NORMALIZE );
  vctQuaternionRotation3<double> q2( v2[0], v2[1], v2[2], v2[3],VCT_NORMALIZE );
  vctQuaternionRotation3<double> q3( v3[0], v3[1], v3[2], v3[3],VCT_NORMALIZE );
  vctQuaternionRotation3<double> q4( v4[0], v4[1], v4[2], v4[3],VCT_NORMALIZE );
  vctQuaternionRotation3<double> q5( v5[0], v5[1], v5[2], v5[3],VCT_NORMALIZE );

  vctFrame4x4<double> Rt1( q1, t1 );
  vctFrame4x4<double> Rt2( q2, t2 );
  vctFrame4x4<double> Rt3( q3, t3 );
  vctFrame4x4<double> Rt4( q4, t4 );
  vctFrame4x4<double> Rt5( q5, t5 );

  double v = rand()*5.0/RAND_MAX;
  double w = rand()*5.0/RAND_MAX;

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddTask( &kb );

  std::vector< vctFrame4x4<double> > Rt;
  Rt.push_back( Rt1 );
  Rt.push_back( Rt2 );
  Rt.push_back( Rt3 );
  Rt.push_back( Rt4 );
  Rt.push_back( Rt1 );
  Rt.push_back( Rt3 );
  Rt.push_back( Rt2 );

  devSetPoints setpoints( "setpoints", Rt );
  taskManager->AddTask( &setpoints );

  File file( "trajso3" );
  taskManager->AddTask( &file );

  devLinearSE3 trajectory( "trajectory",
			   0.01,
			   true,
			   devTrajectory::QUEUE,
			   devTrajectory::POSITION,
			   Rt1,
			   v, w );
  taskManager->AddTask( &trajectory );

  taskManager->Connect( "keyboard",  "next",
			"setpoints", devSetPoints::Control );

  taskManager->Connect( "trajectory", devTrajectory::Input,
                        "setpoints",  devSetPoints::Output );

  taskManager->Connect( "trajectory", devTrajectory::Output,
                        "file",       "input" );


  taskManager->CreateAll();
  //taskManager->StartAll();
  setpoints.Start();
  usleep(100000);
  kb.Start();
  file.Start();
  trajectory.Start();

  pause();

  return 0;
}

