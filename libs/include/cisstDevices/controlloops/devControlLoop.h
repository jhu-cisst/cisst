
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstRobot/robManipulator.h>
#include <cisstDevices/glut/devGeometry.h>
#include <vector>

#ifndef _devControlLoop_h
#define _devControlLoop_h

class devControlLoop : public mtsTaskPeriodic, public robManipulator {
    
protected:
  
  // MTS specific members

  //! MTS bool to enable the controller's output
  /**
     When enabled is true, the controller writes commanded torques. When
     false the torques are all zero.
  */
  mtsBool enabled;

  //! MTS vector of output variables
  mtsVector<double> output;

  //! Function used to acquire the controller's input
  mtsFunctionRead ReadFeedback;

  //! Function to read the reference positions, velocities and accelerations
  mtsFunctionRead ReadReferencePosition;
  mtsFunctionRead ReadReferenceVelocity;
  mtsFunctionRead ReadReferenceAcceleration;
  
  std::vector<devGeometry*> geoms;
  
public:
  
  devControlLoop( const std::string& taskname, 
		  double period, 
		  const std::string& robfile,
		  const vctFrame4x4<double>& Rt = vctFrame4x4<double>(),
		  const std::vector<devGeometry*> geoms = std::vector<devGeometry*>() );

  ~devControlLoop(){}

  void Configure( const std::string& = "" ){}
  void Startup();
  void Run();
  void Cleanup(){}
  
  virtual vctDynamicVector<double> Control(const vctDynamicVector<double>& q)=0;
    
  static const std::string ControlInterface;
  static const std::string ReadEnableCommand;

  static const std::string ReferenceInterface;
  static const std::string ReadReferencePositionCommand;
  static const std::string ReadReferenceVelocityCommand;
  static const std::string ReadReferenceAccelerationCommand;

  static const std::string FeedbackInterface;
  static const std::string ReadFeedbackCommand;

  static const std::string OutputInterface;
  static const std::string WriteOutputCommand;

};

#endif
