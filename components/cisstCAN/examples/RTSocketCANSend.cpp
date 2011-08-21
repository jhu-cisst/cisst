
#include <cisstCAN/osaRTSocketCAN.h>
#include <native/task.h>
#include <sys/mman.h>

int main( int argc, char** argv ){

  mlockall(MCL_CURRENT | MCL_FUTURE);
  RT_TASK task;
  rt_task_shadow( &task, "RTSocketCANSend", 1, 0);

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  if( argc != 2 ){
    std::cerr << "Usage: " << argv[0] << " rtcan[?]" << std::endl;
    return -1;
  }

  osaRTSocketCAN can( argv[1], 
		      cisstCAN::RATE_1000,
		      cisstCAN::LOOPBACK_ON );
  
  if( can.Open() != cisstCAN::ESUCCESS ){
    std::cerr << argv[0] << ": Failed to open device " << argv[1] << std::endl;
    return -1;
  }

  cisstCANFrame::ID id = 0;
  vctDynamicVector<cisstCANFrame::Data> data( 8, 1, 2, 3, 4, 5, 6, 7, 8 );
  cisstCANFrame frame( id, data );
  std::cout << frame << std::endl;

  if( can.Send( frame ) != cisstCAN::ESUCCESS ){
    std::cerr << argv[0] << ": Failed to send on: " << argv[1] << std::endl;
    return -1;
  }

  if( can.Close() != cisstCAN::ESUCCESS ){
    std::cerr << argv[0] << ": Failed to close device " << argv[1] << std::endl;
    return -1;
  }


  return 0;
}
