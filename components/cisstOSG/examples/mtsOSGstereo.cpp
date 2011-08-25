#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstParameterTypes/prmPositionCartesianGet.h>

#include <cisstOSG/cisstOSGWorld.h>
#include <cisstOSG/mtsOSGStereo.h>
#include <cisstOSG/mtsOSGBody.h>

// Class to move the camera
class CameraMotion : public mtsTaskPeriodic {

private:

  // The position that the camera will be fetching
  prmPositionCartesianGet Rt;

public:

  CameraMotion() : mtsTaskPeriodic( "CameraMotion", 0.01, true ){
    Rt.Position().Translation()[2] = 1;

    StateTable.AddData( Rt, "PositionOrientation" );

    // provide the camera position
    mtsInterfaceProvided* output = AddInterfaceProvided( "Output" );
    output->AddCommandReadState( StateTable, Rt, "GetPositionOrientation" );

  }

  void Configure( const std::string& ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    Rt.Position().Translation()[2] += 0.001;
  }

  void Cleanup(){}

};

// Camera to move an object
class HubbleMotion : public mtsTaskPeriodic {

private:

  // The position that the camera will be fetching
  prmPositionCartesianGet Rt;

  double theta;

public:

  HubbleMotion() : mtsTaskPeriodic( "HubbleMotion", 0.01, true ){
    theta = 0;
    Rt.Position().Translation()[2] = 0.5;

    StateTable.AddData( Rt, "PositionOrientation" );

    // provide the camera position
    mtsInterfaceProvided* output = AddInterfaceProvided( "Output" );
    output->AddCommandReadState( StateTable, Rt, "GetPositionOrientation" );

  }

  void Configure( const std::string& ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();
    
    // rotate hubble
    vctFixedSizeVector<double,3> u( 0.0, 0.0, 1.0 );
    vctAxisAngleRotation3<double> Rwh( u, theta );
    vctFrm3 Rtwh( Rwh, vctFixedSizeVector<double,3>( 0.0, 0.0, 0.5 ) );

    Rt.Position() = Rtwh;
    theta += 0.001;

  }
  
  void Cleanup(){}

};

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  osg::ref_ptr< cisstOSGWorld > world = new cisstOSGWorld;
  
  // Create a camera
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.1, Zfar = 10.0;
  osg::ref_ptr< mtsOSGStereo > camera;
  camera = new mtsOSGStereo( "camera", 
			     world,
			     x, y, 
			     width, height,
			     55.0, ((double)width)/((double)height),
			     Znear, Zfar,
			     0.1,
			     false );
  taskManager->AddComponent( camera.get() );

  // create the camera motion
  CameraMotion cmotion;
  taskManager->AddComponent( &cmotion );

  // create the hubble motion
  HubbleMotion hmotion;
  taskManager->AddComponent( &hmotion );

  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt(  vctMatrixRotation3<double>(),
			   vctFixedSizeVector<double,3>( 0.0, 0.0, 0.5 ) );
  osg::ref_ptr< mtsOSGBody > hubble;
  hubble = new mtsOSGBody( "hubble", path+"hst.3ds", world, Rt );
  taskManager->AddComponent( hubble.get() );

  osg::ref_ptr< cisstOSGBody > background;
  background = new cisstOSGBody( path+"background.3ds", world, 
				 vctFrame4x4<double>() );

  taskManager->Connect( camera->GetName(), "Input",
			cmotion.GetName(), "Output" );

  taskManager->Connect( hubble->GetName(), "Input",
			hmotion.GetName(), "Output" );



  taskManager->CreateAll();
  taskManager->WaitForStateAll( mtsComponentState::READY );

  taskManager->StartAll();
  taskManager->WaitForStateAll( mtsComponentState::ACTIVE );

  std::cout << "ENTER to quit" << std::endl;
  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
