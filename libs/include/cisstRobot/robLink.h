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

#ifndef _robLink_h
#define _robLink_h

#include <iostream>

#include <cisstRobot/robDH.h>
#include <cisstRobot/robBody.h>

class robLink : public robBody, public robDH {

public:

  //! Default constructor
  robLink(){}
  
  //! Default destructor
  ~robLink(){}
  
  robLink& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {
    if( *this != Rt )
      robBody::operator=(Rt);
    return *this;
  }

  //! Read the DH and body parameters
  void Read( std::istream& is ){ 
    robDH::Read( is ); 
    robBody::Read( is ); 
  }
  
  //! Write the DH and body parameters
  void Write( std::ostream& os ) const { 
    robDH::Write( os );
    robBody::Write( os ); 
  }
  
};

#endif
