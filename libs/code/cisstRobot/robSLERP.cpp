#include <cisstCommon/cmnLogger.h>

#include <cisstVector/vctMatrixRotation3.h>

#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robTrajectory.h>

#include <typeinfo>

robSLERP::robSLERP( double ti, 
		    const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti, 
		    double tf, 
		    const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf ){
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
  }

  xmin = ti;
  xmax = tf;

  vctMatrixRotation3<double,VCT_ROW_MAJOR> Ri, Rf;
  for(size_t r=0; r<3; r++){
    for(size_t c=0; c<3; c++){
      Ri[r][c] = Rti[r][c];
      Rf[r][c] = Rtf[r][c];
    }
  }

  qinitial.From( Ri );
  qfinal.From( Rf );

  double ctheta = ( qinitial.X()*qfinal.X() +
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
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwi( qinitial );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwf( qfinal );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Riw;
  Riw.InverseOf( Rwi );   //

  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rif;
  Rif = Riw*Rwf;          //
  
  vctAxisAngleRotation3<double> utif(Rif);       // axis angle of Rif
  vctFixedSizeVector<double,3> ui = utif.Axis(); // the axis wrt initial frame
                                                 // this axis remains constant
  vctFixedSizeVector<double,3> uw = Rwi * ui;    // the axis wrt world frame
  double td = utif.Angle() / (xmax-xmin);        // the angular rate

  w = uw * td;                                   // the angular velocity
}

robDomainAttribute robSLERP::IsDefinedFor( const robVariables& input ) const{

  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expected time input" 
			<< std::endl;
    return UNDEFINED;
  }

  double x = input.time;
  if( xmin <= x && x <= xmax )                           return DEFINED;
  if( xmin-robTrajectory::TAU <= x && x <= xmin ) return INCOMING;
  if( xmax <= x && x <= xmax+robTrajectory::TAU ) return OUTGOING;
  if( xmax+robTrajectory::TAU < x )               return EXPIRED;
  
  return UNDEFINED;
}


robError robSLERP::Evaluate( const robVariables& input, robVariables& output ){

  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected time input" 
		      << std::endl;
    return ERROR;
  }

  double t = input.time;
  t = (t-xmin) / (xmax-xmin);
  if( t < 0.0 ) t = 0;
  if( 1.0 < t ) t = 1;
    
  // cos theta
  double ctheta = ( qinitial.X()*qfinal.X() +
		    qinitial.Y()*qfinal.Y() +
		    qinitial.Z()*qfinal.Z() +
		    qinitial.R()*qfinal.R() );
  
  // if the dot product is too close, return one of them
  if ( 1.0 <= fabs(ctheta) ){
    output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>( qinitial, vctFixedSizeVector<double,3>(0.0) ) );
    return SUCCESS;
  }

  double theta = acos(ctheta);
  double stheta = sqrt(1.0 - ctheta*ctheta);
  
  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to qinitial or qfinal
  if (fabs(stheta) < 0.001){ // fabs is floating point absolute
    vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwi( vctQuaternionRotation3<double>( qinitial.X()*0.5 + qfinal.X()*0.5,
			 qinitial.Y()*0.5 + qfinal.Y()*0.5,
			 qinitial.Z()*0.5 + qfinal.Z()*0.5,
			 qinitial.R()*0.5 + qfinal.R()*0.5 ) );
    vctFixedSizeVector<double,6> vw(0.0);
    vw[3] = w[0];
    vw[4] = w[1];
    vw[5] = w[2];
    output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>(Rwi, vctFixedSizeVector<double,3>(0.0)), vw, vctFixedSizeVector<double,6>(0.0) );
    
    return SUCCESS;
  }
    
  double ratioA = sin((1 - t) * theta) / stheta;
  double ratioB = sin(t * theta) / stheta;
  
  //calculate vctQuaternionRotation3<double>.
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwi( vctQuaternionRotation3<double>( qinitial.X()*ratioA + qfinal.X()*ratioB,
		       qinitial.Y()*ratioA + qfinal.Y()*ratioB,
		       qinitial.Z()*ratioA + qfinal.Z()*ratioB,
		       qinitial.R()*ratioA + qfinal.R()*ratioB ) );
  vctFixedSizeVector<double,6> vw(0.0);
  vw[3] = w[0];
  vw[4] = w[1];
  vw[5] = w[2];
  output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>(Rwi, vctFixedSizeVector<double,3>(0.0)), vw, vctFixedSizeVector<double,6>(0.0) );
  /*
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R1w(qinitial);
  R1w.InverseSelf();
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R1i;
  R1i = R1w*Rwi;

  vctAxisAngleRotation3<double> ut(R1i);
  cout << ut.Angle() << " " << ut.Angle()/t << std::endl;

  vctQuaternionRotation3<double> q(Quaternion( qinitial.X()*ratioA + qfinal.X()*ratioB,
			   qinitial.Y()*ratioA + qfinal.Y()*ratioB,
			   qinitial.Z()*ratioA + qfinal.Z()*ratioB,
			   qinitial.R()*ratioA + qfinal.R()*ratioB ) );

  vctQuaternionRotation3<double> qs(qinitial);
  qs.ConjugateSelf();
  vctQuaternionRotation3<double> qsp;
  qsp = qs*qfinal;
  cout << q.R() << std::endl;
  */  
  return SUCCESS;
}
