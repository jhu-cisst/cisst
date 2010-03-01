/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robTrajectoryR1.h>

robTrajectoryR1::robTrajectoryR1( robSpace::Basis codomain, 
				  double t,
				  double xdmax, 
				  double xddmax ) :
  robTrajectory(codomain & ( robSpace::JOINTS_POS | robSpace::TRANSLATION ), t){

  this->xdmax = xdmax;
  this->xddmax = xddmax;

}

double robTrajectoryR1::TransitionTime(  const robTrajectory::Segment& si, 
					 const robTrajectory::Segment& sj ){

  // Ensure that the time values are greater than zero
  if( (si.tf<0) || (sj.ti<0) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Final and initial times must be greater than zero."
		      << std::endl;
    return -1.0;
  }
  
  // Ensure that the mapping match
  if( *si.function != *sj.function ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The mappings do not match"
		      << std::endl;
    return -1.0;
  }

  // Evaluate the first segment at the second segment's initial time
  // This is because the start of the second segment dictates the time of the
  // transition
  robVariable inputi(sj.ti), outputi;  
  if( si.function->Evaluate( inputi,  outputi ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the first segment."
		      << std::endl;
    return -1.0;
  }
    
  // Evaluate the second segment at its initial time
  robVariable inputj(sj.ti), outputj;
  if( sj.function->Evaluate( inputj, outputj ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the second segment."
		      << std::endl;
    return -1.0;
  }

  // Find the transition time for linear velocities (2xtau)
  if( outputi.IsEnabled( robSpace::TRANSLATION ) ){
    vctFixedSizeVector<double,3> dv = outputj.v - outputi.v;
    return dv.Norm() / xddmax;
  }

  // Find the transition time for joint velocities (2xtau)
  if( outputi.IsEnabled( robSpace::JOINTS_POS ) ){
    vctFixedSizeVector<double,9> dqd = outputj.qd - outputi.qd;
    return dqd.Norm() / xddmax;
  }
  return -1.0;
}

robTrajectory::Segment 
robTrajectoryR1::GenerateBlender( const robTrajectory::Segment& si,
				  const robTrajectory::Segment& sj ){
  
  // Ensure that the time values are greater than zero
  if( (si.tf<0) || (sj.ti<0) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Final and initial times must be greater than zero."
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }
  
  // Ensure that the mapping match
  if( *si.function != *sj.function ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The mappings do not match"
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  // Find the transition time
  double tau = TransitionTime( si, sj )/2.0;

  // Evaluate the first segment at the start of the 2nd segment-transition time
  double ti = sj.ti - tau;            // final time for segment i
  if( ti < 0.0 ) ti = 0.0;            // clip to zero if necessary
  robVariable inputi( ti ), outputi;
  if( si.function->Evaluate( inputi,  outputi ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the first segment."
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  // Evaluate the second segment at its initial time + transition time
  double tj = sj.ti + tau;            // start time for segment j
  if( ti == 0.0 ) tj = 2.0*tau;       // adjust the time if ti is zero;
  robVariable inputj( tj ), outputj;
  if( sj.function->Evaluate( inputj, outputj ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the second segment."
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  // This is effing annoying but has to be done...
  // If the codomain is translation, then you have to read translation values for
  // all the values
  double yi, yid, yidd, yj, yjd, yjdd;
  if( outputi.IsEnabled( robSpace::TX ) && outputj.IsEnabled( robSpace::TX ) ){
    yi = outputi.t[0]; yid = outputi.v[0]; yidd = outputi.vd[0];
    yj = outputj.t[0]; yjd = outputj.v[0]; yjdd = outputj.vd[0];
  }
  if( outputi.IsEnabled( robSpace::TY ) && outputj.IsEnabled( robSpace::TY ) ){
    yi = outputi.t[1]; yid = outputi.v[1]; yidd = outputi.vd[1];
    yj = outputj.t[1]; yjd = outputj.v[1]; yjdd = outputj.vd[1];
  }
  if( outputi.IsEnabled( robSpace::TZ ) && outputj.IsEnabled( robSpace::TZ ) ){
    yi = outputi.t[2]; yid = outputi.v[2]; yidd = outputi.vd[2];
    yj = outputj.t[2]; yjd = outputj.v[2]; yjdd = outputj.vd[2];
  }

  // if the codomain is joint space then you have to read joint values for all
  // the values
  robSpace::Basis bi = robSpace::Q1;
  for( size_t i=0; i<9; i++ ){
    if( outputi.IsEnabled( bi ) && outputj.IsEnabled( bi ) ){
      yi = outputi.q[i]; yid = outputi.qd[i]; yidd = outputi.qdd[i];
      yj = outputj.q[i]; yjd = outputj.qd[i]; yjdd = outputj.qdd[i];
    }
    bi <<= 1;
  }

  // Use the values to fit a quintic polynomial
  robQuintic* quintic = new robQuintic( Codomain().GetBasis(),
					ti, yi, yid, yidd,
					tj, yj, yjd, yjdd );

  return robTrajectory::Segment( ti, tj, quintic );
}

