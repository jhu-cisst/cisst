#include <cisstRobot/robSO3Blender.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robSO3Blender::robSO3Blender( real ti,
			      real T1, real T2,
			      const SO3& Rw0, 
			      const SO3& Rw1, 
			      const SO3& Rw2){
  this->ti = ti;
  this->T1 = T1;
  this->T2 = T2;

  SO3 R0w;
  R0w.InverseOf( Rw0 );
  SO3 R01;
  R01 = R0w * Rw1;
  
  SO3 R1w;
  R1w.InverseOf( Rw1 );
  SO3 R12;
  R12 = R1w * Rw2;

  this->n1t1 = vctAxisAngleRotation3<real>(R01);
  this->n2t2 = vctAxisAngleRotation3<real>(R12);
  this->Rw1 = Rw1;

}

robSO3Blender::robSO3Blender( real xi,
			      const SO3& Rw0, const SO3& Rw1, 
			      const R3& w0w){

  xmin = xi;
  xmax = xi+2.0*robFunctionPiecewise::TAU;

  SO3 R0w, R1w;
  R0w.InverseOf(Rw0);
  R1w.InverseOf(Rw1);

  R10 = R1w * Rw0;                              // used by Rb()
  this->Rw0 = Rw0;
  R3 w00 = R0w*w0w;                             // w0 wrt to frame 0
  this->w0 = w00.Norm();                        // usedby Rv()
  uv.NormalizedOf( w00 );                       // used by Rv()

  SO3 RB1 = Rb1( 2.0*robFunctionPiecewise::TAU ); // compute the final Rv
  SO3 R0B( R0w * Rw1 * RB1.InverseSelf() );       // compute the final Rc

  uctc = vctAxisAngleRotation3<real>(R0B);       // used by Rc()

}

SO3 robSO3Blender::Rb1( real h ){

  real h2 = h *h;
  real h3 = h2*h;
  real h4 = h3*h;
  
  real H1 = 2.0*robFunctionPiecewise::TAU;
  real H2 = H1*H1;
  real H3 = H2*H1;

  real gv = (h4/H3 - 2.0*h3/H2 + 2.0*h)*w0/2.0;

  vctAxisAngleRotation3<real> rv( uv, gv );
  SO3 R0h(rv);
  return R0h;
}

SO3 robSO3Blender::R0b( real h ){
  real h2 = h *h;
  real h3 = h2*h;
  real h4 = h3*h;
  real h5 = h4*h;

  real H  = 2.0*robFunctionPiecewise::TAU;
  real H2 = H *H;
  real H3 = H2*H;
  real H4 = H3*H;
  real H5 = H4*H;

  real gc = (6.0*h5)/H5 - (15.0*h4)/H4 + (10.0*h3)/H3;
  vctAxisAngleRotation3<real> rc( uctc.Axis(), gc*uctc.Angle() );
  SO3 Rc(rc);
  return Rc;
}

robDomainAttribute robSO3Blender::IsDefinedFor( const robDOF& input ) const{
  
  try{
    
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
    
    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robSO3Blender::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // test to see that the input is a time value
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robSO3Blender::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    real t = inputrn.x.at(0);
    if( xmin <= t && t <= xmax ) return DEFINED;
    else                         return UNDEFINED;

  }
  catch( std::bad_cast ){
    cout<<"robSO3Blender::IsDefinedFor: unable to cast the input as a Rn"<<endl;
    return UNDEFINED;
  }

}

robError robSO3Blender::Evaluate( const robDOF& input, robDOF& output ){  

  try{
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input); 
    robDOFSE3& outputrn     = dynamic_cast<robDOFSE3&>(output); 

    real t = inputrn.x.at(0) - ti;
    real tau = robFunctionPiecewise::TAU;
    real tp = t-this->T1;
    
    real ta = (( tau - tp ) * ( tau - tp )/ (4.0*tau*T1)) * n1t1.Angle();
    real tb = (( tau + tp ) * ( tau + tp )/ (4.0*tau*T2)) * n2t2.Angle();
    vctAxisAngleRotation3<real> nata( n1t1.Axis(), -ta );
    vctAxisAngleRotation3<real> nbtb( n2t2.Axis(),  tb );

    SO3 Ra( nata );
    SO3 Rb( nbtb );
    SO3 R;
    R = Rw1 * Ra * Rb;

    outputrn = robDOFSE3( SE3( R, R3(0.0))  );
    
    /*
    real h = inputrn.x.at(0) - xmin;
    SO3 RB;
    //RB = R10 * R0b(h);
    //RB = Rw0 * Rb1(h);  // works for initial 
    //RB = R10 * Rb1(h);
    RB = R10 * R0b(h) * Rb1(h);
    //RB = R0b(h) * Rb1(h);
    //cout << SE3( RB, R3(0.0)) << endl;
    outputrn = robDOFSE3( SE3( RB, R3(0.0)), R6(0.0), R6(0.0) );
    */

    return SUCCESS;
  }
  catch( std::bad_cast ){
    cout << "robSO3Blender::Evaluate: unable to cast the input/output" << endl;
    return FAILURE;
  }
}

