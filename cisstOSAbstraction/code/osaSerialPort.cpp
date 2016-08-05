/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet, Min Yang Jung
  Created on: 2004-12-10

  (C) Copyright 2004-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstOSAbstraction/osaSleep.h>

#if (CISST_OS == CISST_WINDOWS)

#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#endif

/* MJUNG: For detailed information about serial/parallel devices on QNX, please refer
   to the following link:

   Connecting Character Devices,
   http://www.qnx.com/developers/docs/qnx_4.25_docs/qnx4/user_guide/chardev.html
*/



std::string osaSerialPort::SetPortNumber(unsigned int portNumber) {
    std::stringstream portName;
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_LINUX_XENOMAI)
    portName << "/dev/ttyS" << (portNumber - 1);
#elif (CISST_OS == CISST_WINDOWS)
    if (portNumber < 10)
        portName << "COM" << portNumber;
    else
        portName << "\\\\.\\COM" << portNumber;
#elif (CISST_OS == CISST_QNX)
    portName << "/dev/ser" << portNumber;
#endif
    PortName = portName.str();
    CMN_LOG_CLASS_INIT_VERBOSE << "Port name set to " << PortName
                               << " based on port number " << portNumber << std::endl;
    return PortName;
}




#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Open(bool blocking) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Start Open for port " << PortName << std::endl;
    // check that the port is not already opened
    if (IsOpenedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "Can not re-open an opened port " << PortName << std::endl;
        return false;
    }
    isBlocking = blocking;
    // create the port handle
    PortHandle = CreateFile(PortName.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0, // do not share access
                            0, // handle cannot be inherited
                            OPEN_EXISTING,
                            isBlocking?NULL:FILE_FLAG_OVERLAPPED,
                            NULL // always for serial port
                            );
    if (PortHandle == INVALID_HANDLE_VALUE) {
        CMN_LOG_CLASS_INIT_ERROR << "Error opening port (" << PortName << ")" << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "Correct port handle for " << PortName << std::endl;
    }

    // create the overlapped events, remember to close them
    if (!isBlocking) {
        OverlappedStructureRead.hEvent = CreateEvent(NULL, true, false, NULL);
        if (OverlappedStructureRead.hEvent == NULL) {
            CMN_LOG_CLASS_INIT_ERROR << "Error creating overlapped read event handle for " << PortName << std::endl;
             return false;
        } else {
            CMN_LOG_CLASS_INIT_VERBOSE << "Correct overlapped read event handle for " << PortName << std::endl;
        }
        OverlappedStructureWrite.hEvent = CreateEvent(NULL, true, false, NULL);
        if (OverlappedStructureWrite.hEvent == NULL) {
            CMN_LOG_CLASS_INIT_ERROR << "Error creating overlapped write event handle for " << PortName << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_VERBOSE << "Correct overlapped write event handle for " << PortName << std::endl;
        }
    }
    // configure using the current parameters (baud rate, etc.)
    IsOpenedFlag = true;
    IsOpenedFlag = Configure();
    return IsOpenedFlag;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool osaSerialPort::Open(bool) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Start Open for port " << this->PortName << std::endl;
    // check that the port is not already opened
    if (this->IsOpenedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "Can not re-open an opened port " << this->PortName << std::endl;
        return false;
    }
    // create the file descriptor
    this->FileDescriptor = open(this->PortName.c_str(), O_RDWR);
    if (this->FileDescriptor < 0) {
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Error opening port (" << this->PortName << ")" << std::endl;
        return false;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "Correct file descriptor for port " << this->PortName << std::endl;
    }
    // configure using the current parameters (baud rate, etc.)
    this->IsOpenedFlag = true;
    if (!this->Configure()) {
        close(this->FileDescriptor);
        this->IsOpenedFlag = false;
    }
    return this->IsOpenedFlag;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Close(void)
{
    if (IsOpenedFlag) {
        if (!isBlocking) {
            // close event to avoid leak
            if (OverlappedStructureRead.hEvent != NULL) {
                CloseHandle(OverlappedStructureRead.hEvent);
            }
            if (OverlappedStructureWrite.hEvent != NULL) {
                CloseHandle(OverlappedStructureWrite.hEvent);
            }
        }
        // close port handle
        CloseHandle(PortHandle);
        IsOpenedFlag = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "Port " << PortName << " sucessfully closed." << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Attempt to close an already close port " << PortName << std::endl;
        return false;
    }
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool osaSerialPort::Close(void)
{
    if (this->IsOpenedFlag) {
        close(this->FileDescriptor);
        this->IsOpenedFlag = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "Port " << this->PortName << " sucessfully closed." << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Attempt to close an already close port " << this->PortName << std::endl;
        return false;
    }
    return true;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Configure(void) {

    CMN_LOG_CLASS_INIT_VERBOSE << "Start Configure for port " << PortName << std::endl;


    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Can not Configure a closed port " << PortName << std::endl;
        return false;
    }

    DCB portOptions = {0};
    if (!GetCommState(PortHandle, &portOptions)) {
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Unable to retrieve current settings for " << PortName << std::endl;
        return false;
    } else {
        // set baud rate
        portOptions.BaudRate = this->BaudRate;

        // set character size
        portOptions.ByteSize = this->CharacterSize;

        // set parity checking
        switch (this->ParityChecking) {
        case ParityCheckingNone:
            portOptions.fParity = false;
            break;
        case ParityCheckingOdd:
            portOptions.fParity = true;
            portOptions.Parity = ODDPARITY;
            break;
        case ParityCheckingEven:
            portOptions.fParity = true;
            portOptions.Parity = EVENPARITY;
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Fatal error on port " << this->PortName << std::endl;
        }

        // set stop bit to 1 or 2
        switch (this->StopBits) {
        case StopBitsTwo:
            portOptions.StopBits = TWOSTOPBITS; // 2 stop bits
            break;
        case StopBitsOne:
            portOptions.StopBits = ONESTOPBIT; // 1 stop bit
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Fatal error on port " << this->PortName << std::endl;
        }

        // try to apply these settings
        if (!SetCommState(PortHandle, &portOptions)) {
            CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Unable to apply current settings for " << PortName << std::endl;
            return false;
        }

        if (isBlocking) {
            // some reasonable values for non-overlapped (blocking) I/O, values in milliseconds
            // Allow 20 msec between consecutive characters
            TimeOuts.ReadIntervalTimeout = 20;
            // Read timeout = 2*NBytes + 10
            TimeOuts.ReadTotalTimeoutMultiplier = 2;
            TimeOuts.ReadTotalTimeoutConstant = 10;
            // Write timeout = 2*NBytes + 10
            TimeOuts.WriteTotalTimeoutMultiplier = 2;
            TimeOuts.WriteTotalTimeoutConstant = 10;
        }
        else {
            // set up for overlapped (non-blocking) I/O
            // Read operation returns immediately
            TimeOuts.ReadIntervalTimeout = MAXDWORD ;
            TimeOuts.ReadTotalTimeoutMultiplier = 0;
            TimeOuts.ReadTotalTimeoutConstant = 0;
            // Total timeouts not used for writes
            TimeOuts.WriteTotalTimeoutMultiplier = 0;
            TimeOuts.WriteTotalTimeoutConstant = 0;
        }
        SetCommTimeouts(PortHandle, &TimeOuts);

        // get any early notifications
        //SetCommMask(PortHandle, EV_RXCHAR);

        // set comm buffer sizes (input buffer = 2048, output buffer = 1024)
        SetupComm(PortHandle, 2048, 1024);

        // purge
        PurgeComm(PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "End of Configure for port " << PortName << std::endl;
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool osaSerialPort::Configure(void) {

    CMN_LOG_CLASS_INIT_VERBOSE << "Start Configure for port " << this->PortName << std::endl;

    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Can not Configure a closed port " << this->PortName << std::endl;
        return false;
    }

    // read current termio settings
    struct termios portOptions;
    tcgetattr(this->FileDescriptor, &portOptions);

    // set input and output speed
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_LINUX_XENOMAI)
    cfsetspeed(&portOptions, this->BaudRate); // might be BSD only, if doesn't compile/work on Linux use CISST_DARWIN
#endif
    cfsetispeed(&portOptions, this->BaudRate);
    cfsetospeed(&portOptions, this->BaudRate);

    // set character size
    portOptions.c_cflag &= ~CSIZE;
    portOptions.c_cflag |= this->CharacterSize; // 5, 6, 7 or 8 data bits

    // set parity checking
    switch (this->ParityChecking) {
    case ParityCheckingNone:
        portOptions.c_cflag &= ~(PARENB); // disable parity
        portOptions.c_iflag &= ~(INPCK); // disable input parity check
        break;
    case ParityCheckingOdd:
        portOptions.c_cflag |= PARODD;
        portOptions.c_cflag |= PARENB;
        portOptions.c_iflag |= INPCK; // enable input parity check
        break;
    case ParityCheckingEven:
        portOptions.c_cflag |= PARENB;
        portOptions.c_cflag &= ~PARODD;
        portOptions.c_iflag |= INPCK; // enable input parity check
        break;
    default:
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Fatal error on port " << this->PortName << std::endl;
    }

    // set stop bit to 1 or 2
    switch (this->StopBits) {
    case StopBitsTwo:
        portOptions.c_cflag |= CSTOPB; // 2 stop bits
        break;
    case StopBitsOne:
        portOptions.c_cflag &= ~CSTOPB; // 1 stop bit
        break;
    default:
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Fatal error on port " << this->PortName << std::endl;
    }

    // set hardware flow control
    /* MJUNG: QNX doesn't recognize CRTSCTS.  Instead, setting both IHFLOW and OHFLOW has
       the same effect as CRTSCTS set.  Refer the following references for details.

       termios (terminal control structure) :
           http://www.qnx.com/developers/docs/6.3.0SP3/neutrino/lib_ref/t/termios.html
       Serial Port - hardware flow Control :
           http://www.openqnx.com/PNphpBB2-viewtopic-t1750-.html
    */
    switch (this->FlowControl) {
    case FlowControlNone:
        portOptions.c_iflag &= ~(IXOFF | IXON);
#if (CISST_OS == CISST_QNX)
        portOptions.c_cflag &= ~IHFLOW;
        portOptions.c_cflag &= ~OHFLOW;
#else
        portOptions.c_cflag &= ~CRTSCTS;
#endif
        break;
    case FlowControlHardware:
        portOptions.c_iflag &= ~(IXOFF | IXON);
#if (CISST_OS == CISST_QNX)
        portOptions.c_cflag |= IHFLOW;
        portOptions.c_cflag |= OHFLOW;
#else
        portOptions.c_cflag |= CRTSCTS;
#endif
        break;
    case FlowControlSoftware:
#if (CISST_OS == CISST_QNX)
        portOptions.c_cflag &= ~IHFLOW;
        portOptions.c_cflag &= ~OHFLOW;
#else
        portOptions.c_cflag &= ~CRTSCTS;
#endif
        portOptions.c_iflag |= (IXON | IXOFF);
        break;
    default:
        CMN_LOG_CLASS_INIT_ERROR << CMN_LOG_DETAILS << "Fatal error on port " << this->PortName << std::endl;
    }

    // defaults we don't modify
    portOptions.c_cflag |= CLOCAL; // modem control off
    portOptions.c_cflag |= CREAD; // enable reads from port
    portOptions.c_lflag &= ~ICANON; // Turn off line by line mode
    portOptions.c_lflag &= ~ECHO; // no echo of TX characters
    portOptions.c_lflag &= ~ISIG; // no input ctrl char checking
    portOptions.c_iflag &= ~ICRNL; // do not map CR to NL on in
    portOptions.c_oflag &= ~OCRNL; // do not map CR to NL on out
    portOptions.c_oflag &= ~OPOST; // no output ctrl char checking
    portOptions.c_iflag &= ~IGNCR; // allow CR characters
    // set minimum number of characters read() returns on
    // set read delay, time = VTIME x 100 mS
    portOptions.c_cc[VMIN] = 0; // read can return with no chars
    portOptions.c_cc[VTIME] = 1; // read waits this much for chars

    // apply changes
    tcsetattr(this->FileDescriptor, TCSADRAIN, &portOptions);
    CMN_LOG_CLASS_INIT_VERBOSE << "End of Configure for port " << this->PortName << std::endl;
    return true;
}
#endif






osaSerialPort::~osaSerialPort(void)
{
    if (this->IsOpenedFlag) {
        this->Close();
    }
}






#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Write(const char * data, int nBytes)
{
    DWORD numBytes = 0;
    bool sent = false; // by default, assume it will fail

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    if (isBlocking) {
        if (!WriteFile(PortHandle, data, nBytes, &numBytes, NULL)) {
            // an error occurred
            CMN_LOG_CLASS_RUN_ERROR << "Write error = " << GetLastError() << std::endl;
            ClearCommError(PortHandle, NULL, NULL);
        }
        return numBytes;
    }

    // Issue write.
    if (!WriteFile(PortHandle, data, nBytes, &numBytes, &OverlappedStructureWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            CMN_LOG_CLASS_RUN_ERROR << "WriteFile failed (not delayed) for " << PortName << std::endl;
        } else {
            CMN_LOG_CLASS_RUN_VERBOSE << "Write is pending for " << PortName << std::endl;
            if (!GetOverlappedResult(PortHandle, &OverlappedStructureWrite, &numBytes, TRUE)) {
                CMN_LOG_CLASS_RUN_ERROR << "GetOverlappedResult failed for Write on port " << PortName << std::endl;
            } else {
                CMN_LOG_CLASS_RUN_VERBOSE << "Write operation completed successfully for " << PortName << std::endl;
                sent = true;
            }
        }
    } else {
        CMN_LOG_CLASS_RUN_VERBOSE << "Immediate write operation completed successfully for " << PortName << std::endl;
        sent = true;
    }

    return sent?numBytes:0;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
int osaSerialPort::Write(const char * data, int nBytes)
{
    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Write on a closed port " << this->PortName << std::endl;
        return 0;
    }

    int numBytes = write(this->FileDescriptor, data, nBytes);
    CMN_LOG_CLASS_RUN_DEBUG << "Wrote " << data << std::endl
                            << "(" << nBytes << " bytes) on port " << this->PortName << std::endl;
    return numBytes;
}
#endif



#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Write(const unsigned char * data, int nBytes)
{
    DWORD numBytes = 0;
    bool sent = false; // by default, assume it will fail

    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Write on a closed port " << this->PortName << std::endl;
        return 0;
    }

    if (isBlocking) {
        if (!WriteFile(PortHandle, data, nBytes, &numBytes, NULL)) {
            // an error occurred
            CMN_LOG_CLASS_RUN_ERROR << "Write error = " << GetLastError() << std::endl;
            ClearCommError(PortHandle, NULL, NULL);
        }
        return numBytes;
    }

    // Issue write.
    if (!WriteFile(PortHandle, data, nBytes, &numBytes, &OverlappedStructureWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            CMN_LOG_CLASS_RUN_WARNING << "WriteFile failed (not delayed) for " << PortName << std::endl;
        } else {
            CMN_LOG_CLASS_RUN_VERBOSE << "Write is pending for " << PortName << std::endl;
            if (!GetOverlappedResult(PortHandle, &OverlappedStructureWrite, &numBytes, TRUE)) {
                CMN_LOG_CLASS_RUN_WARNING << "GetOverlappedResult failed for Write on port " << PortName << std::endl;
            } else {
                CMN_LOG_CLASS_RUN_VERBOSE << "Write operation completed successfully for " << PortName << std::endl;
                sent = true;
            }
        }
    } else {
        CMN_LOG_CLASS_RUN_VERBOSE << "Immediate write operation completed successfully for " << PortName << std::endl;
        sent = true;
    }

    return sent?numBytes:0;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
int osaSerialPort::Write(const unsigned char * data, int nBytes)
{
    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Write on a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = write(FileDescriptor, data, nBytes);
    CMN_LOG_CLASS_RUN_DEBUG << "Wrote " << data << std::endl
                            << "(" << nBytes << " bytes) on port " << PortName << std::endl;
    return numBytes;
}
#endif






#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Read(char * data, int nBytes)
{
    BOOL fReadStat;
    COMSTAT ComStat;
    DWORD dwErrorFlags, dwLength, dwError;

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    if (isBlocking) {
        if (!ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead)) {
            // an error occurred
            CMN_LOG_CLASS_RUN_ERROR << "Read error = " << GetLastError() << std::endl;
            dwLength = -1;
            ClearCommError(PortHandle, NULL, NULL);
        }
        return dwLength;
    }

    // only try to read number of bytes in queue
    ClearCommError(PortHandle, &dwErrorFlags, &ComStat);

    dwLength = std::min((DWORD) nBytes, ComStat.cbInQue);

    if (dwLength > 0) {
        fReadStat = ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead);
        if (!fReadStat) {
            if (GetLastError() == ERROR_IO_PENDING) {
                CMN_LOG_CLASS_RUN_WARNING << "IO Pending in Read" << std::endl;
                // We have to wait for read to complete.
                // Read is right now set up to return nothing if the port
                // is empty, the while loop is ineffective
                while (!GetOverlappedResult(PortHandle, &OverlappedStructureRead, &dwLength, true)) {
                    dwError = GetLastError();
                    if (dwError == ERROR_IO_INCOMPLETE) {
                        continue;
                    } else {
                        // an error occurred, try to recover
                        ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
                        CMN_LOG_CLASS_RUN_ERROR << "I/O error occured in read" << std::endl;
                        break;
                    } // else
                } // while
            } else {
                // some other error occurred
                dwLength = -1;
                ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
            } // else
        } // if freadstat
    } // if dwlength
    return dwLength;
}
#endif


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
int osaSerialPort::Read(char * data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }
    int numBytes = read(FileDescriptor, data, nBytes); // get chars if there
    return numBytes;
}
#endif


#if (CISST_OS == CISST_WINDOWS)
int osaSerialPort::Read(unsigned char * data, int nBytes)
{
    BOOL fReadStat;
    COMSTAT ComStat;
    DWORD dwErrorFlags, dwLength, dwError;

    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    if (isBlocking) {
        if (!ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead)) {
            // an error occurred
            CMN_LOG_CLASS_RUN_ERROR << "Read error = " << GetLastError() << std::endl;
            dwLength = -1;
            ClearCommError(PortHandle, NULL, NULL);
        }
        return dwLength;
    }

    // only try to read number of bytes in queue
    ClearCommError(PortHandle, &dwErrorFlags, &ComStat);

    dwLength = std::min((DWORD) nBytes, ComStat.cbInQue);

    if (dwLength > 0) {
        fReadStat = ReadFile(PortHandle, data, nBytes, &dwLength, &OverlappedStructureRead);
        if (!fReadStat) {
            if (GetLastError() == ERROR_IO_PENDING) {
                CMN_LOG_CLASS_RUN_WARNING << "IO Pending in Read" << std::endl;
                // We have to wait for read to complete.
                // Read is right now set up to return nothing if the port
                // is empty, the while loop is ineffective
                while (!GetOverlappedResult(PortHandle, &OverlappedStructureRead, &dwLength, true)) {
                    dwError = GetLastError();
                    if (dwError == ERROR_IO_INCOMPLETE) {
                        continue;
                    } else {
                        // an error occurred, try to recover
                        ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
                        CMN_LOG_CLASS_RUN_ERROR << "I/O error occured in read" << std::endl;
                        break;
                    } // else
                } // while
            } else {
                // some other error occurred
                dwLength = -1;
                ClearCommError(PortHandle, &dwErrorFlags, &ComStat);
            } // else
        } // if freadstat
    } // if dwlength
    return dwLength;
}
#endif


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
int osaSerialPort::Read(unsigned char * data, int nBytes)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Can not Read from a closed port " << PortName << std::endl;
        return 0;
    }

    int numBytes = read(FileDescriptor, data, nBytes); // get chars if there
    return numBytes;
}
#endif



#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::WriteBreak(double breakLengthInSeconds)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "WriteBreak: can not WriteBreak on a closed port " << PortName << std::endl;
        return false;
    }
    if (!SetCommBreak(PortHandle)) {
        CMN_LOG_CLASS_RUN_ERROR << "WriteBreak: unable to set break on port  " << PortName << std::endl;
        return false;
    }
    osaSleep(breakLengthInSeconds);
    if (!ClearCommBreak(PortHandle)) {
        CMN_LOG_CLASS_RUN_ERROR << "WriteBreak: unable to clear break on port  " << PortName << std::endl;
        return false;
    }
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_LINUX_XENOMAI) || (CISST_OS == CISST_QNX)
bool osaSerialPort::WriteBreak(double breakLengthInSeconds)
{
#if (CISST_OS == LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    breakLengthInSeconds *= 1000.0; // tcsendbreak on Linux uses milliseconds.
#endif // CISST_LINUX || CISST_LINUX_RTAI

    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "WriteBreak: can not WriteBreak on a closed port " << this->PortName << std::endl;
        return false;
    }
    if (tcsendbreak(this->FileDescriptor, static_cast<int>(breakLengthInSeconds)) < 0) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "WriteBreak: serial break failed" << std::endl;
        return false;
    }

    // man tcsendbreak on MacOS 10.5 claims that break duration is
    // ignored so we sleep to simulate a delay.  Actual break duration
    // is 0.4 seconds according to man page so we deduct 0.4 seconds
    // to find sleep time.
#if (CISST_OS == CISST_DARWIN)
    if (breakLengthInSeconds > 0.4) {
        osaSleep(breakLengthInSeconds - 0.4);
    }
#endif // CISST_DARWIN

    CMN_LOG_CLASS_RUN_VERBOSE << "WriteBreak: end of method " << this->PortName << std::endl;
    return true;
}
#endif





#if (CISST_OS == CISST_WINDOWS)
bool osaSerialPort::Flush(void)
{
    // check that the port is opened
    if (!IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Flush: can not Flush a closed port " << PortName << std::endl;
        return false;
    }
    if (!FlushFileBuffers(PortHandle)) {
        CMN_LOG_CLASS_RUN_ERROR << "Flush: unable to flush port " << PortName << std::endl;
        return false;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "Flush: end of method " << PortName << std::endl;
    return true;
}
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool osaSerialPort::Flush(void)
{
    // check that the port is opened
    if (!this->IsOpenedFlag) {
        CMN_LOG_CLASS_RUN_ERROR << CMN_LOG_DETAILS << "Flush: can not Flush a closed port " << this->PortName << std::endl;
        return false;
    }
    tcflush(this->FileDescriptor, TCIOFLUSH);
    return true;
}
#endif

