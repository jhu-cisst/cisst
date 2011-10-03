#include "robWAMKinematics.h"

vctFrame4x4<double> WAM7FKine( size_t i, double t ){

  double DH[7][3] = { {  0.0,   -cmnPI_2, 0.346 },   // link 1
		      {  0.0,    cmnPI_2, 0.0   },   // link 2
		      {  0.045, -cmnPI_2, 0.55  },   // link 3
		      { -0.045,  cmnPI_2, 0.0   },   // link 4
		      {  0.0,   -cmnPI_2, 0.3   },   // link 5
		      {  0.0,    cmnPI_2, 0.0   },   // link 6
		      {  0.0,     0.0,    0.062 } }; // link 7
  
  double A = DH[i][0];
  double sa = sin( DH[i][1] );
  double ca = cos( DH[i][1] );
  double D = DH[i][2];

  double st = sin(t);
  double ct = cos(t);

  vctFrame4x4<double> Rt;
  Rt[0][0] =  ct; Rt[0][1] = -st*ca; Rt[0][2] =  st*sa; Rt[0][3] = A*ct;
  Rt[1][0] =  st; Rt[1][1] =  ct*ca; Rt[1][2] = -ct*sa; Rt[1][3] = A*st;
  Rt[2][0] = 0.0; Rt[2][1] =     sa; Rt[2][2] =     ca; Rt[2][3] =    D;
  Rt[3][0] = 0.0; Rt[3][1] =    0.0; Rt[3][2] =    0.0; Rt[3][3] =  1.0;

  return Rt;
  
}
