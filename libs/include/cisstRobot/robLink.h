
#ifndef _robLink_h
#define _robLink_h

#include <cisstRobot/robDH.h>
#include <cisstRobot/robBody.h>

#include <iostream>

class robLink : public robBody {

private:
  
  robDH  dh;  // the DH parameters of the link

public:
  
  robLink(){}
  
  //! Default constructor
  robLink( double mass, 
	   const vctFixedSizeVector<double,3>& com, 
	   const vctFixedSizeMatrix<double,3,3>& moit, 
	   const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt,
	   const std::string& objfn ) : robBody( mass, com, moit, Rt, objfn ){}
  
  //! Overloaded constructor
  /**
     Create a new link with its kinematics given by the SE3. This is 
     mostly used to attach a tool on the end-effector. The dynamics parameters
     are not set. You should not update the position of a link defined this
     way.
     \param Rt The kinematics of the link
  */
  robLink( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {  dh = robDH( Rt );  }
  
  //! Overloaded constructor
  /**
     This constructor is used to create/initialize links
     \param dh The dh parameter of the link
     \param mass The mass of the link
     \param cntgrav The center of gravity of the link
     \param inertia The inertia tensor. This represents the distribution of the
     mass around a coordinate frame of the link
  */
  robLink(const robDH& dh, 
	  double mass, 
	  const vctFixedSizeVector<double,3>& com, 
	  const vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>& moit){
    this->dh = dh;
    this->mass = mass;
    this->com = com;
    this->moit = moit;
  }
  
  //! Default destructor
  ~robLink(){}
  
  //! Get the postion and orientation of the link
  vctFrame4x4<double,VCT_ROW_MAJOR> ForwardKinematics( double q ) const 
  { return dh.GetSE3( q ); }
  
  //! Get the orientation of the link
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rotation( double q ) const 
  { return dh.GetSO3( q ); }
  
  //!
  bool IsDH() const { return !dh.IsModifiedDH(); }
  bool IsModifiedDH() const { return dh.IsModifiedDH(); }
  bool IsRevolute() const { return dh.IsRevolute(); }
  bool IsPrismatic() const { return dh.IsPrismatic(); }
  
  //! The center of mass
  /**
     \return The center of mass (com) of the link (in the links coordinate
     frame)
  */
  vctFixedSizeVector<double,3> COM() const { return com; }
  
  //! The mass
  /**
     \return The mass of the link (kg)
  */
  double Mass() const { return mass; }
  
  //! The Moment of Inertia tensor
  /**
     Return the inertia tensor (kgm^2)
  */
  vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> MOI() const { return moit; }
  
  //! Position of the next coordinate frame
  /**
     \return The position of the next link wrt to the current one
  */
  vctFixedSizeVector<double,3>  PStar() const { return dh.PStar(); }
  
  //! Read from an input stream
  /**
     Read the parameters of the link from an input stream. The parameters are
     read in this order: DH, mass, cog, inertia
     \param is The input stream
     \param l The link
  */
  friend std::istream& operator >> (std::istream& is, robLink& l){
    double m;
    double x, y, z;
    double Ixx, Iyy, Izz, Ixy, Iyz, Ixz;
    std::string filename;
    
    is >> l.dh 
       >> m 
       >> x >> y >> z 
       >> Ixx >> Iyy >> Izz >> Ixy >> Iyz >> Ixz
       >> filename;
    
    vctFixedSizeVector<double,3> com(x, y, z);
    vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> moitcom( Ixx, Ixy, Ixz,
							     Ixy, Iyy, Iyz,
							     Ixz, Iyz, Izz );
    
    // The inertia is given for the center of mass aligned with coord frame.
    // it needs to be translated to coord frame of the link

    // MOIT wrt link coordinate frame
    vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> moitt;
    moitt[0][0]=m*(y*y+z*z); moitt[0][1]=m*x*y;       moitt[0][2]=m*x*z;
    moitt[1][0]=m*x*y;       moitt[1][1]=m*(x*x+z*z); moitt[1][2]=m*y*z;
    moitt[2][0]=m*x*z;       moitt[2][1]=m*y*z;       moitt[2][2]=m*(x*x+y*y);
    
    l.mass = m;
    l.com = com;
    l.moit = moitcom;// + moitt;
    
    l.LoadOBJ( filename );
    
    return is;
  }
  
  //! Write from an output stream
  /**
     Write the parameters of the link from an input stream. The parameters are
     read in this order: DH, mass, cog, inertia
     \param os The output stream
     \param l The link
  */
  friend std::ostream& operator << (std::ostream& os, const robLink& l){
    
    os << "DH: "   << l.dh      << std::endl 
       << "Mass: " << l.mass    << std::endl 
       << "COM: "  << l.com     << std::endl 
       << "vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>: " << l.moit    << std::endl;
    
    return os;
  }
  
};

#endif
