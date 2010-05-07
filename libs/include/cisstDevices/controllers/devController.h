
#include <string>

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstRobot/robManipulator.h>

#include <cisstDevices/devExport.h>

#include <cisstMultiTask/mtsTransformationTypes.h>

#ifndef _devController_h
#define _devController_h

class CISST_EXPORT devController: public mtsTaskPeriodic, public robManipulator{

 private:
    
  //! MTS function to write the output of the controller
  mtsFunctionWrite mtsFnSetOutput;

  //! MTS function to read the feedback from the robot
  mtsFunctionRead mtsFnGetFeedback;
  
  double told;

  vctDynamicVector<double> qold;

 protected:
  
  std::string name;

  //! MTS bool to enable the controller's output
  /**
     When enabled is true, the controller writes commanded torques. When
     false the torques are all zero.
  */
  mtsBool mtsEnabled;

  
  bool risingedge;

 public:
  
  devController( const std::string& taskname, 
		 double period, 
		 const std::string& robfile,
		 const vctFrame4x4<double>& Rtw0,
		 bool enabled );

  ~devController(){}

  void Configure( const std::string& = "" ){}
  void Startup();
  void Run();
  void Cleanup(){}
  
  virtual vctDynamicVector<double>  
    Control( const vctDynamicVector<double>& q, 
	     const vctDynamicVector<double>& qd, 
	     double t ) = 0;

  void GetFeedbackJoint(mtsVector<double>& q) const;
  void GetFeedbackCartesian(const mtsVector<double>& qJoint, mtsFrm4x4& qCartesian) const;
    
  static const std::string ControlInterface;
  static const std::string Enable;

  static const std::string InputInterface;

  static const std::string InputJointPosition;
  static const std::string InputJointVelocity;
  static const std::string InputJointAcceleration;

  static const std::string InputPosition;
  static const std::string InputLinearVelocity;
  static const std::string InputLinearAcceleration;

  static const std::string InputOrientation;
  static const std::string InputAngularVelocity;
  static const std::string InputAngularAcceleration;

  static const std::string FeedbackInterface;
  static const std::string Feedback;

  static const std::string FeedbackOutputInterface;
  static const std::string FeedbackOutputJoint;
  static const std::string FeedbackOutputCartesian;

  static const std::string OutputInterface;
  static const std::string Output;

};

#endif
