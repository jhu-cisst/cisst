
#ifndef _cisstCANFrame_h
#define _cisstCANFrame_h

#include <cisstVector.h>
#include <cisstCAN/cisstCANExport.h>

//! CAN frame implementation
/**
   This defines a basic CAN frame. It does not support the extended frame 
   format. The frame is composed of an identifier, 8 bytes of data and a data 
   length. A CAN frame contain an 11 bits identifier, 0-8 bytes of data and a 
   data length code (0-8).
   This class does not support the extended frame format.
*/
class CISST_EXPORT cisstCANFrame {
  
 public:
  
  //! The id of a CAN frame
  /**
     A CAN ID has 11 bits so we use 16 bits to represent a CAN id of which 
     only the 11 LSB are used.
  */
  typedef unsigned short ID;
  
  typedef unsigned short Mask;
  
  //! The data type
  typedef unsigned char Data;
  
  //! The field of data
  typedef Data DataField[8];
  
  //! The data length type
  typedef unsigned char DataLength;
  
 protected:
  
  //! The ID of the node origin
  ID id;
  
  //! The message (8 bytes)
  DataField data;
  
  //! The lenght of the message in bytes
  DataLength nbytes;
  
 public:
  
  
  //! Default constructor
  /**
     Set the IDs to "0" the message length to "0" and all the message bytes to
     "0"
  */
  cisstCANFrame();
  
  //! Overloaded constructor
  /**
     Set the id of the CAN frame and the message
  */
  cisstCANFrame( ID canid, DataField data, DataLength nbytes );
  
  cisstCANFrame( ID canid, const vctDynamicVector<Data>& data );
  
  //! Return the identifier of the frame
  ID GetID() const { return id; }
  
  //! Return the length in bytes of the data
  DataLength GetLength() const { return nbytes; }
  
  //! Return a pointer to the data
  Data* GetData() { return  &(data[0]); }
  
  //! Return a const pointer to the data
  const Data* GetData() const { return  &(data[0]); }
  
  //! Output the can frame
  /**
     Prints out the frame's identifier, data length and data
     \param os[in] An ouptput stream
     \param cf[in] A CAN frame
  */
  friend std::ostream& operator<<(std::ostream& os, const cisstCANFrame& frame){
    os <<"ID: 0x" 
       << std::hex << std::setfill('0') << std::setw(4) 
       << (int)frame.GetID() << std::endl
       << "Length: " << (int)frame.GetLength() << std::endl 
       << "Data: ";
    for( DataLength i=0; i<frame.GetLength(); i++ )
      os << "0x" << std::hex << std::setfill('0') << std::setw(2) 
	 << (int)(frame.data[i]) << " ";
    os << std::dec;
    return os;
  }
  
}; // Frame

#endif
