
#ifndef _CiA301_h
#define _CiA301_h

#include <cisstDevices/can/devCAN.h>

namespace CiA301{
   
   typedef unsigned short COBID;
   
   namespace Node
     {
	typedef unsigned char ID;
	static const CiA301::Node::ID NODE_0 = 0x000;
	static const CiA301::Node::ID NODE_1 = 0x001;
	static const CiA301::Node::ID NODE_2 = 0x002;
	static const CiA301::Node::ID NODE_3 = 0x003;
	static const CiA301::Node::ID NODE_4 = 0x004;
	static const CiA301::Node::ID NODE_5 = 0x005;
     };
   
   
   // An object has one index and one sub index
   struct Object{
      
    public:
      
      typedef unsigned char                               Data;
      typedef vctDynamicVector<CiA301::Object::Data> DataField; // data field
      
      CiA301::Object::DataField    data;
      
      Object(){} 
      Object( const CiA301::Object::DataField&  data ) : data( data ) {}

      friend std::ostream& operator<<( std::ostream& os, 
				       const CiA301::Object& object ){
	 for( size_t i=0; i<object.data.size(); i++ ){
	   os << "0x" << std::hex << std::setfill('0') << std::setw(2) 
	      << (int)(object.data[i]) << " ";
	 }
	 os << std::dec;
	 return os;
      }
   };

   struct SYNC : public CiA301::Object{
      SYNC() : Object(){}	   
   };

   struct NMT : public CiA301::Object{
      
      static const CiA301::COBID COBID = 0x000;
      
      enum State
	{
	   OPERATIONAL         = 0x01,
	   STOPPED             = 0x02,
	   PRE_OPERATIONAL     = 0x80,
	   RESET_NODE          = 0x81,
	   RESET_COMMUNICATION = 0x82
	};
      NMT( CiA301::NMT::State state, CiA301::Node::ID nodeid ) :
	Object( DataField( 2, state, nodeid ) ){}
   };   
   
   struct NMTOperational : public CiA301::NMT {
      NMTOperational( CiA301::Node::ID nodeid ):
	NMT( CiA301::NMT::OPERATIONAL, nodeid ){}
   };
   
   struct NMTStop : public CiA301::NMT {
      NMTStop( CiA301::Node::ID nodeid ): 
	NMT( CiA301::NMT::STOPPED, nodeid ){}
   };
   
   struct NMTPreOperational : public CiA301::NMT {
      NMTPreOperational( CiA301::Node::ID nodeid ): 
	NMT( CiA301::NMT::PRE_OPERATIONAL, nodeid ){}
   };
   
   struct NMTResetNode : public CiA301::NMT {
      NMTResetNode( CiA301::Node::ID nodeid ): 
	NMT( CiA301::NMT::RESET_NODE, nodeid ){}
   };
   
   struct NMTResetCommunication : public CiA301::NMT {
      NMTResetCommunication( CiA301::Node::ID nodeid ):
	NMT( CiA301::NMT::RESET_COMMUNICATION, nodeid ){}
   };


  namespace EMERGENCY
  {

    const CiA301::COBID LOW  = 0x0080;
    const CiA301::COBID HIGH = 0x00FF;
    
    enum Code
      {
	PVT_SEQUENCE_COUNTER  = 0x00,
	PVT_CANNOT_BE_STARTED = 0x01,
	PVT_BUFFER_UNDERFLOW  = 0x02,
      };
  }
  
  namespace TIMESTAMP
  {
    const CiA301::COBID COBID = 0x0100;
  }
  
  namespace PDO
  {

    namespace COBID
    {
      const CiA301::COBID LOW  = 0x0180;
      const CiA301::COBID HIGH = 0x057F;
    }

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
  
  struct SDO : public Object{

    typedef unsigned short   Index; // 2 bytes
    typedef unsigned char SubIndex; // 1 byte
    typedef int               Data; // 4 bytes

    static const CiA301::COBID TRANSMIT      = 0x580;
    static const CiA301::COBID TRANSMIT_LOW  = 0x581;
    static const CiA301::COBID TRANSMIT_HIGH = 0x5FF;

    static const CiA301::COBID RECEIVE       = 0x600;
    static const CiA301::COBID RECEIVE_LOW   = 0x601;
    static const CiA301::COBID RECEIVE_HIGH  = 0x680;
     
    enum Command
      {
	INITIATE_READ  = 0x40,
	INITIATE_WRITE = 0x22
      };
   
     SDO( CiA301::SDO::Command   command,
	  CiA301::SDO::Index       index,
	  CiA301::SDO::SubIndex subindex ) : 
       Object( CiA301::Object::DataField( 8, 
					  command, 
					  (index>>0) & 0xFF, 
					  (index>>8) & 0xFF,
					  subindex,
					  0x00, 0x00, 0x00, 0x00 ) ){}
     SDO( CiA301::SDO::Command   command,
	  CiA301::SDO::Index       index,
	  CiA301::SDO::SubIndex subindex,
	  CiA301::SDO::Data         data ) : 
       Object( CiA301::Object::DataField( 8, 
					  command, 
					  (index>>0) & 0xFF, 
					  (index>>8) & 0xFF,
					  subindex,
					  (data>> 0) & 0xFF,
					  (data>> 8) & 0xFF,
					  (data>>16) & 0xFF,
					  (data>>24) & 0xFF ) ){}
  };
   
   struct RPDO2 : public CiA301::SDO {
      RPDO2( CiA301::COBID cobid ) : 
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1401, 
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      RPDO2( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1401, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };
   
   struct RPDO21 : public CiA301::SDO {
      RPDO21( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1414, 
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      RPDO21( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1414, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };

   struct RPDO24 : public CiA301::SDO {
      RPDO24( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1417, 
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      RPDO24( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1417, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };
   

   struct TPDO1 : public CiA301::SDO {
      TPDO1( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1800,
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      TPDO1( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1800, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };
   

   struct TPDO24 : public CiA301::SDO {
      TPDO24( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1817,
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      TPDO24( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1817, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };
   
   struct TPDO25 : public CiA301::SDO {
      TPDO25( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1818,
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      TPDO25( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,  
	     0x1818, 
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
   };
   
   struct TPDO26 : public CiA301::SDO {

      enum Mapping
	{ 
	   MAPPING_CNT = 0x00,
	   MAPPING_1   = 0x01, 
	   MAPPING_2   = 0x02, 
	   MAPPING_3   = 0x03, 
	   MAPPING_4   = 0x04,
	   MAPPING_5   = 0x05,
	};
      
      TPDO26( CiA301::COBID cobid ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 
	     0x1819,
	     CiA301::PDO::COMMUNICATION, 
	     cobid ) {}
      
      TPDO26( CiA301::PDO::Transmission transmission ) : 
	SDO( CiA301::SDO::INITIATE_WRITE,
	     0x1819,
	     CiA301::PDO::TRANSMISSION, 
	     transmission ){}
      
      TPDO26( CiA301::TPDO26::Mapping mapping,  CiA301::TPDO26::Data data ) :
	SDO( CiA301::SDO::INITIATE_WRITE, 0x1A19, mapping, data ){}
      
   };
   
}

#endif

