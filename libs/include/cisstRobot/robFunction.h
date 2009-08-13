#ifndef _robFunction_h
#define _robFunction_h

#include <cisstRobot/robDOF.h>

namespace cisstRobot{

  enum robDomainAttribute{
    UNDEFINED = 0,
    DEFINED, 
    INCOMING,
    OUTGOING,
    EXPIRED
  };
  
  class robFunction {
    
  protected:
    
    // Mutex
    
  public:

    virtual real Duration() const { return 0.0; }
    
    //! Is the function defined for the input
    /**
       \param[in] in An element drawn from a space
       \return true if the function is define for in. false otherwise
    */
    virtual robDomainAttribute IsDefinedFor( const robDOF& input ) const = 0;
    
    //! Evaluate the function 
    /**
       Evaluate the function and its 1st and 2nd derivative
       \param[in] in The element of the domain
       \param[out] out The the output of the function
       \param[out] outd The output's first derivative
       \param[out] outdd The output's second derivative
    */
    virtual robError Evaluate( const robDOF& input, robDOF& output ) = 0;  
    
  };

}

#endif
