#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
using namespace std;
using namespace cisstRobot;

robSLERP::robSLERP( Real ti, const SE3& Rti, Real tf, const SE3& Rtf ){
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
  }

  xmin = ti;
  xmax = tf;

  SO3 Ri, Rf;                    // Don't know why Rti.Rotation() doesn't work
  for(size_t r=0; r<3; r++){     // here (linker error)
    for(size_t c=0; c<3; c++){
      Ri[r][c] = Rti[r][c];
      Rf[r][c] = Rtf[r][c];
    }
  }

  qinitial.From( Ri );
  qfinal.From( Rf );

  Real ctheta = ( qinitial.X()*qfinal.X() +
		  qinitial.Y()*qfinal.Y() +
		  qinitial.Z()*qfinal.Z() +
		  qinitial.R()*qfinal.R() );

  if( ctheta < 0 ){
    qfinal.X() = -qfinal.X();
    qfinal.Y() = -qfinal.Y();
    qfinal.Z() = -qfinal.Z();
    qfinal.R() = -qfinal.R();
  }  

  // find the angular velocity in the world frame from the angular velocity
  // in the constant velocity in the plane of rotation
  SO3 Rwi( qinitial );    // initial orientation wrt world frame
  SO3 Rwf( qfinal );      // final orientation wrt world frame
  SO3 Riw;                // inverse of Rw1;
  Riw.InverseOf( Rwi );   //

  SO3 Rif;                // relative rotation wrt initial frame
  Rif = Riw*Rwf;          //
  
  vctAxisAngleRotation3<Real> utif(Rif); // axis angle of Rif
  R3 ui = utif.Axis();                   // the axis wrt initial frame
                                         // this axis remains constant
  R3 uw = Rwi * ui;                      // the axis wrt world frame
  Real td = utif.Angle() / (xmax-xmin);  // the angular rate

  w = uw * td;                           // the angular velocity
}

robDomainAttribute robSLERP::IsDefinedFor( const robDOF& input ) const{

  if( !input.IsTime() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ << ": Expected time input" <<endl;
    return UNDEFINED;
  }

  Real x = input.t;
  if( xmin <= x && x <= xmax )                           return DEFINED;
  if( xmin-robFunctionPiecewise::TAU <= x && x <= xmin ) return INCOMING;
  if( xmax <= x && x <= xmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( xmax+robFunctionPiecewise::TAU < x )               return EXPIRED;
  
  return UNDEFINED;
}


robError robSLERP::Evaluate( const robDOF& input, robDOF& output ){

  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Expected time input" << endl;
    return FAILURE;
  }

  Real t = input.t;
  t = (t-xmin) / (xmax-xmin);
  if( t < 0.0 ) t = 0;
  if( 1.0 < t ) t = 1;
    
  // cos theta
  Real ctheta = ( qinitial.X()*qfinal.X() +
		  qinitial.Y()*qfinal.Y() +
		  qinitial.Z()*qfinal.Z() +
		  qinitial.R()*qfinal.R() );
  
  // if the dot product is too close, return one of them
  if ( 1.0 <= fabs(ctheta) ){
    output = robDOF( SE3( qinitial, R3(0.0) ), R6(0.0), R6(0.0) );
    return SUCCESS;
  }

  Real theta = acos(ctheta);
  Real stheta = sqrt(1.0 - ctheta*ctheta);
  
  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qinitial or qfinal
  if (fabs(stheta) < 0.001){ // fabs is floating point absolute
    SO3 Rwi( Quaternion( qinitial.X()*0.5 + qfinal.X()*0.5,
			 qinitial.Y()*0.5 + qfinal.Y()*0.5,
			 qinitial.Z()*0.5 + qfinal.Z()*0.5,
			 qinitial.R()*0.5 + qfinal.R()*0.5 ) );
    R6 vw(0.0);
    vw[3] = w[0];
    vw[4] = w[1];
    vw[5] = w[2];
    output = robDOF( SE3(Rwi, R3(0.0)), vw, R6(0.0) );
    
    return SUCCESS;
  }
    
  Real ratioA = sin((1 - t) * theta) / stheta;
  Real ratioB = sin(t * theta) / stheta;
  
  //calculate Quaternion.
  SO3 Rwi( Quaternion( qinitial.X()*ratioA + qfinal.X()*ratioB,
		       qinitial.Y()*ratioA + qfinal.Y()*ratioB,
		       qinitial.Z()*ratioA + qfinal.Z()*ratioB,
		       qinitial.R()*ratioA + qfinal.R()*ratioB ) );
  R6 vw(0.0);
  vw[3] = w[0];
  vw[4] = w[1];
  vw[5] = w[2];
  output = robDOF( SE3(Rwi, R3(0.0)), vw, R6(0.0) );
  /*
  SO3 R1w(qinitial);
  R1w.InverseSelf();
  SO3 R1i;
  R1i = R1w*Rwi;

  vctAxisAngleRotation3<Real> ut(R1i);
  cout << ut.Angle() << " " << ut.Angle()/t << endl;

  Quaternion q(Quaternion( qinitial.X()*ratioA + qfinal.X()*ratioB,
			   qinitial.Y()*ratioA + qfinal.Y()*ratioB,
			   qinitial.Z()*ratioA + qfinal.Z()*ratioB,
			   qinitial.R()*ratioA + qfinal.R()*ratioB ) );

  Quaternion qs(qinitial);
  qs.ConjugateSelf();
  Quaternion qsp;
  qsp = qs*qfinal;
  cout << q.R() << endl;
  */  
  return SUCCESS;
}
