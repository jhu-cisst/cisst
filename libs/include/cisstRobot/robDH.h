#ifndef _robDH_h
#define _robDH_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robDefines.h>
#include <iostream>

//! DH parameters of a link
class robDH{
private:
  
  bool modifiedDH;    // modified DH?
  double sigma;       // prismatic joint=1.0, revolute=0.0
  
  double alpha, a;    // x components
  double theta, d;    // z components
  
  double   offset;    // offset angle (revolute) or length (prismatic)
  
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt; // SE3 transformation to the DH parameters
  
public:
  
  //! Default constructor
  robDH() { alpha = a = theta = d = offset = 0.0; }
  //! Default destructor
  ~robDH(){}
  
  //! Overloaded constructor
  /**
     Set the kinematics parameters to Rt. This is mostly used to attach a tool
     to the end-effector.
     \param Rt The vctFrame4x4<double,VCT_ROW_MAJOR> transformation of the link
  */
  robDH(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt){ this->Rt = Rt; }
  
  //! Overloaded constructor
  /**
     \param alpha The \f$\alpha\f$ angle (rotation around X)
     \param a The \f$a\f$ length (translation along X)
     \theta t The \f$\theta\f$ angle (rotation around Z)
     \param d The \f$d\f$ length (translation along Z)
     \param offset The offset of the joint angle (rotation around Z)
     \param type PRISMATIC for prismatic joint or REVOLUTE for revolute 
     joint
     \param convention MODIFIED for modified DH or STANDARD for standard DH
  */
  robDH( double alpha, double a, 
	 double theta, double d, 
	 double offset,
	 const std::string& type,
	 const std::string& convention ){
    
    this->alpha = alpha;
    this->a = a;
    this->theta = theta;
    this->d = d;
    this->offset = offset;
      
    if( type.compare("PRISMATIC") == 0 || 
	type.compare("prismatic") == 0 )
      this->sigma = 1.0;
    else if( type.compare("REVOLUTE") == 0 || 
	     type.compare("revolute") == 0){
      this->sigma = 0.0;
    }
    else{
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Expected PRISMATIC or REVOLUTE" 
			<< std::endl;
    }
    
    if( convention.compare("MODIFIED") == 0 || 
	convention.compare("modified") == 0 )
      this->modifiedDH = true;
    else if( convention.compare("STANDARD") == 0 || 
	     convention.compare("standard") == 0 ){
      this->modifiedDH = false;
    }
    else{
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Expected MODIFIED or standard" 
			<< std::endl;
    }
  }
  
  //!
  bool IsRevolute()  const { return sigma== 0.0; }
  bool IsPrismatic() const { return 0.0 < sigma; }

  //! Modified DH convention
  bool IsModifiedDH() const { return modifiedDH; }
  
  //! Sigma (prismatic/revolute)
  double Sigma() const { return sigma; }
  
  //! Return the position and orientation
  /**
     \return The position and orientation associated with the DH parameters
  */
  vctFrame4x4<double,VCT_ROW_MAJOR> GetSE3( double q ) const { 
    double d = this->d;
    double theta = this->theta;

    if( IsModifiedDH() ){

      if( 0 < IsPrismatic() )
	d = offset + q;       // add the joint offset to the joint length
      else
	theta = offset + q;   // add the joint offset to the joint angle
      
      double ca = cos(alpha);	  double sa = sin(alpha);	
      double ct = cos(theta);	  double st = sin(theta);
      
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct,    -st,     0,
						  st*ca,  ct*ca, -sa,
						  st*sa,  ct*sa,  ca );
      vctFixedSizeVector<double,3> t( a, -sa*d, ca*d );

      return vctFrame4x4<double,VCT_ROW_MAJOR>( R, t );
    }
    
    else{
      if( 0 < IsPrismatic() )
	d = offset + q;       // add the joint offset to the joint length
      else
	theta = offset + q;   // add the joint offset to the joint angle
      
      double ca = cos(alpha);	  double sa = sin(alpha);	
      double ct = cos(theta);	  double st = sin(theta);
      
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct, -st*ca,  st*sa,
						  st,  ct*ca, -ct*sa,
						  0,     sa,     ca );

      vctFixedSizeVector<double,3> t(a*ct, a*st, d);
      return vctFrame4x4<double,VCT_ROW_MAJOR>( R, t );
    }
  }
  
  //! Return the orientation
  /**
     \return The orientation associated with the DH parameters
  */
  vctMatrixRotation3<double,VCT_ROW_MAJOR> GetSO3( double q ) const {
    vctFrame4x4<double,VCT_ROW_MAJOR> Rt = GetSE3( q );
    return vctMatrixRotation3<double,VCT_ROW_MAJOR>(Rt[0][0],Rt[0][1],Rt[0][2], 
						    Rt[1][0],Rt[1][1],Rt[1][2], 
						    Rt[2][0],Rt[2][1],Rt[2][2]);
  }

  //! Return the position of the next link
  /**
     \return The position of the next coordinate frame wrt to the current
     frame
  */
  vctFixedSizeVector<double,3> PStar() const 
  { return vctFixedSizeVector<double,3>( a, d*sin(alpha), d*cos(alpha) ); }
  
  //! Set the joint angle
  /**
     Update the joint angle (\f$\theta\f$) and the SE3 transformation
     \param t The joint angle
  */
  /*
  vctFrame4x4<double,VCT_ROW_MAJOR> SetDOF( double q ){
    
    if( IsModifiedDH() ){
      if( 0 < IsPrismatic() )
	d = offset + q;       // add the joint offset to the joint length
      else
	theta = offset + q;   // add the joint offset to the joint angle
      
      double ca = cos(alpha);	  double sa = sin(alpha);	
      double ct = cos(theta);	  double st = sin(theta);
      
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct,    -st,     0,
						  st*ca,  ct*ca, -sa,
						  st*sa,  ct*sa,  ca );
      Rt = 
	vctFrame4x4<double,VCT_ROW_MAJOR>( R, 
					   vctFixedSizeVector<double,3>( a, 
									 -sa*d, 
									 ca*d ) );
    }
    
    else{
      if( 0 < IsPrismatic() )
	d = offset + q;       // add the joint offset to the joint length
      else
	theta = offset + q;   // add the joint offset to the joint angle
      
      double ca = cos(alpha);	  double sa = sin(alpha);	
      double ct = cos(theta);	  double st = sin(theta);
      
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R( ct, -st*ca,  st*sa,
						  st,  ct*ca, -ct*sa,
						  0,     sa,     ca );
      Rt = vctFrame4x4<double,VCT_ROW_MAJOR>( R, vctFixedSizeVector<double,3>(a*ct, a*st, d) );
    }
    return Rt;
  }
  */  
  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: alpha, a, \f$\theta\f$-offset, d
     \param is The input stream
     \param dh The parameters
  */
  friend std::istream& operator >> (std::istream& is, robDH& dh){
    double alpha, a, theta, d, offset;
    std::string prismatic, modified;
    is >> alpha >> a >> theta >> d >> offset >> prismatic >> modified;
    if( alpha == -1.5708 )	alpha = -M_PI_2;
    if( alpha == 1.5708 )	alpha =  M_PI_2;
    
    dh = robDH(alpha, a, theta, d, offset, prismatic, modified);
    return is;
  }
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: alpha, a, theta, d
     \param os The output stream
     \param dh The parameters
  */
  friend std::ostream& operator << (std::ostream& os, const robDH& dh){
    os << std::setw(10) << dh.alpha 
       << std::setw(10) << dh.a 
       << std::setw(10) << dh.theta
       << std::setw(10) << dh.d
       << std::setw(10) << dh.offset
       << std::setw(10) << dh.sigma
       << std::setw(10) << dh.modifiedDH;
    return os;
  }
};
#endif
