
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

  //! Return the mode of the trajectory
  devManipulator::Mode GetMode();
  void SetPositionMode() { mtsMode = POSITION; }
  void SetForceTorqueMode() { mtsMode = FORCETORQUE; }
  
  void RunComponent();

  virtual void Read() = 0;
  virtual void Write() = 0;

 public:

  devManipulator( const std::string& taskname, 
		  double period, 
		  devManipulator::State state,
		  osaCPUMask cpumask,
		  devManipulator::Mode mode );
  ~devManipulator(){}
  
  static const std::string Input;
  static const std::string Output;

  static const std::string SetMode;

};

#endif
