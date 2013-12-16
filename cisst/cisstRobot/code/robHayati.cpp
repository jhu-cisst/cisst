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

#include <cisstRobot/robHayati.h>

#include <iomanip>
#include <iostream>

robHayati::robHayati() : 
  robKinematics( robKinematics::HAYATI ){ 
  alpha = beta = theta = d = 0.0;
}

robHayati::robHayati( double alpha, 
		      double beta, 
		      double theta, 
		      double d,
		      const robJoint& joint ) :
  robKinematics( joint, robKinematics::HAYATI ),
  alpha( alpha ),
  beta( beta ),
  theta( theta ),
  d( d ){}

robHayati::~robHayati(){}

robKinematics* robHayati::Clone() const 
{ return (robKinematics*) new robHayati( *this ); }

vctFixedSizeVector<double,3> robHayati::PStar() const
{ return vctFixedSizeVector<double,3>( d, d*sin(alpha), d*cos(alpha) ); }
  
vctFrame4x4<double> robHayati::ForwardKinematics( double q ) const { 

  double d = this->d;           // copy the prismatic value
  double theta = this->theta;   // copy the revolute value

  // Add the position offset to the joint value
  switch( GetType() ){

  case robJoint::HINGE:
    {
      theta = theta + PositionOffset() + q;
 
      double ca = cos(this->alpha); double sa = sin(this->alpha);	
      double cb = cos(this->beta);  double sb = sin(this->beta);	
      double ct = cos(theta);       double st = sin(theta);
    
      vctFrame4x4<double> Rz( vctMatrixRotation3<double>(  ct, -st, 0.0,
							   st,  ct, 0.0,
							   0.0, 0.0, 1.0 ),
			      vctFixedSizeVector<double,3>( 0.0 ) );
      
      vctFrame4x4<double> Tx( vctMatrixRotation3<double>(),
			      vctFixedSizeVector<double,3>( d, 0.0, 0.0 ) );
      
      vctFrame4x4<double> Rx( vctMatrixRotation3<double>( 1.0, 0.0, 0.0, 
							  0.0,  ca, -sa,
							  0.0,  sa,  ca ),
			      vctFixedSizeVector<double,3>( 0.0 ) );
      
      vctFrame4x4<double> Ry( vctMatrixRotation3<double>(  cb, 0.0,  sb,
							  0.0, 1.0, 0.0,
							  -sb, 0.0,  cb ),
			      vctFixedSizeVector<double,3>( 0.0 ) );
      
      return Rz*Tx*Rx*Ry;
    }

  case robJoint::SLIDER:
    {
      d = d + PositionOffset() + q;

      double ca = cos(this->alpha); double sa = sin(this->alpha);	
      double cb = cos(this->beta);  double sb = sin(this->beta);	
    
      vctFrame4x4<double> Tz( vctMatrixRotation3<double>(),
                              vctFixedSizeVector<double,3>( 0.0, 0.0, d ) );

      vctFrame4x4<double> Rx( vctMatrixRotation3<double>( 1.0, 0.0, 0.0,
							  0.0,  ca, -sa,
							  0.0,  sa,  ca ),
			      vctFixedSizeVector<double,3>( 0.0 ) );
      
      vctFrame4x4<double> Ry( vctMatrixRotation3<double>(  cb, 0.0,  sb,
							  0.0, 1.0, 0.0,
							  -sb, 0.0,  cb ),
			      vctFixedSizeVector<double,3>( 0.0 ) );
      
      return Tz*Rx*Ry;
    }

  default:
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Unsupported joint type."
		      << std::endl;
    
    return vctFrame4x4<double>();
  }

}

vctMatrixRotation3<double> robHayati::Orientation( double q ) const {
  vctFrame4x4<double> Rt = ForwardKinematics( q );
  return vctMatrixRotation3<double>( Rt[0][0], Rt[0][1], Rt[0][2],
				     Rt[1][0], Rt[1][1], Rt[1][2],
				     Rt[2][0], Rt[2][1], Rt[2][2] );
}

void robHayati::ReadParameters( std::istream& is ) {
  is >> this->alpha  // read the stuff from the stream
     >> this->beta 
     >> this->theta 
     >> this->d;

  // just make sure we're accureate
  if( this->alpha == -1.5708 ) this->alpha = -cmnPI_2;
  if( this->alpha ==  1.5708 ) this->alpha =  cmnPI_2;
}

#if CISST_HAS_JSON
void robHayati::ReadParameters(const Json::Value &config)
{
    this->alpha = config.get("alpha", "0.00").asDouble();
    this->beta = config.get("beta", "0.00").asDouble();
    this->theta = config.get("theta", "0.00").asDouble();
    this->d = config.get("D", "0.00").asDouble();

    // just make sure we're accureate
    if( this->alpha == -1.5708 ) this->alpha = -cmnPI_2;
    if( this->alpha ==  1.5708 ) this->alpha =  cmnPI_2;
}
#endif // CISST_HAS_JSON

void robHayati::WriteParameters( std::ostream& os ) const {
  os << std::setw(10) << "HAYATI"
     << std::setw(10) << alpha 
     << std::setw(10) << beta
     << std::setw(10) << theta
     << std::setw(10) << d;
}
