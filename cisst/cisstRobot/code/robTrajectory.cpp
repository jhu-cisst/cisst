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

#include <exception> // for bad_alloc

//#include <values.h>  // FLT_MAX
#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robTrajectory.h>

// Create an empty trajectory
robTrajectory::robTrajectory( robSpace::Basis codomain, double t ) : 
  robFunction( robSpace::TIME, codomain ), t(t) {
  this->t = 0.0;
}

// This needs to be improve for the case where the last segment is a inactive
// hold. In this case, it should evaluate final point of the next to last 
// segment.

robFunction::Errno 
robTrajectory::EvaluateLastSegment( robVariable& input,
				    robVariable& output ) const {

  // ensure that the list of segments isn't empty
  if( segments.empty() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The trajectory is empty!"
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // get the last motion segment
  const robTrajectory::Segment last = segments.back();
  
  if( last.tf == FLT_MAX &&             // if the last segment is "holding" 
      this->t <= last.ti )              // and the last segment is not "active"
    { input = robVariable( last.ti ); } // then return the initial time

  if( last.tf == FLT_MAX &&             // if the last segment is "holding" 
      last.ti <= this->t &&             // and the last segment is "active"
      this->t <= last.tf )
    { input = robVariable( this->t ); } // then, return the current time

  if( last.tf <  FLT_MAX )              // if the last segment is not "holding"
    { input = robVariable( last.tf ); } // then return the final time 

  return last.function->Evaluate( input, output );
  
}

// Insert a function in the trajectory
robFunction::Errno robTrajectory::Insert( robFunction* function,
					  double ti, double tf ){

  // Test that the initial time and final time are coherent
  if( (ti < 0) || (tf < 0) || (tf <= ti) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << ti << " must be less than " << tf << "." 
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // Test that the function exists
  if( function == NULL ) { 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is NULL" 
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // Ensure that the domain of the function matches the domain of the trajectory
  if( (function->Domain()!=Domain()) || (function->Codomain()!=Codomain()) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Spaces do not match."
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // Ensure that the function is defined for ti
  robVariable inputi( ti );
  if( function->GetContext( inputi ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for initial time " << ti 
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // Ensure that the function is defined for tf
  robVariable inputf( tf );
  if( function->GetContext( inputf ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for final time " << tf
		      << std::endl;
    return robFunction::EFAILURE;
  }

  // Create the motion segment
  robTrajectory::Segment segment( ti, tf, function );
 
  // Is this the first segment? If yes, then we can't blend. Thus insert the
  // segment as is.
  if( segments.size() == 0 ){
    segments.push_back( segment );
    return robFunction::ESUCCESS;
  }

  // Create a blender to blend the last segment and the new segment
  robTrajectory::Segment blender = GenerateBlender( segments.back(), segment );

  // now we must adjust the time of both segments...
  segments.back().tf = blender.ti;  // adjust the final time of the last segment
                                    // to the start time of the blending segment
  segment.ti = blender.tf;          // adjust the initial time of the new segment
                                    // to the final time of the blending segment
  segments.push_back( blender );    // insert the blending segment
  segments.push_back( segment );    // insert the motion segment

  return robFunction::ESUCCESS;
}  

robFunction::Context robTrajectory::GetContext(const robVariable& input)const{

  // test the dof are double numbers
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input." 
		      << std::endl;
    return robFunction::CUNDEFINED;
  }

  // check if the input is defined for any function
  std::list<robTrajectory::Segment>::const_iterator s;
  for( s=segments.begin(); s!=segments.end(); s++ ){ 

    if( s->function->GetContext( input ) == robFunction::CDEFINED ) 
      return robFunction::CDEFINED; 
  }

  return robFunction::CUNDEFINED;
}

robFunction::Errno robTrajectory::Evaluate( const robVariable& input, 
					    robVariable& output ){
  
  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    //CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
    //		      << ": Function is undefined for the input." 
    //		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  // save the time
  this->t = input.time;

  // for each segment (backward)
  std::list< robTrajectory::Segment >::reverse_iterator s;
  for( s=segments.rbegin(); s!=segments.rend(); s++ ){ 
    // test if the ith function is defined for the time input
    if( s->ti <= t && t <= s->tf )
      { return s->function->Evaluate( input, output ); }

  }
  
  return robFunction::EUNDEFINED;
}

