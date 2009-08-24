#include <cisstRobot/robDOF.h>

#include <iostream>
using namespace std;
using namespace cisstRobot;

robDOF::robDOF(){ dof = 0; t=0.0; }

robDOF::robDOF( uint64_t dof ){
  this->dof = dof;
  this->t = 0.0;
  this->Rt.Identity();
  this->vw.SetAll(0.0);
  this->vdwd.SetAll(0.0);
}

robDOF::robDOF( const SE3& Rt ){
  this->dof = robDOF::RT;
  this->t = 0.0;
  this->Rt = Rt;
  this->vw.SetAll(0.0);
  this->vdwd.SetAll(0.0);
}

robDOF::robDOF( const SE3& Rt, const R6& vw, const R6& vdwd ){
  this->dof = robDOF::RT | robDOF::VW | robDOF::VDWD;
  this->t = 0.0;
  this->Rt = Rt;
  this->vw = vw;
  this->vdwd = vdwd;
}

robDOF::robDOF( real x ){
  this->dof = robDOF::TIME;
  this->t = x; 
}

robDOF::robDOF( const Rn& x ){
  this->dof = robDOF::XPOS;
  this->t = 0.0;
  this->x = x; 
}

robDOF::robDOF( const Rn& x, const Rn& xd, const Rn& xdd ){
  this->dof = robDOF::XPOS | robDOF::XVEL | robDOF::XACC;
  this->t = 0.0;
  this->x = x;
  this->xd = xd;
  this->xdd = xdd;
}


bool robDOF::IsTime()        const {return 0 < (dof & TIME);}
bool robDOF::IsReal()        const {return 0 < (dof & REAL);}
bool robDOF::IsCartesian()   const {return 0 < (dof & CARTESIAN);}
bool robDOF::IsTranslation() const {return 0 < (dof & ( TX | TY | TZ )); }
bool robDOF::IsRotation()    const {return 0 < (dof & ( RX | RY | RZ )); }

uint64_t robDOF::GetDOF( )             const  {  return dof;  }
bool     robDOF::IsSet( uint64_t dof ) const  {  return 0 < ((this->dof & dof));  }

size_t robDOF::DOFtoIndex( uint64_t dof ){
  if( dof & ( robDOF::X1 | robDOF::X1D | robDOF::X1DD ) )
    return 0;
  if( dof & ( robDOF::X2 | robDOF::X2D | robDOF::X2DD ) )
    return 1;
  if( dof & ( robDOF::X3 | robDOF::X3D | robDOF::X3DD ) )
    return 2;
  if( dof & ( robDOF::X4 | robDOF::X4D | robDOF::X4DD ) )
    return 3;
  if( dof & ( robDOF::X5 | robDOF::X5D | robDOF::X5DD ) )
    return 4;
  if( dof & ( robDOF::X6 | robDOF::X6D | robDOF::X6DD ) )
    return 5;
  if( dof & ( robDOF::X7 | robDOF::X7D | robDOF::X7DD ) )
    return 6;
  if( dof & ( robDOF::X8 | robDOF::X8D | robDOF::X8DD ) )
    return 7;
  return 8;
}

void robDOF::Set( uint64_t dof, const SE3& Rt, const R6& vw, const R6& vdwd ){

  SetPos( dof, Rt );
  SetVel( dof, vw );
  SetAcc( dof, vdwd );

}

void robDOF::Set( uint64_t dof, const Rn& x, const Rn& xd, const Rn& xdd ){

  SetPos( dof, x );
  SetVel( dof, xd );
  SetAcc( dof, xdd );

}

void robDOF::SetPos( uint64_t dof, const SE3& Rt ){
  // ensure that only position/orientations are considered
  dof &= robDOF::RT;

  if( dof ){            // should we bother with this dof?
    if( dof & robDOF::RX ){ 
      for(int r=0; r<3; r++) this->Rt[r][0] = Rt[r][0]; 
      this->dof |= robDOF::RX;
    }
    if( dof & robDOF::RY ){ 
      for(int r=0; r<3; r++) this->Rt[r][1] = Rt[r][1]; 
      this->dof |= robDOF::RY;
    }
    if( dof & robDOF::RZ ){ 
      for(int r=0; r<3; r++) this->Rt[r][2] = Rt[r][2]; 
      this->dof |= robDOF::RZ;
    }
    if( dof & robDOF::TX ){  
      this->Rt[0][3] = Rt[0][3]; 
      this->dof |= robDOF::TX;
    }
    if( dof & robDOF::TY ){ 
      this->Rt[1][3] = Rt[1][3]; 
      this->dof |= robDOF::TY;
    }
    if( dof & robDOF::TZ ){ 
      this->Rt[2][3] = Rt[2][3]; 
      this->dof |= robDOF::TZ;
    }
  }
}

void robDOF::SetPos( uint64_t dof, const Rn& x ){

  // ensure only real positions are considered
  dof &= robDOF::XPOS;

  if( dof ){                                     // should we bother with this dof?

    uint64_t mask = robDOF::X1;                  // start the bit mask at X1
    size_t sidx = 0;                             // the source index
    for(size_t i=0; i<8; i++){                   // scan the 8 positions bits

      if( dof & mask ){                          // test the bit

	if( sidx < x.size() ){

	  size_t tidx=robDOF::DOFtoIndex(mask);  // get target vector index
	  if( tidx < 8 ){                        // make sure the index is valid
	    if(this->x.size()<=tidx)             // target vector big enough?
	      {this->x.resize( tidx+1 );}        // resize target vector
	    this->x.at(tidx) = x.at(sidx++);     // copy the value
	    this->dof |= mask;                   // mark the position bit
	  }
	  else { cout << "robDOF::SetPos: invalid index" << endl; }

	}
	else {cout << "robDOF::SetPos: source does not contain DOF" << endl;}
      }
      mask <<= 1;                                // shift the mask
    }
  }  
}

void robDOF::SetVel( uint64_t dof, const R6& vw ){
  // ensure that only linear/angular velocities are considered
  dof &= robDOF::VW;

  if( dof ){            // should we bother with this dof?
    if( dof & robDOF::VX ){ this->vw[0] = vw[0]; this->dof |= robDOF::VX; }
    if( dof & robDOF::VY ){ this->vw[1] = vw[1]; this->dof |= robDOF::VY; }
    if( dof & robDOF::VZ ){ this->vw[2] = vw[2]; this->dof |= robDOF::VZ; }
    if( dof & robDOF::WX ){ this->vw[3] = vw[3]; this->dof |= robDOF::WX; }
    if( dof & robDOF::WY ){ this->vw[4] = vw[4]; this->dof |= robDOF::WY; }
    if( dof & robDOF::WZ ){ this->vw[5] = vw[5]; this->dof |= robDOF::WZ; }
  }
}

void robDOF::SetVel( uint64_t dof, const Rn& xd ){

  // ensure only real velocities are considered
  dof &= robDOF::XVEL;

  if( dof ){                                     // should we bother with this dof?

    uint64_t mask = robDOF::X1D;                 // start the bit mask at X1D
    size_t sidx = 0;                             // the source index
    for(size_t i=0; i<8; i++){                   // scan the 8 positions bits

      if( dof & mask ){                          // test the bit

	if( sidx < xd.size() ){

	  size_t tidx=robDOF::DOFtoIndex(mask);// get target vector index
	  if( tidx < 8 ){                        // make sure the index is valid
	    if(this->xd.size()<=tidx)            // target vector big enough?
	      {this->xd.resize( tidx+1 );}       // resize target vector
	    this->xd.at(tidx) = xd.at(sidx++);   // copy the value
	    this->dof |= mask;                   // mark the position bit
	  }
	  else { cout << "robDOF::SetVel: invalid index" << endl; }

	}
	else {cout << "robDOF::SetVel: source does not contain DOF" << endl;}
      }
      mask <<= 1;                                // shift the mask
    }
  }  
}

void robDOF::SetAcc( uint64_t dof, const R6& vdwd ){
  // ensure that only linear/angular velocities are considered
  dof &= robDOF::VDWD;

  if( dof ){            // should we bother with this dof?
    if( dof & robDOF::VXD ){ this->vdwd[0] = vdwd[0]; this->dof |= robDOF::VXD; }
    if( dof & robDOF::VYD ){ this->vdwd[1] = vdwd[1]; this->dof |= robDOF::VYD; }
    if( dof & robDOF::VZD ){ this->vdwd[2] = vdwd[2]; this->dof |= robDOF::VZD; }
    if( dof & robDOF::WXD ){ this->vdwd[3] = vdwd[3]; this->dof |= robDOF::WXD; }
    if( dof & robDOF::WYD ){ this->vdwd[4] = vdwd[4]; this->dof |= robDOF::WYD; }
    if( dof & robDOF::WZD ){ this->vdwd[5] = vdwd[5]; this->dof |= robDOF::WZD; }
  }
}

void robDOF::SetAcc( uint64_t dof, const Rn& xdd ){

  // ensure only real velocities are considered
  dof &= robDOF::XACC;

  if( dof ){                                     // should we bother with this dof?

    uint64_t mask = robDOF::X1DD;                // start the bit mask at X1D
    size_t sidx = 0;                             // the source index
    for(size_t i=0; i<8; i++){                   // scan the 8 positions bits

      if( dof & mask ){                          // test the bit

	if( sidx < xdd.size() ){

	  size_t tidx=robDOF::DOFtoIndex(mask);// get target vector index
	  if( tidx < 8 ){                        // make sure the index is valid
	    if(this->xdd.size()<=tidx)           // target vector big enough?
	      {this->xdd.resize( tidx+1 );}      // resize target vector
	    this->xdd.at(tidx) = xdd.at(sidx++); // copy the value
	    this->dof |= mask;                   // mark the position bit
	  }
	  else { cout << "robDOF::SetAcc: invalid index" << endl; }

	}
	else {cout << "robDOF::SetAcc: source does not contain DOF" << endl;}
      }
      mask <<= 1;                                // shift the mask
    }
  }  
}

