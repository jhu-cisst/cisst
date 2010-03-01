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
#include <cisstRobot/robTrajectorySO3.h>
#include <cisstRobot/robCasteljauSO3.h>

robTrajectorySO3::robTrajectorySO3( double t, double wmax, double wdmax ) :
  robTrajectory( robSpace::ORIENTATION, t ){

  this->wmax  = wmax;
  this->wdmax = wdmax;

}

double robTrajectorySO3::TransitionTime(  const robTrajectory::Segment& si, 
					  const robTrajectory::Segment& sj ){

  // Ensure that the time values are greater than zero
  if( (si.tf<0) || (sj.ti<0) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Final and initial times must be greater than zero."
		      << std::endl;
    return -1.0;
  }
  
  // Evaluate the first segment at its final time
  robVariable inputi(si.tf);
  robVariable outputi;  
  if( si.function->Evaluate( inputi,  outputi ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the first segment."
		      << std::endl;
    return -1.0;
  }
    
  // Evaluate the second segment at its initial time
  robVariable inputj(sj.ti);
  robVariable outputj;
  if( sj.function->Evaluate( inputj, outputj ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the second segment."
		      << std::endl;
    return -1.0;
  }

  // Ensure that the velocity basis match
  if( outputi.GetBasis() != outputj.GetBasis() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected identical basis."
		      << std::endl;
    return -1.0;
  }
			 
  // Find the transition time (2xtau) (approximation)
  vctFixedSizeVector<double,3> dw = outputj.w - outputi.w;
  return dw.Norm()/wdmax;

}

robTrajectory::Segment 
robTrajectorySO3::GenerateBlender( const robTrajectory::Segment& si,
				   const robTrajectory::Segment& sj ){
  
  
  // Ensure that the time values are greater than zero
  if( (si.tf<0) || (sj.ti<0) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Final and initial times must be greater than zero."
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }
  
  // Need to handle -1.0 error code
  // Find the transition time
  double tau = TransitionTime( si, sj )/2.0;

  // Set up the blender

  // Evaluate the first control point at sj.ti-tau
  double t0 = sj.ti - tau;            // final time for segment i
  if( t0 < 0.0 ) t0 = 0.0;            // clip to zero if necessary
  robVariable input0( t0 ), output0;
  if( si.function->Evaluate( input0,  output0 ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the second control point at t= "
		      << si.tf-tau
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  // Evaluate the second segment at its initial time + transition time
  double t1 = sj.ti;                  // start time for segment j
  if( t0 == 0.0 ) t1 = tau;           // adjust the time if ti is zero;
  robVariable input1( t1 ), output1;
  if( sj.function->Evaluate( input1, output1 ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate the third control point at t= "
		      << sj.ti
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  // Evaluate the fourth control point (final time of the second segment)
  double t2 = sj.ti + tau;            // final time for segment i
  if( t0 < 0.0 ) t2 = 2.0*tau;        // clip to zero if necessary
  robVariable input2( t2 ), output2;
  if( sj.function->Evaluate( input2, output2 ) != robFunction::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to evaluate fourth control point at time "
		      << sj.tf
		      << std::endl;
    return robTrajectory::Segment( -1.0, -1.0, NULL );
  }

  robCasteljauSO3* squad = new robCasteljauSO3( t0,
						t2,
						output0.R, 
						output1.R, 
						output2.R );

  return robTrajectory::Segment( t0, t2, squad );

}

