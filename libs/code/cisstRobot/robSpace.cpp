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

#include <cisstRobot/robSpace.h>
#include <cisstCommon/cmnLogger.h>

//! Clear all the variables
robSpace::robSpace(){ basis = 0; }

// Only set the basis
robSpace::robSpace( robSpace::Basis basis )
{ this->basis = basis; }

// return the basis
robSpace::Basis robSpace::GetBasis() const 
{ return this->basis; }

robSpace::Basis& robSpace::GetBasis() 
{ return this->basis; }

// compare the basis 
bool robSpace::IsEnabled( robSpace::Basis basis ) const 
{ return this->basis & basis; }

// is the time variable set
bool robSpace::IsTimeEnabled() const 
{ return IsEnabled( robSpace::TIME ); }

// is any translation set
bool robSpace::IsTranslationEnabled() const 
{ return IsEnabled( robSpace::TRANSLATION ); }

// is any orientation set
bool robSpace::IsOrientationEnabled() const 
{ return IsEnabled( robSpace::ORIENTATION ); }

bool robSpace::IsCartesianVelocityEnabled() const
{ return IsEnabled( robSpace::CARTESIAN_VEL ); }

bool robSpace::IsCartesianAccelerationEnabled() const
{ return IsEnabled( robSpace::CARTESIAN_ACC ); }

bool robSpace::IsCartesianPositionEnabled() const
{ return IsTranslationEnabled() || IsOrientationEnabled(); }

bool robSpace::IsJointPositionEnabled() const
{ return IsEnabled( robSpace::JOINTS_POS ); }

bool robSpace::IsJointVelocityEnabled() const
{ return IsEnabled( robSpace::JOINTS_VEL ); }

bool robSpace::IsJointAccelerationEnabled() const
{ return IsEnabled( robSpace::JOINTS_ACC ); }

// is any joint variable set
bool robSpace::IsJointEnabled() const
{ return ( IsJointPositionEnabled() ||
	   IsJointVelocityEnabled() ||
	   IsJointAccelerationEnabled() ); }

std::vector<robSpace::Basis> robSpace::ListBasis() const {

  std::vector<robSpace::Basis> listofbasis;
  robSpace::Basis b = robSpace::TIME;
  for( size_t i=0; i<sizeof(robSpace::Basis)*8; i++ ){
    if( basis & b ) listofbasis.push_back( b );
    b <<= 1;
  }

  return listofbasis;
}

size_t robSpace::JointBasis2ArrayIndex() const {
  switch( basis & (robSpace::JOINTS_POS | robSpace::JOINTS_VEL) ){
  case robSpace::Q1:  return 0;
  case robSpace::Q1D: return 0;

  case robSpace::Q2:  return 1;
  case robSpace::Q2D: return 1;

  case robSpace::Q3:  return 2;
  case robSpace::Q3D: return 2;

  case robSpace::Q4:  return 3;
  case robSpace::Q4D: return 3;

  case robSpace::Q5:  return 4;
  case robSpace::Q5D: return 4;

  case robSpace::Q6:  return 5;
  case robSpace::Q6D: return 5;

  case robSpace::Q7:  return 6;
  case robSpace::Q7D: return 6;

  case robSpace::Q8:  return 7;
  case robSpace::Q8D: return 7;

  case robSpace::Q9:  return 8;
  case robSpace::Q9D: return 8;

  default:            return 9;
  }
}
