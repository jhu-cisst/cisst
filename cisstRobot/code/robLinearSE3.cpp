#include <cisstRobot/robLinearSE3.h>
#include <cisstRobot/robLinearRn.h>
#include <cisstRobot/robSLERP.h>

robLinearSE3::robLinearSE3( const vctFrame4x4<double>& Rtw1,
			    const vctFrame4x4<double>& Rtw2,
			    double dt ) :
  robFunctionSE3( 0.0, 
		  Rtw1, 
		  vctFixedSizeVector<double,6>( 0.0 ),
		  vctFixedSizeVector<double,6>( 0.0 ),
		  dt, 
		  Rtw2,
		  vctFixedSizeVector<double,6>( 0.0 ),
		  vctFixedSizeVector<double,6>( 0.0 ) ){

  double tduration = dt;
  double rduration = dt;

  vctFrame4x4<double> Rt1w( Rtw1 );
  Rt1w.InverseSelf();
  vctFrame4x4<double> Rt12 = Rt1w * Rtw2;
  vctMatrixRotation3<double> R12( Rt12[0][0], Rt12[0][1], Rt12[0][2], 
				  Rt12[1][0], Rt12[1][1], Rt12[1][2], 
				  Rt12[2][0], Rt12[2][1], Rt12[2][2],
				  VCT_NORMALIZE  );

  vctFixedSizeVector<double,3> t12 = Rt12.Translation();
  vctAxisAngleRotation3<double> r12( R12 );

  // Synchronize the motion
  vmax = t12.Norm() / tduration;
  wmax = r12.Angle() / rduration;

  translation = new robLinearRn( Rtw1.Translation(), 
				 Rtw2.Translation(), 
				 vmax,
				 0.0 );

  vctMatrixRotation3<double> Rw1( Rtw1[0][0], Rtw1[0][1], Rtw1[0][2], 
				  Rtw1[1][0], Rtw1[1][1], Rtw1[1][2], 
				  Rtw1[2][0], Rtw1[2][1], Rtw1[2][2],
				  VCT_NORMALIZE );
  vctMatrixRotation3<double> Rw2( Rtw2[0][0], Rtw2[0][1], Rtw2[0][2], 
				  Rtw2[1][0], Rtw2[1][1], Rtw2[1][2], 
				  Rtw2[2][0], Rtw2[2][1], Rtw2[2][2],
				  VCT_NORMALIZE );
  rotation = new robSLERP( Rw1, Rw2, wmax, 0.0 );  

  if( translation != NULL )
    { StopTime() = translation->StopTime(); }

  if( rotation != NULL ){
    if( StopTime() < rotation->StopTime() )
      { StopTime() = rotation->StopTime(); }
  }

  vctDynamicVector<double> t, v, vd;
  translation->Evaluate( StartTime(), t, v, vd );

}

robLinearSE3::robLinearSE3( const vctFrame4x4<double>& Rtw1,
			    const vctFrame4x4<double>& Rtw2,
			    double vmax, double wmax,
			    double t1 ) : 
  robFunctionSE3( t1, 
		  Rtw1, 
		  vctFixedSizeVector<double,6>( 0.0 ),
		  vctFixedSizeVector<double,6>( 0.0 ),
		  t1, 
		  Rtw2,
		  vctFixedSizeVector<double,6>( 0.0 ),
		  vctFixedSizeVector<double,6>( 0.0 ) ),
  vmax( vmax ), wmax( wmax ){

  vctFrame4x4<double> Rt1w( Rtw1 );
  Rt1w.InverseSelf();
  vctFrame4x4<double> Rt12 = Rt1w * Rtw2;
  vctMatrixRotation3<double> R12( Rt12[0][0], Rt12[0][1], Rt12[0][2], 
				  Rt12[1][0], Rt12[1][1], Rt12[1][2], 
				  Rt12[2][0], Rt12[2][1], Rt12[2][2],
				  VCT_NORMALIZE  );

  vctFixedSizeVector<double,3> t12 = Rt12.Translation();
  vctAxisAngleRotation3<double> r12( R12 );

  double tduration = t12.Norm() / vmax;
  double rduration = r12.Angle() / wmax;

  // Synchronize the motion
  if( tduration < rduration ){        // If the rotation last longer 
    vmax = t12.Norm() / rduration;    // Recompute the linear velocity
    this->vmax = vmax;
  }
  if( rduration < tduration ){        // If the translation last longer
    wmax = r12.Angle() / tduration;   // Recompute the angular velocity
    this->wmax = wmax;
  }

  translation = new robLinearRn( Rtw1.Translation(), 
				 Rtw2.Translation(), 
				 vmax,
				 t1 );

  vctMatrixRotation3<double> Rw1( Rtw1[0][0], Rtw1[0][1], Rtw1[0][2], 
				  Rtw1[1][0], Rtw1[1][1], Rtw1[1][2], 
				  Rtw1[2][0], Rtw1[2][1], Rtw1[2][2],
				  VCT_NORMALIZE );
  vctMatrixRotation3<double> Rw2( Rtw2[0][0], Rtw2[0][1], Rtw2[0][2], 
				  Rtw2[1][0], Rtw2[1][1], Rtw2[1][2], 
				  Rtw2[2][0], Rtw2[2][1], Rtw2[2][2],
				  VCT_NORMALIZE );
  rotation = new robSLERP( Rw1, Rw2, wmax, t1 );  

  if( translation != NULL )
    { StopTime() = translation->StopTime(); }

  if( rotation != NULL ){
    if( StopTime() < rotation->StopTime() )
      { StopTime() = rotation->StopTime(); }
  }

  vctDynamicVector<double> t, v, vd;
  translation->Evaluate( StartTime(), t, v, vd );

}

robLinearSE3& robLinearSE3::operator=( const robLinearSE3& function ){

  if( this != &function ){

    robFunctionSE3::operator=( function );
    this->vmax = function.vmax;
    this->wmax = function.wmax;

    if( this->translation != NULL ){ 

      robLinearRn* linear = NULL;
      linear = dynamic_cast<robLinearRn*>( function.translation );

      if( linear != NULL )
	{ *( (robLinearRn*)this->translation ) = *linear; }

    }

    if( this->rotation != NULL ){ 

      robSLERP* slerp = NULL;
      slerp = dynamic_cast<robSLERP*>( function.rotation );

      if( slerp != NULL )
	{ *( (robSLERP*)this->rotation ) = *slerp; }
      
    }

  }

  return *this;
}

void robLinearSE3::Blend( robFunction* function, double, double ){

  robLinearSE3* next = dynamic_cast<robLinearSE3*>( function );

  if( next != NULL ){      // cast must be successful

    vctFrame4x4<double> Rtwi, Rtwf;
    vctFixedSizeVector<double,6> vwi, vdwdi, vwf, vdwdf;

    next->InitialState( Rtwi, vwi, vdwdi );
    next->FinalState( Rtwf, vwf, vdwdf );

    robLinearSE3 tmp( Rtwi, Rtwf, next->vmax, next->wmax, this->StopTime());
    *next = tmp;

  }  

}

void robLinearSE3::Evaluate( double t, 
			     vctFrame4x4<double>& Rt, 
			     vctFixedSizeVector<double,6>& vw, 
			     vctFixedSizeVector<double,6>& vdwd ){

  vctFixedSizeVector<double,3> p(0.0), v(0.0), vd(0.0);

  if( translation != NULL ){

    robLinearRn* linear = dynamic_cast< robLinearRn* >( translation );
    if( linear != NULL )
      { linear->Evaluate( t, p, v, vd ); }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": The translation is not a linear trajectory." 
			<< std::endl;
    }
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": No translation trajectory." 
		      << std::endl;
  }

  vctQuaternionRotation3<double> q;
  vctFixedSizeVector<double,3> w(0.0), wd(0.0);
  if( rotation != NULL ){

    robSLERP* slerp = dynamic_cast< robSLERP* >( rotation );
    if( slerp != NULL )
      { slerp->Evaluate( t, q, w, wd ); }
    else{
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< ": The rotation is not a SLERP trajectory." 
			<< std::endl;
    }
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": No rotation trajectory."
		      << std::endl;
  }

  Rt = vctFrame4x4<double>( q, p );
  vw = vctFixedSizeVector<double,6>( v[0], v[1], v[2],
				     w[0], w[1], w[2] );
  vdwd = vctFixedSizeVector<double,6>( vd[0], vd[1], vd[2],
				       wd[0], wd[1], wd[2] );
  
}
