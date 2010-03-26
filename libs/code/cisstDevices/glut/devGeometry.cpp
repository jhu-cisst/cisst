#include <GL/gl.h>
#include <cisstDevices/glut/devGeometry.h>


devGeometry::devGeometry(){ Rt.Eye(); }
devGeometry::~devGeometry(){}

void devGeometry::glMultMatrix( const vctFrame4x4<double>& Rt ) const {
  double H[4*4];
  H[0] = Rt[0][0]; H[4] = Rt[0][1]; H[8] = Rt[0][2]; H[12] = Rt[0][3];
  H[1] = Rt[1][0]; H[5] = Rt[1][1]; H[9] = Rt[1][2]; H[13] = Rt[1][3];
  H[2] = Rt[2][0]; H[6] = Rt[2][1]; H[10] =Rt[2][2]; H[14] = Rt[2][3];
  H[3] = 0.0;      H[7] = 0.0;      H[11] =0.0;      H[15] = 1.0;
  
  // WARNING: This is for double precision
  glMultMatrixd(H);
}

void devGeometry::SetPosition( const vctFixedSizeVector<double,3>& t ){
  Rt[0][3] = t[0];
  Rt[1][3] = t[1];
  Rt[2][3] = t[2];
}

void devGeometry::SetOrientation( const vctMatrixRotation3<double>& R ){
  Rt[0][0] = R[0][0]; Rt[0][1] = R[0][1]; Rt[0][2] = R[0][2];
  Rt[1][0] = R[1][0]; Rt[1][1] = R[1][1]; Rt[1][2] = R[1][2];
  Rt[2][0] = R[2][0]; Rt[2][1] = R[2][1]; Rt[2][2] = R[2][2];
}

void devGeometry::SetPositionOrientation( const vctFrame4x4<double>& Rt ){
  this->Rt[0][0] = Rt[0][0]; this->Rt[0][1] = Rt[0][1]; this->Rt[0][2] =Rt[0][2];
  this->Rt[1][0] = Rt[1][0]; this->Rt[1][1] = Rt[1][1]; this->Rt[1][2] =Rt[1][2];
  this->Rt[2][0] = Rt[2][0]; this->Rt[2][1] = Rt[2][1]; this->Rt[2][2] =Rt[2][2];

  this->Rt[0][3] = Rt[0][3];
  this->Rt[1][3] = Rt[1][3];
  this->Rt[2][3] = Rt[2][3];
}
