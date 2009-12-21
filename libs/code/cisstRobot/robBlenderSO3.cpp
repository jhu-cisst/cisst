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
#include <cisstRobot/robBlenderSO3.h>

#include <typeinfo>
#include <iostream>

robBlenderSO3::robBlenderSO3( double ti,
			      double T1, double T2,
			      const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw0,
			      const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1,
			      const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw2,
			      double tA,
			      double tB ){
  this->ti = ti;
  this->T1 = T1;
  this->T2 = T2;
  this->tauA = tA;
  this->tauB = tB;

  tmin = ti;
  tmax = tmin+T1+T2;

  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0w;
  R0w.InverseOf( Rw0 );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R01;
  R01 = R0w * Rw1;
  
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R1w;
  R1w.InverseOf( Rw1 );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R12;
  R12 = R1w * Rw2;

  this->n1t1 = vctAxisAngleRotation3<double>(R01);
  this->n2t2 = vctAxisAngleRotation3<double>(R12);
  /*
  cout << n1t1 << endl << n2t2 << std::endl;
  cout <<"R0" << Rw0 <<endl << std::endl;
  cout <<"R1" << Rw1 <<endl << std::endl;
  cout <<"vctFixedSizeVector<double,2>" << Rw2 <<endl << std::endl;
  */
  this->Rw1 = Rw1;

}

robBlenderSO3::robBlenderSO3( double xi,
			      const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw0, 
			      const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1, 
			      const vctFixedSizeVector<double,3>& w0w){

  xmin = xi;
  //xmax = xi+2.0*robFunctionPiecewise::TAU;
  xmax = xi+2.0*0.01;

  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0w, R1w;
  R0w.InverseOf(Rw0);
  R1w.InverseOf(Rw1);

  R10 = R1w * Rw0;                              // used by Rb()
  this->Rw0 = Rw0;
  vctFixedSizeVector<double,3> w00 = R0w*w0w;                             // w0 wrt to frame 0
  this->w0 = w00.Norm();                        // usedby Rv()
  uv.NormalizedOf( w00 );                       // used by Rv()
  //TAU
  vctMatrixRotation3<double,VCT_ROW_MAJOR> RB1 = Rb1( 2.0*0.01 );// compute the final Rv
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0B( R0w * Rw1 * RB1.InverseSelf() );      // compute the final Rc

  uctc = vctAxisAngleRotation3<double>(R0B);       // used by Rc()

}

vctMatrixRotation3<double,VCT_ROW_MAJOR> robBlenderSO3::Rb1( double h ){

  double h2 = h *h;
  double h3 = h2*h;
  double h4 = h3*h;
  
  double H1 = 2.0*0.01;
  //double H1 = 2.0*robFunctionPiecewise::TAU;
  double H2 = H1*H1;
  double H3 = H2*H1;

  double gv = (h4/H3 - 2.0*h3/H2 + 2.0*h)*w0/2.0;

  vctAxisAngleRotation3<double> rv( uv, gv );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0h(rv);
  return R0h;
}

vctMatrixRotation3<double,VCT_ROW_MAJOR> robBlenderSO3::R0b( double h ){
  double h2 = h *h;
  double h3 = h2*h;
  double h4 = h3*h;
  double h5 = h4*h;

  double H  = 2.0*0.01;
  //double H  = 2.0*robFunctionPiecewise::TAU;
  double H2 = H *H;
  double H3 = H2*H;
  double H4 = H3*H;
  double H5 = H4*H;

  double gc = (6.0*h5)/H5 - (15.0*h4)/H4 + (10.0*h3)/H3;
  vctAxisAngleRotation3<double> rc( uctc.Axis(), gc*uctc.Angle() );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rc(rc);
  return Rc;
}

robDomainAttribute robBlenderSO3::IsDefinedFor( const robVariables& input ) const{
  
  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": Expected time input" <<std::endl;
    return UNDEFINED;
  }

  double t = input.time;
  if( xmin <= t && t <= xmax ) return DEFINED;
  else                         return UNDEFINED;
}

robError robBlenderSO3::Evaluate( const robVariables& input, robVariables& output ){  

  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected time input" <<std::endl;
    return ERROR;
  }

  double t = input.time - ti;
  //double tau = robFunctionPiecewise::TAU;
  //double tau = 0.005;//robFunctionPiecewise::TAU;
  double tp = t-this->T1;
    
  double ta = (( tauA - tp ) * ( tauA - tp )/ (4.0*tauA*T1)) * n1t1.Angle();
  double tb = (( tauB + tp ) * ( tauB + tp )/ (4.0*tauB*T2)) * n2t2.Angle();
  vctAxisAngleRotation3<double> nata( n1t1.Axis(), -ta );
  vctAxisAngleRotation3<double> nbtb( n2t2.Axis(),  tb );
  
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Ra( nata );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rb( nbtb );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R;
  R = Rw1 * Ra * Rb;

  output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>( R, vctFixedSizeVector<double,3>(0.0))  );
  
  return SUCCESS;
}

    /*
    double h = inputrn.x.at(0) - xmin;
    vctMatrixRotation3<double,VCT_ROW_MAJOR> RB;
    //RB = R10 * R0b(h);
    //RB = Rw0 * Rb1(h);  // works for initial 
    //RB = R10 * Rb1(h);
    RB = R10 * R0b(h) * Rb1(h);
    //RB = R0b(h) * Rb1(h);
    //cout << vctFrame4x4<double,VCT_ROW_MAJOR>( RB, vctFixedSizeVector<double,3>(0.0)) << std::endl;
    outputrn = robVariablesvctFrame4x4<double,VCT_ROW_MAJOR>( SE3( RB, vctFixedSizeVector<double,3>(0.0)), vctFixedSizeVector<double,6>(0.0), R6(0.0) );
    */

