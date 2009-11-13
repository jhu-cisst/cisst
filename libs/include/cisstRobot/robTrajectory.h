/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robTrajectory_h
#define _robTrajectory_h

#include <cisstRobot/robFunction.h>
#include <vector>

#include <cisstRobot/robExport.h>

class robDomainLimit {};

class CISST_EXPORT robTrajectory : public robFunction {

protected:
  
  std::vector< robFunction* > functions;
  robFunction* blender;
  
  robError PackRn( const robVariables& input1, 
		   const robVariables& input2,
		   robVariables& output );

  robError PackSO3( const robVariables& input1, 
		    const robVariables& input2,
		    robVariables& output ) ;
  
  robError BlendRn( robFunction* initial, 
		    robFunction* final,
		    const robVariables& input, 
		    robVariables& output );

  robError BlendSO3( robFunction* initial, 
		     robFunction* final,
		     const robVariables& input, 
		     robVariables& output );
  
  
public:
  
  static const double TAU;
  
  robTrajectory(){blender=NULL;}
  
  //! Insert a function
  /**
     This inserts a new function in the piecewise function. The function is
     inserted at the begining of a queue and thus its domain has higher priority
     than the functions after.
     \param function The new function to be added
  */
  robError Insert( robFunction* function );
  
  //! Is function defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const ;
    
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
  robError Evaluate( const robVariables& input, robVariables& output );  
  
};

#endif
