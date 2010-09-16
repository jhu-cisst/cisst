
#ifndef _CiA402_h
#define _CiA402_h

#include <cisstDevices/can/CiA301.h>

namespace CiA402{

  struct ControlWord : public CiA301::Object {
    enum Word
      {
	RESET_FAULT      = 0x80,
	DISABLE_VOLTAGE  = 0x04,
	SHUTDOWN         = 0x06,
	SWITCH_ON        = 0x07,
	ENABLE_OPERATION = 0x0F,
	QUICK_STOP       = 0x02,
	START_HOMING     = 0x1F,
	END_HOMING       = 0x0F
      }; // Control Word
    ControlWord( ControlWord::Word word ) : 
      CiA301::Object( 0x6040, 0, word ){}
  };
  
  struct OperationMode : public CiA301::Object {
    enum Mode
      {	
	POSITION = 1,
	VELOCITY = 3,
	TORQUE   = 4,
	HOMING   = 6,
	PVT      = 7,
	CUSTOM   = 0xFF
      };
    OperationMode( OperationMode::Mode mode ) :
      CiA301::Object( 0x6060, 0, mode ){}
  };
  
  struct ActualPosition : public CiA301::Object{
    ActualPosition() : 
      CiA301::Object( 0x6064 ){}
  };

   
   // Homing stuff
  struct HomeOffset : public CiA301::Object {
    HomeOffset( int offset ) : 
      CiA301::Object( 0x607C, 0, offset ){}
  };

  struct HomingMethod : public CiA301::Object {
    HomingMethod( int method ) : 
      CiA301::Object( 0x6098, 0, method ){}
  };

  struct HomingSpeed : public CiA301::Object {
    enum Search{ SWITCH = 0x01, ZERO = 0x02 };
    HomingSpeed( HomingSpeed::Search search, unsigned int speed ) : 
      CiA301::Object( 0x6099, search, speed ){}
  };

  struct HomingAcceleration : public CiA301::Object {
    HomingAcceleration( int acceleration ) : 
      CiA301::Object( 0x609A, 0, acceleration ){}
  };

   
  // PVT stuff 
  struct PVTClearBuffer : public CiA301::Object{
    PVTClearBuffer() : 
    CiA301::Object( 0x60C4, 6, 0 ){}
  };

   
   /*
  enum Index
    {
      HOMING_ACCELERATION      = 0x609A,
      PVT_BUFFER_CONFIGURATION = 0x60C4
    };  // Index
  typedef unsigned short SubIndex;
  */


  /*
  namespace PVTBufferConfig
  {
    const SubIndex MAX_BUFFER_SIZE     = 0x01;
    const SubIndex ACTUAL_BUFFER_SIZE  = 0x02;
    const SubIndex BUFFER_ORGANIZATION = 0x03;
    const SubIndex BUFFER_POSITION     = 0x04;
    const SubIndex DATA_RECORD_SIZE    = 0x05;
    const SubIndex BUFFER_CLEAR        = 0x06;
  } // PVT buffer
  */

}

#endif
