#ifndef _robMassBase_h
#define _robMassBase_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFrame4x4.h>

class robMassBase{
protected:

  //! Parallel Axis Theorem
  /**
     Finds the moment of inertia wrt to a parallel axis
  */
  void ParallelAxis( const vctFixedSizeVector<double,3>& t ) {

    double m = Mass();
    vctFixedSizeVector<double,3> com = CenterOfMass();
    vctFixedSizeMatrix<double,3,3> Icm = MomentOfInertia();
    
    // MOIT wrt link coordinate frame
    vctFixedSizeMatrix<double,3,3> It;

    // inner product
    double tTt = t[0]*t[0] + t[1]*t[1] + t[2]*t[2];
    // outer product;
    vctFixedSizeMatrix<double,3,3> ttT( t[0]*t[0], t[0]*t[1], t[0]*t[2],
					t[1]*t[0], t[1]*t[1], t[1]*t[2],
					t[2]*t[0], t[2]*t[1], t[2]*t[2] );

    It= m * ( tTt * vctFixedSizeMatrix<double,3,3>::Eye() - ttT );
    SetParameters( m, com, Icm + It );

  }
  
  //! Set the mass parameters
  /**
     \param mass The mass of the body
     \param COM The center of gravity
     \param MOIT The moment of inertia tensor
  */
  virtual void SetParameters( double mass, 
			      const vctFixedSizeVector<double,3>& COM,
			      const vctFixedSizeMatrix<double,3,3>& MOIT,
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& offset = 
			            vctFrame4x4<double,VCT_ROW_MAJOR>() ) = 0;

public:

  robMassBase(){}
  virtual ~robMassBase(){}

  //! Return the mass
  virtual double Mass() const = 0;
  
  //! Return the center of mass
  virtual vctFixedSizeVector<double,3> CenterOfMass() const = 0;

  //! Return the moment of inertia tensor
  virtual vctFixedSizeMatrix<double,3,3> MomentOfInertia() const = 0;

  //! Read the mass from a input stream
  virtual void Read( std::istream& is ){
    double m;
    double comx, comy, comz;
    double Ixx, Iyy, Izz, Ixy, Iyz, Ixz;
    is >> m >> comx >> comy >> comz >> Ixx >> Iyy >> Izz >> Ixy >> Iyz >> Ixz;

    vctFixedSizeVector<double,3>   com(comx, comy, comz);
    vctFixedSizeMatrix<double,3,3> I (Ixx, Ixy, Ixz,
				      Ixy, Iyy, Iyz,
				      Ixz, Iyz, Izz );
    SetParameters( m, com, I );
  }
  
  //! Write the mass to an output stream
  virtual void Write( std::ostream& os ) const {
    double m = Mass();
    vctFixedSizeVector<double,3> com = CenterOfMass();
    vctFixedSizeMatrix<double,3,3> moi = MomentOfInertia();
    os << std::setw(10) << m 
       << std::setw(10) << com[0] 
       << std::setw(10) << com[1] 
       << std::setw(10) << com[2] 
       << std::setw(10) << moi[0][0] 
       << std::setw(10) << moi[1][1] 
       << std::setw(10) << moi[2][2] 
       << std::setw(10) << moi[0][1] 
       << std::setw(10) << moi[1][2] 
       << std::setw(10) << moi[0][2];
  }

};

#endif
