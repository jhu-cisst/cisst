#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSE3Track.h>
#include <cisstRobot/robQuintic.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robSO3Blender.h>
#include <cisstRobot/robSO3Bezier.h>

using namespace cisstRobot;

robSE3Track::robSE3Track( Real vmax, Real wmax, Real vdmax, Real wdmax ){
  txblender = NULL;
  tyblender = NULL;
  tzblender = NULL;
  so3blender = NULL;

  this->vmax = vmax;
  this->wmax = wmax;
  this->vdmax = vdmax;
  this->wdmax = wdmax;

  numwp=0;
}

robDomainAttribute robSE3Track::IsDefinedFor( const robDOF& input ) const{
  // test the dof are SE3 or time
  if( input.IsCartesian() || input.IsTime() ){
    return DEFINED;
  }
  return UNDEFINED;
}

robError robSE3Track::Evaluate( const robDOF& input, robDOF& output ){

  // are the DOF Cartesian?
  if( input.IsCartesian() && input.IsTime() ){

    if( numwp == 0 ){                      // set the 1st waypoint
      t0 = input.t;                        // the new time
      t1 = input.t;                        // the new time
      tw0 = input.Rt.Translation();        // the new position
      tw1 = input.Rt.Translation();        // the new position
      Rw0 = SO3(input.Rt[0][0], input.Rt[0][1], input.Rt[0][2], 
		input.Rt[1][0], input.Rt[1][1], input.Rt[1][2], 
		input.Rt[2][0], input.Rt[2][1], input.Rt[2][2], VCT_NORMALIZE);
      Rw1 = SO3(input.Rt[0][0], input.Rt[0][1], input.Rt[0][2], 
		input.Rt[1][0], input.Rt[1][1], input.Rt[1][2], 
		input.Rt[2][0], input.Rt[2][1], input.Rt[2][2], VCT_NORMALIZE);
      v1 = R3(0.0);
      w1 = R3(0.0);
      numwp++;
    }

    else{

      // if the blenders are there
      if( txblender != NULL && 
	  tyblender != NULL && 
	  tzblender != NULL &&
	  so3blender!= NULL ){

	Evaluate(input);       // do one more evaluation to avoid 
	
	delete txblender;
	delete tyblender;
	delete tzblender;
	delete so3blender;

      }

      t2 = input.t;                        // the new time
      tw2 = input.Rt.Translation();        // the new position
      Rw2 = SO3(input.Rt[0][0], input.Rt[0][1], input.Rt[0][2], 
		input.Rt[1][0], input.Rt[1][1], input.Rt[1][2], 
		input.Rt[2][0], input.Rt[2][1], input.Rt[2][2], VCT_NORMALIZE);

      Real dt = t2-t0;

      if( input.IsSet( robDOF::VW ) ){
	v2 = R3( input.vw[0], input.vw[1], input.vw[2] );
	w2 = R3( input.vw[3], input.vw[4], input.vw[5] );
      }

      else{

	R3 tw = tw2-tw0;                     // relative position wrt W

	SO3 R0w, R02;                        // relative orientation wrt 0
	R0w.InverseOf(Rw0);                  //
	R02 = R0w * Rw2;                     // relative orientation wrt frame i
	vctAxisAngleRotation3<Real> r02(R02);//
	
	v2 = tw/(t2-t0)/2.0;                        // approximate v2
	w2 = r02.Axis()*r02.Angle()/(t2-t0)/2000.0; // approximate w2
	//w2 = Rw0*w2;                                // orient w2 wrt W

	if( 0 < vmax )                      // limit on linear acceleration?
	  dt = tw.Norm()/vmax;              // time for a straight line at vmax
	
	if( 0 < vdmax )
	  dt = (dt +                        // this is a bound on the time 
		fabs(vmax-v1.Norm())/vdmax +
		fabs(vmax-v2.Norm())/vdmax);// time to accelerate to/from vmax
	
	if( 0 < wmax ){
	  if( t2 < r02.Angle()/wmax )       // check if the rotation is longer
	    dt = r02.Angle()/wmax;
	}
	
	if( dt < t2-t0  ){  // should we get "there" before the next reading?
	  v2 = R3(0.0);     // if so, put v2 to 0. 
	}
      }

      txblender=new robQuintic(t2,tw1[0],v1[0], 0.0, t2+dt, tw2[0], v2[0],0.0);
      tyblender=new robQuintic(t2,tw1[1],v1[1], 0.0, t2+dt, tw2[1], v2[1],0.0);
      tzblender=new robQuintic(t2,tw1[2],v1[2], 0.0, t2+dt, tw2[2], v2[2],0.0);

      so3blender = new robSO3Bezier( t2, t2+dt, Rw1, w1, Rw2, w2, wmax );

      t0 = t2;                             // backup the old time
      tw0 = tw2;                           // backup the old position
      Rw0 = Rw2;                           // backup the old orientation
      numwp++;
      w1 = w2;
    }
  }

  if ( input.IsTime() ){

    t1 = input.t;
    // nothing has been set yet! Rw1, tw1 are empty
    if( numwp == 0 ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Not initialized" << endl;
      return FAILURE;
    }

    // only one measurement has been processed...there's no trajectory yet
    if( numwp == 1 ){
      output = robDOF( SE3( Rw1, tw1 ), R6(0.0), R6(0.0) );
      return SUCCESS;
    }
  
    // evaluate Rw1, tw1,...
    Evaluate(input);
    output = robDOF( SE3(Rw1, tw1), 
		     R6( v1[0], v1[1], v1[2], w1[0], w1[1], w1[2] ),
		     R6( v1d[0], v1d[1], v1d[2], w1d[0], w1d[1], w1d[2] ) );

  }

  return SUCCESS;
}

robError robSE3Track::Evaluate(const robDOF& input){

  // make sure that each blender is defined for the input because these

  // polynomials can go crazy when extrapolating

  if( txblender->IsDefinedFor( input ) == DEFINED ){ 
    robDOF dof;
    if( txblender->Evaluate( input, dof ) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to evaluate TX blender" << endl;
      return FAILURE;
    }
    tw1[0] = dof.x[0];     // set the X position
    v1[0]  = dof.xd[0];    // set the X lin vel
    v1d[0] = dof.xdd[0];   // set the X lin acc
  }
  else{                    // no function: stay at tw2
    tw1[0] = tw2[0];
    v1[0]  = 0;
    v1d[0] = 0;
  }

  if( tyblender->IsDefinedFor( input ) == DEFINED ){ 
    robDOF dof;
    if( tyblender->Evaluate( input, dof ) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to evaluate TY blender" << endl;
      return FAILURE;
    }
    tw1[1] = dof.x[0];     // set the Y position
    v1[1]  = dof.xd[0];    // set the Y lin vel
    v1d[1] = dof.xdd[0];   // set the Y lin acc
  }
  else{
    tw1[1] = tw2[1];
    v1[1]  = 0;
    v1d[1] = 0;
  }

  if( tzblender->IsDefinedFor( input ) == DEFINED ){ 
    robDOF dof;
    if( tzblender->Evaluate( input, dof ) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to evaluate TZ blender" << endl;
      return FAILURE;
    }
    tw1[2] = dof.x[0];     // set the Z position
    v1[2]  = dof.xd[0];    // set the Z lin vel
    v1d[2] = dof.xdd[0];   // set the Z lin acc
  }
  else{
    tw1[2] = tw2[2];
    v1[2]  = 0;
    v1d[2] = 0;
  }

  if( so3blender->IsDefinedFor( input ) == DEFINED ){ 

    robDOF dof;

    if( so3blender->Evaluate( input, dof ) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to evaluate SO3 blender" << endl;
      return FAILURE;
    }
    Rw1 = SO3( dof.Rt[0][0], dof.Rt[0][1], dof.Rt[0][2], 
	       dof.Rt[1][0], dof.Rt[1][1], dof.Rt[1][2], 
	       dof.Rt[2][0], dof.Rt[2][1], dof.Rt[2][2] );
    /*
    w1[0] = dof.vw[3];     // set the X rot vel
    w1[1] = dof.vw[4];     // set the Y rot vel
    w1[2] = dof.vw[5];     // set the Z rot vel

    w1d[0] = dof.vdwd[3];  // set the X rot acc
    w1d[1] = dof.vdwd[4];  // set the Y rot acc
    w1d[2] = dof.vdwd[5];  // set the Z rot acc
    */
  }
  else{
    Rw1 = Rw2;
    w1 = R3(0.0);
    w1d = R3(0.0);
  }

  return SUCCESS;
}
