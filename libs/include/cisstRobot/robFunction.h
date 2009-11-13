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

#ifndef _robFunction_h
#define _robFunction_h

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robVariables.h>

enum robDomainAttribute{
  UNDEFINED = 0,
  DEFINED, 
  INCOMING,
  OUTGOING,
  EXPIRED
};
  
class robFunction {
  
public:
  
  virtual double Duration() const { return 0.0; }
  
  //! Is the function defined for the input
  /**
     \param[in] in An element drawn from a space
     \return true if the function is define for in. false otherwise
  */
  virtual robDomainAttribute IsDefinedFor( const robVariables& input ) const=0;
  
  //! Evaluate the function 
  /**
     Evaluate the function and its 1st and 2nd derivative
     \param[in] in The element of the domain
     \param[out] out The the output of the function
     \param[out] outd The output's first derivative
     \param[out] outdd The output's second derivative
  */
  virtual robError Evaluate( const robVariables& input, 
			     robVariables& output ) = 0;  
  
};

#endif
