#ifndef _robDOF_h
#define _robDOF_h

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <stdint.h>
#include <iostream>
using namespace std;

typedef double real;
typedef vctFrame4x4< real,VCT_ROW_MAJOR> SE3;
typedef vctMatrixRotation3<real,VCT_ROW_MAJOR> SO3;
typedef vctQuaternionRotation3<real> Quaternion;
typedef vctFixedSizeVector<real,1> R1;
typedef vctFixedSizeVector<real,2> R2;
typedef vctFixedSizeVector<real,3> R3;
typedef vctFixedSizeVector<real,4> R4;
typedef vctFixedSizeVector<real,6> R6;
typedef vctDynamicVector<real>     Rn;

namespace cisstRobot{

  enum robError{ SUCCESS, FAILURE };

  
  //! Degrees for freedom (DOF) of a robot.
  /**
     robDOF defines the degrees of freedom used to generate trajectories and 
     control a robot. The DOF include joint position, velocities and accelerations
     as well as Cartesian position, velocities and accelerations. Forces and
     torques are also available.
  */
  class robDOF{
  private:

    static size_t DOFtoIndex( uint64_t dofid );

    uint64_t dof;
  
  public:

    real t;

    //! The orientation and position
    SE3 Rt;
    //! The angular and linear velocities
    R6  vw;
    //! The angular and linear accelerations
    R6  vdwd;
    
    //! A vector of real numbers (joint positions)
    Rn  x;
    //! A vector of 1st time derivatives (joint velocities)
    Rn  xd;
    //! A vector of 2nd time derivatives (joint accelerations)
    Rn  xdd;
    
    //! Create a DOF mask with all DOF "turned off"
    robDOF();
    robDOF(uint64_t dof);

    //! Is one of the DOF Cartesian?
    /**
       \return true if one of the DOF is a Cartesian value. That is any 
       Cartesian position, orientation, velocity or acceleration.
     */
    bool IsCartesian()  const;

    //! Is one of the DOF a Cartesian translation
    /**
       \return true if one of the DOF is a Cartesian translation. That is any 
       Cartesian position, a linear velocity or acceleration.
     */
    bool IsTranslation()const;

    //! Is one of the DOF a Cartesian orientation
    /**
       \return true if one of the DOF is a Cartesian orientation. That is any 
       Cartesian orientation, an angular velocity or acceleration.
     */
    bool IsRotation()   const;

    //! Is one of the DOF a real value (read joint value)
    /**
       \return true if one of the DOF is real valued. That is any 
       real position, an velocity or acceleration. These DOF are used to represent
       joint positions, velocities and acceleration. A maximum of 8 joints are
       supported.
     */
    bool IsReal()       const;
    bool IsTime()       const;
    
    //! Set one or many DOF
    void Set( uint64_t dof );

    //! Get the bit mask
    uint64_t GetDOF( ) const ;

    //! Return true if any of the DOF is set
    /**
       \param dof The DOF mask to test
       \return true if any of the DOF in dof are set
    */
    bool IsSet( uint64_t dof ) const;
    
    //! Used by the map
    friend bool operator <  ( const robDOF& dof1, const robDOF& dof2 )
    {  return dof1.dof < dof2.dof;   }
   //! Used by the map
    friend bool operator == ( const robDOF& dof1, const robDOF& dof2 )
    {  return dof1.dof == dof2.dof;  }
    

    //! Create a real DOF object with a single value
    /**
       Create a 1D DOF object and only sets the TIME DOF mask
     */
    robDOF( real x );

    //! Create a real DOF object with a vector of values
    /**
       Create a nD DOF object and sets the X1-X8 DOF mask
     */
    robDOF( const Rn& x );

    //! Create a real DOF object with a vector of values and their derivatives
    /**
       Create a nD DOF object and sets the X, XD and XDD DOF mask
    */
    robDOF( const Rn& x, const Rn& xd, const Rn& xdd );

    //! Create a Cartesian DOF object with orientation and position
    /**
       This creates a Cartesian DOF object with the given position and orientation
       and only sets the mask of Cartesian position and orientation
     */
    robDOF( const SE3& Rt );

    //! Create a Cartesian DOF object with position velocities and accelerations
    /**
       This creates a Cartesian DOF object with the given position and orientation
       velocities and accelerations and sets the mask of Cartesian 
       position/orientation, linear/angular velocities and linear/angular
       accelerations
     */
    robDOF( const SE3& Rt, const R6& vw, const R6& vdwd );



    void Set( uint64_t dof, const Rn& x, const Rn& xd=Rn(), const Rn& xdd=Rn() );
    void Set( uint64_t dof, const SE3& x, const R6& xd=R6(), const R6& xdd=R6() );

    void SetPos( uint64_t dof, const Rn& x );
    void SetPos( uint64_t dof, const SE3& Rt );

    void SetVel( uint64_t dof, const Rn& xd );
    void SetVel( uint64_t dof, const R6& vw );

    void SetAcc( uint64_t dof, const Rn& xdd );
    void SetAcc( uint64_t dof, const R6& vdwd );

    static const uint64_t CARTESIAN  = 0x00000000003F3F3FULL;
    static const uint64_t REAL       = 0x00FFFFFF00000000ULL;
  
    //! DOF of all Cartesian orientation (R) and translations (T) (bits 1-6)
    static const uint64_t RT  = 0x000000000000003FULL;
    //! DOF of the Cartesian translation along the X axis
    static const uint64_t TX  = 0x0000000000000001ULL;
    //! DOF of the Cartesian translation along the Y axis
    static const uint64_t TY  = 0x0000000000000002ULL;
    //! DOF of the Cartesian translation along the Z axis
    static const uint64_t TZ  = 0x0000000000000004ULL;
    //! DOF of the Cartesian orientation along the X axis
    static const uint64_t RX  = 0x0000000000000008ULL;
    //! DOF of the Cartesian orientation along the Y axis
    static const uint64_t RY  = 0x0000000000000010ULL;
    //! DOF of the Cartesian orientation along the Z axis
    static const uint64_t RZ  = 0x0000000000000020ULL;
    
    //! DOF of all Cartesian angular (W) and linear (V) velocities (bits 9-14)
    static const uint64_t VW  = 0x0000000000003F00ULL;
    //! DOF of the Cartesian linear velocity along the X axis
    static const uint64_t VX  = 0x0000000000000100ULL;
    //! DOF of the Cartesian linear velocity along the Y axis
    static const uint64_t VY  = 0x0000000000000200ULL;
    //! DOF of the Cartesian linear velocity along the Z axis
    static const uint64_t VZ  = 0x0000000000000400ULL;
    //! DOF of the Cartesian angular velocity about the X axis
    static const uint64_t WX  = 0x0000000000000800ULL;
    //! DOF of the Cartesian angular velocity about the Y axis
    static const uint64_t WY  = 0x0000000000001000ULL;
    //! DOF of the Cartesian angular velocity about the Z axis 
    static const uint64_t WZ  = 0x0000000000002000ULL;
    
    //! DOF of all Cartesian angular (WD) and linear (VD) accelerations(bits 17-22)
    static const uint64_t VDWD= 0x00000000003F0000ULL;
    //! DOF of the Cartesian linear acceleration along the X axis
    static const uint64_t VXD = 0x0000000000010000ULL;
    //! DOF of the Cartesian linear acceleration along the Y axis
    static const uint64_t VYD = 0x0000000000020000ULL;
    //! DOF of the Cartesian linear acceleration along the Z axis
    static const uint64_t VZD = 0x0000000000040000ULL;
    //! DOF of the Cartesian angular acceleration along the X axis
    static const uint64_t WXD = 0x0000000000080000ULL;
    //! DOF of the Cartesian angular acceleration along the Y axis
    static const uint64_t WYD = 0x0000000000100000ULL;
    //! DOF of the Cartesian angular acceleration along the Z axis
    static const uint64_t WZD = 0x0000000000200000ULL;
    
    //! DOF of all real positions (joint positions)   (bits 33-40)
    static const uint64_t XPOS= 0x000000FF00000000ULL;
    //! DOF of the 1st joint position
    static const uint64_t X1  = 0x0000000100000000ULL;
    //! DOF of the 2nd joint position
    static const uint64_t X2  = 0x0000000200000000ULL;
    //! DOF of the 3rd joint position
    static const uint64_t X3  = 0x0000000400000000ULL;
    //! DOF of the 4th joint position
    static const uint64_t X4  = 0x0000000800000000ULL;
    //! DOF of the 5th joint position
    static const uint64_t X5  = 0x0000001000000000ULL;
    //! DOF of the 6th joint position
    static const uint64_t X6  = 0x0000002000000000ULL;
    //! DOF of the 7th joint position
    static const uint64_t X7  = 0x0000004000000000ULL;
    //! DOF of the 8th joint position
    static const uint64_t X8  = 0x0000008000000000ULL;
    
    //! DOF of all real velocities (joint velocities) (bits 41-48)
    static const uint64_t XVEL= 0x0000FF0000000000ULL;
    //! DOF of the 1st joint velocity
    static const uint64_t X1D = 0x0000010000000000ULL;
    //! DOF of the 2nd joint velocity
    static const uint64_t X2D = 0x0000020000000000ULL;
    //! DOF of the 3rd joint velocity
    static const uint64_t X3D = 0x0000040000000000ULL;
    //! DOF of the 4th joint velocity
    static const uint64_t X4D = 0x0000080000000000ULL;
    //! DOF of the 5th joint velocity
    static const uint64_t X5D = 0x0000100000000000ULL;
    //! DOF of the 6th joint velocity
    static const uint64_t X6D = 0x0000200000000000ULL;
    //! DOF of the 7th joint velocity
    static const uint64_t X7D = 0x0000400000000000ULL;
    //! DOF of the 8th joint velocity
    static const uint64_t X8D = 0x0000800000000000ULL;
    
    //! DOF of all real velocities (joint accelerations) (bits 49-56)
    static const uint64_t XACC= 0x00FF000000000000ULL;
    //! DOF of the 1st joint acceleration
    static const uint64_t X1DD= 0x0001000000000000ULL;
    //! DOF of the 2nd joint acceleration
    static const uint64_t X2DD= 0x0002000000000000ULL;
    //! DOF of the 3rd joint acceleration
    static const uint64_t X3DD= 0x0004000000000000ULL;
    //! DOF of the 4th joint acceleration
    static const uint64_t X4DD= 0x0008000000000000ULL;
    //! DOF of the 5th joint acceleration
    static const uint64_t X5DD= 0x0010000000000000ULL;
    //! DOF of the 6th joint acceleration
    static const uint64_t X6DD= 0x0020000000000000ULL;
    //! DOF of the 7th joint acceleration
    static const uint64_t X7DD= 0x0040000000000000ULL;
    //! DOF of the 8th joint acceleration
    static const uint64_t X8DD= 0x0080000000000000ULL;
    
    //! DOF that represents the time
    static const uint64_t TIME= 0x8000000000000000ULL; 
    
    friend std::ostream& operator << (std::ostream& os, const robDOF& dof){
      os << dof.dof;
      return os;
    }    
  };  
}

#endif
