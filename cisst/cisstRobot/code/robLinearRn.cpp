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

#include <cisstRobot/robLinearRn.h>

#include <cisstCommon/cmnLogger.h>

robLinearRn::robLinearRn( const vctFixedSizeVector<double,3>& p1, 
			  const vctFixedSizeVector<double,3>& p2,
			  double v,
			  double t1 ) : 
  robFunctionRn( t1, 
		 p1,
		 vctDynamicVector<double>( 3, 0.0 ),
		 vctDynamicVector<double>( 3, 0.0 ),
		 t1,
		 p2,
		 vctDynamicVector<double>( 3, 0.0 ),
		 vctDynamicVector<double>( 3, 0.0 ) ){

  vctFixedSizeVector<double,3> dp = p2 - p1;
  if( 0 < fabs(v) ){
    StopTime() = StartTime() + dp.Norm() / fabs(v);// Compute the final time t2
  }
  ComputeParameters();                             // evaluate the parameters

}

robLinearRn::robLinearRn( const vctDynamicVector<double>& q1, 
			  const vctDynamicVector<double>& q2,
			  const vctDynamicVector<double>& qd,
			  double t1 ) : 
  robFunctionRn( t1,
		 q1,
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 t1,
		 q2,
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 vctDynamicVector<double>( q1.size(), 0.0 ) ){

  if( (q1.size() == q2.size()) && (q1.size() == qd.size()) ){
    
    // Compute the final time t2
    for( size_t i=0; i<q1.size(); i++ ){
      if( 0 < fabs( qd[i] ) ){
	double t2i = StartTime() + fabs( q2[i]-q1[i] ) / fabs( qd[i] );
	if( StopTime() < t2i )
	  { StopTime() = t2i; }
      }
    }
    ComputeParameters();
  }

}

void robLinearRn::Blend( robFunction* function, 
			 const vctDynamicVector<double>&, 
			 const vctDynamicVector<double>& ){

  // The function must be a QLQ trajectory
  robLinearRn* next = dynamic_cast<robLinearRn*>( function );

  if( next != NULL ){      // cast must be successful

    vctDynamicVector<double> q2i, q2id, q2idd;
    vctDynamicVector<double> q2f, q2fd, q2fdd;
    vctDynamicVector<double> slope = next->Slope();
    next->InitialState( q2i, q2id, q2idd );
    next->FinalState( q2f, q2fd, q2fdd );

    // Create a new cruise segment but this one will start at StopTime 
    *next = robLinearRn( q2i, q2f, slope, this->StopTime() );

  }

}

void robLinearRn::Blend( robFunction* function, double, double ){

  // The function must be a QLQ trajectory
  robLinearRn* next = dynamic_cast<robLinearRn*>( function );

  if( next != NULL ){      // cast must be successful

    vctDynamicVector<double> q2i, q2id, q2idd;
    vctDynamicVector<double> q2f, q2fd, q2fdd;
    vctDynamicVector<double> slope = next->Slope();
    next->InitialState( q2i, q2id, q2idd );
    next->FinalState( q2f, q2fd, q2fdd );

    // Create a new cruise segment but this one will start at StopTime 
    *next = robLinearRn( q2i, q2f, slope, this->StopTime() );

  }

}

vctDynamicVector<double> robLinearRn::Slope() const { return m; }

void robLinearRn::ComputeParameters(){

                                 // Compute the parameters 
  if( StartTime() < StopTime() ){// if t1 < t2, then we move
    m = (y2-y1) / Duration();    // compute the slope
    b =  y1 - m*StartTime();     // compute the zero offset
  }
  else{                     // if t2 <= t1 then we stay at y1
    b = y1;                 // zero offset to y1
    m = y1d;                // no velocity
    y2 = y1;                // final config to y1
  }

}



void robLinearRn::Evaluate( double t, double& q, double& qd, double& qdd ){
  

  if( t < StartTime() ){
    if( y1.size() == 1 && y1d.size() == 1 && y1dd.size() == 1 ){
      q   = y1[0];
      qd  = y1d[0];
      qdd = y1dd[0];
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R1 trajectory. Sizes of vectors are"
			<< ": size(y1) = " << y1.size() 
			<< "; size(y1d) = " << y1d.size()
			<< "; size(y1dd) = " << y1dd.size()
			<< std::endl;
    }
  }

  if( StartTime() <= t && t <= StopTime() ){
    if( m.size() == 1 && b.size() == 1 ){
      q   = m[0]*t + b[0];                  // interpolate 
      qd  = m[0];                           // constant velocity
      qdd = 0.0;                            // zero acc
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R1 trajectory."
			<< std::endl;
    }
  }

  if( StopTime() < t ){              // if t2 < t
    if( y2.size() == 1 && y2d.size() == 1 && y2dd.size() == 1 ){
      q   = y2[0];
      qd  = y2d[0];
      qdd = y2dd[0];
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R1 trajectory. Sizes of vectors are"
			<< ": size(y2) = " << y2.size() 
			<< "; size(y2d) = " << y2d.size()
			<< "; size(y2dd) = " << y2dd.size()
			<< std::endl;
    }
  }

}


void robLinearRn::Evaluate( double t,
			    vctFixedSizeVector<double,3>& p,
			    vctFixedSizeVector<double,3>& v,
			    vctFixedSizeVector<double,3>& vd ){
  
  if( t < StartTime() ){
    if( y1.size() == 3 && y1d.size() == 3 && y1dd.size() == 3 ){
      p =  vctFixedSizeVector<double,3>( y1[0],   y1[1],   y1[2] );
      v =  vctFixedSizeVector<double,3>( y1d[0],  y1d[1],  y1d[2] );
      vd = vctFixedSizeVector<double,3>( y1dd[0], y1dd[1], y1dd[2] );
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R3 trajectory. Sizes of vectors are"
			<< ": size(y1) = " << y1.size() 
			<< "; size(y1d) = " << y1d.size()
			<< "; size(y1dd) = " << y1dd.size()
			<< std::endl;
    }
  }

  if( StartTime() <= t && t <= StopTime() ){
    if( m.size() == 3 && b.size() == 3 ){
      vctDynamicVector<double> tmp = m*t+b;                      // interpolate 
      p = vctFixedSizeVector<double,3>( tmp[0], tmp[1], tmp[2] );
      v = vctFixedSizeVector<double,3>( m[0], m[1], m[2] );      // slope
      vd.SetAll( 0.0 );                                          // zero acc
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R3 trajectory."
			<< std::endl;
    }
  }

  if( StopTime() < t ){              // if t2 < t
    if( y2.size() == 3 && y2d.size() == 3 && y2dd.size() == 3 ){
      p =  vctFixedSizeVector<double,3>( y2[0],   y2[1],   y2[2] );
      v =  vctFixedSizeVector<double,3>( y2d[0],  y2d[1],  y2d[2] );
      vd = vctFixedSizeVector<double,3>( y2dd[0], y2dd[1], y2dd[2] );
    }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": Not an R3 trajectory. Sizes of vectors are"
			<< ": size(y2) = " << y2.size() 
			<< "; size(y2d) = " << y2d.size()
			<< "; size(y2dd) = " << y2dd.size()
			<< std::endl;
    }
  }

}




void robLinearRn::Evaluate( double t,
			    vctDynamicVector<double>& q,
			    vctDynamicVector<double>& qd,
			    vctDynamicVector<double>& qdd ){

  if( t < StartTime() ){
    q = y1;                  // return p1
    qd = y1d;                // zero velocity
    qdd = y1dd;              // zero acceleration
  }
  
  if( StartTime() <= t && t <= StopTime() ){
    q = m*t+b;               // interpolate 
    qd = m;                  // constant velocity
    qdd.SetSize( q.size() );
    qdd.SetAll( 0.0 );       // zero acceleration
  }

  if( StopTime() < t ){
    q = y2;                  // return p2
    qd = y2d;                // zero velocity
    qdd = y2dd;              // zero acceleration
  }

}

