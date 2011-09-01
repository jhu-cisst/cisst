

#ifndef _devOSGBH_h
#define _devOSGBH_h

#include <cisstOSG/cisstOSGManipulator.h>
#include <cisstOSG/cisstOSGExport.h>


//! OSG Barrett Hand
/**
   This class implements a Barrett hand device for OpenSceneGraph. The class
   is derived from cisstOSGManipulator yet it reimplements most of the virtual 
   methods due to the parallel and underactuated architecture.
   The hand creates 3 fingers, themselves OSG manipulators devices and 
   dispatches the I/O to each finger.
*/
class CISST_EXPORT cisstOSGBH : public cisstOSGManipulator {

 protected:

  osg::ref_ptr<cisstOSGManipulator> f1; // Finger 1 
  osg::ref_ptr<cisstOSGManipulator> f2; // Finger 2
  osg::ref_ptr<cisstOSGManipulator> f3; // Finger 3

 protected:

  cisstOSGBH( const vctFrame4x4<double>& Rtw0, const std::string& robotfile ):
    cisstOSGManipulator( Rtw0, robotfile ),
    f1( NULL ),
    f2( NULL ),
    f3( NULL ){}

  cisstOSGBH( const vctFrm3& Rtw0, const std::string& robotfile ):
    cisstOSGManipulator( Rtw0, robotfile ),
    f1( NULL ),
    f2( NULL ),
    f3( NULL ){}

 public:

  //! Barrett Hand constructor
  cisstOSGBH( const std::string& palmmodel,
	      const std::string& metacarpalmodel,
	      const std::string& proximalmodel,
	      const std::string& intermediatemodel,
	      cisstOSGWorld* world,
	      const vctFrame4x4<double>& Rtw0,
	      const std::string& f1f2filename,
	      const std::string& f3filename );

  //! Barrett Hand constructor  
  cisstOSGBH( const std::string& palmmodel,
	      const std::string& metacarpalmodel,
	      const std::string& proximalmodel,
	      const std::string& intermediatemodel,
	      cisstOSGWorld* world,
	      const vctFrm3& Rtw0,
	      const std::string& f1f2filename,
	      const std::string& f3filename );

  ~cisstOSGBH();

  //! Initialize the hand
  void Initialize();

  //! Return the joints positions
  /**
     Query each joint and return the joint positions
     \param q[in] A vector of joints positions
     \return ESUCCESS if successfull. EFAILURE otherwise.
  */
  virtual cisstOSGBH::Errno GetPositions( vctDynamicVector<double>& q ) const;

  //! Set the joint position
  /**
     This sets the position command and stores a local copy
     \param q A vector of joint positions
     \return ESUCCESS if successfull. EFAILURE otherwise.
  */
  virtual cisstOSGBH::Errno SetPositions( const vctDynamicVector<double>& qs );

};

#endif
