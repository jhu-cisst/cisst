

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstDevices/devExport.h>

#ifndef _devTrajectory_h
#define _devTrajectory_h

class CISST_EXPORT devTrajectory : public mtsTaskPeriodic{

 protected:
  
  //! Function to read the input to the trajectory
  mtsFunctionRead mtsFnGetInput;

  //! Function to set the position command of a controller
  mtsFunctionWrite mtsFnSetJointsPosition;
  mtsFunctionWrite mtsFnSetPosition;
  mtsFunctionWrite mtsFnSetOrientation;

  //! Function to set the velocity command of a controller
  mtsFunctionWrite mtsFnSetJointsVelocity;
  mtsFunctionWrite mtsFnSetLinearVelocity;
  mtsFunctionWrite mtsFnSetAngularVelocity;

  //! Function to set the acceleration command of a controller
  mtsFunctionWrite mtsFnSetJointsAcceleration;
  mtsFunctionWrite mtsFnSetLinearAcceleration;
  mtsFunctionWrite mtsFnSetAngularAcceleration;

  //! MTS bool to enable the controller's output
  /**
     When enabled is true, the controller writes commanded torques. When
     false the torques are all zero.
  */
  mtsBool mtsEnabled;

  //! Status of the trajectory
  mtsInt  mtsStatus;

  //! Detect the rising edge of the enable signal
  bool risingedge;

  //! Start time
  double tstart;

  vctDynamicVector<double> inputold;

  static const int EXPIRED = 1;

  enum Space{ JOINTS = 0x01, 
	      R3     = 0x02, 
	      SO3    = 0x04,
	      SE3    = 0x08 };

  devTrajectory::Space space;

 public:

  devTrajectory( const std::string& TaskName,
		 const std::string& InputFunctionName,
		 double period, 
		 bool enabled,
		 devTrajectory::Space space,
		 const vctDynamicVector<double>& yinit );

  ~devTrajectory(){}

  void Configure( const std::string& = "" ){}
  void Startup();
  void Run();
  void Cleanup(){}

  virtual void Reset( double t,
		      const vctDynamicVector<double>& newy ) = 0;

  virtual void Evaluate( double t,
			 vctDynamicVector<double>& y,
			 vctDynamicVector<double>& yd,
			 vctDynamicVector<double>& ydd ) = 0;
  
  static const std::string ControlInterface;
  static const std::string Enable;

  static const std::string InputInterface;
  static const std::string OutputInterface;

};

#endif
