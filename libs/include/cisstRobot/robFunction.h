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

#include <cisstRobot/robMapping.h>
#include <cisstRobot/robVariable.h>

class robFunction : public robMapping {
  
public:

  enum Errno { ESUCCESS, EFAILURE, EUNDEFINED };

  //! The attribute of a function
  /** 
      The attribute of a function indicates wheter the function is define for 
      a given input. This is used to determine which function, in a piecewise 
      function, is defined for a given input.
  */
  enum Context{
    CUNDEFINED = 0,
    CDEFINED, 
    //INCOMING,
    //OUTGOING,
    //EXPIRED
  };
  
  robFunction( robSpace input, robSpace output ) :  
    robMapping( input, output ){}

  //virtual double Duration() const { return 0.0; }
  
  //! Is the function defined for the input
  /**
     \param[in] in An element drawn from a space
     \return The context of the function 
  */
  virtual robFunction::Context GetContext( const robVariable& input ) const=0;
  
  //! Evaluate the function 
  /**
     Evaluate the function and its 1st and 2nd derivative
     \param[in] input The element of the domain
     \param[out] output The the output of the function
  */
  virtual robFunction::Errno Evaluate( const robVariable& input, 
				       robVariable& output ) = 0;  
  
};

#endif
