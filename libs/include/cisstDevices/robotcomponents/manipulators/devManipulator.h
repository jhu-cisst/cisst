
#ifndef _devManipulator_h
#define _devManipulator_h

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devManipulator : public devRobotComponent {

 public:

  enum Mode
  { 
    POSITION    = 0,
    VELOCITY    = 1,
    FORCETORQUE = 2,
    IDLE        = 3
  };


 private:

  //! MTS int set/get the mode of the trajectory
  mtsInt mtsMode;

 protected:

  //! Return the mode of the manipulator
  devManipulator::Mode GetMode();

  //! Set the manipulator in position mode
  void SetPositionMode() { mtsMode = POSITION; }

  //! Set the manipulator in force/torque mode
  void SetForceTorqueMode() { mtsMode = FORCETORQUE; }
  
  //! Implement devRobotComponent::RunComponent()
  virtual void RunComponent();

  //! Read the state of the manipulator
  virtual void Read() = 0;

  //! Write the state of the manipulator
  virtual void Write() = 0;

 public:

  //! Default constructor
  devManipulator( const std::string& taskname, 
		  double period, 
		  devManipulator::State state,
		  osaCPUMask cpumask,
		  devManipulator::Mode mode );

  //! Default destructor
  ~devManipulator(){}
  
  static const std::string Input;
  static const std::string Output;

  static const std::string SetMode;

};

#endif
