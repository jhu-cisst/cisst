#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>

#include <cisstBarrett/osaWAM.h>
#include <cisstCAN/osaRTSocketCAN.h>
#include <native/task.h>
#include <sys/mman.h>

int main( int argc, char** argv ){

  mlockall(MCL_CURRENT | MCL_FUTURE);
  RT_TASK task;
  rt_task_shadow( &task, "GroupTest", 99, 0 );

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  if( argc != 2 ){
    std::cout << "Usage: " << argv[0] << " rtcan[0-1]" << std::endl;
    return -1;
  }

  osaRTSocketCAN can( argv[1], cisstCAN::RATE_1000 );

  if( can.Open() != cisstCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << argv[0] << "Failed to open " << argv[1] << std::endl;
    return -1;
  }

  osaWAM WAM( &can );

  if( WAM.Initialize() != osaWAM::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to initialize WAM" << std::endl;
    return -1;
  }

  double t1 = osaGetTime();
  size_t cnt=0;

  while( 1 ){

    vctDynamicVector<double> q, t(7, 0.0);
    WAM.GetPositions( q );
    WAM.SetTorques( t );

    cnt++;
    if( cnt == 1000 ){
      double t2 = osaGetTime();
      std::cout << 1000.0 / (t2 - t1) << std::endl;
      t1 = t2;
      cnt = 0;
    }

  }

  return 0;
}
