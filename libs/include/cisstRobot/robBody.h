#ifndef _robBody_h
#define _robBody_h

#include <cisstRobot/robMass.h>
#include <cisstRobot/robMesh.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFrame4x4.h>

class robBody : public robMass, public robMesh {

public:
  
  //! Default constructor
  robBody(){}

  //! Read the body from an input stream
  virtual void Read( std::istream& is ){
    robMass::Read( is );
    robMesh::Read( is );
  }

  //! Write the body to an output stream
  virtual void Write( std::ostream& os ) const{
    robMass::Write( os );
  }

  robBody& operator= ( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {
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
  friend std::istream& operator >> (std::istream& is, robBody& body){
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
  friend std::ostream& operator << ( std::ostream& os, const robBody& body ){
    body.Write( os );
    return os;
  }
  
};

#endif
