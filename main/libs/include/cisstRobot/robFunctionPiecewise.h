#ifndef _robFunctionPiecewise_h
#define _robFunctionPiecewise_h

#include <cisstRobot/robFunction.h>
#include <vector>

namespace cisstRobot{

  class robDomainLimit {};

  class robFunctionPiecewise : public robFunction {

  protected:

    std::vector< robFunction* > functions;
    robFunction* blender;

    robError PackRn( const robDOF& input1, const robDOF& input2,robDOF& output );
    robError PackSO3(const robDOF& input1, const robDOF& input2,robDOF& output);

    robError BlendRn( robFunction* initial, 
		      robFunction* final,
		      const robDOF& input, 
		      robDOF& output );
    robError BlendSO3( robFunction* initial, 
		       robFunction* final,
		       const robDOF& input, 
		       robDOF& output );


  public:
    
    static const double TAU;
    
    robFunctionPiecewise(){blender=NULL;}

    //! Insert a function
    /**
       This inserts a new function in the piecewise function. The function is
       inserted at the begining of a queue and thus its domain has higher priority
       than the functions after.
       \param function The new function to be added
     */
    robError Insert( robFunction* function );
    
    //! Is function defined for the given input
    robDomainAttribute IsDefinedFor( const robDOF& input ) const ;
    
    //! Evaluate the function for the given input
    /**
       This querries all the functions starting by the most recently inserted.
       If a function is defined for the input, it is evaluated. The function also
       perform blending if it determines that an other function is ramping up
       or ramping down. Finally, this method performs cleaning of the functions.
       Functions that are expired or that have a lower priority are removed.
       \param [in] input The input to the function
       \param [out] output The output of the function
     */
    robError Evaluate( const robDOF& input, robDOF& output );  
    
  };
}

#endif
