
#ifndef _devCANopen_h
#define _devCANopen_h

#include <cisstDevices/can/devCAN.h>
#include <cisstDevices/can/CiA301.h>
#include <cisstDevices/can/CiA402.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devCANopen {

 public:
   
   enum Errno
   {
     ESUCCESS, EFAILURE
   };
   
 public:
   
   devCAN* candevice;
   bool deviceopened;

   /*! \brief Read CAN message from CAN driver */
   devCANopen::Errno Read( devCAN::Frame& frame );

   /*! \brief Write CAN frame. */
   devCANopen::Errno Write( const devCAN::Frame& frame );

   devCAN::Frame Pack( CiA301::Node::ID nodeid,
		       CiA301::SDO::Command command,
		       const CiA301::Object& object );

   devCAN::Frame Pack( CiA301::Node::ID nodeid, CiA301::NMT::State state );

   void Unpack( const devCAN::Frame& frame,
		CiA301::Node::ID& nodeid,
		CiA301::NMT::State& state );

   void Unpack( const devCAN::Frame& frame,
		CiA301::Node::ID& nodeid,
		CiA301::SDO::Command& command,
		CiA301::Object& object );

   devCANopen::Errno SDO( CiA301::Node::ID nodeid,
			  CiA301::SDO::Command command,
			  const CiA301::Object& object );

   devCANopen::Errno NMT( CiA301::Node::ID nodeid, CiA301::NMT::State command );

   devCANopen::Errno SYNC();

   devCANopen::Errno SetControlWord( CiA301::Node::ID nodeid, 
				     CiA402::ControlWord::Word word );
   
   devCANopen::Errno SetOperationMode( CiA301::Node::ID nodeid, 
				       CiA402::OperationMode::Mode mode );

 public:
   
   devCANopen( devCAN* can );
   ~devCANopen();

   devCANopen::Errno Open(void);

   devCANopen::Errno Close(void);

   bool IsOpen(void) const { return deviceopened; }
   
   static bool IsNMTFrame( const devCAN::Frame& frame );
   static bool IsNMTErrorFrame( const devCAN::Frame& frame );
   static bool IsSyncFrame( const devCAN::Frame& frame );
   static bool IsEmergencyFrame( const devCAN::Frame& frame );
   static bool IsTimeStampFrame( const devCAN::Frame& frame );
   static bool IsPDOFrame( const devCAN::Frame& frame );
   static bool IsSDOFrame( const devCAN::Frame& frame );
   static bool IsBootUpFrame( const devCAN::Frame& frame );

   // CiA 301

   devCANopen::Errno RPDO2 ( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno RPDO2 ( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno RPDO21( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno RPDO21( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno RPDO24( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno RPDO24( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno TPDO1 ( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno TPDO1 ( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno TPDO24( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno TPDO24( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno TPDO25( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno TPDO25( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );

   devCANopen::Errno TPDO26( CiA301::Node::ID nodeid, CiA301::COBID cobid );
   devCANopen::Errno TPDO26( CiA301::Node::ID nodeid, 
			     CiA301::PDO::Transmission transmission );
   devCANopen::Errno TPDO26( CiA301::Node::ID nodeid, 
			     CiA301::TPDO26::Mapping mapping,
			     CiA301::TPDO26::Data );

   // CiA 402
   devCANopen::Errno DisableVoltage ( CiA301::Node::ID nodeid );
   devCANopen::Errno ResetFaults    ( CiA301::Node::ID nodeid );
   devCANopen::Errno Shutdown       ( CiA301::Node::ID nodeid );
   devCANopen::Errno SwitchOn       ( CiA301::Node::ID nodeid );
   devCANopen::Errno EnableOperation( CiA301::Node::ID nodeid );
   devCANopen::Errno StartHoming    ( CiA301::Node::ID nodeid );
   devCANopen::Errno EndHoming      ( CiA301::Node::ID nodeid );

   devCANopen::Errno PositionMode( CiA301::Node::ID nodeid );
   devCANopen::Errno PVTMode     ( CiA301::Node::ID nodeid );
   devCANopen::Errno HomingMode  ( CiA301::Node::ID nodeid );

   devCANopen::Errno SetPreOperational ( CiA301::Node::ID nodeid );
   devCANopen::Errno SetOperational    ( CiA301::Node::ID nodeid );
   
   devCANopen::Errno ConfigureRPDO2 ( CiA301::Node::ID nodeid );
   devCANopen::Errno ConfigureRPDO21( CiA301::Node::ID nodeid );
   devCANopen::Errno ConfigureRPDO24( CiA301::Node::ID nodeid );


   devCANopen::Errno HomeOffset  ( CiA301::Node::ID nodeid, int offset );
   devCANopen::Errno HomingZeroSpeed( CiA301::Node::ID nodeid, 
				      unsigned int speed );
   devCANopen::Errno HomingSwitchSpeed( CiA301::Node::ID nodeid, 
					unsigned int speed );
   devCANopen::Errno HomingAcceleration ( CiA301::Node::ID nodeid, 
					int acceleration );
   devCANopen::Errno HomingMethod ( CiA301::Node::ID nodeid, 
				    int method );

   devCANopen::Errno PVTBufferClear( CiA301::Node::ID nodeid );
};

#endif  // _CANOPEN_INCLUDED


   //! Pack SDO frame
   /** devCANopen states that a SDO message has the following structure:
    *  [ 3bits ][ 1bit ][ 2bits ][ 1bit ][ 1bit ][ 2bytes ][ 1byte ][ 4bytes ]
    * Where the first 8 bits are a command, the next 2 bytes are an object index
    * the next byte is a subindex (if any) and the last four bytes are the data
    * (if any)
    * \param cobid Node id.  Note that the base id is one, not zero
    * \param command SDO command
    * \return A packed CAN frame
    * \sa UnpackSDO
    */

   //! Unpack SDO frame
   /** CANopen states that an SDO message has the following structure:
    *  [ 3bits ][ 1bit ][ 2bits ][ 1bit ][ 1bit ][ 2bytes ][ 1byte ][ 4bytes ]
    * Where the first 8 bits are a command, the next 2 bytes are an object index
    * the next byte is a subindex (if any) and the last four bytes are the data
    * (if any)
    * \param[in] cobid Node id.  Note that the base id is one, not zero
    * \param[out] command SDO command
    * \sa UnpackSDO
    */   

   /*! \brief Pack NMT frame 
     \param id Node id.  Note that the base id is one, not zero 
     \param command NMT command
     \param frame Packed CAN frame 
   */   
