
#ifndef _devRobotComponent_h
#define _devRobotComponent_h

#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <cisstOSAbstraction/osaCPUAffinity.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devRobotComponent : public mtsTaskPeriodic {

 public:

  enum Variables
  {
    POSITION     = 0x01,
    VELOCITY     = 0x02,
    ACCELERATION = 0x04,
    FORCETORQUE  = 0x08
  };
  
  friend
    devRobotComponent::Variables operator|( devRobotComponent::Variables v1, 
					    devRobotComponent::Variables v2 )
  { return devRobotComponent::Variables( int(v1) | int(v2) ); }
    

 protected:

  //! Base IO class for a component
  class IO { 

  public:
    
    enum Type
      {
	PROVIDE_INPUT,
	PROVIDE_OUTPUT,
	REQUIRE_INPUT,
	REQUIRE_OUTPUT
      };
    
  private:
    
    mtsTask* task;
    
    IO::Type      type;
    devRobotComponent::Variables variables;

  protected:
    
    mtsInterfaceProvided* pinterface;
    mtsInterfaceRequired* rinterface;
    
    IO::Type IOType() const;
    
    bool PositionEnabled()     const;
    bool VelocityEnabled()     const;
    bool AccelerationEnabled() const;
    bool ForceTorqueEnabled()  const;
    
    std::string ReadPositionFnName;
    std::string ReadVelocityFnName;
    std::string ReadAccelerationFnName;
    std::string ReadForceTorqueFnName;

    mtsFunctionRead mtsFnReadPosition;
    mtsFunctionRead mtsFnReadVelocity;
    mtsFunctionRead mtsFnReadAcceleration;
    mtsFunctionRead mtsFnReadForceTorque;
    
    std::string WritePositionFnName;
    std::string WriteVelocityFnName;
    std::string WriteAccelerationFnName;
    std::string WriteForceTorqueFnName;

    mtsFunctionWrite mtsFnWritePosition;
    mtsFunctionWrite mtsFnWriteVelocity;
    mtsFunctionWrite mtsFnWriteAcceleration;
    mtsFunctionWrite mtsFnWriteForceTorque;
    
    mtsStateTable* StateTable();
    
  public:
    
    IO( mtsTask* task, 
	const std::string& name, 
	IO::Type type,
	devRobotComponent::Variables variables );
    
    virtual ~IO();
    
  };  // class IO
  
 protected:

  class CISST_EXPORT RnIO : public IO {
    
  private:
    
    mtsVector<double> mtsq;
    mtsVector<double> mtsqd;
    mtsVector<double> mtsqdd;
    mtsVector<double> mtsft;

    void CreatePositionIO( size_t N );
    void CreateVelocityIO( size_t N );
    void CreateAccelerationIO( size_t N );
    void CreateForceTorqueIO( size_t N );

  public:
    
    RnIO( mtsTask* task,
	     const std::string& name,
	     devRobotComponent::IO::Type type,
	     devRobotComponent::Variables variables,
	     size_t N );
    
    void SetPosition    ( const vctDynamicVector<double>& q   );
    void SetVelocity    ( const vctDynamicVector<double>& qd  );
    void SetAcceleration( const vctDynamicVector<double>& qdd );
    void SetForceTorque ( const vctDynamicVector<double>& ft  );
        
    void GetPosition    ( vctDynamicVector<double>& q,   double& t );
    void GetVelocity    ( vctDynamicVector<double>& qd,  double& t );
    void GetAcceleration( vctDynamicVector<double>& qdd, double& t );
    void GetForceTorque ( vctDynamicVector<double>& ft,  double& t );

  };  // RnIO



  class CISST_EXPORT R3IO : public IO {
    
  private:
    
    mtsVector<double> mtsp;
    mtsVector<double> mtsv;
    mtsVector<double> mtsvd;
    mtsVector<double> mtsf;

    void CreatePositionIO();
    void CreateVelocityIO();
    void CreateAccelerationIO();
    void CreateForceIO();

  public:
    
    R3IO( mtsTask* task,
	  const std::string& name,
	  devRobotComponent::IO::Type type,
	  devRobotComponent::Variables variables );
    
    void SetPosition    ( const vctFixedSizeVector<double,3>& p  );
    void SetVelocity    ( const vctFixedSizeVector<double,3>& v  );
    void SetAcceleration( const vctFixedSizeVector<double,3>& vd );
    void SetForce       ( const vctFixedSizeVector<double,3>& f  );
        
    void GetPosition    ( vctFixedSizeVector<double,3>& p,  double& t );
    void GetVelocity    ( vctFixedSizeVector<double,3>& v,  double& t );
    void GetAcceleration( vctFixedSizeVector<double,3>& vd, double& t );
    void GetForce       ( vctFixedSizeVector<double,3>& f,  double& t );

  };  // R3IO



  class CISST_EXPORT SE3IO : public IO {

  private:
    
    mtsDoubleFrm4x4 mtsRt;
    mtsVector<double> mtsvw;
    mtsVector<double> mtsvdwd;
    mtsVector<double> mtsft;

    void CreatePositionIO( );
    void CreateVelocityIO( );
    void CreateAccelerationIO( );
    void CreateForceTorqueIO( );

  public:

    SE3IO( mtsTask* task,
	   const std::string& name,
	   devRobotComponent::IO::Type type,
	   devRobotComponent::Variables variables );

    void SetPosition    ( const vctFrame4x4<double>& Rt            );
    void SetVelocity    ( const vctFixedSizeVector<double,6>& vw   );
    void SetAcceleration( const vctFixedSizeVector<double,6>& vdwd );
    void SetForceTorque ( const vctFixedSizeVector<double,6>& ft   );

    void GetPosition    ( vctFrame4x4<double>& Rt,            double& t );
    void GetVelocity    ( vctFixedSizeVector<double,6>& vw,   double& t );
    void GetAcceleration( vctFixedSizeVector<double,6>& vdwd, double& t );
    void GetForceTorque ( vctFixedSizeVector<double,6>& ft ,  double& t );    

  }; // End SE3iO


  class CISST_EXPORT SO3IO : public IO {

  private:
    
    mtsDoubleQuatRot3 mtsq;
    mtsVector<double> mtsw;
    mtsVector<double> mtswd;
    mtsVector<double> mtstau;

    void CreateRotationIO( );
    void CreateVelocityIO( );
    void CreateAccelerationIO( );
    void CreateTorqueIO( );

  public:

    SO3IO( mtsTask* task,
	   const std::string& name,
	   devRobotComponent::IO::Type type,
	   devRobotComponent::Variables variables );
    
    void SetRotation    ( const vctQuaternionRotation3<double>& q );
    void SetVelocity    ( const vctFixedSizeVector<double,3>& w   );
    void SetAcceleration( const vctFixedSizeVector<double,3>& wd  );
    void SetTorque      ( const vctFixedSizeVector<double,3>& tau );

    void GetRotation    ( vctQuaternionRotation3<double>& q, double& t );
    void GetVelocity    ( vctFixedSizeVector<double,3>& w,   double& t );
    void GetAcceleration( vctFixedSizeVector<double,3>& wd,  double& t );
    void GetTorque      ( vctFixedSizeVector<double,3>& tau, double& t );    

  }; // End SO3IO


 public:

  enum State
  {
    DISABLED,
    ENABLED
  };

 private:

  osaCPUMask cpumask;

  devRobotComponent::State state;
  bool statechange;
  
 protected:

  osaCPUMask GetCPUMask() const { return cpumask; }

  devRobotComponent::State GetState() const;

  devRobotComponent( const std::string& name, 
		     double period,
		     devRobotComponent::State state,
		     osaCPUMask mask = OSA_CPUANY );

 public:

  void Configure(){}
  void Startup();
  void Run();
  void Cleanup(){}

  void EnableCommand() { state = devRobotComponent::ENABLED; }
  void DisableCommand() { state = devRobotComponent::DISABLED; }

  static const std::string Control;
  static const std::string Enable;
  static const std::string Disable;

 protected:

  RnIO* ProvideOutputRn( const std::string& name,
			 devRobotComponent::Variables variables,
			 size_t N );

  RnIO* ProvideInputRn( const std::string& name,
			devRobotComponent::Variables variables,
			size_t N );

  RnIO* RequireOutputRn( const std::string& name,
			 devRobotComponent::Variables variables,
			 size_t N );

  RnIO* RequireInputRn( const std::string& name,
			devRobotComponent::Variables variables,
			size_t N );

  R3IO* ProvideOutputR3( const std::string& name,
			 devRobotComponent::Variables variables );

  R3IO* ProvideInputR3( const std::string& name,
			devRobotComponent::Variables variables );

  R3IO* RequireOutputR3( const std::string& name,
			 devRobotComponent::Variables variables );

  R3IO* RequireInputR3( const std::string& name,
			devRobotComponent::Variables variables );


  SO3IO* ProvideOutputSO3( const std::string& name,
			   devRobotComponent::Variables variables );
  
  SO3IO* ProvideInputSO3( const std::string& name,
			  devRobotComponent::Variables variables );
  
  SO3IO* RequireOutputSO3( const std::string& name,
			   devRobotComponent::Variables variables );

  SO3IO* RequireInputSO3( const std::string& name,
			  devRobotComponent::Variables variables );


  SE3IO* ProvideOutputSE3( const std::string& name,
			   devRobotComponent::Variables variables );

  SE3IO* ProvideInputSE3( const std::string& name,
			  devRobotComponent::Variables variables );
  
  SE3IO* RequireOutputSE3( const std::string& name,
			   devRobotComponent::Variables variables );
  
  SE3IO* RequireInputSE3( const std::string& name,
			  devRobotComponent::Variables variables );
  
  virtual void RunComponent(){}

};


#endif

