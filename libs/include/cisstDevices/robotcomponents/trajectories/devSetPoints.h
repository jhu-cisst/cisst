

#include <cisstCommon.h>
#include <list>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/devExport.h>

#ifndef _devSetPoints_h
#define _devSetPoints_h

class CISST_EXPORT devSetPoints : public devRobotComponent {

 private:

  RnIO*  rnoutput;
  R3IO*  r3output;
  SO3IO* so3output;
  SE3IO* se3output;

  std::vector< vctFixedSizeVector<double,3> >    r3setpoints;
  std::vector< vctDynamicVector<double> >        rnsetpoints;
  std::vector< vctQuaternionRotation3<double> > so3setpoints;
  std::vector< vctFrame4x4<double> >            se3setpoints;
  size_t cnt;

  mtsBool state;
  bool    stateold;

 public:

  devSetPoints( const std::string& taskname,
		const vctDynamicVector<double>& q );

  devSetPoints( const std::string& taskname,
		const std::vector< vctDynamicVector<double> >& setpoints );

  devSetPoints( const std::string& taskname,
		const std::vector< vctFixedSizeVector<double,3> >& setpoints );

  devSetPoints(const std::string& taskname,
	       const std::vector< vctQuaternionRotation3<double> >& setpoints);

  devSetPoints( const std::string& taskname,
		const std::vector< vctFrame4x4<double> >& setpoints );

  ~devSetPoints(void) {};

  void RunComponent();

  void Insert( const vctDynamicVector<double>& q );
  void Latch();
  
  static const std::string Output;
  static const std::string NextSetPoint;

};

  
#endif
