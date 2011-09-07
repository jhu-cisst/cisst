#ifndef _devSetPoints_h
#define _devSetPoints_h

#include <list>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSetPoints : public devRobotComponent {

 private:

  class SetPoint{ 
  public:
    enum Type{ RN, R3, SO3, SE3 };    
  private:
    virtual void dummy() const {} // for polymorphism
    Type type; 
  public:
    Type GetType() const { return type; }
    SetPoint( Type type ) : type( type ){}
  };
  
  class SetPointRn : public devSetPoints::SetPoint{ 
  public:
    vctDynamicVector<double> point; 
    SetPointRn( const vctDynamicVector<double>& q ) :
      SetPoint( SetPoint::RN ),
      point( q ){}
  };
  
  class SetPointR3 : public devSetPoints::SetPoint{ 
  public:
    vctFixedSizeVector<double,3> point; 
    SetPointR3( const vctFixedSizeVector<double,3>& t ) :
      SetPoint( SetPoint::R3 ), 
      point( t ){}
  };
  
  class SetPointSO3 : public devSetPoints::SetPoint{ 
  public:
    vctQuaternionRotation3<double> point; 
    SetPointSO3( const vctQuaternionRotation3<double>& q ) :
      SetPoint( SetPoint::SO3 ),
      point( q ){}
  };
  
  class SetPointSE3 : public devSetPoints::SetPoint{ 
  public:
    vctFrame4x4<double> point; 
    SetPointSE3( const vctFrame4x4<double>& Rt ) :
      SetPoint( SetPoint::SE3 ),
      point( Rt ){}
  };
  
  RnIO*  rnoutput;
  R3IO*  r3output;
  SO3IO* so3output;
  SE3IO* se3output;

  std::list< SetPoint* > setpoints;

  mtsBool state;
  bool    stateold;

 public:

  devSetPoints( const std::string& taskname, size_t dim );
  /*
  devSetPoints( const std::string& taskname, const vctDynamicVector<double>& q);

  devSetPoints( const std::string& taskname, const vctFrame4x4<double>& Rt );

  devSetPoints( const std::string& taskname, const vctFrm3& Rt );

  devSetPoints( const std::string& taskname,
		const std::vector< vctDynamicVector<double> >& setpoints );

  devSetPoints( const std::string& taskname,
		const std::vector< vctFixedSizeVector<double,3> >& setpoints );

  devSetPoints(const std::string& taskname,
	       const std::vector< vctQuaternionRotation3<double> >& setpoints);

  devSetPoints( const std::string& taskname,
		const std::vector< vctFrame4x4<double> >& setpoints );
  */
  ~devSetPoints(void) {};

  void RunComponent();

  void Insert( const vctDynamicVector<double>& q );
  void Insert( const vctFrame4x4<double>& Rt );
  void Insert( const vctFrm3& Rt );
  void Latch();
  
  static const std::string OutputRn;
  static const std::string OutputR3;
  static const std::string OutputSO3;
  static const std::string OutputSE3;
  static const std::string NextSetPoint;

};

  
#endif
