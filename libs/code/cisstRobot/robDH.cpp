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

#include <cisstRobot/robDH.h>
#include <iostream>
using namespace std;
robDH::robDH() { alpha = a = theta = d = offset = 0.0; }
robDH::~robDH(){}

void robDH::SetParameters( double alpha, double a, 
			   double theta, double d, 
			   double offset,
			   const std::string& type,
			   const std::string& convention ){

  this->alpha = alpha;
  this->a = a;
  this->theta = theta;
  this->d = d;
  this->offset = offset;
  
  if( type.compare("PRISMATIC") == 0 ||
      type.compare("prismatic") == 0 )
    this->sigma = 1.0;
  else if( type.compare("REVOLUTE") == 0 ||
	   type.compare("revolute") == 0){
    this->sigma = 0.0;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected PRISMATIC or REVOLUTE" 
		      << std::endl;
  }
  
  if( convention.compare("MODIFIED") == 0 || 
      convention.compare("modified") == 0 )
    this->modifiedDH = true;
  else if( convention.compare("STANDARD") == 0 || 
	   convention.compare("standard") == 0 ){
    this->modifiedDH = false;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected MODIFIED or standard" 
		      << std::endl;
  }
}

bool robDH::IsRevolute()  const { return sigma== 0.0; }
bool robDH::IsPrismatic() const { return 0.0 < sigma; }

//! Modified DH convention
bool robDH::IsModifiedDH() const { return modifiedDH; }

//! Sigma (prismatic/revolute)
double robDH::Sigma() const { return sigma; }

vctFixedSizeVector<double,3> robDH::PStar() const
{ return vctFixedSizeVector<double,3>( a, d*sin(alpha), d*cos(alpha) ); }
  
vctFrame4x4<double,VCT_ROW_MAJOR> robDH::ForwardKinematics( double q ) const { 
  double d = this->d;
  double theta = this->theta;

  if( IsModifiedDH() ){
    
    if( 0 < IsPrismatic() )
      d = offset + q;       // add the joint offset to the joint length
    else
      theta = offset + q;   // add the joint offset to the joint angle
    
    double ca = cos(alpha);       double sa = sin(alpha);	
    double ct = cos(theta);       double st = sin(theta);
    
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct,    -st,     0,
						st*ca,  ct*ca, -sa,
						st*sa,  ct*sa,  ca );
    vctFixedSizeVector<double,3> t( a, -sa*d, ca*d );
    
    return vctFrame4x4<double,VCT_ROW_MAJOR>( R, t );
  }
  
  else{
    if( 0 < IsPrismatic() )
      d = offset + q;       // add the joint offset to the joint length
    else
      theta = offset + q;   // add the joint offset to the joint angle
    
    double ca = cos(alpha);	  double sa = sin(alpha);	
    double ct = cos(theta);	  double st = sin(theta);
    
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct, -st*ca,  st*sa,
						st,  ct*ca, -ct*sa,
						0,     sa,     ca );
    
    vctFixedSizeVector<double,3> t(a*ct, a*st, d);
    return vctFrame4x4<double,VCT_ROW_MAJOR>( R, t );
  }
}

vctMatrixRotation3<double,VCT_ROW_MAJOR> robDH::Orientation( double q ) const {

  vctFrame4x4<double,VCT_ROW_MAJOR> Rt = ForwardKinematics( q );
  return vctMatrixRotation3<double,VCT_ROW_MAJOR>(Rt[0][0], Rt[0][1], Rt[0][2],
						  Rt[1][0], Rt[1][1], Rt[1][2],
						  Rt[2][0], Rt[2][1], Rt[2][2]);
}

void robDH::Read( std::istream& is ) {
  double alpha, a, theta, d, offset;
  std::string prismatic, convention;
  is >> alpha >> a >> theta >> d >> offset >> prismatic >> convention;

  if( alpha == -1.5708 )   alpha = -M_PI_2;
  if( alpha == 1.5708 )	   alpha =  M_PI_2;
  
  SetParameters( alpha, a, theta, d, offset, prismatic, convention );
}

void robDH::Write( std::ostream& os ) const {
  os << std::setw(10) << alpha 
     << std::setw(10) << a 
     << std::setw(10) << theta
     << std::setw(10) << d
     << std::setw(10) << offset;
  if( IsRevolute() )
    os << std::setw(10) << "REVOLUTE";
  else
    os << std::setw(10) << "PRISMATIC";
  if( IsModifiedDH() )
    os << std::setw(10) << "MODIFIED";
  else
    os << std::setw(10) << "STANDARD";
}
