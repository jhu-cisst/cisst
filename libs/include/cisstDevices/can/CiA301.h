
#ifndef _CiA301_h
#define _CiA301_h

#include <cisstDevices/can/devCAN.h>

namespace CiA301{

  typedef devCAN::Frame::ID COBID;

  namespace Node
  {
    enum ID
      {
	NODE_0 = 0x000,
	NODE_1 = 0x001,
	NODE_2 = 0x002,
	NODE_3 = 0x003,
	NODE_4 = 0x004,
	NODE_5 = 0x005,
	NODE_6 = 0x006,
      };
    
    CiA301::Node::ID operator++( CiA301::Node::ID& id, int  );

  }
  
  namespace NMT
  {
    const devCAN::Frame::ID COBID   = 0x0000;
    enum State
      {
	OPERATIONAL         = 0x01,
	STOPPED             = 0x02,
	PRE_OPERATIONAL     = 0x80,
	RESET_NODE          = 0x81,
	RESET_COMMUNICATION = 0x82
      };
  }
  
  namespace BOOTUP
  {
    namespace COBID{
      const CiA301::COBID LOW  = 0x0700;
      const CiA301::COBID HIGH = 0x077F;
    }
  }
  
  namespace SYNC
  {
    const devCAN::Frame::ID COBID = 0x0080;
  }
  
  namespace EMERGENCY
  {
    namespace COBID
    {
      const devCAN::Frame::ID LOW  = 0x0080;
      const devCAN::Frame::ID HIGH = 0x00FF;
    }
    
    enum Code
      {
	PVT_SEQUENCE_COUNTER  = 0x00,
	PVT_CANNOT_BE_STARTED = 0x01,
	PVT_BUFFER_UNDERFLOW  = 0x02,
      };
  }
  
  namespace TIMESTAMP
  {
    const devCAN::Frame::ID COBID = 0x0100;
  }
  
  namespace PDO
  {

    namespace COBID
    {
      const devCAN::Frame::ID LOW  = 0x0180;
      const devCAN::Frame::ID HIGH = 0x057F;
    } // COBID

    enum SubIndex { COMMUNICATION = 0x01, TRANSMISSION  = 0x02 };

    enum Transmission 
      { 
	SYNC_ACYCLIC   = 0x00,
	SYNC_CYCLIC    = 0x01,   // not really it's a range
	SYNC_RTR       = 0xFC,  
	ASYNC_RTR      = 0xFD,
	ASYNC          = 0xFE
      };
  }
  
  namespace SDO
  {
    
    namespace COBID
    {
      const devCAN::Frame::ID LOW  = 0x0600;
      const devCAN::Frame::ID HIGH = 0x067F;
    }
    
    enum Command
      {
	INITIATE_READ  = 0x40,
	INITIATE_WRITE = 0x20
      };
    
  }   

  // An object has one index and one sub index
  struct Object{
    typedef unsigned short   Index;
    typedef unsigned char SubIndex;
    typedef int               Data;
    CiA301::Object::Index       index;
    CiA301::Object::SubIndex subindex;
    CiA301::Object::Data         data;
    Object( CiA301::Object::Index       idx, 
	    CiA301::Object::SubIndex sidx=0, 
	    CiA301::Object::Data     data=0 ) : 
      index( idx ), subindex( sidx ), data( data ){}
  };

  struct RPDO2 : public CiA301::Object {
    RPDO2( CiA301::COBID cobid )          : 
      Object( 0x1401, CiA301::PDO::COMMUNICATION, cobid ){}
    RPDO2( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1401, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct RPDO21 : public CiA301::Object{
    RPDO21( CiA301::COBID cobid )          : 
      Object( 0x1414, CiA301::PDO::COMMUNICATION, cobid ){}
    RPDO21( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1414, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct RPDO24 : public CiA301::Object{
    RPDO24( CiA301::COBID cobid )          : 
      Object( 0x1417, CiA301::PDO::COMMUNICATION, cobid ){}
    RPDO24( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1417, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct TPDO1 : public CiA301::Object{
    TPDO1( CiA301::COBID cobid )          : 
      Object( 0x1800, CiA301::PDO::COMMUNICATION, cobid ){}
    TPDO1( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1800, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct TPDO24 : public CiA301::Object{
    TPDO24( CiA301::COBID cobid )          : 
      Object( 0x1817, CiA301::PDO::COMMUNICATION, cobid ){}
    TPDO24( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1817, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct TPDO25 : public CiA301::Object{
    TPDO25( CiA301::COBID cobid )          : 
      Object( 0x1818, CiA301::PDO::COMMUNICATION, cobid ){}
    TPDO25( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1818, CiA301::PDO::TRANSMISSION, transmission ){}
  };

  struct TPDO26 : public CiA301::Object{
    enum Mapping
      { 
	MAPPING_CNT = 0x00,
	MAPPING_1   = 0x01, 
	MAPPING_2   = 0x02, 
	MAPPING_3   = 0x03, 
	MAPPING_4   = 0x04,
	MAPPING_5   = 0x05,
      };
    TPDO26( CiA301::COBID cobid )          : 
      Object( 0x1819, CiA301::PDO::COMMUNICATION, cobid ){}
    TPDO26( CiA301::PDO::Transmission transmission ) : 
      Object( 0x1819, CiA301::PDO::TRANSMISSION, transmission ){}
    TPDO26( CiA301::TPDO26::Mapping mapping,  CiA301::TPDO26::Data data ) :
      Object( 0x1A19, mapping, data ){} 
  };

  

  
  /*
      namespace TPDO26
      {
	enum Index
	  {
	    MAPPING          = 0x1A19
	  } // 

	enum Mapping
	  {
	    COUNT         = 0x00;
	    MAPPING_1     = 0x01;
	    MAPPING_2     = 0x02;
	    MAPPING_3     = 0x03;
	    MAPPING_4     = 0x04;
	    MAPPING_5     = 0x05;
	    MAPPING_6     = 0x06;
	    MAPPING_7     = 0x07;
	    MAPPING_8     = 0x08;
	  } // mapping

      } // TPDO26
    */      
}

#endif

