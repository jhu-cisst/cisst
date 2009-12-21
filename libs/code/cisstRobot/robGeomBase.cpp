#include <cisstRobot/robGeomBase.h>

#include <GL/gl.h>

robGeomBase::robGeomBase(){ Rt.Eye(); }
robGeomBase::~robGeomBase(){}

void 
robGeomBase::glMultMatrix( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) const {

  double H[4*4];
  
  H[0] = Rt[0][0]; H[4] = Rt[0][1]; H[8] = Rt[0][2]; H[12] = Rt[0][3];
  H[1] = Rt[1][0]; H[5] = Rt[1][1]; H[9] = Rt[1][2]; H[13] = Rt[1][3];
  H[2] = Rt[2][0]; H[6] = Rt[2][1]; H[10] =Rt[2][2]; H[14] = Rt[2][3];
  H[3] = 0.0;      H[7] = 0.0;      H[11] =0.0;      H[15] = 1.0;
  
  // WARNING: This is for double precision
  glMultMatrixd(H);

}

robError 
robGeomBase::Read( std::istream& is ){

  std::string filename;
  is >> filename;
  return Load( filename );

}

void 
robGeomBase::SetGeomPosition( const vctFixedSizeVector<double,3>& t ){

  Rt[0][3] = t[0];
  Rt[1][3] = t[1];
  Rt[2][3] = t[2];

}

void 
robGeomBase::SetGeomOrientation( const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R ){

  Rt[0][0] = R[0][0]; Rt[0][1] = R[0][1]; Rt[0][2] = R[0][2];
  Rt[1][0] = R[1][0]; Rt[1][1] = R[1][1]; Rt[1][2] = R[1][2];
  Rt[2][0] = R[2][0]; Rt[2][1] = R[2][1]; Rt[2][2] = R[2][2];

}

vctFixedSizeVector<double,3> 
robGeomBase::GetGeomPosition() const { 

  return vctFixedSizeVector<double,3>( Rt[0][3], 
				       Rt[1][3],
				       Rt[2][3] );

}

vctMatrixRotation3<double,VCT_ROW_MAJOR> 
robGeomBase::GetGeomOrientation() const {

  return vctMatrixRotation3<double,VCT_ROW_MAJOR>(Rt[0][0], Rt[0][1], Rt[0][2],
						  Rt[1][0], Rt[1][1], Rt[1][2],
						  Rt[2][0], Rt[2][1], Rt[2][2]);

}
