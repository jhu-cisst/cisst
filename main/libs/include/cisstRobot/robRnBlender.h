#ifndef _robRnBlender_h
#define _robRnBlender_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robHermite.h>

namespace cisstRobot{

  //! A blending function for Real vectors
  /**
     This function "blends" two functions F:R1->Rn by using a 5th order Hermite
     polynomial. The blending is C2 continuous
   */
  class robRnBlender : public robFunction {
  private:
    
    std::vector<robHermite*> blenders;
    
    Real xmin, xmax;

  public:

    //! Create a Hermite blender
    /**
       \param xi The lower value of the domain. This is the start point of the
                 blender
       \param yi A vector of Real values \f$yi=f(xi)\f$
       \param yid A vector of 1st derivatives \f$yid=\dot{f}(xi)\f$
       \param yidd A vector of 2nd derivatives \f$yidd=\ddot{f}(xi)\f$
       \param yf A vector of Real values \f$yi=f(xi)\f$
       \param yfd A vector of 1st derivatives \f$yid=\dot{f}(xi)\f$
       \param yfdd A vector of 2nd derivatives \f$yidd=\ddot{f}(xi)\f$
     */
    robRnBlender( Real xi, const Rn& yi, const Rn& yid, const Rn& yidd,
		  Real xf, const Rn& yf, const Rn& yfd, const Rn& yfdd );
    
    //! Return true if the function is defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const; 
    
    //! Evaluate the function
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };

}

#endif
