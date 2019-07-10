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

#include <cisstRobot/robModifiedDH.h>

#include <iomanip>
#include <iostream>

robModifiedDH::robModifiedDH() : 
  robKinematics( robKinematics::MODIFIED_DH ){ 
  alpha = a = theta = d = 0.0;
}

robModifiedDH::robModifiedDH( double alpha, 
			      double a, 
			      double theta, 
			      double d,
			      const robJoint& joint ) :
  robKinematics( joint, robKinematics::MODIFIED_DH ),
  alpha( alpha ),
  a( a ),
  theta( theta ),
  d( d ){}

robModifiedDH::~robModifiedDH(){}

robKinematics* robModifiedDH::Clone() const 
{ return (robKinematics*) new robModifiedDH( *this ); }

vctFixedSizeVector<double,3> robModifiedDH::PStar() const
{ return vctFixedSizeVector<double,3>( a, -d*sin(alpha), d*cos(alpha) ); }
  
vctFrame4x4<double> robModifiedDH::ForwardKinematics( double q ) const { 

  double d = this->d;           // copy the prismatic value
  double theta = this->theta;   // copy the revolute value

  // Add the position offset to the joint value
  switch( GetType() ){
  case robJoint::HINGE:
    theta = theta + PositionOffset() + q; // add the joint offset to the joint angle
    break;
  case robJoint::SLIDER:
    d = d + PositionOffset() + q;     // add the joint offset to the joint length
    break;
  default:
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Unsupported joint type."
		      << std::endl;
  }

  // should be computed once
  double ca = cos(this->alpha); double sa = sin(this->alpha);	
  double ct = cos(theta);       double st = sin(theta);
    
  // modified DH transformation
  vctMatrixRotation3<double> R( ct,    -st,     0,
				st*ca,  ct*ca, -sa,
				st*sa,  ct*sa,  ca );
  vctFixedSizeVector<double,3> t( a, -sa*d, ca*d );

  return vctFrame4x4<double>( R, t );

}

vctMatrixRotation3<double> robModifiedDH::Orientation( double q ) const {
  vctFrame4x4<double> Rt = ForwardKinematics( q );
  return vctMatrixRotation3<double>( Rt[0][0], Rt[0][1], Rt[0][2],
				     Rt[1][0], Rt[1][1], Rt[1][2],
				     Rt[2][0], Rt[2][1], Rt[2][2] );
}

void robModifiedDH::ReadParameters( std::istream& is ) {
  is >> this->alpha  // read the stuff from the stream
     >> this->a 
     >> this->theta 
     >> this->d;

  // just make sure we're accureate
  if( this->alpha == -1.5708 ) this->alpha = -cmnPI_2;
  if( this->alpha ==  1.5708 ) this->alpha =  cmnPI_2;
}

#if CISST_HAS_JSON
void robModifiedDH::ReadParameters(const Json::Value &config)
{
    this->alpha = config.get("alpha", "0.00").asDouble();
    this->a = config.get("A", "0.00").asDouble();
    this->theta = config.get("theta", "0.00").asDouble();
    this->d = config.get("D", "0.00").asDouble();

    // just make sure we're accureate
    if( this->alpha == -1.5708 ) this->alpha = -cmnPI_2;
    if( this->alpha ==  1.5708 ) this->alpha =  cmnPI_2;
}
#endif

void robModifiedDH::WriteParameters( std::ostream& os ) const {
  os << std::setw(15) << "MODIFIED DH"
     << std::setw(10) << alpha 
     << std::setw(10) << a 
     << std::setw(10) << theta
     << std::setw(10) << d;
}
