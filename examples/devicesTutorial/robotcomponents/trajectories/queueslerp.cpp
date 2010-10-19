

#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devSLERP.h>
#include <cisstDevices/devKeyboard.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <fstream>
using namespace std;

class File : public devRobotComponent {

private:

  std::ofstream ofs;
  SO3IO* input;

public:

  File( const std::string& fname ): 
    devRobotComponent( "file", 0.01, true ){
    input = ProvideInputSO3( "input", 
			     devRobotComponent::POSITION |
			     devRobotComponent::VELOCITY |
			     devRobotComponent::ACCELERATION );
    ofs.open( fname.data() );
  }

  void RunComponent(){
    vctQuaternionRotation3<double> q;
    double t;
    input->GetRotation( q, t );
    cout << vctMatrixRotation3<double>(q) << std::endl;
  }

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

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
  double w = rand()*15.0/RAND_MAX;

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "next", devSetPoints::NextSetPoint, false );
  taskManager->AddComponent( &kb );

  std::vector< vctQuaternionRotation3<double> > q;
  q.push_back( q1 );
  q.push_back( q2 );
  q.push_back( q3 );
  q.push_back( q4 );
  q.push_back( q1 );
  q.push_back( q3 );
  q.push_back( q2 );

  devSetPoints setpoints( "setpoints", q );
  taskManager->AddComponent( &setpoints );

  File file( "trajso3" );
  taskManager->AddComponent( &file );

  devSLERP trajectory( "trajectory",
		       0.01,
		       true,
		       devTrajectory::QUEUE,
		       devTrajectory::POSITION,
		       q1,
		       w );
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
  file.Start();
  trajectory.Start();

  getchar();

  return 0;
}

