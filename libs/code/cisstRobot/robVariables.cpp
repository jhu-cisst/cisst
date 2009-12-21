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
#include <cisstRobot/robVariables.h>

#include <iostream>

//! Clear all the variables
robVariables::robVariables(){ Clear(); }

//! Set the time variable
robVariables::robVariables( double t ){
  Clear();
  Set( TIME, t);
}

robVariables::robVariables( robVariablesMask mask ){
  Clear();
  this->mask = mask;
}

robVariables::robVariables(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt){
  Clear();
  Set( CARTESIAN_POS, Rt );
}

robVariables::robVariables( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
			    const vctFixedSizeVector<double,6>& vw, 
			    const vctFixedSizeVector<double,6>& vdwd ){
  Clear();

  Set( CARTESIAN_POS, Rt );
  Set( CARTESIAN_VEL, vw );
  Set( CARTESIAN_ACC, vdwd );
}

robVariables::robVariables( const vctDynamicVector<double>& q, 
			    const vctDynamicVector<double>& qd, 
			    const vctDynamicVector<double>& qdd ){
  Clear();

  if( !q.empty() ){                     // ensures the vector isn't empty
    robVariablesMask mask = 0;          // start with an empty mask
    for( size_t i=0; i<q.size(); i++ )  // for each joint in the vector
      mask |= (Q1 << i);                // build a mask
    Set( mask, q );
  }      

  if( !qd.empty() ){                    // ensures the vector isn't empty
    robVariablesMask mask = 0;          // start with an empty mask
    for( size_t i=0; i<qd.size(); i++ ) // for each joint in the vector
      mask |= (Q1D << i);               // build a mask
    Set( mask, qd );
  }      

  if( !qdd.empty() ){                   // ensures the vector isn't empty
    robVariablesMask mask = 0;          // start with an empty mask
    for( size_t i=0; i<qdd.size(); i++ )// for each joint in the vector
      mask |= (Q1DD << i);              // build a mask
    Set( mask, qdd );
  }      

}

void robVariables::Clear(){
  mask = 0;

  time=0.0; 

  Rt.Identity();
  vw.SetAll(0.0);
  vdwd.SetAll(0.0);

  q.SetSize(0);
  qd.SetSize(0);
  qdd.SetSize(0);

}


robVariablesMask robVariables::GetVariables() const 
{ return this->mask; }

bool robVariables::IsVariableSet( robVariablesMask mask ) const 
{ return this->mask & mask; }

bool robVariables::IsTimeSet() const 
{ return IsVariableSet( TIME ); }

bool robVariables::IsTranslationSet() const 
{ return IsVariableSet( TRANSLATION ); }

bool robVariables::IsOrientationSet() const 
{ return IsVariableSet( ROTATION ); }

bool robVariables::IsCartesianSet() const
{ return IsVariableSet( ROTATION | TRANSLATION ); }

bool robVariables::IsJointSet() const
{ return IsVariableSet( JOINTS_POS | JOINTS_VEL | JOINTS_ACC ); }

void robVariables::Set( robVariablesMask mask, double t ){
  this->mask |= mask;
  this->time = t;
}

void robVariables::Set( robVariablesMask mask, 
			const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
			const vctFixedSizeVector<double,6>& vw, 
			const vctFixedSizeVector<double,6>& vdwd ){
  Set( mask & CARTESIAN_POS, Rt );
  Set( mask & CARTESIAN_VEL, vw );
  Set( mask & CARTESIAN_ACC, vdwd );
}

void robVariables::Set( robVariablesMask mask, 
			const vctDynamicVector<double>& q, 
			const vctDynamicVector<double>& qd, 
			const vctDynamicVector<double>& qdd ){
  Set( mask & JOINTS_POS, q );
  Set( mask & JOINTS_VEL, qd );
  Set( mask & JOINTS_ACC, qdd );
}

void robVariables::Set( robVariablesMask mask,
			const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ){

  // quick check to see if there's anything
  if( mask & CARTESIAN_POS ){

    if( mask & RX ){ 
      for(int r=0; r<3; r++) this->Rt[r][0] = Rt[r][0]; 
      this->mask |= RX;
    }
    
    if( mask & RY ){ 
      for(int r=0; r<3; r++) this->Rt[r][1] = Rt[r][1]; 
      this->mask |= RY;
    }
    
    if( mask & RZ ){ 
      for(int r=0; r<3; r++) this->Rt[r][2] = Rt[r][2]; 
      this->mask |= RZ;
    }
    
    if( mask & TX ){  
      this->Rt[0][3] = Rt[0][3]; 
      this->mask |= TX;
    }
    
    if( mask & TY ){ 
      this->Rt[1][3] = Rt[1][3]; 
      this->mask |= TY;
    }
    
    if( mask & TZ ){ 
      this->Rt[2][3] = Rt[2][3]; 
      this->mask |= TZ;
    }
  }
}

void robVariables::Set( robVariablesMask mask, 
			const vctDynamicVector<double>& x ){

  if( mask & JOINTS_POS){                   // should we bother with this dof?

    robVariablesMask qimask = Q1;           // start the bit mask at Q1
    size_t sidx = 0;                        // the source index

    for(size_t i=0; i<9; i++){              // scan the 9 joints positions bits

      if( mask & qimask ){                  // test the bit at position i

	if( sidx < x.size() ){              // ensure the source is available

	  size_t tidx = MaskToIndex(qimask);// get target vector index

	  if( tidx < 9 ){                   // make sure the index is valid
	    if( this->q.size()<=tidx )      // is target vector big enough?
	      { this->q.resize( tidx+1 ); } // if not resize the target vector
	    this->q.at(tidx) = x.at(sidx++);// copy the value
	    this->mask |= qimask;           // set the bit
	  }
	  else { 
	    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
				<< ": Invalid index" 
				<< std::endl;
	  }

	}
	else {
	  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			      << ": Source does not contain the variable"
			      << std::endl;
	}
      }
      qimask <<= 1;                              // shift the mask
    }
  }

  else if( mask & JOINTS_VEL ){
    
    robVariablesMask qimask = Q1D;               // start the bit mask at X1D
    size_t sidx = 0;                             // the source index
    for(size_t i=0; i<9; i++){                   // scan the 8 positions bits
      
      if( mask & qimask ){                       // test the bit
	
	if( sidx < x.size() ){
	  
	  size_t tidx = MaskToIndex(qimask);     // get target vector index
	  if( tidx < 9 ){                        // make sure the index is valid
	    if( this->qd.size()<=tidx )          // target vector big enough?
	      { this->qd.resize( tidx+1 ); }     // resize target vector
	    this->qd.at(tidx) = x.at(sidx++);    // copy the value
	    this->mask |= qimask;                // mark the position bit
	  }
	  else { 
	    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
				<< ": Invalid index" 
				<< std::endl;
	  }
	}

	else {
	  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			      << ": Source does not contain DOF" 
			      << std::endl;
	}
      }
      qimask <<= 1;                              // shift the mask
    }
  }
  else if( mask & JOINTS_ACC ){

    robVariablesMask qimask = Q1DD;              // start the bit mask at X1D
    size_t sidx = 0;                             // the source index
    for(size_t i=0; i<9; i++){                   // scan the 8 positions bits
      
      if( mask & qimask ){                       // test the bit
	
	if( sidx < x.size() ){
	  
	  size_t tidx = MaskToIndex(qimask);     // get target vector index
	  if( tidx < 9 ){                        // make sure the index is valid
	    if( this->qdd.size()<=tidx )         // target vector big enough?
	      { this->qdd.resize( tidx+1 ); }    // resize target vector
	    this->qdd.at(tidx) = x.at(sidx++);   // copy the value
	    this->mask |= qimask;                // mark the position bit
	  }
	  else { 
	    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
				<< ": Invalid index" 
				<< std::endl;
	  }
	}

	else {
	  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			      << ": Source does not contain DOF" 
			      << std::endl;
	}
      }
      qimask <<= 1;                              // shift the mask
    }
  }
}

void robVariables::Set( robVariablesMask mask, 
			const vctFixedSizeVector<double,6>& x ){
  
  if( mask & CARTESIAN_VEL){            // should we bother with this dof?
    if( mask & VX ){ this->vw[0] = x[0]; this->mask |= VX; }
    if( mask & VY ){ this->vw[1] = x[1]; this->mask |= VY; }
    if( mask & VZ ){ this->vw[2] = x[2]; this->mask |= VZ; }
    if( mask & WX ){ this->vw[3] = x[3]; this->mask |= WX; }
    if( mask & WY ){ this->vw[4] = x[4]; this->mask |= WY; }
    if( mask & WZ ){ this->vw[5] = x[5]; this->mask |= WZ; }
  }
  else if( mask & CARTESIAN_ACC){       // should we bother with this dof?
    if( mask & VXD ){ this->vdwd[0] = x[0]; this->mask |= VXD; }
    if( mask & VYD ){ this->vdwd[1] = x[1]; this->mask |= VYD; }
    if( mask & VZD ){ this->vdwd[2] = x[2]; this->mask |= VZD; }
    if( mask & WXD ){ this->vdwd[3] = x[3]; this->mask |= WXD; }
    if( mask & WYD ){ this->vdwd[4] = x[4]; this->mask |= WYD; }
    if( mask & WZD ){ this->vdwd[5] = x[5]; this->mask |= WZD; }
  }
}

size_t robVariables::MaskToIndex( robVariablesMask mask ){
  if( mask & ( Q1 | Q1D | Q1DD ) )  return 0;
  if( mask & ( Q2 | Q2D | Q2DD ) )  return 1;
  if( mask & ( Q3 | Q3D | Q3DD ) )  return 2;
  if( mask & ( Q4 | Q4D | Q4DD ) )  return 3;
  if( mask & ( Q5 | Q5D | Q5DD ) )  return 4;
  if( mask & ( Q6 | Q6D | Q6DD ) )  return 5;
  if( mask & ( Q7 | Q7D | Q7DD ) )  return 6;
  if( mask & ( Q8 | Q8D | Q8DD ) )  return 7;
  if( mask & ( Q9 | Q9D | Q9DD ) )  return 8;
  return 9;
}

