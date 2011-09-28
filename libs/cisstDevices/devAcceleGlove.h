
#ifndef _devAcceleGlove_h
#define _devAcceleGlove_h

#include <cisstVector/vctFixedSizeMatrix.h>

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsVector.h>
#include <cisstOSAbstraction/osaSerialPort.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devAcceleGlove : public mtsTaskPeriodic {
  
  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION , CMN_LOG_ALLOW_ALL );
  
 public:
  
  enum Errno{ ESUCCESS, EFAILURE };

  enum Finger
  { 
    FTHUMB  = 0,
    FFIRST  = 1,
    FMIDDLE = 2,
    FRING   = 3
    //FRING   = RING_FINGER
  };

 private:

  std::list<double> tangles, fangles, mangles, rangles, pangles, hangles;

  //! Calibration parameters
  vctFixedSizeMatrix<double,3,2> pthumb;
  vctFixedSizeMatrix<double,3,2> pfirst;
  vctFixedSizeMatrix<double,3,2> pmiddle;;
  vctFixedSizeMatrix<double,3,2> pring;
  vctFixedSizeMatrix<double,3,2> ppinky;
  vctFixedSizeMatrix<double,3,2> ppalm;

  //! The serial port
  osaSerialPort serialport;

  //! Position of the palm
  mtsVector<double> palmposition;

  //! Joint angles of the thumb [ distal, proximal, base, rotation ]
  mtsVector<double> thumbangles;

  //! Joint angles of the first finger [ distal, proximal, base ]
  mtsVector<double> firstangles;

  //! Joint angles of the middle finger [ distal, proximal, base ]
  mtsVector<double> middleangles;

  //! Joint angles of the ring finger [ distal, proximal, base ]
  mtsVector<double> ringangles;

  //! Joint angles of the  [ distal, proximal, base ]
  //mtsVector<double> ringposition;

  std::string FingerName( devAcceleGlove::Finger finger ) const;

public:

  devAcceleGlove( const std::string& taskname,
		  double period );
  ~devAcceleGlove();

  void Configure( const std::string& filename = "" );
  void Startup();
  void Cleanup();
  void Run();

  void RxRz( const vctFixedSizeVector<double,3>& axis, 
	     double angle,
	     double& rx, 
	     double& ry );

  static const std::string PalmInterface;
  static const std::string ThumbInterface;
  static const std::string FirstInterface;
  static const std::string MiddleInterface;
  static const std::string RingInterface;
  //static const std::string RingInterfaceName;

  static const std::string GetPalmPosition;
  static const std::string GetThumbAngles;
  static const std::string GetFirstAngles;
  static const std::string GetMiddleAngles;
  static const std::string GetRingAngles;
};

CMN_DECLARE_SERVICES_INSTANTIATION( devAcceleGlove );

#endif
