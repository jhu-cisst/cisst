

#ifndef _devTrajectory_h
#define _devTrajectory_h

#include <cisstRobot/robFunction.h>

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devTrajectory : public devRobotComponent {

 public:

  enum Mode
  { 
    QUEUE = 0,
    TRACK = 1,
    IDLE  = 2
  };

 private:

  std::vector< robFunction* > functions;

  //! MTS int set/get the mode of the trajectory
  mtsInt mtsMode;

  //! Start time
  double tstart;

  //! Search and return the function that is active at time t
  robFunction* GetCurrentFunction( double t );

  //! Process the input of the trajectory
  void ProcessInput( double t );

  //! Process the output of the trajectory
  void ProcessOutput( double t );


 protected:

  //! Return the mode of the trajectory
  devTrajectory::Mode GetMode();

  //! Test if a new input has been latched on the input interface
  virtual bool IsInputNew() = 0;

  //! Create a new function that will be added to the list of functions
  virtual robFunction* Queue( double t, robFunction* function ) = 0;
  virtual robFunction* Track( double t, robFunction* function ) = 0;

  //! This implements the virtual method
  void RunComponent();
  
  //! Evaluate a function
  virtual void Evaluate( double t, robFunction* function ) = 0;

 public:
  
  devTrajectory( const std::string& name,
		 double period, 
		 devTrajectory::State state,
		 osaCPUMask mask,
		 devTrajectory::Mode mode );

  ~devTrajectory(){}

  void Startup();

  //! Name of the trajectory input interface
  static const std::string Input;

  //! Name of the trajectory output interface
  static const std::string Output;

  static const std::string SetMode;

};

#endif
