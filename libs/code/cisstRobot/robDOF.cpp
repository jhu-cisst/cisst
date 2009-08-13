#include <cisstRobot/robDOF.h>

#include <iostream>
using namespace std;
using namespace cisstRobot;

robDOF::robDOF(){ dof = 0; }
robDOF::robDOF( uint64_t dof ) { this->dof = dof; }
robDOF::~robDOF(){}

bool robDOF::IsCartesian()   const {return dof & CARTESIAN;}
bool robDOF::IsTranslation() const {return dof & ( TX | TY | TZ ); }
bool robDOF::IsRotation()    const {return dof & ( RX | RY | RZ ); }
bool robDOF::IsReal()        const {return dof & REAL;}

size_t robDOF::HowMany()   const{
  if( IsCartesian() && IsReal() ){ 
    cout << "robDOF::HowMany: DOF is both Cartesian and real" << endl;
    return 0;
  }

  uint64_t tmp;
  if( IsCartesian() )  tmp = (dof & robDOF::CARTESIAN);
  if( IsReal() )       tmp = (dof & robDOF::REAL) >> 32;

  uint64_t mask = 0x01;
  size_t n = 0;
  for(size_t i=0; i<32; i++){
    if( tmp & mask ) {n++;}
    mask <<= 1;
  }
  return n;
}

void robDOF::Set( uint64_t dof )        {  this->dof = dof;  }
bool robDOF::IsSet( uint64_t dof ) const{  return (this->dof & dof);  }

robDOFForceTorque::robDOFForceTorque()
{forcetorque.SetAll(0.0); }
robDOFForceTorque::robDOFForceTorque( const R6& ft )
{ forcetorque=ft; }

robDOFSE3::robDOFSE3() : robDOF(0) {
  this->Rt.Identity();
  this->vw.SetAll(0.0);
  this->vdwd.SetAll(0.0);
}

robDOFSE3::robDOFSE3( const SE3& Rt, const R6& vw, const R6& vdwd )
  : robDOF( robDOFRn::RT | robDOFRn::VW | robDOFRn::VDWD ) {
  this->Rt = Rt;
  this->vw = vw;
  this->vdwd = vdwd;
}

robDOFSE3::robDOFSE3( const SE3& Rt ) : robDOF( robDOFRn::RT ) {
  this->Rt = Rt;
  this->vw.SetAll(0.0);
  this->vdwd.SetAll(0.0);
}

robDOFRn::robDOFRn() : robDOF( 0 ) {}

robDOFRn::robDOFRn( const Rn& x, const Rn& xd, const Rn& xdd )
  : robDOF( robDOFRn::XPOS | robDOFRn::XVEL | robDOFRn::XACC ) {
  this->x = x;
  this->xd = xd;
  this->xdd = xdd;
}

robDOFRn::robDOFRn( const Rn& x ) : robDOF( robDOFRn::XPOS ) {
  if( 8 < x.size() )
    { cout << "robDOFRn::robDOFRn: Vector too large!" << endl; }
  this->x = x;
}

robDOFRn::robDOFRn( real x ) : robDOF( robDOFRn::TIME ) {
  this->x = Rn(1, x);
}
