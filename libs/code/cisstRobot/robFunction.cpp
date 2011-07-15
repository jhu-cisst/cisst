#include <cisstRobot/robFunction.h>
#include <cisstCommon/cmnLogger.h>

robFunction::robFunction( double t1, double t2 ) : t1( t1 ), t2( t2 ){
  
  if( t2 < t1 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": t2 = " << t2 
		      << " is less than or equal to t1 = " << t1
		      << std::endl;
    
  }
}

double& robFunction::StartTime() { return t1; }
double& robFunction::StopTime()  { return t2; }
double robFunction::Duration() const{ return t2-t1; }

