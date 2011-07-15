#include <cisstRobot/robQLQRn.h>

robQLQRn::robQLQRn( const vctDynamicVector<double>& q1,
		    const vctDynamicVector<double>& q2,
		    const vctDynamicVector<double>& qdmax,
		    const vctDynamicVector<double>& qddmax,
		    double t1, 
		    bool forceblend ) :
  robFunctionRn( t1, 
		 q1,
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 t1,
		 q2,
		 vctDynamicVector<double>( q1.size(), 0.0 ),
		 vctDynamicVector<double>( q1.size(), 0.0 ) ),
  qdmax( qdmax ),            // save the maximum velocity
  qddmax( qddmax ),          // save the maximum acceleration
  tauin( 0.0 ),              // clear the initial transition time
  tauout( 0.0 ),             // clear the final transition time
  transitionin( NULL ),      // initial transition
  cruise( NULL ),            // cruise
  transitionout( NULL ){     // final transition
  
  if( (q1.size() == q2.size()) &&      // size matters
      (q1.size() == qdmax.size()) &&
      (q1.size() == qddmax.size()) ){

    // Create the cruise part. At this point the cruise will occupy the entire
    // time frame of the segment. That is the transitions will be created when
    // needed
    cruise = new robLinearRn( q1, q2, qdmax, StartTime() );

    // Estimate the duration based on the duration of the cruise segment
    if( cruise != NULL ){ 
      this->StartTime() = cruise->StartTime();
      this->StopTime() = cruise->StopTime();
    }

    // A vector of zeros
    vctDynamicVector<double> zeros( q1.size(), 0.0 );
    
    // Estimate the transition time from start/stop to cruise speed
    double tau = EvaluateTransitionTime( zeros, qdmax ) / 2.0;

    // Those are save for future need. For now we do not create the transition
    // But now we now that to accelerate/deccelerate from zero velocity to 
    // qdmax we we will need tau seconds
    this->tauin  = tau;
    this->tauout = tau;

    if( forceblend ){

      delete cruise;
      cruise = new robLinearRn( q1, q2, qdmax, StartTime()+2*tauin );
      
      // Estimate the duration based on the duration of the cruise segment
      if( cruise != NULL ){ 
	this->StartTime() = cruise->StartTime();
	this->StopTime() = cruise->StopTime();
      }
      CreateInputTransition( tauin, y1, zeros, zeros );
    }

  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Size mismatch."
		      << " size(q1) = "     << q1.size()     << " " 
		      << " size(q2) = "     << q2.size()     << " " 
		      << " size(qdmax) = "  << qdmax.size()  << " " 
		      << " size(qddmax) = " << qddmax.size()
		      << std::endl;
  }

}

void robQLQRn::Blend( robFunction* function,
		      const vctDynamicVector<double>& qdmax,
		      const vctDynamicVector<double>& qddmax ){

  // The function must be a QLQ trajectory
  robQLQRn* next = dynamic_cast<robQLQRn*>( function );

  if( next != NULL ){      // cast must be successful

    // Evaluate the transition time
    double tau = EvaluateTransitionTime( this->cruise->Slope(), 
					 next->cruise->Slope() ) / 2.0;

    // Evaluate this cruise segments at -tau
    vctDynamicVector<double> q1, q1d, q1dd;
    this->cruise->Evaluate( this->cruise->StopTime()  - tau, q1, q1d, q1dd );

    // Evaluate the next cruise segments at +tau
    vctDynamicVector<double> q2, q2d, q2dd;
    next->cruise->Evaluate( next->cruise->StartTime() + tau, q2, q2d, q2dd );

    // Start by creating an output transition for this segment
    this->CreateOutputTransition( tau, q2, q2d, q2dd );
    // Update the stop time. This is necessary since CreateOutputTransition will
    // set the stop time to cruise->StopTime() + tau
    this->StopTime() = this->cruise->StopTime();
    // Tell the output transition to stop at the same time
    this->transitionout->StopTime() = this->cruise->StopTime();

    // Blend the linear segments
    this->cruise->Blend( next->cruise, qdmax, qddmax );

    // Create the input transition for the next segment
    next->CreateInputTransition( tau, q1, q1d, q1dd );

    // Update the start and stop time of the next segment
    next->StartTime() = next->cruise->StartTime();
    next->StopTime()  = next->cruise->StopTime();
    // Tell the next input transition to stop at the same time
    next->transitionin->StartTime() = next->StartTime();

  }
  
}

void robQLQRn::Blend( robFunction* function, double vmax, double vdmax ){

  /*// The function must be a QLQ trajectory
  robQLQRn* next = dynamic_cast<robQLQRn*>( function );

  if( next != NULL ){      // cast must be successful

    // Evaluate the transition time
    double tau = EvaluateTransitionTime( this->cruise->Slope(), 
					 next->cruise->Slope() ) / 2.0;

    // Evaluate this cruise segments at -tau
    vctDynamicVector<double> q1, q1d, q1dd;
    this->cruise->Evaluate( this->cruise->StopTime()  - tau, q1, q1d, q1dd );

    // Evaluate the next cruise segments at +tau
    vctDynamicVector<double> q2, q2d, q2dd;
    next->cruise->Evaluate( next->cruise->StartTime() + tau, q2, q2d, q2dd );

    // Start by creating an output transition for this segment
    this->CreateOutputTransition( tau, q2, q2d, q2dd );
    // Update the stop time. This is necessary since CreateOutputTransition will
    // set the stop time to cruise->StopTime() + tau
    this->StopTime() = this->cruise->StopTime();
    // Tell the output transition to stop at the same time
    this->transitionout->StopTime() = this->cruise->StopTime();

    // Blend the linear segments
    this->cruise->Blend( next->cruise, qdmax, qddmax );

    // Create the input transition for the next segment
    next->CreateInputTransition( tau, q1, q1d, q1dd );

    // Update the start and stop time of the next segment
    next->StartTime() = next->cruise->StartTime();
    next->StopTime()  = next->cruise->StopTime();
    // Tell the next input transition to stop at the same time
    next->transitionin->StartTime() = next->StartTime();

  }
  */  
}

void robQLQRn::Evaluate( double t,
			 vctDynamicVector<double>& q,
			 vctDynamicVector<double>& qd,
			 vctDynamicVector<double>& qdd ){

  // The order of the "if"s matters

  // First is the panic ifs. Those are used if the trajectory enters what 
  // "should" a transition period. That is if the trajectory should be 
  // transitioning to/from cruise velocity but no transition is available

  // Given that this trajectory was not blended, we ASSUME that the initial
  // state is q = y1, qd = 0 and qdd = 0 (the initial values of this 
  // trajectory). Thus, if t1-tauin < t < t1 and no transition exists, we create
  // a transition that will ramp up to reach the cruise velocity
  if( StartTime() - tauin <= t && t < StartTime() ){
    if( transitionin == NULL ){
      vctDynamicVector<double> zeros( y1.size(), 0.0 );
      CreateInputTransition( tauin, y1, zeros, zeros );
    }
  }


  // Given that this trajectory was not blended, we ASSUME that the final
  // state is q = y2, qd = 0 and qdd = 0 (the final values of this 
  // trajectory). Thus, if t2-tauout < t < t2 and no transition exists, we 
  // create a transition that will ramp down from the cruise velocity
  if( StopTime() - tauout <= t && t < StopTime() ){
    if( transitionout == NULL ){
      vctDynamicVector<double> zeros( y2.size(), 0.0 );
      CreateOutputTransition( tauout, y2, zeros, zeros );
    }
  }


  // Now that we have transitions we can deal with evaluating them
  //std::cout << StartTime() << " " << t << " " << StopTime() << std::endl;
    
  // First, is it too early?
  if( t < StartTime() ){     // if t < t1
    q = y1;                  // return p1
    qd = y1d;                // zero velocity
    qdd = y1dd;              // zero acceleration
    return;
  }

  // Second, is it too late?
  if( StopTime() < t ){      // if t2 < t
    q = y2;                  // return p2
    qd = y2d;                // zero velocity
    qdd = y2dd;              // zero acceleration
    return;
  }

  // Now we deal with the trajectories
  // Order matters since the interval of the cruise trajectory can overlap
  // the interval of the transition trajectories. Therefore we need to deal 
  // with the transition first

  // is it time for incoming transition
  if( transitionin != NULL ){
    if( transitionin->StartTime() <= t && t <= transitionin->StopTime() )
      { return transitionin->Evaluate( t, q, qd, qdd ); }
  }

  // is it time for outgoing transition
  if( transitionout != NULL ){
    if( transitionout->StartTime() <= t && t <= transitionout->StopTime() )
      { return transitionout->Evaluate( t, q, qd, qdd ); }
  }

  // now we deal with the crusie trajectory
  if( cruise != NULL ){
    if( cruise->StartTime() <= t && t <= cruise->StopTime() )
      { return cruise->Evaluate( t, q, qd, qdd ); }
  }

}

double robQLQRn::EvaluateTransitionTime( const vctDynamicVector<double>& q1d,
					 const vctDynamicVector<double>& q2d ){

  if( q1d.size() == q2d.size() && q1d.size() == qddmax.size() ){

    double tau = -1.0;

    // Find the duration of the transition from q1d to q2d at using qddmax
    for( size_t i=0; i<qddmax.size(); i++ ){
      // estimate the time to accelerate from q1d[i] to q2d[i]
      double taui = fabs( q1d[i] - q2d[i] ) / fabs( qddmax[i] );
      if( tau < taui )
	{ tau = taui; }
    }
    return tau;
  }
  return -1.0;
}

void robQLQRn::CreateInputTransition( double tau,
				      const vctDynamicVector<double>& q1, 
				      const vctDynamicVector<double>& q1d, 
				      const vctDynamicVector<double>& q1dd ){

  // Evaluate the position at t = t1 + tau
  vctDynamicVector<double> qtau, qtaud, qtaudd;
  cruise->Evaluate( cruise->StartTime() + tau, qtau, qtaud, qtaudd );
  
  // create the transition from [q1, 0, 0] to [qtau, qd, 0]
  if( transitionin != NULL ) delete transitionin;
  transitionin = new robQuintic( cruise->StartTime()-tau, q1,   q1d,   q1dd,
				 cruise->StartTime()+tau, qtau, qtaud, qtaudd );

  this->tauin = tau;
  this->StartTime() = cruise->StartTime() - tau; 

}

void robQLQRn::CreateOutputTransition( double tau,
				       const vctDynamicVector<double>& q2, 
				       const vctDynamicVector<double>& q2d, 
				       const vctDynamicVector<double>& q2dd ){

  // Evaluate the position at t = t2 - tau
  vctDynamicVector<double> qtau, qtaud, qtaudd;
  cruise->Evaluate( cruise->StopTime() - tau, qtau, qtaud, qtaudd );
  
  // create the transition from [q1, 0, 0] to [qtau, qd, 0]
  if( transitionout != NULL ) delete transitionout;
  transitionout = new robQuintic( cruise->StopTime()-tau, qtau, qtaud, qtaudd,
				  cruise->StopTime()+tau, q2,   q2d,   q2dd );

  this->tauout = tau;
  this->t2 = cruise->StopTime() + tau;
  
}

/*

    // Evaluate this segment at the time t to get the current velocity
    this->cruise->Evaluate( t, q1, q1d, q1dd );

    // Evaluate the next segment at time t to get the next velocity
    next->cruise->Evaluate( t, q2, q2d, q2dd );

    double tau = EvaluateTransitionTime( q1d, q1dd ) / 2.0;
    this->tauout = tau;       // half goes to the incoming transition
    next->tauin  = tau;       // half goes to the outgoing transition
      
    // get the current position, velocity and acceleration at t-tau
    this->cruise->Evaluate( t - tau, q1, q1d, q1dd); 

    // get the current position, velocity and acceleration at t+tau
    next->cruise->Evaluate( t + tau, q2, q2d, q2dd);
    
    if( this->transitionout != NULL )  delete this->transitionout;
    this->transitionout =  new robQuintic( t - tau, q1, q1d, q1dd,
					   t + tau, q2, q2d, q2dd );
    
    if( next->transitionin  != NULL )  delete next->transitionin;
    next->transitionin  = new robQuintic( t - tau, q1, q1d, q1dd,
					  t + tau, q2, q2d, q2dd );
    
*/
