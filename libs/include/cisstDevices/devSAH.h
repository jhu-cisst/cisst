
#ifndef _devSAH_h
#define _devSAH_h

#include <SAHandCtrlApi.h>

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsVector.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSAH : public mtsTaskPeriodic {
  
  CMN_DECLARE_SERVICES( CMN_NO_DYNAMIC_CREATION , CMN_LOG_ALLOW_ALL );
  
 public:
  
  enum Errno{ ESUCCESS, EFAILURE };

  enum Configuration
  { 
    CRIGHT = HAND_CONFIG_RIGHT, 
    CLEFT  = HAND_CONFIG_LEFT 
  };

  enum Port
  { 
    P1 = PORT_1, 
    P2 = PORT_2 
  };

  enum Controller
  { 
    CRESET     = CONTROLLER_RESET,
    CDIRECT    = CONTROLLER_DIRECT,
    CPOSITION  = CONTROLLER_POSITION, 
    CIMPEDANCE = CONTROLLER_IMPEDANCE
  };

  enum Finger
  { 
    FTHUMB  = THUMB,
    FFIRST  = FIRST_FINGER, 
    FMIDDLE = MIDDLE_FINGER,
    FRING   = RING_FINGER
  };

 private:

  //! MTS function used to query the thumb position
  mtsFunctionRead SetThumbPosition;

  //! MTS function used to query the first finger position
  mtsFunctionRead SetFirstPosition;

  //! MTS function used to query the middle finger position
  mtsFunctionRead SetMiddlePosition;

  //! MTS function used to query the ring finger position
  mtsFunctionRead SetRingPosition;

  CSAHandCtrlApi sah;         // the hand
  devSAH::Port port;          // the port

  std::string FingerName( devSAH::Finger finger ) const;

  devSAH::Errno EnableFinger( devSAH::Finger );
  devSAH::Errno DisableFinger( devSAH::Finger );

public:

  devSAH( const std::string& taskname,
	  double period,
	  devSAH::Port port = devSAH::P1, 
	  devSAH::Controller controller = devSAH::CPOSITION );

  ~devSAH();

  void Configure( const std::string& filename = "" );
  void Startup();
  void Cleanup();
  void Run();

  void move(double q1);

  static const std::string ThumbInterfaceName;
  static const std::string FirstInterfaceName;
  static const std::string MiddleInterfaceName;
  static const std::string RingInterfaceName;

  static const std::string SetThumbPositionCmd;
  static const std::string SetFirstPositionCmd;
  static const std::string SetMiddlePositionCmd;
  static const std::string SetRingPositionCmd;
};

CMN_DECLARE_SERVICES_INSTANTIATION( devSAH );

#endif
