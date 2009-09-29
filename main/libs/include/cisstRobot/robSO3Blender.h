#ifndef _robSO3Blender_h
#define _robSO3Blender_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <cisstVector/vctTransformationTypes.h>

namespace cisstRobot{

  class robSO3Blender : public robFunction {
  private:
    // RCCL blender
    SO3 Rw0;           // rotation from final frame to initial frame
    SO3 R10;           // rotation from final frame to initial frame
    Real w0;           // initial angular velocity
    R3 uv;             // axis for RV
    vctAxisAngleRotation3<Real> uctc;

    Real xmin, xmax;
    
    SO3 Rb1( Real h );
    SO3 R0b( Real h );
  
    // Taylor's blender
    Real T1, T2, ti;
    Real tauA, tauB;
    SO3 Rw1;
    vctAxisAngleRotation3<Real> n1t1;
    vctAxisAngleRotation3<Real> n2t2;
    
  public:
    
    //! Build a blender for R1->SO3 mappings based on Taylor's paper
    /**
       \param T1 The length (time) of the first segment
       \param T2 The length (time) of the second segment
       \param R0 The initial orientation
       \param R1 The midpoint orientation
       \param R2 The final orientation
     */
    robSO3Blender( Real ti, 
		   Real T1, Real T2, 
		   const SO3& R0, const SO3& R1, const SO3& R2,
		   Real tA=robFunctionPiecewise::TAU,
		   Real tB=robFunctionPiecewise::TAU );

    //! Build a blender for R1->SO3 mappings based on Lloyd's paper
    /**
       \param x1 The time at the start of the blending
       \param R0 The 1st orientation at t=x1
       \param R1 The 2nd orientation at t=x1
       \param w0w The 1st velocity at t=x1
     */
    robSO3Blender( Real x1, const SO3& R0, const SO3& R1, const R3& w0w );
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& x ) const; 
    
    //! evaluate the mapping
    robError Evaluate( const robDOF& x, robDOF& y );  
    
  };
  
}

#endif
