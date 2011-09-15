#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstODE/mtsODEWorld.h>
#include <cisstODE/mtsODEBH.h>
#include <cisstODE/mtsODEManipulator.h>
#include <cisstOSG/mtsOSGMono.h>

class WAMMotion : public mtsTaskPeriodic {

private:
  
  mtsFunctionRead  GetPositions;
  mtsFunctionWrite SetPositions;

  vctDynamicVector<double> q;

public:

  WAMMotion() : mtsTaskPeriodic( "WAMMotion", 0.01, true ){

    q.SetSize(7);
    q.SetAll(0.0);

    mtsInterfaceRequired* input = AddInterfaceRequired( "Input" );
    mtsInterfaceRequired* output = AddInterfaceRequired( "Output" );

    input->AddFunction( "GetPositionJoint", GetPositions );
    output->AddFunction( "SetPositionJoint", SetPositions );

  }

  void Configure( const std::string& ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    
    prmPositionJointGet qin;
    GetPositions( qin );
    
    prmPositionJointSet qout;
    qout.SetSize( 7 );
    qout.Goal() = q;
    SetPositions( qout );

    for( size_t i=0; i<q.size(); i++ ) q[i] += 0.001;

  }
  
  void Cleanup(){}

};

class BHMotion : public mtsTaskPeriodic {

private:
  
  mtsFunctionRead  GetPositions;
  mtsFunctionWrite SetPositions;

  vctDynamicVector<double> q;

public:

  BHMotion() : mtsTaskPeriodic( "BHMotion", 0.01, true ){

    q.SetSize(4);
    q.SetAll(0.0);

    mtsInterfaceRequired* input = AddInterfaceRequired( "Input" );
    mtsInterfaceRequired* output = AddInterfaceRequired( "Output" );

    input->AddFunction( "GetPositionJoint", GetPositions );
    output->AddFunction( "SetPositionJoint", SetPositions );

  }

  void Configure( const std::string& ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    
    prmPositionJointGet qin;
    GetPositions( qin );
    
    prmPositionJointSet qout;
    qout.SetSize( 4 );
    qout.Goal() = q;
    SetPositions( qout );
    
    for( size_t i=0; i<q.size(); i++ ) q[i] += 0.001;

  }
  
  void Cleanup(){}

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  // Create the OSG World
  mtsODEWorld* world = new mtsODEWorld( "world", 0.0005 );
  taskManager->AddComponent( world );

  // Create a camera
  int x = 0, y = 0;
  int width = 640, height = 480;
  double Znear = 0.01, Zfar = 10.0;
  mtsOSGMono* camera;
  camera = new mtsOSGMono( "camera", 
			   world,
			   x, y, width, height,
			   55, ((double)width)/((double)height),
			   Znear, Zfar );
  taskManager->AddComponent( camera );


  // Create objects
  std::string wampath( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/" );

  // Create a rigid body. Make up some mass + com + moit
  double mass = 1.0;
  vctFixedSizeVector<double,3> com( 0.0 );
  vctFixedSizeMatrix<double,3,3> moit = vctFixedSizeMatrix<double,3,3>::Eye();
  
  std::string hubblepath( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );
  vctFixedSizeVector<double,3> u( 0.780004, 0.620257, 0.082920 );
  u.NormalizedSelf();
  vctFrame4x4<double> Rtwh( vctAxisAngleRotation3<double>( u, 0.7391 ),
			    vctFixedSizeVector<double,3>( 0.0, 0.5, 1.0 ) );
  osg::ref_ptr<cisstODEBody> hubble;
  hubble = new cisstODEBody( hubblepath+"hst.3ds", world, Rtwh, mass, com, moit );


  std::vector< std::string > wammodels;
  wammodels.push_back( wampath + "l1.obj" );
  wammodels.push_back( wampath + "l2.obj" );
  wammodels.push_back( wampath + "l3.obj" );
  wammodels.push_back( wampath + "l4.obj" );
  wammodels.push_back( wampath + "l5.obj" );
  wammodels.push_back( wampath + "l6.obj" );
  wammodels.push_back( wampath + "l7.obj" );

  mtsODEManipulator* WAM;
  WAM = new mtsODEManipulator( "WAM",
			       0.01,
			       OSA_CPU1,
			       20,
			       wammodels,
			       world,
			       vctFrame4x4<double>(),
			       wampath + "wam7.rob",
			       wampath + "l0.obj",
			       vctDynamicVector<double>( 7, 0.0 ) );
  taskManager->AddComponent( WAM );

  robManipulator robwam( wampath+"wam7.rob", vctFrame4x4<double>() );
  std::string bhpath( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/BH/" );
  vctFrame4x4<double> Rtw0 = robwam.ForwardKinematics( vctDynamicVector<double>( 7, 0.0 ) );
  mtsODEBH* BH;
  BH = new mtsODEBH( "BH",
		     0.01,
		     OSA_CPU1,
		     20,
		     bhpath + "l0.obj",
		     bhpath + "l1.obj",
		     bhpath + "l2.obj",
		     bhpath + "l3.obj",
		     world,
		     Rtw0,
		     bhpath + "f1f2.rob",
		     bhpath + "f3.rob" );
  taskManager->AddComponent( BH );
  
  WAM->Attach( BH );

  WAMMotion wammotion;
  taskManager->AddComponent( &wammotion );

  taskManager->Connect( wammotion.GetName(), "Input",  WAM->GetName(), "Output" );
  taskManager->Connect( wammotion.GetName(), "Output", WAM->GetName(), "Input" );

  BHMotion bhmotion;
  taskManager->AddComponent( &bhmotion );

  taskManager->Connect( bhmotion.GetName(), "Input",  BH->GetName(), "Output" );
  taskManager->Connect( bhmotion.GetName(), "Output", BH->GetName(), "Input" );

  taskManager->CreateAll();
  taskManager->WaitForStateAll( mtsComponentState::READY );

  taskManager->StartAll();
  taskManager->WaitForStateAll( mtsComponentState::ACTIVE );

  std::cout << "ESC to quit" << std::endl;
  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;
}
