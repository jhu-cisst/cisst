
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
     ESUCCESS = 0x00,
     EFAILURE = 0x01
   };

 private:
   
   devCAN* candevice;
   bool deviceopened;
   
   devCAN::Frame Pack( CiA301::COBID cobid, const CiA301::Object& object );
   void Unpack( const devCAN::Frame& frame, 
		CiA301::COBID& cobid,
		CiA301::Object& object );
   
 public:
   
   devCANopen( devCAN* can );
   ~devCANopen();
   
   devCANopen::Errno Open(void);
   devCANopen::Errno Close(void);

   /*! \brief Read CAN message from CAN driver */
   devCANopen::Errno Read( CiA301::COBID& cobid,       CiA301::Object& object );

   /*! \brief Write CAN frame. */
   devCANopen::Errno Write( CiA301::COBID cobid, const CiA301::Object& object );

   bool IsOpen(void) const { return deviceopened; }
   
};

#endif


