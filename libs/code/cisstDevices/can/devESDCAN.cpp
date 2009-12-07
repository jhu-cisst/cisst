
#ifdef ESDCAN

#include "esdcan.hpp"
#include <iomanip>


openman::ESDCAN::ESDCAN(uint32_t baudrate) : openman::CANbus(baudrate){}

openman::Retval openman::ESDCAN::open(){
  mutex.lock();
  if(canOpen(0,              // net = 0
	     0,              // flags = 0
	     TXQUEUESIZE,    // queue size
	     RXQUEUESIZE, 
	     TXTIMEOUT,      // timeout
	     RXTIMEOUT,
	     &handle) != NTCAN_SUCCESS){
    std::cerr << "ESDCAN::open: canOpen failed." << std::endl;
    mutex.unlock();
    return openman::FAILURE;
  }

  mutex.unlock();
  return openman::SUCCESS;
}

openman::Retval openman::ESDCAN::close(){
  mutex.lock();
  if( canClose(handle) != NTCAN_SUCCESS ){
    std::cerr << "ESDCAN::close: canOpen failed." << std::endl;
    mutex.unlock();
    return openman::FAILURE;
  }
  mutex.unlock();
  return openman::SUCCESS;
}

openman::Retval openman::ESDCAN::send(void* data, size_t nbytes, bool block){

  openman::CANFrame* canframe = (CANFrame*)data;
  uint8_t* framedata = canframe->data();
  
  CMSG cmsg;
  long len=1;
  int err;

  mutex.lock();

  cmsg.id = (long)canframe->id();
  cmsg.len = (unsigned char)(canframe->length() & 0x0F);
  for(uint8_t i=0; i<canframe->length(); i++)  
    cmsg.data[i] = (unsigned char)framedata[i];

  if(block){
    if( (err=canWrite(handle, &cmsg, &len, NULL)) != NTCAN_SUCCESS){
      std::cerr<< "ESDCAN::send: canWrite failed: "<< std::hex<< err<< std::endl;
      mutex.unlock();
      return openman::FAILURE;
    }
  }
  else{
    if( (err=canSend(handle, &cmsg, &len)) != NTCAN_SUCCESS){
      std::cerr<< "ESDCAN::send: canSend failed: "<< std::hex<< err<< std::endl;
      mutex.unlock();
      return openman::FAILURE;
    }
  }

  mutex.unlock();
  return openman::SUCCESS;
}

openman::Retval openman::ESDCAN::recv(void* data, size_t& nbytes, bool block){

  openman::CANFrame* canframe = (openman::CANFrame*)data;

  CMSG cmsg;
  long len = 1;
  int err;
   
  mutex.lock();

  if(block){
    if((err=canRead(handle, &cmsg, &len, NULL)) != NTCAN_SUCCESS){
      std::cerr<< "ESDCAN::recv: canRead failed: "<< std::hex<< err<< std::endl;
      mutex.unlock();
      return openman::FAILURE;
    }
  }
  else{
    if((err=canTake(handle, &cmsg, &len)) != NTCAN_SUCCESS){
      std::cerr<< "ESDCAN::recv: canTake failed: "<< std::hex<< err<< std::endl;
      mutex.unlock();
      return openman::FAILURE;
    }
  }

  if( 1 < len ){
    mutex.unlock();
    return openman::FAILURE;
  }

  *canframe = openman::CANFrame( (CANID)cmsg.id, 
				 (uint8_t*)cmsg.data,
				 (uint8_t)cmsg.len );

  nbytes = sizeof(CANFrame);

  return openman::SUCCESS;
}

#endif
