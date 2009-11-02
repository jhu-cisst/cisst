#ifndef _robMassGeneric_h
#define _robMassGeneric_h

#include <cisstRobot/robMassBase.h>

class robMassGeneric : public robMassBase{

private:

  //! The mass
  double mass; 

  //! The center of mass
  vctFixedSizeVector<double,3> com;

  //! The moment of inertia tensor
  vctFixedSizeMatrix<double,3,3> moit;

protected:

  //! Set the mass parameters
  /**
     \param mass The mass of the body
     \param COM The center of gravity
     \param MOIT The moment of inertia tensor
  */
  void SetParameters( double mass, 
		      const vctFixedSizeVector<double,3>& com,
		      const vctFixedSizeMatrix<double,3,3>& moit,
		      const vctFrame4x4<double,VCT_ROW_MAJOR>&  ){
    this->mass = mass;
    this->com = com;
    this->moit = moit;
  }

public:

  robMassGeneric(){}
  ~robMassGeneric(){}

  //! Return the mass
  double Mass() const { return mass; }

  //! Return the center of mass
  vctFixedSizeVector<double,3> CenterOfMass() const { return com; }

  //! Return the moment of inertia tensor
  vctFixedSizeMatrix<double,3,3> MomentOfInertia() const { return moit; }
  
};

#endif
