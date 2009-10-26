#ifndef _robBlenderRn_h
#define _robBlenderRn_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robQuintic.h>


//! A blending function for double vectors
/**
   This function "blends" two functions F:R1->Rn by using a 5th order Hermite
   polynomial. The blending is C2 continuous
*/
class robBlenderRn : public robFunction {
private:
  
  std::vector<robQuintic*> blenders;
  
  double tmin, tmax;
  
public:
  
  //! Create a Hermite blender
  /**
     \param ti The initial time of the blender
     \param yi A vector of double values \f$yi=f(xi)\f$
     \param yid A vector of 1st derivatives \f$yid=\dot{f}(xi)\f$
     \param yidd A vector of 2nd derivatives \f$yidd=\ddot{f}(xi)\f$
     \param tf The final time of the blender
     \param yf A vector of double values \f$yi=f(xi)\f$
     \param yfd A vector of 1st derivatives \f$yid=\dot{f}(xi)\f$
     \param yfdd A vector of 2nd derivatives \f$yidd=\ddot{f}(xi)\f$
  */

  robBlenderRn( double ti, 
		const vctDynamicVector<double>& yi, 
		const vctDynamicVector<double>& yid, 
		const vctDynamicVector<double>& yidd,
		double tf, 
		const vctDynamicVector<double>& yf, 
		const vctDynamicVector<double>& yfd, 
		const vctDynamicVector<double>& yfdd );
  
    //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const; 
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );  
  
};

#endif
