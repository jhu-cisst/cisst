

#ifndef _cisstODEBH_h
#define _cisstODEBH_h

#include <cisstOSG/cisstOSGBH.h>
#include <cisstODE/cisstODEWorld.h>
#include <cisstODE/cisstODEExport.h>


//! ODE Barrett Hand
/**
   This class implements a Barrett hand device for ODE simulation. The class
   is derived from cisstODEManipulator yet it reimplements most of the virtual 
   methods due to the parallel and underactuated architecture.
   The hand creates 3 fingers, themselves ODE manipulators devices and 
   dispatches the I/O to each finger.
*/
class CISST_EXPORT cisstODEBH : public cisstOSGBH {

 public:

  //! Barrett Hand constructor
  cisstODEBH( const std::string& palmmodel,
	      const std::string& metacarpalmodel,
	      const std::string& proximalmodel,
	      const std::string& intermediatemodel,
	      cisstODEWorld* world,
	      const vctFrame4x4<double>& Rtw0,
	      const std::string& f1f2filename,
	      const std::string& f3filename );

  //! Barrett Hand constructor  
  cisstODEBH( const std::string& palmmodel,
	      const std::string& metacarpalmodel,
	      const std::string& proximalmodel,
	      const std::string& intermediatemodel,
	      cisstODEWorld* world,
	      const vctFrm3& Rtw0,
	      const std::string& f1f2filename,
	      const std::string& f3filename );

  ~cisstODEBH();

};

#endif
