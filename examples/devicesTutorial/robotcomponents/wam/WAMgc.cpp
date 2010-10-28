#include <cisstDevices/can/devRTSocketCAN.h>

#include <cisstCommon/cmnConstants.h>

#include <cisstDevices/devKeyboard.h>
#include <cisstDevices/robotcomponents/controllers/devGravityCompensation.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devWAM.h>

#include <cisstMultiTask/mtsTaskManager.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devKeyboard kb;
  kb.SetQuitKey( 'q' );
  kb.AddKeyWriteFunction('G', "ctrlenable", devController::EnableCommand, true);
  taskManager->AddComponent( &kb );

  devRTSocketCAN can( "rtcan0", devCAN::RATE_1000 );

  vctDynamicVector<double> qinit(7, 0.0);
  qinit[1] = -cmnPI_2;
  qinit[3] =  cmnPI;

  devWAM wam( "WAM", 0.002, OSA_CPU1, &can, qinit );
  wam.Configure();
  taskManager->AddComponent( &wam );

  std::string path("libs/etc/cisstRobot/WAM/");
  vctMatrixRotation3<double> Rw0(  0.0,  0.0, -1.0, 
				   0.0,  1.0,  0.0, 
				   1.0,  0.0,  0.0 );
  vctFixedSizeVector<double,3> tw0(0.0);
  vctFrame4x4<double> Rtw0( Rw0, tw0 );
  devGravityCompensation gc( "controller", 
			     0.002,
			     devController::DISABLED,
			     OSA_CPU2,
			     path+"wam7.rob",
			     Rtw0 );
  taskManager->AddComponent( &gc );

  taskManager->Connect( kb.GetName(), "ctrlenable",
                        gc.GetName(), devController::Control );

  taskManager->Connect( gc.GetName(), devController::Output,
			wam.GetName(),devManipulator::Input );

  taskManager->Connect( gc.GetName(),   devController::Feedback,
			wam.GetName(),  devManipulator::Output );

  taskManager->CreateAll();
  taskManager->StartAll();

  pause();

  return 0;
}
