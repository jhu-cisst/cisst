#include <cisstRobot/robBodyBase.h>
#include <cisstCommon/cmnLogger.h>

#include <iostream>
using namespace std;

robBodyBase::robBodyBase(){}
robBodyBase::~robBodyBase(){}

robError robBodyBase::Read( std::istream& is ){

  // Read the mass
  if( robMass::Read( is ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to read the mass." 
		      << std::endl;
    return ERROR;
  }

  // Read the geometry
  if( robGeom::Read( is ) == ERROR ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to read the geometry." 
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}

robError robBodyBase::Write( std::ostream& os ) const { 

  return robMass::Write( os ); 

}


robBodyBase& 
robBodyBase::operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ){

  if( *this != Rt ){
    // Set the body's pos+ori
    vctFrame4x4<double,VCT_ROW_MAJOR>::operator=( Rt );
    // set the geometry pos+ori
  }
  return *this;

}

void 
robBodyBase::SetBodyPosition( const vctFixedSizeVector<double,3>& t){

  TranslationRefType p = Translation();
  p[0] = t[0];
  p[1] = t[1];
  p[2] = t[2];

  robGeom::SetGeomPosition( t );

}

void 
robBodyBase::SetBodyOrientation(const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R){

  RotationRefType r = Rotation();
  r[0][0] = R[0][0]; r[0][1] = R[0][1]; r[0][2] = R[0][2];
  r[1][0] = R[1][0]; r[1][1] = R[1][1]; r[1][2] = R[1][2];
  r[2][0] = R[2][0]; r[2][1] = R[2][1]; r[2][2] = R[2][2];

  robGeom::SetGeomOrientation( R );

}

void 
robBodyBase::SetBodyLinearVelocity( const vctFixedSizeVector<double,3>&){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

}

void 
robBodyBase::SetBodyAngularVelocity( const vctFixedSizeVector<double,3>&){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

}

void 
robBodyBase::SetBodyForce( const vctFixedSizeVector<double,3>&){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

}

void 
robBodyBase::SetBodyTorque( const vctFixedSizeVector<double,3>&){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

}

vctFixedSizeVector<double,3> 
robBodyBase::GetBodyPosition(){

  return Translation();

}

vctMatrixRotation3<double,VCT_ROW_MAJOR> 
robBodyBase::GetBodyOrientation(){

  ConstRotationRefType R = Rotation();
  return vctMatrixRotation3<double,VCT_ROW_MAJOR>( R[0][0], R[0][1], R[0][2],
						   R[1][0], R[1][1], R[1][2],
						   R[2][0], R[2][1], R[2][2] );

}

vctFixedSizeVector<double,3> 
robBodyBase::GetBodyLinearVelocity(){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

  return vctFixedSizeVector<double,3>(0.0);

}

vctFixedSizeVector<double,3> 
robBodyBase::GetBodyAngularVelocity(){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

  return vctFixedSizeVector<double,3>(0.0);

}

vctFixedSizeVector<double,3>
robBodyBase::GetBodyForce(){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

  return vctFixedSizeVector<double,3>(0.0);

}

vctFixedSizeVector<double,3>
robBodyBase::GetBodyTorque(){

  CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                      << "Not implemented."
                      << std::endl;

  return vctFixedSizeVector<double,3>(0.0);

}

