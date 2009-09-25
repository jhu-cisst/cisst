#ifndef _robDH_h
#define _robDH_h

#include <cisstRobot/robDefines.h>

#include <iostream>
using namespace std;

namespace cisstRobot{

  //! DH parameters of a link
  class robDH{
  private:

    bool modifiedDH;  // modified DH?
    Real sigma;       // prismatic joint=1.0, revolute=0.0

    Real alpha, a;    // x components
    Real theta, d;    // z components

    Real   offset;    // offset angle (revolute) or length (prismatic)

    SE3        Rt;    // corresponding SE3 transformation to the DH parameters

  public:

    //! Default constructor
    robDH() { alpha = a = theta = d = offset = 0.0; }
    //! Default destructor
    ~robDH(){}

    //! Overloaded constructor
    /**
       Set the kinematics parameters to Rt. This is mostly used to attach a tool
       to the end-effector.
       \param Rt The SE3 transformation of the link
     */
    robDH(const SE3& Rt){ this->Rt = Rt; }

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
    robDH( Real alpha, Real a, 
	   Real theta, Real d, 
	   Real offset,
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
			  << ": Expected PRISMATIC or REVOLUTE" << std::endl;
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
			  << ": Expected MODIFIED or standard" << std::endl;
      }
    }

    //!
    bool IsRevolute() const { return sigma == 0.0 ; }
    bool IsPrismatic() const { return 0.0 < sigma; }

    //! Modified DH convention
    bool IsModifiedDH() const { return modifiedDH; }

    //! Sigma (prismatic/revolute)
    Real Sigma() const { return sigma; }

    //! Return the position and orientation
    /**
       \return The position and orientation associated with the DH parameters
    */
    operator SE3() const { return Rt; }

    //! Return the orientation
    /**
       \return The orientation associated with the DH parameters
    */
    operator SO3() const { return SO3( Rt[0][0], Rt[0][1], Rt[0][2], 
				       Rt[1][0], Rt[1][1], Rt[1][2], 
				       Rt[2][0], Rt[2][1], Rt[2][2] ); }
    //! Return the position of the next link
    /**
       \return The position of the next coordinate frame wrt to the current
       frame
    */
    R3 PStar() const 
    { return R3( a, d*sin(alpha), d*cos(alpha) ); }

    //! Set the joint angle
    /**
        Update the joint angle (\f$\theta\f$) and the SE3 transformation
        \param t The joint angle
    */
    SE3 SetDOF( Real q ){

      if( IsModifiedDH() ){
	if( 0 < IsPrismatic() )
	  d = offset + q;       // add the joint offset to the joint length
	else
	  theta = offset + q;   // add the joint offset to the joint angle

	Real ca = cos(alpha);	  Real sa = sin(alpha);	
	Real ct = cos(theta);	  Real st = sin(theta);
	
	SO3 R( ct,    -st,     0,
	       st*ca,  ct*ca, -sa,
	       st*sa,  ct*sa,  ca );
	Rt = SE3( R, R3( a, -sa*d, ca*d ) );
      }

      else{
	if( 0 < IsPrismatic() )
	  d = offset + q;       // add the joint offset to the joint length
	else
	  theta = offset + q;   // add the joint offset to the joint angle

	  Real ca = cos(alpha);	  Real sa = sin(alpha);	
	  Real ct = cos(theta);	  Real st = sin(theta);

	  SO3 R( ct, -st*ca,  st*sa,
		 st,  ct*ca, -ct*sa,
		  0,     sa,     ca );
	  Rt = SE3( R, R3(a*ct, a*st, d) );
      }
      return Rt;
    }

    //! Read the parameters from an input stream
    /**
       Read the parameters from an input stream. The parameters are in the
       following order: alpha, a, \f$\theta\f$-offset, d
       \param is The input stream
       \param dh The parameters
    */
    friend std::istream& operator >> (std::istream& is, robDH& dh){
      Real alpha, a, theta, d, offset;
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
};

#endif
