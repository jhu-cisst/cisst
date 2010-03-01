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

#include <cisstRobot/robVariable.h>
#include <cisstCommon/cmnLogger.h>

//! Clear all the variables
robVariable::robVariable(){ Clear(); }

// Only set the basis
robVariable::robVariable( robSpace::Basis basis ) : robSpace( basis )
{  Clear(); }

// Only set the time
robVariable::robVariable( double t ) : robSpace( robSpace::TIME ){
  Clear();
  time = t;
}

robVariable::robVariable( robSpace::Basis basis,
			  const vctMatrixRotation3<double>& R,
			  const vctFixedSizeVector<double,3>& w,
			  const vctFixedSizeVector<double,3>& wd ) :
  robSpace( basis & ( robSpace::ORIENTATION | 
		      robSpace::ANGULAR_VEL |
		      robSpace::ANGULAR_ACC ) ){
  Clear();
  this->R = R;
  this->w = w;
  this->wd =wd;

}

/*
// only set joints pos, vel, acc
robVariable::robVariable(robSpace::Basis basis,double x,double xd,double xdd) :
  robSpace( basis & ( robSpace::JOINTS_POS | robSpace::TRANSLATION |
		      robSpace::JOINTS_VEL | robSpace::LINEAR_VEL  |
		      robSpace::JOINTS_ACC | robSpace::LINEAR_ACC ) ){

  Clear();
}
*/

// only set joints pos, vel, acc
robVariable::robVariable( robSpace::Basis basis, 
			  const vctDynamicVector<double>& q,
			  const vctDynamicVector<double>& qd,
			  const vctDynamicVector<double>& qdd ) : 
  robSpace( basis & ( robSpace::JOINTS_POS | robSpace::TRANSLATION |
		      robSpace::JOINTS_VEL | robSpace::LINEAR_VEL  |
		      robSpace::JOINTS_ACC | robSpace::LINEAR_ACC ) ){

  Clear();
  this->q = q;
  this->qd = qd;
  this->qdd = qdd;
}

// clear everything 
void robVariable::Clear(){

  this->time = 0.0; 

  //this->x = 0;
  //this->xd = 0;
  //this->xdd = 0;

  this->R.Identity();
  this->w.SetAll(0.0);
  this->wd.SetAll(0.0);

  this->t.SetAll(0.0);
  this->v.SetAll(0.0);
  this->vd.SetAll(0.0);

  this->q.SetAll(0.0);
  this->qd.SetAll(0.0);
  this->qdd.SetAll(0.0);
}

void robVariable::IncludeBasis( robSpace::Basis basis,
			        const vctMatrixRotation3<double>& R,
				const vctFixedSizeVector<double,3>& w,
				const vctFixedSizeVector<double,3>& wd ){

  // "or" the basis
  GetBasis() |= (basis & robSpace::ORIENTATION);

  this->R = R;
  this->w = w;
  this->wd = wd;

}

void robVariable::IncludeBasis( robSpace::Basis basis,
				double y,
				double yd,
				double ydd ){

  // "or" the basis
  this->GetBasis() |= ( basis & (robSpace::TRANSLATION | robSpace::JOINTS_POS) );

  // Handle the translation positions
  if( basis == robSpace::TX )
    { this->t[0] = y;   this->v[0] = yd;   this->vd[0] = ydd; }
  if( basis == robSpace::TY )
    { this->t[1] = y;   this->v[1] = yd;   this->vd[1] = ydd; }
  if( basis == robSpace::TZ )
    { this->t[2] = y;   this->v[2] = yd;   this->vd[2] = ydd; }

  // Handle the joint positions    
  if( basis == robSpace::Q1 )
    { this->q[0] = y;   this->qd[0] = yd;   this->qdd[0] = ydd; }    
  if( basis == robSpace::Q2 )
    { this->q[1] = y;   this->qd[1] = yd;   this->qdd[1] = ydd; }    
  if( basis == robSpace::Q3 )
    { this->q[2] = y;   this->qd[2] = yd;   this->qdd[2] = ydd; }    
  if( basis == robSpace::Q4 )
    { this->q[3] = y;   this->qd[3] = yd;   this->qdd[3] = ydd; }    
  if( basis == robSpace::Q5 )
    { this->q[4] = y;   this->qd[4] = yd;   this->qdd[4] = ydd; }    
  if( basis == robSpace::Q6 )
    { this->q[5] = y;   this->qd[5] = yd;   this->qdd[5] = ydd; }    
  if( basis == robSpace::Q7 )
    { this->q[6] = y;   this->qd[6] = yd;   this->qdd[6] = ydd; }    
  if( basis == robSpace::Q8 )
    { this->q[7] = y;   this->qd[7] = yd;   this->qdd[7] = ydd; }    
  if( basis == robSpace::Q9 )
    { this->q[8] = y;   this->qd[8] = yd;   this->qdd[8] = ydd; }    

}

