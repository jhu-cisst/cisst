#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSO3Bezier.h>

using namespace std;
using namespace cisstRobot;

robSO3Bezier::robSO3Bezier( const SO3& Rw0, 
			    const SO3& Rw1, 
			    const SO3& Rw2, 
			    const SO3& Rw3) {
  this->qa = Quaternion(Rw0);
  this->q1 = Quaternion(Rw1);
  this->q2 = Quaternion(Rw2);
  this->qb = Quaternion(Rw3);

  Rwi = SO3(q1);
}

robSO3Bezier::robSO3Bezier( Real t1, 
			    Real t2,
			    const SO3& Rw1, 
			    const R3& w1, 
			    const SO3& Rw2, 
			    const R3& w2,
			    Real wmax ) {
  if( t2 < t1 ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			<< ": t initial must be less than t final" << endl;
  }

  this->tmin=t1;
  this->tmax=t2;

  this->q1 = Quaternion(Rw1);
  this->q2 = Quaternion(Rw2);
  this->Rwi = SO3(this->q1);
  nctrlpoints = 2;            // by default have 2 control points

  // Determine how many control points are required
  if( this->q1 == this->q2 ){    // Rw1 == Rw2 are the same => only ctrl point
    nctrlpoints=1;
  }

  else{                          // we have at least 2 ctrl points

    if( 0 < w1.Norm() && 0 < w2.Norm() ){ // 4 control points
      nctrlpoints = 4;

      // 1st control point
      {
	R3 w = w1;                 // get the angular velocity (outgoing)
	Real theta = w.Norm();     // get the unit vector
	w.NormalizedSelf();
	vctAxAnRot3 ut(w, theta);  // create a rotation from Rw1 to Rwa by 
	SO3 R1a(ut);               // going at the constant velocity w for 1
	SO3 Rwa;                   // SLERP from Rw1
	Rwa = Rw1*R1a;
	this->qa = Quaternion(Rwa);
      }

      // 2nd control point
      {
	R3 w = -w2;                // get the angular velocity (incoming vel)
	Real theta = w.Norm();     // get the unit vector
	w.NormalizedSelf();
	vctAxAnRot3 ut(w, theta);  // create a rotation from Rw1 to Rwa by
	SO3 R2b(ut);               // going at the constant velocity w for 1
	SO3 Rwb;                   // SLERP from Rw2
	Rwb = Rw2*R2b;
	this->qb = Quaternion(Rwb);
      }
    }

    else if( 0 < w1.Norm() ){    // 3 ctrl point: leaving q1 along w1
      nctrlpoints = 3;
      R3 w = w1;                 // get the angular velocity (outgoing)
      Real theta = w.Norm();     // get the unit vector
      w.NormalizedSelf();
      vctAxAnRot3 ut(w, theta);  // create a rotation from Rw1 to Rwa by 
      SO3 R1a(ut);               // going at the constant velocity w for 1
      SO3 Rwa;                   // SLERP from Rw1
      Rwa = Rw1*R1a;
      this->qa = Quaternion(Rwa);
    }

    else if( 0 < w2.Norm() ){    // 3 ctrl point: arriving to q2 along w2
      nctrlpoints = 3;
      R3 w = -w2;                // get the angular velocity (incoming vel)
      Real theta = w.Norm();     // get the unit vector
      w.NormalizedSelf();
      vctAxAnRot3 ut(w, theta);  // create a rotation from Rw1 to Rwa by
      SO3 R2b(ut);               // going at the constant velocity w for 1
      SO3 Rwb;                   // SLERP from Rw2
      Rwb = Rw2*R2b;
      this->qa = Quaternion(Rwb);
    }
  }
}

robDomainAttribute robSO3Bezier::IsDefinedFor( const robDOF& input ) const{ 
  // test the dof are Real numbers
  if( !input.IsTime() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": Expected time input" <<endl;
    return UNDEFINED;
  }

  Real t = input.t;
  if( tmin <= t && t <= tmax ) return DEFINED;
    
  return DEFINED;
}

Quaternion robSO3Bezier::SLERP( const Quaternion &qi, 
				const Quaternion &qf, 
				Real t){

  Real ctheta=(qi.X()*qf.X() + qi.Y()*qf.Y() + qi.Z()*qf.Z() + qi.R()*qf.R());

  Real theta = acosf(ctheta);

  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qinitial or qfinal
  if (fabs(sin(theta)) < 0.001){ // fabs is floating point absolute
    return Quaternion( qi.X()*0.5 + qf.X()*0.5,
		       qi.Y()*0.5 + qf.Y()*0.5,
		       qi.Z()*0.5 + qf.Z()*0.5,
		       qi.R()*0.5 + qf.R()*0.5, VCT_NORMALIZE );
  }
  
  Real A = sin( (1-t) * theta) / sin(theta);
  Real B = sin(    t  * theta) / sin(theta);

  return Quaternion( qi.X()*A + qf.X()*B, 
		     qi.Y()*A + qf.Y()*B,
		     qi.Z()*A + qf.Z()*B,
		     qi.R()*A + qf.R()*B, 
		     VCT_NORMALIZE );
}

robError robSO3Bezier::Evaluate( const robDOF& input, robDOF& output ){

  Real t= (input.t-tmin)/(tmax-tmin);    // set time between 0 and 1
  Quaternion q;
  // cout << nctrlpoints << endl;
  // nctrlpoints=2;
  switch( nctrlpoints ){
    
  case 1:
    q = qa;
    break;

  case 2:
    q = Quaternion( SLERP( q1, q2, t ) );
    break;

  case 3:
    {
      Quaternion q11 = SLERP( q1, qa, t );
      Quaternion q12 = SLERP( qa, q2, t );
      q = Quaternion( SLERP( q11, q12, t ) );
    }
    break;

  case 4:
    {
      Quaternion q11 = SLERP( q1, qa, t );
      Quaternion q12 = SLERP( qa, qb, t );
      Quaternion q13 = SLERP( qb, q2, t );
      q = Quaternion( SLERP ( SLERP( q11, q12, t ), SLERP( q12, q13, t ), t ) );
    }
    break;
  }

  // evaluate the next angular velocity
  Real dt = t-ti;        // time difference
  SO3 Rwj(q);            // current orientation
  SO3 Riw;               // inverse of previous orrientation 
  Riw.InverseOf(Rwi);

  SO3 Rij;               // relative orientation
  Rij = Riw * Rwj;

  vctAxAnRot3 ut( Rij ); // angular velocity
  R3 w = (ut.Angle()/dt)*ut.Axis();

  output = robDOF( SE3( q, R3(0.0) ), R6( 0,0,0,w[0],w[1],w[2]), R6(0) );
  Rwi = Rwj;
  ti = t;

  return SUCCESS;
}
