

#ifndef _robMapping_h
#define _robMapping_h

#include <cisstRobot/robSpace.h>
#include <cisstRobot/robExport.h>

//! robMapping defines the spaces used by a function.
/** 
    This class defines the spaces that are used by a function. For example, for
    a function \f$ F: R^1 \rightarrow R^1 \f$, the mapping will represent the
    domain \f$R^1\f$ and the codomain \f$R^1\f$.
*/
class CISST_EXPORT robMapping{

 private:
  
  robSpace domain;
  robSpace codomain;  
  
 public:
  
  //! Default constructor
  robMapping();

  //! Overloaded constructor
  /**
     Set the domain and the codomain of the mapping
     \param domain The input space of the mapping
     \param codomain The output space of the mapping
  */
  robMapping( const robSpace& domain, const robSpace& codomain );
  
  //! Return the domain space
  robSpace Domain()   const;

  //! Return the image space
  robSpace Codomain() const;
  
  //! Equal operator
  /**
     This operator looks at the space codes between each mapping and return true 
     if the codes in the first mapping are equal to the space codes in the second
     mapping. This operation has no use other than sorting mappings in a list
     or map.
     \param mapping1 The first mapping
     \param mapping2 The second mapping
     \return true if mapping1 equals mapping2
  */
  friend bool operator==(const robMapping& mapping1, const robMapping& mapping2){
    if( ( mapping1.Domain()   == mapping2.Domain() ) &&
	( mapping1.Codomain() == mapping2.Codomain() ) )
      { return true; }
    else
      { return false; }
  }

  friend bool operator!=(const robMapping& mapping1, const robMapping& mapping2){
    if( ( mapping1.Domain()   != mapping2.Domain() ) ||
	( mapping1.Codomain() != mapping2.Codomain() ) )
      { return true; }
    else
      { return false; }
  }
  
};

#endif
