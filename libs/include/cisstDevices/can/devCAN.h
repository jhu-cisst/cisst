/*

  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devCAN_h
#define _devCAN_h

#include <iostream>
#include <iomanip>

//! The id of a CAN frame
/**
   A CAN ID has 11 bits so we use 16 bits to represent a CAN id of which only
   the 11 LSB are used.
*/
typedef unsigned short devCANID;

//! A base frame of the CAN bus
/**
   This defines a basic CAN frame. It does not support the extended frame format
   The frame is composed of an identifier, 8 bytes of data and a data length. A
   CAN frame contain an 11 bits identifier, 0-8 bytes of data and a data length
   code (0-8).
   This class does not support the extended frame format.
*/
class devCANFrame{
  
protected:

  //! The ID of the node origin
  devCANID         canid;
  //! The lenght of the message in bytes
  size_t          nbytes;
  //! The message (8 bytes)
  unsigned char bytes[8];
  
public:

  //! Default constructor
  /**
     Set the IDs to "0" the message length to "0" and all the message bytes to 
     "0"
  */
  devCANFrame();

  //! Overloaded constructor
  /**
     Set the id of the CAN frame and the message
  */
  devCANFrame( devCANID canid, unsigned char bytes[8], size_t nbytes );
  
  //! Return the identifier of the frame
  devCANID               Id() const { return canid; }
  
  //! Return the length in bytes of the data
  size_t             Length() const { return nbytes; }

  //! Return a pointer to the data
  unsigned char*       Data()       { return bytes;  }

  //! Return a const pointer to the data
  const unsigned char* Data() const { return bytes;  }

  //! Output the can frame
  /**
     Prints out the frame's identifier, data length and data
     \param os[in] An ouptput stream
     \param cf[in] A CAN frame
  */
  friend std::ostream& operator<<( std::ostream& os, const devCANFrame& cf ){
    os << "ID: 0x" 
       << std::hex << std::setfill('0') << std::setw(4) << cf.canid << std::endl
       << "Length: " << cf.nbytes << std::endl 
       << "Data: ";
    for(size_t i=0; i<cf.nbytes; i++)
      os << "0x" << std::hex << std::setfill('0') << std::setw(2) 
	 << (int)cf.bytes[i] << " ";
    os << std::dec;
    return os;
  }
  
};

//! The different CAN rates 
enum devCANRate{ devCAN_150 =150000, 
		 devCAN_300 =300000, 
		 devCAN_1000=1000000 };
  
//! Generic CAN bus
/**
   The only thing this class does is to define the interface that must be
   implemented by a CAN bus. Any implementation of a CAN device depends on an 
   actual device driver.
*/
class devCAN {

protected:

  //! The rate of the device
  devCANRate rate;

public:

  //! Default constructor
  /**
     Initialize a CAN device with the given rate
     \param rate The CAN rate
  */
  devCAN( devCANRate rate );

  //! Default destructor
  virtual ~devCAN();

  //! Open the CAN device
  virtual bool Open() = 0;

  //! Close the CAN device
  virtual bool Close() = 0;

  //! Send a CAN frame on the bus
  /**
     Call this method to send a CAN frame on the bus. The method can be blocking
     \param frame[in] The CAN frame to send on the bus
     \param block Block the device until the operation is completed
  */
  virtual bool Send( const devCANFrame& frame, bool block=false ) = 0;

  //! Receive a CAN frame
  /**
     Call this method to receive a CAN frame. The method can be blocking.
     \param frame[out] The CAN frame received from the bus
     \param block Block the device until a CAN frame is received
  */
  virtual bool Recv( devCANFrame& frame, bool block=false ) = 0;
  
};

#endif // _devCAN_hpp
