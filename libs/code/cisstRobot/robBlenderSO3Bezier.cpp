#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robBlenderSO3Bezier.h>

using namespace std;

robBlenderSO3Bezier::robBlenderSO3Bezier( const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw0, 
					  const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1, 
					  const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw2, 
					  const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw3) {
  this->qa = vctQuaternionRotation3<double>(Rw0);
  this->q1 = vctQuaternionRotation3<double>(Rw1);
  this->q2 = vctQuaternionRotation3<double>(Rw2);
  this->qb = vctQuaternionRotation3<double>(Rw3);

  Rwi = vctMatrixRotation3<double,VCT_ROW_MAJOR>(q1);
}

robBlenderSO3Bezier::robBlenderSO3Bezier( double t1, 
					  double t2,
					  const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1, 
					  const vctFixedSizeVector<double,3>&              w1, 
					  const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw2, 
					  const vctFixedSizeVector<double,3>&              w2,
					  double wmax ) {
  if( t2 < t1 ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": t initial must be less than t final" << std::endl;
  }

  this->tmin=t1;
  this->tmax=t2;

  this->q1 = vctQuaternionRotation3<double>(Rw1);
  this->q2 = vctQuaternionRotation3<double>(Rw2);
  this->Rwi = vctMatrixRotation3<double,VCT_ROW_MAJOR>(this->q1);
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
	vctFixedSizeVector<double,3> w = w1;              // get the angular velocity (outgoing)
	double theta = w.Norm();                          // get the unit vector
	w.NormalizedSelf();
	vctAxAnRot3 ut(w, theta);                         // create a rotation from Rw1 to Rwa by 
	vctMatrixRotation3<double,VCT_ROW_MAJOR> R1a(ut); // going at the constant velocity w for 1
	vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwa;     // SLERP from Rw1
	Rwa = Rw1*R1a;
	this->qa = vctQuaternionRotation3<double>(Rwa);
      }

      // 2nd control point
      {
	vctFixedSizeVector<double,3> w = -w2;             // get the angular velocity (incoming vel)
	double theta = w.Norm();                          // get the unit vector
	w.NormalizedSelf();
	vctAxAnRot3 ut(w, theta);                         // create a rotation from Rw1 to Rwa by
	vctMatrixRotation3<double,VCT_ROW_MAJOR> R2b(ut); // going at the constant velocity w for 1
	vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwb;     // SLERP from Rw2
	Rwb = Rw2*R2b;
	this->qb = vctQuaternionRotation3<double>(Rwb);
      }
    }

    else if( 0 < w1.Norm() ){                              // 3 ctrl point: leaving q1 along w1
      nctrlpoints = 3;
      vctFixedSizeVector<double,3> w = w1;                 // get the angular velocity (outgoing)
      double theta = w.Norm();                             // get the unit vector
      w.NormalizedSelf();
      vctAxAnRot3 ut(w, theta);                            // create a rotation from Rw1 to Rwa by 
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R1a(ut);    // going at the constant velocity w for 1
      vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwa;        // SLERP from Rw1
      Rwa = Rw1*R1a;
      this->qa = vctQuaternionRotation3<double>(Rwa);
    }

    else if( 0 < w2.Norm() ){                              // 3 ctrl point: arriving to q2 along w2
      nctrlpoints = 3;
      vctFixedSizeVector<double,3> w = -w2;                // get the angular velocity (incoming vel)
      double theta = w.Norm();                             // get the unit vector
      w.NormalizedSelf();
      vctAxAnRot3 ut(w, theta);                            // create a rotation from Rw1 to Rwa by
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R2b(ut);    // going at the constant velocity w for 1
      vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwb;        // SLERP from Rw2
      Rwb = Rw2*R2b;
      this->qa = vctQuaternionRotation3<double>(Rwb);
    }
  }
}

robDomainAttribute robBlenderSO3Bezier::IsDefinedFor( const robVariables& input ) const{ 
  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ << ": Expected time input" <<std::endl;
    return UNDEFINED;
  }

  double t = input.time;
  if( tmin <= t && t <= tmax ) return DEFINED;
    
  return DEFINED;
}

vctQuaternionRotation3<double> robBlenderSO3Bezier::SLERP( const vctQuaternionRotation3<double> &qi, 
							   const vctQuaternionRotation3<double> &qf, 
							   double t){

  double ctheta=(qi.X()*qf.X() + qi.Y()*qf.Y() + qi.Z()*qf.Z() + qi.R()*qf.R());

  double theta = acosf(ctheta);

  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qinitial or qfinal
  if (fabs(sin(theta)) < 0.001){ // fabs is floating point absolute
    return vctQuaternionRotation3<double>( qi.X()*0.5 + qf.X()*0.5,
					   qi.Y()*0.5 + qf.Y()*0.5,
					   qi.Z()*0.5 + qf.Z()*0.5,
					   qi.R()*0.5 + qf.R()*0.5, VCT_NORMALIZE );
  }
  
  double A = sin( (1-t) * theta) / sin(theta);
  double B = sin(    t  * theta) / sin(theta);

  return vctQuaternionRotation3<double>( qi.X()*A + qf.X()*B, 
					 qi.Y()*A + qf.Y()*B,
					 qi.Z()*A + qf.Z()*B,
					 qi.R()*A + qf.R()*B, 
					 VCT_NORMALIZE );
}

robError robBlenderSO3Bezier::Evaluate( const robVariables& input, robVariables& output ){

  double t= (input.time-tmin)/(tmax-tmin);    // set time between 0 and 1
  vctQuaternionRotation3<double> q;
  // cout << nctrlpoints << std::endl;
  // nctrlpoints=2;
  switch( nctrlpoints ){
    
  case 1:
    q = qa;
    break;

  case 2:
    q = vctQuaternionRotation3<double>( SLERP( q1, q2, t ) );
    break;

  case 3:
    {
      vctQuaternionRotation3<double> q11 = SLERP( q1, qa, t );
      vctQuaternionRotation3<double> q12 = SLERP( qa, q2, t );
      q = vctQuaternionRotation3<double>( SLERP( q11, q12, t ) );
    }
    break;

  case 4:
    {
      vctQuaternionRotation3<double> q11 = SLERP( q1, qa, t );
      vctQuaternionRotation3<double> q12 = SLERP( qa, qb, t );
      vctQuaternionRotation3<double> q13 = SLERP( qb, q2, t );
      q = vctQuaternionRotation3<double>( SLERP ( SLERP( q11, q12, t ), SLERP( q12, q13, t ), t ) );
    }
    break;
  }

  // evaluate the next angular velocity
  double dt = t-ti;        // time difference
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwj(q);            // current orientation
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Riw;               // inverse of previous orrientation 
  Riw.InverseOf(Rwi);

  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rij;               // relative orientation
  Rij = Riw * Rwj;

  vctAxAnRot3 ut( Rij ); // angular velocity
  vctFixedSizeVector<double,3> w = (ut.Angle()/dt)*ut.Axis();

  output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>( q, vctFixedSizeVector<double,3>(0.0) ), 
			 vctFixedSizeVector<double,6>( 0.0, 0.0, 0.0, w[0], w[1], w[2]), 
			 vctFixedSizeVector<double,6>( 0.0 ) );
  Rwi = Rwj;
  ti = t;

  return SUCCESS;
}
