
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

devCANopen::Errno devCANopen::Read( CiA301::COBID& cobid,
				    CiA301::Object& object ){
   
   
  if( candevice == NULL ){
    CMN_LOG_INIT_ERROR << "Failed to read CAN frame." << std::endl;
    return devCANopen::EFAILURE;
  }

  devCAN::Frame frame;
  if( candevice->Recv( frame ) != devCAN::ESUCCESS ){
    CMN_LOG_INIT_ERROR << "Failed to read CAN frame." << std::endl;
    return devCANopen::EFAILURE;
  }

  Unpack( frame, cobid, object );
   
  return devCANopen::ESUCCESS;
}

devCANopen::Errno devCANopen::Write( CiA301::COBID cobid,
				     const CiA301::Object& object ){
   
   if( candevice == NULL ){
      CMN_LOG_INIT_ERROR << "Failed to write CAN frame." << std::endl;
     return devCANopen::EFAILURE;
   }

   if( candevice->Send( Pack( cobid, object ) ) != devCAN::ESUCCESS ){
     CMN_LOG_INIT_ERROR << "Failed to write CAN frame." << std::endl;
     return devCANopen::EFAILURE;
   }

   return devCANopen::ESUCCESS;
}

// Pack
// 
devCAN::Frame devCANopen::Pack( CiA301::COBID cobid,
				const CiA301::Object& object )
{ return devCAN::Frame( cobid, object.data ); }

// Unpack
// 
void devCANopen::Unpack( const devCAN::Frame& frame,
			 CiA301::COBID& cobid,
			 CiA301::Object& object ){

   const devCAN::Frame::Data* src = frame.GetData();
   CiA301::Object::DataField dest( frame.GetLength() );
  
  for( size_t i=0; i<frame.GetLength(); i++ )
     { dest[i] = src[i]; }
   
  cobid = frame.GetID();
  object = CiA301::Object( dest );
	
}

