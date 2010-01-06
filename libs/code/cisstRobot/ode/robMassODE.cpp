#include <cisstRobot/ode/robMassODE.h>
#include <cisstVector/vctFixedSizeMatrix.h>

robMassODE::robMassODE() : robMassBase() {}
robMassODE::~robMassODE(){}

void robMassODE::ConfigureMassODE( dBodyID bodyid ) { 

  // set the mass parameters used for ODE
  // find the moment of inertia tensor about the center of mass aligned with
  // the coordinate frame
  vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> moit = V.Transpose() * D * V;

  // Set the mass parameters for ODE the COM must be the body
  // reference (0,0,0) and the MOIT must be centered around the COM
  dMassSetParameters( &massode,
		      mass,
		      0.0, 0.0, 0.0,
		      moit[0][0], moit[1][1], moit[2][2],
		      moit[0][1], moit[0][2], moit[1][2] );
  
  dBodySetMass( bodyid, &massode );
  
}

