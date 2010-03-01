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

#ifndef _robSpace_h
#define _robSpace_h

#include <vector>
#include <iostream>
#include <cisstRobot/robExport.h>

#ifdef __GNUC__
#include <stdint.h>
#endif

//! Variables of a robot.
/**
   robSpace defines the variables used to generate trajectories and control
   a robot. The variables include joint position, velocities and accelerations
   as well as Cartesian position, velocities and accelerations. Forces and
   torques are also available.
*/
class CISST_EXPORT robSpace{
  
 public:

  //! Bit mask for the space basis
  /**
     The mask is designed to hold 64 basis, hence 64 variables. The mask
     determine which basis are "enabled" and which basis are "disabled"
  */
#ifdef __GNUC__
  typedef uint64_t Basis;
#endif
  
#ifdef _WIN32
  typedef unsigned __int64 Basis;
#endif
    
 private:

  //! The basis that determine which variables are enabled
  robSpace::Basis basis;

 protected:
  
  //! Determine the index of the largest "enabled" joint variables
  /**
     This method determines the largest index among the enabled joints 
     variables. For example, if only Q1 and Q3 are enabled, then the largest
     index would be 3-1=2 (the result is zero-indexed)
     \param mask A mask with joints variables
   */
  int LargestJointIndex( robSpace::Basis basis );
  
 public:
  
  //! Create an empty space
  robSpace();
  
  //! Create a space defined by the basis
  /**
     This only sets the basis to those of the given parameter.
     \param var The basis defining the space
  */
  robSpace( robSpace::Basis basis );

  robSpace::Basis  GetBasis() const;
  robSpace::Basis& GetBasis();

  //! Is any of the given basis enabled?
  /**
     \param basis A basis
     \return true if any of the basis are enabled
  */
  bool IsEnabled( robSpace::Basis basis ) const;
  

  //! Is the time basis enabled?
  /**
     \return true if the time basis is set
  */
  bool IsTimeEnabled() const;
  

  //! Is one translation basis enabled?
  /**
     \return true if any of the Cartesian translation basis is set.
  */
  bool IsTranslationEnabled() const;
  
  //! Is one orientation basis enabled?
  /**
     \return true if any of the Cartesian orientation basis is set.
  */
  bool IsOrientationEnabled() const;
  
  //! Is Cartesian position enabled?
  /**
     \return true if Cartesian position is set.
  */
  bool IsCartesianPositionEnabled() const;  
  bool IsCartesianVelocityEnabled() const;
  bool IsCartesianAccelerationEnabled() const;

  //! Is any joint basis enabled?
  /**
     \return true if any joint variable is set
  */
  bool IsJointPositionEnabled() const;
  bool IsJointVelocityEnabled() const;
  bool IsJointAccelerationEnabled() const;
  bool IsJointEnabled() const;
  
  //! Compare two spaces
  friend bool operator==( const robSpace& space1, const robSpace& space2 )
  {  return space1.GetBasis() == space2.GetBasis();  }

  //! Compare two spaces
  friend bool operator!=( const robSpace& space1, const robSpace& space2 )
  {  return space1.GetBasis() != space2.GetBasis();  }

  //! Enumerate the basis of the space in a vector of basis
  std::vector<robSpace::Basis> ListBasis() const;

  size_t JointBasis2ArrayIndex() const;

  //! Basis for the time variable
  static const robSpace::Basis TIME= 0x0000000000000001LL;
  
  //! Basiss for the Cartesian variables
  static const robSpace::Basis TX  = 0x0000000000000002LL;
  static const robSpace::Basis TY  = 0x0000000000000004LL;
  static const robSpace::Basis TZ  = 0x0000000000000008LL;
  static const robSpace::Basis RX  = 0x0000000000000010LL;
  static const robSpace::Basis RY  = 0x0000000000000020LL;
  static const robSpace::Basis RZ  = 0x0000000000000040LL;

  static const robSpace::Basis TRANSLATION   = TX | TY | TZ;
  static const robSpace::Basis ORIENTATION   = RX | RY | RZ;
  static const robSpace::Basis CARTESIAN_POS = ORIENTATION | TRANSLATION;

  static const robSpace::Basis VX  = 0x0000000000000080LL;
  static const robSpace::Basis VY  = 0x0000000000000100LL;
  static const robSpace::Basis VZ  = 0x0000000000000200LL;
  static const robSpace::Basis WX  = 0x0000000000000400LL;
  static const robSpace::Basis WY  = 0x0000000000000800LL;
  static const robSpace::Basis WZ  = 0x0000000000001000LL;

  static const robSpace::Basis LINEAR_VEL    = VX  | VY  | VZ;
  static const robSpace::Basis ANGULAR_VEL   = WX  | WY  | WZ;
  static const robSpace::Basis CARTESIAN_VEL = LINEAR_VEL | ANGULAR_VEL;

  static const robSpace::Basis VXD = 0x0000000000002000LL;
  static const robSpace::Basis VYD = 0x0000000000004000LL;
  static const robSpace::Basis VZD = 0x0000000000008000LL;
  static const robSpace::Basis WXD = 0x0000000000010000LL;
  static const robSpace::Basis WYD = 0x0000000000020000LL;
  static const robSpace::Basis WZD = 0x0000000000040000LL;

  static const robSpace::Basis LINEAR_ACC    = VXD | VYD | VZD;
  static const robSpace::Basis ANGULAR_ACC   = WXD | WYD | WZD;
  static const robSpace::Basis CARTESIAN_ACC = LINEAR_ACC | ANGULAR_ACC;

  static const robSpace::Basis FX =  0x0000000000080000LL;
  static const robSpace::Basis FY =  0x0000000000100000LL;
  static const robSpace::Basis FZ =  0x0000000000200000LL;
  static const robSpace::Basis MX =  0x0000000000400000LL;
  static const robSpace::Basis MY =  0x0000000000800000LL;
  static const robSpace::Basis MZ =  0x0000000001000000LL;

  //! Basiss for the joint variables
  static const robSpace::Basis Q1  = 0x0000000002000000LL;
  static const robSpace::Basis Q2  = 0x0000000004000000LL;
  static const robSpace::Basis Q3  = 0x0000000008000000LL;
  static const robSpace::Basis Q4  = 0x0000000010000000LL;
  static const robSpace::Basis Q5  = 0x0000000020000000LL;
  static const robSpace::Basis Q6  = 0x0000000040000000LL;
  static const robSpace::Basis Q7  = 0x0000000080000000LL;
  static const robSpace::Basis Q8  = 0x0000000100000000LL;
  static const robSpace::Basis Q9  = 0x0000000200000000LL;

  static const robSpace::Basis JOINTS_POS = Q1|Q2|Q3|Q4|Q5|Q6|Q7|Q8|Q9;

  static const robSpace::Basis Q1D = 0x0000000400000000LL;
  static const robSpace::Basis Q2D = 0x0000000800000000LL;
  static const robSpace::Basis Q3D = 0x0000001000000000LL;
  static const robSpace::Basis Q4D = 0x0000002000000000LL;
  static const robSpace::Basis Q5D = 0x0000004000000000LL;
  static const robSpace::Basis Q6D = 0x0000008000000000LL;
  static const robSpace::Basis Q7D = 0x0000010000000000LL;
  static const robSpace::Basis Q8D = 0x0000020000000000LL;
  static const robSpace::Basis Q9D = 0x0000040000000000LL;

  static const robSpace::Basis JOINTS_VEL = Q1D|Q2D|Q3D|Q4D|Q5D|Q6D|Q7D|Q8D|Q9D;

  static const robSpace::Basis Q1DD= 0x0000080000000000LL;
  static const robSpace::Basis Q2DD= 0x0000100000000000LL;
  static const robSpace::Basis Q3DD= 0x0000200000000000LL;
  static const robSpace::Basis Q4DD= 0x0000400000000000LL;
  static const robSpace::Basis Q5DD= 0x0000800000000000LL;
  static const robSpace::Basis Q6DD= 0x0001000000000000LL;
  static const robSpace::Basis Q7DD= 0x0002000000000000LL;
  static const robSpace::Basis Q8DD= 0x0004000000000000LL;
  static const robSpace::Basis Q9DD= 0x0008000000000000LL;


  static const robSpace::Basis JOINTS_ACC = Q1DD|Q2DD|Q3DD|Q4DD|Q5DD|Q6DD|Q7DD|Q8DD|Q9DD;

  static const robSpace::Basis FT1 = 0x0010000000000000LL;
  static const robSpace::Basis FT2 = 0x0020000000000000LL;
  static const robSpace::Basis FT3 = 0x0040000000000000LL;
  static const robSpace::Basis FT4 = 0x0080000000000000LL;
  static const robSpace::Basis FT5 = 0x0100000000000000LL;
  static const robSpace::Basis FT6 = 0x0200000000000000LL;
  static const robSpace::Basis FT7 = 0x0400000000000000LL;
  static const robSpace::Basis FT8 = 0x0800000000000000LL;
  static const robSpace::Basis FT9 = 0x1000000000000000LL;

  friend std::ostream& operator << (std::ostream& os, const robSpace& var){
    os << var.GetBasis();
    return os;
  }    

};  

#endif
