#ifndef _robVariables_h
#define _robVariables_h

#include <cisstVector/vctFrame4x4.h>

#include <iostream>
#include <bitset>

//! Variables of a robot.
/**
   robVariables defines the variables used to generate trajectories and control a
   robot. The variables include joint position, velocities and accelerations
   as well as Cartesian position, velocities and accelerations. Forces and
   torques are also available.
*/

typedef unsigned long long int robVariablesMask;

class robVariables{
private:

  // A mask that holds 64 bits variables
  robVariablesMask mask;

  size_t MaskToIndex( robVariablesMask mask );

  void Clear();

public:

  //! Time variable
  /**
     Use this variable for time varying trajectories
  */
  double time;
  
  //! Cartesian orientation and position variables
  /**
     A SE3 variable. Use this variable for Cartesian position trajectories
  */
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt;
  
  //! Cartesian angular and linear velocities variables
  /**
     A R6 variable. Use this variable for Cartesian velocity trajectories
  */
  vctFixedSizeVector<double,6>  vw;
  
  //! Cartesian angular and linear accelerations variables
  /**
     A R6 variable. Use this variable for Cartesian acceleration trajectories
  */
  vctFixedSizeVector<double,6>  vdwd;
  
  //! Joint positions variables
  /**
     An Rn variable. Use this variable for joints position trajectories
  */
  vctDynamicVector<double>  q;
  
  //! Joint velocities variables
  /**
     An Rn variable. Use this variable for joints velocity trajectories
  */
  vctDynamicVector<double>  qd;
  
  //! Joint accelerations variables
  /**
     An Rn variable. Use this variable for joints acceleration trajectories
  */
  vctDynamicVector<double>  qdd;
  
  //! Create a variables mask with all variables "turned off"
  robVariables();

  //! Create a variables mask with the given variables "turned on"
  /**
     \param var The mask of variables to use
  */
  robVariables( robVariablesMask var);

  //! Return true if a variable is set
  /**
     \param var A mask of variables
     \return true if any of variables in var is set
  */
  bool IsVariableSet( robVariablesMask var ) const;
    
  //! Is one of the variables Cartesian?
  /**
     \return true if any of the Cartesian variables is set. That is any 
     Cartesian position, orientation, velocity or acceleration.
  */
  bool IsCartesianSet()  const;

  //! Is one of the variables a Cartesian translation
  /**
     \return true if any of the Cartesian translation variables is set.
  */
  bool IsTranslationSet()const;

  //! Is one of the variables a Cartesian orientation
  /**
     \return true if any of the Cartesian orientation variables is set.
  */
  bool IsOrientationSet()   const;
  
  //! Is the time variable set
  /**
     \return true if the time variable is set
  */
  bool IsTimeSet() const;
  
  //! Is the joints variables set
  /**
     \return true if any joint variable is set
  */
  bool IsJointSet() const;
  
  //! Set variables mask
  /**
     Replace the variables mask by the one given
     \param mask The new variables mask
   */
  void SetVariables( const robVariablesMask& mask );
  
  //! Return the variable mask
  /**
     \return The mask of variables
  */
  robVariablesMask GetVariables() const ;

  //! Used by the map
  friend bool operator <  ( const robVariables& var1,
			    const robVariables& var2 )
  {  return var1.mask < var2.mask;   }

  //! Used by the map
  friend bool operator == ( const robVariables& var1, 
			    const robVariables& var2 )
  {  return var1.mask == var2.mask;  }
    

  //! A variables object with a time variable
  /**
     Create a trajectory variables object by using the time variable. This
     sets TIME bit in the mask of variables.     
     \param t The value of the time variable
  */
  robVariables( double t );

  //! A variables object with a joints positions, velocities and accelerations
  /**
     Create a trajectory variables object by using the joint positions, 
     velocities and accelerations. This sets the variables, Q1,...,Q9, 
     Q1D,...,Q9D and Q1DD,...,Q9DD accordingly.
     \param q A vector of joint positions. If q has N values, then the
              bits Q1,...,QN will be set in the mask.
     \param qd A vector of joint velocities. If qd has N values, then the
              bits Q1D,...,QNDD will be set in the mask.
     \param qdd A vector of joint accelerations. If qdd has N values, then the
              bits Q1DD,...,QNDD will be set in the mask.
  */
  robVariables( const vctDynamicVector<double>& q, 
		const vctDynamicVector<double>& qd = vctDynamicVector<double>(), 
		const vctDynamicVector<double>& qdd= vctDynamicVector<double>());

  //! A variables object with Cartesian position and orientation
  /** 
     Creates a trajectory variables object by using a Cartesian position and
     orientation. This sets the variables TX, TY, TZ, RX, RY and RZ in the 
     mask
     \param Rt The position and orientation variables
  */
  robVariables( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt );

  //! A variables object with all Cartesian variables
  /**
     Creates a trajectory variables object by using a Cartesian position and
     orientation, linear and angular velocities and linear and angular 
     accelerations. This sets the variables TX, TY, TZ, RX, RY, RZ, VX, VY, VZ,
     WX, WY, WZ, VXD, VYD, VZD, WXD, WYD, WZD in the mask
     \param Rt The position and orientation variables
     \param vw The linear and angular velocities
     \param vdwd The linear and angular accelerations
  */
  robVariables( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
		const vctFixedSizeVector<double,6>& vw, 
		const vctFixedSizeVector<double,6>& vdwd );

  //! Set the time variable
  /**
     Set the time variable. This automatically sets the time bit in the mask
     \param t The time value
  */
  void Set( robVariablesMask mask, double t );

  //! Set Cartesian variables
  /**
     Set Cartesian variables that are specified by the mask to those of the
     parameters. Only the variables that are specified by the mask will be 
     copied.
     \param mask The mask of variables to be used
     \param Rt   The position and orientation
     \param vw The linear and angular velocities
     \param vdwd The linear and angular accelerations
  */
  void Set( robVariablesMask mask, 
	    const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
	    const vctFixedSizeVector<double,6>& vw, 
	    const vctFixedSizeVector<double,6>& vdwd );

  //! Set Cartesian variables
  /**
     Set Cartesian variables that are specified by the mask to those of the
     parameters. Only the variables that are specified by the mask will be 
     copied.
     \param mask The mask of variables to be used
     \param Rt   The position and orientation
  */
  void Set( robVariablesMask mask, 
	    const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt );

  /**
     Set Cartesian velocities OR accelerations variables that are specified by 
     the mask to those of the parameters. Only the variables that are 
     specified by the mask will be 
     copied.
     \param mask The mask of variables to be used
     \param x The linear and angular velocities
  */
  void Set( robVariablesMask var,
	    const vctFixedSizeVector<double,6>& x );

  void Set( robVariablesMask var, 
	    const vctDynamicVector<double>& q, 
	    const vctDynamicVector<double>& qd, 
	    const vctDynamicVector<double>& qdd );

  void Set( robVariablesMask var, 
	    const vctDynamicVector<double>& x );

  //! Variable index of time
  static const robVariablesMask TIME= 0x0000000000000001LL;
  
  //! Cartesian variables
  static const robVariablesMask TX  = 0x0000000000000002LL;
  static const robVariablesMask TY  = 0x0000000000000004LL;
  static const robVariablesMask TZ  = 0x0000000000000008LL;
  static const robVariablesMask RX  = 0x0000000000000010LL;
  static const robVariablesMask RY  = 0x0000000000000020LL;
  static const robVariablesMask RZ  = 0x0000000000000040LL;

  static const robVariablesMask VX  = 0x0000000000000080LL;
  static const robVariablesMask VY  = 0x0000000000000100LL;
  static const robVariablesMask VZ  = 0x0000000000000200LL;
  static const robVariablesMask WX  = 0x0000000000000400LL;
  static const robVariablesMask WY  = 0x0000000000000800LL;
  static const robVariablesMask WZ  = 0x0000000000001000LL;

  static const robVariablesMask VXD = 0x0000000000002000LL;
  static const robVariablesMask VYD = 0x0000000000004000LL;
  static const robVariablesMask VZD = 0x0000000000008000LL;
  static const robVariablesMask WXD = 0x0000000000010000LL;
  static const robVariablesMask WYD = 0x0000000000020000LL;
  static const robVariablesMask WZD = 0x0000000000040000LL;

  static const robVariablesMask FX =  0x0000000000080000LL;
  static const robVariablesMask FY =  0x0000000000100000LL;
  static const robVariablesMask FZ =  0x0000000000200000LL;
  static const robVariablesMask MX =  0x0000000000400000LL;
  static const robVariablesMask MY =  0x0000000000800000LL;
  static const robVariablesMask MZ =  0x0000000001000000LL;

  //! Joint variables
  static const robVariablesMask Q1  = 0x0000000002000000LL;
  static const robVariablesMask Q2  = 0x0000000004000000LL;
  static const robVariablesMask Q3  = 0x0000000008000000LL;
  static const robVariablesMask Q4  = 0x0000000010000000LL;
  static const robVariablesMask Q5  = 0x0000000020000000LL;
  static const robVariablesMask Q6  = 0x0000000040000000LL;
  static const robVariablesMask Q7  = 0x0000000080000000LL;
  static const robVariablesMask Q8  = 0x0000000100000000LL;
  static const robVariablesMask Q9  = 0x0000000200000000LL;

  static const robVariablesMask Q1D = 0x0000000400000000LL;
  static const robVariablesMask Q2D = 0x0000000800000000LL;
  static const robVariablesMask Q3D = 0x0000001000000000LL;
  static const robVariablesMask Q4D = 0x0000002000000000LL;
  static const robVariablesMask Q5D = 0x0000004000000000LL;
  static const robVariablesMask Q6D = 0x0000008000000000LL;
  static const robVariablesMask Q7D = 0x0000010000000000LL;
  static const robVariablesMask Q8D = 0x0000020000000000LL;
  static const robVariablesMask Q9D = 0x0000040000000000LL;

  static const robVariablesMask Q1DD= 0x0000080000000000LL;
  static const robVariablesMask Q2DD= 0x0000100000000000LL;
  static const robVariablesMask Q3DD= 0x0000200000000000LL;
  static const robVariablesMask Q4DD= 0x0000400000000000LL;
  static const robVariablesMask Q5DD= 0x0000800000000000LL;
  static const robVariablesMask Q6DD= 0x0001000000000000LL;
  static const robVariablesMask Q7DD= 0x0002000000000000LL;
  static const robVariablesMask Q8DD= 0x0004000000000000LL;
  static const robVariablesMask Q9DD= 0x0008000000000000LL;

  static const robVariablesMask FT1 = 0x0010000000000000LL;
  static const robVariablesMask FT2 = 0x0020000000000000LL;
  static const robVariablesMask FT3 = 0x0040000000000000LL;
  static const robVariablesMask FT4 = 0x0080000000000000LL;
  static const robVariablesMask FT5 = 0x0100000000000000LL;
  static const robVariablesMask FT6 = 0x0200000000000000LL;
  static const robVariablesMask FT7 = 0x0400000000000000LL;
  static const robVariablesMask FT8 = 0x0800000000000000LL;
  static const robVariablesMask FT9 = 0x1000000000000000LL;

  static const robVariablesMask TRANSLATION   = TX | TY | TZ;
  static const robVariablesMask ROTATION      = RX | RY | RZ;
  static const robVariablesMask CARTESIAN_POS = ROTATION | TRANSLATION;

  static const robVariablesMask LINEAR_VEL =  VX  | VY  | VZ;
  static const robVariablesMask ANGULAR_VEL = WX  | WY  | WZ;
  static const robVariablesMask CARTESIAN_VEL = LINEAR_VEL | ANGULAR_VEL;

  static const robVariablesMask LINEAR_ACC =  VXD | VYD | VZD;
  static const robVariablesMask ANGULAR_ACC = WXD | WYD | WZD;
  static const robVariablesMask CARTESIAN_ACC = LINEAR_ACC | ANGULAR_ACC;

  static const robVariablesMask JOINTS_POS = Q1 | Q2 | Q3 | Q4 | Q5 | Q6 | Q7 | Q8 | Q9;
  static const robVariablesMask JOINTS_VEL = Q1D |Q2D |Q3D |Q4D |Q5D |Q6D |Q7D |Q8D |Q9D;
  static const robVariablesMask JOINTS_ACC = Q1DD|Q2DD|Q3DD|Q4DD|Q5DD|Q6DD|Q7DD|Q8DD|Q9DD;

public:

  friend std::ostream& operator << (std::ostream& os, const robVariables& var){
    os << var.mask;
    return os;
  }    

};  

#endif
