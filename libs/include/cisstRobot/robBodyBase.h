#ifndef _robBodyBase_h
#define _robBodyBase_h

#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>

#include <cisstRobot/robDefines.h>

#include <vector>

class robBodyBase{
  
private:
  
  void glMultMatrix( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) const;
  
protected:
  
  //! position and orientation of the body
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt; 
  
  //! The mass
  double mass; 
  //! The center of mass
  vctFixedSizeVector<double,3> com;
  //! The moment of inertia tensor
  vctFixedSizeMatrix<double,3,3> moit;

  //! the coordinates of vertices
  std::vector< vctFixedSizeVector<double,3> > vertices;
  
  //! the index of the vertices
  std::vector< vctFixedSizeVector<int,3> > triangles;

  //! the triangles normals
  std::vector< vctFixedSizeVector<double,3> > normals;

public:
  
  //! Default constructor
  robBodyBase();

  //! Constructor with dynamics and geometry
  robBodyBase( double mass,
	       const vctFixedSizeVector<double,3>& com,
	       const vctFixedSizeMatrix<double,3,3>& moit,
	       const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
	       const std::string& fn);  

  //! Default destructor
  ~robBodyBase();
  
  //! Return the position and orientation
  vctFrame4x4<double,VCT_ROW_MAJOR>  PositionOrientation( ) const { return Rt; } 
  //! Set the position and orientation
  void PositionOrientation( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt )
  { this->Rt = Rt; } 
  
  void Draw() const;    
  
  robError LoadOBJ( const std::string& filename );

  //! Return the mass
  double Mass() const { return mass; }
  //! Return the center of mass
  vctFixedSizeVector<double,3> CenterOfMass() const { return com; }
  //! Return the moment of inertia tensor
  vctFixedSizeMatrix<double,3,3> MomentOfInertia() const { return moit; }
  
};

#endif
