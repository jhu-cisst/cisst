#ifndef _robLink_h
#define _robLink_h

#include <cisstRobot/robDH.h>
#include <cisstRobot/robBody.h>

#include <iostream>

class robLink : public robBody, public robDH {

public:

  //! Default constructor
  robLink(){}
  
  //! Default destructor
  ~robLink(){}

  robLink& operator= ( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {
    if( *this != Rt )
      robMesh::operator=(Rt);
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
