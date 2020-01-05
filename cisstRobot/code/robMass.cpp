#include <cisstRobot/robMass.h>
#include <cisstCommon/cmnLogger.h>

robMass::robMass(){
  mass = 0.0;       // set the mass to zero
  com.SetAll(0.0);  // set the center of mass to zero

  D[0][0] = 1.0;   D[0][1] = 0.0;   D[0][2] = 0.0; 
  D[1][0] = 0.0;   D[1][1] = 1.0;   D[1][2] = 0.0; 
  D[2][0] = 0.0;   D[2][1] = 0.0;   D[2][2] = 1.0; 

  V[0][0] = 1.0;   V[0][1] = 0.0;   V[0][2] = 0.0; 
  V[1][0] = 0.0;   V[1][1] = 1.0;   V[1][2] = 0.0; 
  V[2][0] = 0.0;   V[2][1] = 0.0;   V[2][2] = 1.0; 

}

robMass::robMass( double m,
		  const vctFixedSizeVector<double,3>& com,
		  const vctFixedSizeMatrix<double,3,3>& D,
		  const vctFixedSizeMatrix<double,3,3>& V ) :
  mass( m ),
  com( com ),
  D( D ),
  V( V ) {}

const double & robMass::Mass(void) const 
{ return mass; }
double & robMass::Mass(void)
{ return mass; }

vctFixedSizeVector<double,3> robMass::CenterOfMass() const 
{ return com; }

vctFixedSizeMatrix<double,3,3> robMass::MomentOfInertia() const { 
  // Rotate and translate the moment of inertia to the body's origin.
  return ParallelAxis( Mass(), -CenterOfMass(), V.Transpose() * D * V );
}

vctFixedSizeMatrix<double,3,3> robMass::MomentOfInertiaAtCOM() const {
  return V.Transpose() * D * V;
}

vctFixedSizeMatrix<double,3,3> 
robMass::ParallelAxis( double m, 
		       const vctFixedSizeVector<double,3>& t, 
		       const vctFixedSizeMatrix<double,3,3>& I ) const {

  // inner product
  double tTt = t[0]*t[0] + t[1]*t[1] + t[2]*t[2];

  // outer product;
  vctFixedSizeMatrix<double,3,3> ttT( t[0]*t[0], t[0]*t[1], t[0]*t[2],
				      t[1]*t[0], t[1]*t[1], t[1]*t[2],
				      t[2]*t[0], t[2]*t[1], t[2]*t[2] );

  // compute the offset It
  vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> It;
  It = m* ( tTt * vctFixedSizeMatrix<double,3,3>::Eye() - ttT );
  
  return I + It;
}

//! Read the mass from an input stream
robMass::Errno robMass::ReadMass( std::istream& is ) {

  double x1=1, x2=0, x3=0, y1=0, y2=1, y3=0, z1=0, z2=0, z3=1; // principal axes


  mass = 1;
  com = vctFixedSizeVector<double,3>(0.0);
  D[0][0] = D[1][1] = D[2][2] = 1.0;

  is >> mass                                 // read everything
     >> com[0] >> com[1] >> com[2] 
     >> D[0][0] >> D[1][1] >> D[2][2]
     >> x1 >> x2 >> x3
     >> y1 >> y2 >> y3
     >> z1 >> z2 >> z3;

  if( D[0][0] < 0.0 || D[1][1] < 0.0 || D[2][2] < 0.0 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Principal moments of inertia must be non-negative"
		      << std::endl;
    return robMass::EFAILURE;
  }
  
  vctDynamicVector<double> e1(3, x1, x2, x3);
  vctDynamicVector<double> e2(3, y1, y2, y3);
  vctDynamicVector<double> e3(3, z1, z2, z3);
  V = vctMatrixRotation3<double>( e1, e2, e3, true, VCT_NORMALIZE );

  return robMass::ESUCCESS;
}

#if CISST_HAS_JSON
robMass::Errno robMass::ReadMass(const Json::Value &config)
{
    double x1=1, x2=0, x3=0, y1=0, y2=1, y3=0, z1=0, z2=0, z3=1; // principal axes

    mass = 1;
    com = vctFixedSizeVector<double,3>(0.0);
    D[0][0] = D[1][1] = D[2][2] = 1.0;

    // Read everything from JSON config
    mass = config.get("mass", 1.0).asDouble();
    com[0] = config.get("cx", 0.0).asDouble();
    com[1] = config.get("cy", 0.0).asDouble();
    com[2] = config.get("cz", 0.0).asDouble();
    D[0][0] = config.get("Ixx", 0.0001).asDouble();
    D[1][1] = config.get("Iyy", 0.0001).asDouble();
    D[2][2] = config.get("Izz", 0.0001).asDouble();
    x1 = config.get("x1", 1.0).asDouble();
    x2 = config.get("x2", 0.0).asDouble();
    x3 = config.get("x3", 0.0).asDouble();
    y1 = config.get("y1", 0.0).asDouble();
    y2 = config.get("y2", 1.0).asDouble();
    y3 = config.get("y3", 0.0).asDouble();
    z1 = config.get("z1", 0.0).asDouble();
    z2 = config.get("z2", 0.0).asDouble();
    z3 = config.get("z3", 1.0).asDouble();

    if( D[0][0] < 0.0 || D[1][1] < 0.0 || D[2][2] < 0.0 ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                << ": Principal moments of inertia must be non-negative"
                << std::endl;
      return robMass::EFAILURE;
    }

    vctDynamicVector<double> e1(3, x1, x2, x3);
    vctDynamicVector<double> e2(3, y1, y2, y3);
    vctDynamicVector<double> e3(3, z1, z2, z3);
    V = vctMatrixRotation3<double>( e1, e2, e3, true, VCT_NORMALIZE );

    return robMass::ESUCCESS;
}
#endif

//! Write the mass to an output stream
robMass::Errno robMass::WriteMass( std::ostream& os ) const {

  vctFixedSizeMatrix<double,3,3> moi = MomentOfInertia();
  os << std::setw(13) << mass
     << std::setw(13) << com[0] 
     << std::setw(13) << com[1] 
     << std::setw(13) << com[2] 
     << std::setw(13) << moi[0][0] 
     << std::setw(13) << moi[1][1] 
     << std::setw(13) << moi[2][2] 
     << std::setw(13) << moi[0][1] 
     << std::setw(13) << moi[1][2] 
     << std::setw(13) << moi[0][2];
  return robMass::ESUCCESS;

}
