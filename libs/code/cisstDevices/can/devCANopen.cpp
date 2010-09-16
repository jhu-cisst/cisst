
#include <cisstDevices/can/devCANopen.h>

#include <cisstCommon.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <fcntl.h>

devCANopen::devCANopen( devCAN* candevice ) : 
  candevice( candevice ), 
  deviceopened( false ){}

devCANopen::~devCANopen()
{ Close(); }

devCANopen::Errno devCANopen::Open(void){

  if( candevice == NULL ){
    CMN_LOG_RUN_ERROR << "Failed to open CAN device." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  if( candevice->Open() != devCAN::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to open CAN device." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  deviceopened = true;
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::Close(void){
  if( candevice == NULL ){
    CMN_LOG_INIT_ERROR << "Failed to close CAN device." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  if( candevice->Close() != devCAN::ESUCCESS ){
    CMN_LOG_INIT_ERROR << "Failed to close CAN device." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  deviceopened = false;
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::Read( devCAN::Frame& frame ){

  if( candevice == NULL ){
    CMN_LOG_INIT_ERROR << "Failed to read CAN frame." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  if( candevice->Recv( frame ) != devCAN::ESUCCESS ){
    CMN_LOG_INIT_ERROR << "Failed to read CAN frame." << std::endl;
    return devCANopen::EFAILURE;
  }
  
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::Write( const devCAN::Frame& frame ){
   if( candevice == NULL ){
      CMN_LOG_INIT_ERROR << "Failed to write CAN frame." << std::endl;
     return devCANopen::EFAILURE;
   }
   std::cout << frame << std::endl;
   if( candevice->Send( frame ) != devCAN::ESUCCESS ){
     CMN_LOG_INIT_ERROR << "Failed to write CAN frame." << std::endl;
     return devCANopen::EFAILURE;
   }
   return devCANopen::ESUCCESS;
}

/*******

	Is*Frame

*******/

bool devCANopen::IsNMTFrame(const devCAN::Frame& frame)
{ return ( frame.GetID() == CiA301::NMT::COBID ); }

bool devCANopen::IsNMTErrorFrame(const devCAN::Frame& frame)
{ return IsBootUpFrame(frame); }

bool devCANopen::IsSyncFrame(const devCAN::Frame& frame)
{ return ( frame.GetID() == CiA301::SYNC::COBID ); }

bool devCANopen::IsEmergencyFrame(const devCAN::Frame& frame){
  return ( CiA301::EMERGENCY::COBID::LOW <= frame.GetID() &&
	   frame.GetID() <= CiA301::EMERGENCY::COBID::HIGH );
}

bool devCANopen::IsTimeStampFrame(const devCAN::Frame& frame)
{ return ( frame.GetID() == CiA301::TIMESTAMP::COBID ); }

bool devCANopen::IsPDOFrame(const devCAN::Frame& frame){
  return ( CiA301::PDO::COBID::LOW <= frame.GetID() &&
	   frame.GetID() <= CiA301::PDO::COBID::HIGH );
}

bool devCANopen::IsSDOFrame(const devCAN::Frame& frame){
  return ( CiA301::SDO::COBID::LOW <= frame.GetID() &&
	   frame.GetID() <= CiA301::SDO::COBID::HIGH );
}

bool devCANopen::IsBootUpFrame(const devCAN::Frame& frame)
{
  return ( CiA301::BOOTUP::COBID::LOW <= frame.GetID() &&
	   frame.GetID() <= CiA301::BOOTUP::COBID::HIGH );
}

/*
  Pack
*/

devCAN::Frame devCANopen::Pack( CiA301::Node::ID nodeid, 
				CiA301::NMT::State state ){
   devCAN::Frame::DataField data;
   data[0] = state  & 0xFF;
   data[1] = nodeid & 0xFF;
   return devCAN::Frame( CiA301::NMT::COBID, data, 2 );
}

devCAN::Frame devCANopen::Pack( CiA301::Node::ID nodeid,
				CiA301::SDO::Command command,
				const CiA301::Object& object ){

  devCAN::Frame::ID cobid = CiA301::SDO::COBID::LOW + nodeid;
  devCAN::Frame::DataLength datalength = 8;
  devCAN::Frame::DataField datafield;

  CiA301::Object::Index       index = object.index;
  CiA301::Object::SubIndex subindex = object.subindex;
  CiA301::Object::Data        data  = object.data;

  // first four bytes: command, index, subindex
  datafield[0] = (devCAN::Frame::Data) ( command       & 0xFF );
  datafield[1] = (devCAN::Frame::Data) (  index        & 0xFF );
  datafield[2] = (devCAN::Frame::Data) ( (index >> 8 ) & 0xFF );
  datafield[3] = (devCAN::Frame::Data) ( subindex      & 0xFF );

  // last four bytes: data
  datafield[4] = (devCAN::Frame::Data) (  data         & 0xFF );
  datafield[5] = (devCAN::Frame::Data) ( (data >> 8  ) & 0xFF );
  datafield[6] = (devCAN::Frame::Data) ( (data >> 16 ) & 0xFF );
  datafield[7] = (devCAN::Frame::Data) ( (data >> 24 ) & 0xFF );
  
  return devCAN::Frame( cobid, datafield, datalength );
   
}

// unpack
void devCANopen::Unpack( const devCAN::Frame& frame,
			 CiA301::Node::ID& nodeid,
			 CiA301::SDO::Command& command,
			 CiA301::Object& object ){

  nodeid = CiA301::Node::ID( frame.GetID() - CiA301::SDO::COBID::LOW );
  
  const devCAN::Frame::Data* datafield = frame.GetData();
  
  command = CiA301::SDO::Command( datafield[0] );
  object.index   = CiA301::Object::Index( ( datafield[2] << 8 ) | 
					  ( datafield[1]      ) );
  object.subindex= CiA301::Object::SubIndex( datafield[3] );
  
  object.data = ( ( datafield[7] << 24 ) |
		  ( datafield[6] << 18 ) |
		  ( datafield[5] <<  8 ) |
		  ( datafield[4]       ) );
}

// protocols
devCANopen::Errno devCANopen::SDO( CiA301::Node::ID nodeid,
				   CiA301::SDO::Command command,
				   const CiA301::Object& object ){
  if( Write( Pack( nodeid, command, object ) ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to send SDO message."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}


devCANopen::Errno devCANopen::NMT( CiA301::Node::ID nodeid,
				   CiA301::NMT::State state ){
  if( Write( Pack( nodeid, state ) ) != devCANopen::ESUCCESS ) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to send NMT message."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

   
devCANopen::Errno devCANopen::SYNC(){
  devCAN::Frame::DataField data;
  return Write( devCAN::Frame( CiA301::SYNC::COBID, data, 0 ) );
}

// CiA 301

// RPDO 2
devCANopen::Errno devCANopen::RPDO2( CiA301::Node::ID nodeid,
				     CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO2( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO2 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::RPDO2( CiA301::Node::ID nodeid, 
				     CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO2( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO2 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// RPDO 21
devCANopen::Errno devCANopen::RPDO21( CiA301::Node::ID nodeid,
				      CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO21( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO21 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::RPDO21( CiA301::Node::ID nodeid, 
				      CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO21( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO21 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// RPDO 24
devCANopen::Errno devCANopen::RPDO24( CiA301::Node::ID nodeid,
				      CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO24( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO24 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::RPDO24( CiA301::Node::ID nodeid, 
				      CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::RPDO24( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set RPDO24 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}


// TPDO 1
devCANopen::Errno devCANopen::TPDO1( CiA301::Node::ID nodeid,
				     CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO1( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO1 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::TPDO1( CiA301::Node::ID nodeid, 
				     CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO1( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO1 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// TPDO 24
devCANopen::Errno devCANopen::TPDO24( CiA301::Node::ID nodeid, 
				      CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO24( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO24 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::TPDO24( CiA301::Node::ID nodeid, 
				      CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO24( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO24 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// TPDO 25
devCANopen::Errno devCANopen::TPDO25( CiA301::Node::ID nodeid, 
				      CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO25( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO25 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::TPDO25( CiA301::Node::ID nodeid, 
				      CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO25( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO25 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// TPDO 26
devCANopen::Errno devCANopen::TPDO26( CiA301::Node::ID nodeid, 
				      CiA301::COBID cobid ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO26( cobid ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO26 communication."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::TPDO26( CiA301::Node::ID nodeid, 
				      CiA301::PDO::Transmission transmission ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA301::TPDO26( transmission ))
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO26 transmission."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::TPDO26( CiA301::Node::ID nodeid, 
				      CiA301::TPDO26::Mapping mapping,
				      CiA301::TPDO26::Data value ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA301::TPDO26( mapping, value ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set TPDO26 mapping."
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}




// CiA 402

// Set the control word Control word
devCANopen::Errno devCANopen::SetControlWord( CiA301::Node::ID nodeid, 
					      CiA402::ControlWord::Word word ){
   if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA402::ControlWord( word ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set the control word " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::DisableVoltage( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::DISABLE_VOLTAGE  )
      != devCANopen::ESUCCESS ){ 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to disable voltages for node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::ResetFaults( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::RESET_FAULT ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to reset faults for node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::Shutdown( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::SHUTDOWN )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to shutdown node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::SwitchOn( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::SWITCH_ON ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to switch on node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::EnableOperation( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::ENABLE_OPERATION )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to enable operation for node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::StartHoming( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::START_HOMING )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to start homing node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::EndHoming( CiA301::Node::ID nodeid ){
  if( SetControlWord( nodeid, CiA402::ControlWord::END_HOMING )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to end homing node " << nodeid
 		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// communication state
devCANopen::Errno devCANopen::SetPreOperational( CiA301::Node::ID nodeid ){
  if( NMT( nodeid, CiA301::NMT::PRE_OPERATIONAL ) != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set pre-operational mode of node " <<nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::SetOperational( CiA301::Node::ID nodeid ){
  if( NMT( nodeid, CiA301::NMT::OPERATIONAL ) != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set operational mode of node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}


// Set operation mode
devCANopen::Errno 
devCANopen::SetOperationMode( CiA301::Node::ID nodeid, 
			      CiA402::OperationMode::Mode mode ){
  if( SDO( nodeid, CiA301::SDO::INITIATE_WRITE, CiA402::OperationMode( mode ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set the operation mode " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// Set position operation
devCANopen::Errno devCANopen::PositionMode( CiA301::Node::ID nodeid ){
  if( SetOperationMode( nodeid, CiA402::OperationMode::POSITION ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set position operation of node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

// Set PVT operation
devCANopen::Errno devCANopen::PVTMode( CiA301::Node::ID nodeid ){
  if( SetOperationMode( nodeid, CiA402::OperationMode::PVT ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set PVT operation of node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::HomingMode( CiA301::Node::ID nodeid ){
  if( SetOperationMode( nodeid, CiA402::OperationMode::HOMING ) 
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing mode of node " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::HomeOffset( CiA301::Node::ID nodeid,
					  int offset ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA402::HomeOffset( offset ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing offset " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::HomingZeroSpeed( CiA301::Node::ID nodeid, 
					       unsigned int speed ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA402::HomingSpeed( CiA402::HomingSpeed::ZERO, speed ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing zero speed " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::HomingSwitchSpeed( CiA301::Node::ID nodeid,
						 unsigned int speed ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA402::HomingSpeed( CiA402::HomingSpeed::SWITCH, speed ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing switch speed " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::HomingMethod( CiA301::Node::ID nodeid, 
					    int method ){
   if( SDO( nodeid, 
	    CiA301::SDO::INITIATE_WRITE, 
	    CiA402::HomingMethod( method ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing method " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}


devCANopen::Errno devCANopen::HomingAcceleration ( CiA301::Node::ID nodeid, 
						   int acceleration ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA402::HomingAcceleration( acceleration ) )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set homing acceleration " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}



// PVT
devCANopen::Errno devCANopen::PVTBufferClear( CiA301::Node::ID nodeid ){
  if( SDO( nodeid, 
	   CiA301::SDO::INITIATE_WRITE, 
	   CiA402::PVTClearBuffer() )
      != devCANopen::ESUCCESS ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Failed to set clear PVT buffer " << nodeid
		      << std::endl;
    return devCANopen::EFAILURE;
  }
  return devCANopen::ESUCCESS;
}
