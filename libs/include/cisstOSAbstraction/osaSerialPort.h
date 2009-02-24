/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2004-12-06

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Declaration of osaSerialPort
*/

#ifndef _osaSerialPort_h
#define _osaSerialPort_h


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstOSAbstraction/osaExport.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#else
#include <termios.h>
#endif

#include <sstream>


/*!
  \ingroup cisstOSAbstraction
  \brief Serial port.

  Create a serial port for sending and receiving data.
 */
class CISST_EXPORT osaSerialPort: public cmnGenericObject
{
    /*! Services for osaSerialPort.  This allows to register the class
      and control the level of detail for the log.

      \sa #CMN_DECLARE_SERVICES. */
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    /*! Type used to define the baud rate.
      \sa SetBaudRate */
#if (CISST_OS == CISST_WINDOWS)
    enum BaudRateType {BaudRate9600 = CBR_9600,
                       BaudRate19200 = CBR_19200,
                       BaudRate38400 = CBR_38400,
                       BaudRate57600 = CBR_57600,
                       BaudRate115200 = CBR_115200};
#else
    enum BaudRateType {BaudRate300 = B300,
                       BaudRate9600 = B9600,
                       BaudRate19200 = B19200,
                       BaudRate38400 = B38400,
                       BaudRate57600 = B57600,
                       BaudRate115200 = B115200};
#endif


    /*! Type used to define the character size.
      \sa SetCharacterSize */
#if (CISST_OS == CISST_WINDOWS)
    enum CharacterSizeType {CharacterSize5 = 5,
                            CharacterSize6 = 6,
                            CharacterSize7 = 7,
                            CharacterSize8 = 8};
#else
    enum CharacterSizeType {CharacterSize5 = CS5,
                            CharacterSize6 = CS6,
                            CharacterSize7 = CS7,
                            CharacterSize8 = CS8};
#endif


    /*! Type used to define the parity checking.
      \sa SetParityChecking */
#if (CISST_OS == CISST_WINDOWS)
    enum ParityCheckingType {ParityCheckingNone = NOPARITY,
                             ParityCheckingEven = EVENPARITY,
                             ParityCheckingOdd = ODDPARITY};
#else
    enum ParityCheckingType {ParityCheckingNone,
                             ParityCheckingEven,
                             ParityCheckingOdd};
#endif


    /*! Default constructor.  Set parameters to default 8N1 (8 bits
      per character, no parity checking, 1 stop bit) and 9600 baud
      rate.  This constructor doesn't start anything, use Open() to
      actually start the serial port connection. */
    osaSerialPort(void):
        PortName("Undefined"),
        IsOpenedFlag(false),
        BaudRate(BaudRate9600),
        CharacterSize(CharacterSize8),
        ParityChecking(ParityCheckingNone),
        TwoStopBits(false),
        HardwareFlowControl(false)
    {
#if (CISST_OS == CISST_WINDOWS)
        memset(&OverlappedStructureRead, 0, sizeof(OverlappedStructureRead));
        memset(&OverlappedStructureWrite, 0, sizeof(OverlappedStructureWrite));
#else // Unix
        FileDescriptor = -1;
#endif
    }

  
    /*! Destructor. */
    virtual ~osaSerialPort(void);
    
    /*! Set a different baud rate. */
    inline void SetBaudRate(const BaudRateType & baudRate) {
        BaudRate = baudRate;
        CMN_LOG_CLASS(3) << "Baud rate modified to " << BaudRate
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set a different character size. */
    inline void SetCharacterSize(const CharacterSizeType & characterSize) {
        CharacterSize = characterSize;
        CMN_LOG_CLASS(3) << "Character size modified to " << CharacterSize
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set a different parity checking. */
    inline void SetParityChecking(const ParityCheckingType & parityChecking) {
        ParityChecking = parityChecking;
        CMN_LOG_CLASS(3) << "Parity checking modified to " << ParityChecking
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Use two stop bits.  By default, one stop bit is used. */
    inline void SetTwoStopBits(const bool & twoStopBits) {
        TwoStopBits = twoStopBits;
        CMN_LOG_CLASS(3) << "Use two stop bits modified to " << TwoStopBits
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Activate/de-activate the hardware flow control. */
    inline void SetHardwareFlowControl(const bool & hardwareFlowControl) {
        HardwareFlowControl = hardwareFlowControl;
        CMN_LOG_CLASS(3) << "Hardware flow control modified to " << HardwareFlowControl
                         << " on port " << PortName << " (not effective until Configure)" << std::endl;
    }

    /*! Set the port name.  For a more portable code, use SetPortNumber. */
    inline void SetPortName(const std::string & portName) {
        PortName = portName;
    }

    /*! Get the current port name. */
    inline std::string GetPortName(void) const {
        return PortName;
    }

    /*! Set the serial port name based on a number, starting from 1. */
    std::string SetPortNumber(unsigned int portNumber);

    /*! Open the serial port.  This method starts the serial port
      based on the parameters previously set (either the defaults of
      those defined by SetBaudRate, SetCharacterSize,
      SetParityChecking, SetHardwareFlowControl, ...). */
    bool Open(void);

    bool Configure(void);

    bool Close(void);

    /*! Indicates if the port has been opened or closed correctly. */
    inline bool IsOpened(void) const {
        return IsOpenedFlag;
    }

    /*! Send raw data. */
    int Write(const char * data, int nBytes);
    int Write(const unsigned char * data, int nBytes);
    inline int Write(const std::string & data) {
        return this->Write(data.c_str(), data.size());
    }
    
    /*! Receive raw data, non blocking. */
    int Read(char * data, int nBytes);
    int Read(unsigned char * data, int nBytes);
  
    /*! Sends a serial break for a given duration in seconds.
      
     * On Linux, if the break duration is set to 0, the actual
       duration will be at least 0.25 seconds and at most 0.5 seconds.

     * On Mac OS, the implementation doesn't use the
       breakLengthInSeconds. To provide a similar runtime, osaSleep is
       used internaly.  As the default duration on MacOS it 0.4
       seconds, the actual sleep time is breakLengthInSeconds - 0.4.

     * On Windows, this method is implemented using SetCommBreak,
       osaSleep and ClearCommBreak.

     */

    bool WriteBreak(double breakLengthInSeconds);
  
    /*! Flush. */
    bool Flush(void);

private:
    // full port name
    std::string PortName;
    bool IsOpenedFlag;

    // parameters
    BaudRateType BaudRate;
    CharacterSizeType CharacterSize;
    ParityCheckingType ParityChecking;
    bool TwoStopBits;
    bool HardwareFlowControl;
    
#if (CISST_OS == CISST_WINDOWS)
    HANDLE PortHandle;
    OVERLAPPED OverlappedStructureRead, OverlappedStructureWrite;
    COMMTIMEOUTS TimeOuts;
#else // Unix
    int FileDescriptor;
#endif  
};


CMN_DECLARE_SERVICES_INSTANTIATION(osaSerialPort);


#endif // _osaSerialPort_h

