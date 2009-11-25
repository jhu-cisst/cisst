/*

  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robBodyGeneric_h
#define _robBodyGeneric_h

#include <iostream>

#include <cisstRobot/robMass.h>
#include <cisstRobot/robMesh.h>

class robBodyGeneric : public robMesh, public robMass {

public:
  
  //! Default constructor
  robBodyGeneric(){}
  ~robBodyGeneric(){}

  //! Read the body from an input stream
  virtual robError Read( std::istream& is ){
    if( robMass::Read( is ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to read the mass." 
			<< std::endl;
      return ERROR;
    }
    if( robMesh::Read( is ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__
			<< ": Failed to read the mesh." 
			<< std::endl;
      return ERROR;
    }
    return SUCCESS;
  }

  //! Write the body to an output stream
  virtual void Write( std::ostream& os ) const
  { robMass::Write( os ); }

  //! Set the position and orientation
  /**
     Overload the = operator to be able to set the position and orienation
     of the body with an assignment.
     \param Rt The new position and orientation 
  */
  robBodyGeneric& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {
    //cout << __PRETTY_FUNCTION__ << endl;
    if( *this != Rt )
      robMesh::operator=(Rt);    
    return *this;
  }

  //! Read from an input stream
  /**
     Read the parameters of the link from an input stream. The parameters are
     read in this order: mass, center of mass, moment of inertia tensor
     \param is The input stream
     \param body The body
  */
  friend std::istream& operator>>( std::istream& is, robBodyGeneric& body ){
    body.Read( is );
    return is;
  }
  
  //! Write from an output stream
  /**
     Write the parameters of the link from an input stream. The parameters are
     read in this order: DH, mass, cog, inertia
     \param os The output stream
     \param body The body
  */
  friend std::ostream& operator<<(std::ostream& os, const robBodyGeneric& body){
    body.Write( os );
    return os;
  }

};

#endif
