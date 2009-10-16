/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Eric Lin, Joseph Vidalis
  Created on: 2008-09-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief A wrapper for NDI Polaris optical tracker
  \ingroup cisstDevices

  \bug _snprintf in CommandAppend(int) is Windows-dependent

  \todo Missing support for 14400bps, 921600bps and 1228739bps baud rates in osaSerialPort
  \todo Check for buffer overflow in SendCommand()
  \todo Handle error values returning from the device
  \todo Implement a timeout for CheckResponse(), maybe have a CheckTimeout()
  \todo Handle supported features for different Polaris versions.
  \todo Check for OKAY, WARNING, ERROR<code>, etc. in CheckRespone()
  \todo CRC check for CommandSend()
  \todo Pretty print for SerialNumber, to extract date, etc.
  \todo Probe-SN for unknown tools, otherwise use the interface name provided
  \todo Check if correct number of items are scanned using sscanf
  \todo Overload Tool class to have a stream
*/

#ifndef _devNDiSerial_h
#define _devNDiSerial_h

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>


class devNDiSerial : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    class Tool
    {
        public:
            Tool(void);
            ~Tool(void) {};

            std::string Name;
            mtsProvidedInterface * Interface;
            prmPositionCartesianGet Position;

            char PortHandle[3];
            // PHINF 0001
            char MainType[3];
            char ManufacturerID[13];
            char ToolRevision[4];
            char SerialNumber[9];
            // PHINF 0004
            char PartNumber[21];
    };

public:
    devNDiSerial(const std::string & polarisName, const std::string & serialPort);
    ~devNDiSerial(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void) {};
    void Run(void) {};
    void Cleanup(void) {};

protected:
    enum { MAX_BUFFER_SIZE = 512 };
    enum { CRC_SIZE = 4 };

    size_t GetSerialBufferSize(void) const {
        return SerialBufferPointer - SerialBuffer;
    }
    size_t GetSerialBufferAvailableSize(void) const {
        return MAX_BUFFER_SIZE - GetSerialBufferSize();
    }
    size_t GetSerialBufferStringSize(void) const {
        if (*(SerialBufferPointer - 1) == '\0') {
            return GetSerialBufferSize() - 1;
        }
        CMN_LOG_CLASS_RUN_ERROR << "GetSerialBufferStringSize: string is not null terminated" << std::endl;
        return 0;
    }

    void CommandInitialize(void);
    void CommandAppend(const char command);
    void CommandAppend(const char * command);
    void CommandAppend(const unsigned int command);
    bool CommandSend(void);
    bool CommandSend(const char * command) {
        CommandInitialize();
        CommandAppend(command);
        return CommandSend();
    }

    bool ResponseRead(void);
    bool ResponseRead(const char * expectedMessage);
    unsigned int ComputeCRC(const char * data);
    bool ResponseCheckCRC(void);

    bool ResetSerialPort(void);
    bool SetSerialPortSettings(osaSerialPort::BaudRateType baudRate,
                               osaSerialPort::CharacterSizeType characterSize,
                               osaSerialPort::ParityCheckingType parityChecking,
                               osaSerialPort::StopBitsType stopBits,
                               osaSerialPort::FlowControlType flowControl);
    bool Beep(unsigned int numberOfBeeps);

    void PortHandlesInitialize(void);
    void PortHandlesQuery(void);
    void PortHandlesEnable(void);

    osaSerialPort SerialPort;
    char SerialBuffer[MAX_BUFFER_SIZE];
    char * SerialBufferPointer;

    typedef cmnNamedMap<Tool> ToolsMapType;
    ToolsMapType ToolsMap;
    cmnNamedMap<Tool> PortToTool;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDiSerial);

#endif  //_devNDiSerial_h_
