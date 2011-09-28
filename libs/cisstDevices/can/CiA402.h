
#ifndef _CiA402_h
#define _CiA402_h

#include <cisstDevices/can/CiA301.h>

namespace CiA402{
   
   struct ControlWord : public CiA301::SDO {
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
	};
      ControlWord( CiA402::ControlWord::Word word ) :
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x6040, 0, word ){}
   };
   
   struct StatusWord : public CiA301::SDO {
      enum Word
	{
	   NOT_READY_TO_SWITCH_ON = 0,
	   SWITCH_ON_DISABLED     = 1,
	   READY_TO_SWITCH_ON     = 2,
	   OPERATION_DISABLED     = 3,
	   OPERATION_ENABLED      = 4,
	   QUICK_STOP_ACTIVE      = 5,
	   FAULT                  = 6
	};
      StatusWord() :
	CiA301::SDO( CiA301::SDO::INITIATE_READ, 0x6041, 0, 0 ){}
   };
   
   struct OperationMode : public CiA301::SDO {
      enum Mode
	{	
           IDLE     = 0,
	   POSITION = 1,
	   VELOCITY = 3,
	   TORQUE   = 4,
	   HOMING   = 6,
	   PVT      = 7,
	   CUSTOM   = 0xFF
	};
      OperationMode( CiA402::OperationMode::Mode mode ) :
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x6060, 0, mode ){}
   };
   
   struct ActualPosition : public CiA301::SDO {
      ActualPosition() : CiA301::SDO( CiA301::SDO::INITIATE_READ, 0x6064, 0 ){}
   };
   
   
   // Homing stuff
   // 
   struct HomeOffset : public CiA301::SDO {
      HomeOffset( int offset ) : 
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x607C, 0, offset ){}
   };
   
   struct HomingMethod : public CiA301::SDO {
      HomingMethod( int method ) : 
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x6098, 0, method ){}
   };

   struct HomingSpeed : public CiA301::SDO {
      enum Search{ SWITCH = 0x01, ZERO = 0x02 };
      HomingSpeed( CiA402::HomingSpeed::Search search, unsigned int speed ) : 
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x6099, search, speed ){}
   };

   struct HomingAcceleration : public CiA301::SDO {
      HomingAcceleration( int acceleration ) : 
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x609A, 0, acceleration ){}
   };

   // PVT stuff 
   // 
   struct PVTBuffer : public CiA301::SDO {
      enum Config
	{
	   MAX_SIZE         = 0x01,
	   ACTUAL_IZE       = 0x02,
	   ORGANIZATION     = 0x03,
	   POSITION         = 0x04,
	   DATA_RECORD_SIZE = 0x05,
	   CLEAR            = 0x06
	};
      PVTBuffer( CiA402::PVTBuffer::Config config ) :
	CiA301::SDO( CiA301::SDO::INITIATE_WRITE, 0x60C4, config, 0 ){}
      PVTBuffer( CiA402::PVTBuffer::Config config, int val ) :
	CiA301::SDO( CiA301::SDO::INITIATE_READ, 0x60C4, config, val ){}
   };
   
   


  /*
  namespace PVTBufferConfig
  */

}

#endif
