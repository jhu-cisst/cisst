
#include <cisstCommon/cmnConstants.h>
#include <cisstDevices/can/devRTSocketCAN.h>
#include <cisstDevices/robotcomponents/manipulators/Barrett/devWAM.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstOSAbstraction/osaGetTime.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devRTSocketCAN can( "rtcan0", devCAN::RATE_1000 );
  vctDynamicVector<double> qinit(7, 0.0);
  qinit[1] = -cmnPI_2;
  qinit[3] =  cmnPI;

  devWAM wam( "WAM", 0.002, OSA_CPU1, &can, qinit );
  wam.Configure();
    
  taskManager->AddComponent( &wam );
  taskManager->CreateAll();
  taskManager->StartAll();

  getchar();

  return 0;
}
