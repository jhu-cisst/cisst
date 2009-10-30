/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief A wrapper for NDI trackers with serial interface
  \ingroup cisstDevices

  \bug _snprintf in CommandAppend(int) is Windows-dependent
  \bug LoadToolDefinitionFile() requires input definition to be a multiple of 128 characters (via 0 padding)

  \todo Comment the header file using Doxygen comments.
  \todo Missing support for 14400bps, 921600bps and 1228739bps baud rates in osaSerialPort
  \todo Check for buffer overflow in CommandAppend()
  \todo Handle error values returned from the device
  \todo Implement a timeout for CheckResponse(), maybe have a CheckTimeout() method?
  \todo Handle supported features for newer Polaris versions.
  \todo Implement CRC check for CommandSend()
  \todo Pretty print for SerialNumber, to extract date, etc.
  \todo Overload Tool class to have a stream
  \todo Implement an "adaptive sleep" for the run method
  \todo Every sscanf() should check if valid number of items have been read (wrapper for sscanf?)
  \todo Main Type to human readable provided method
  \todo Error handling for all strncpy()
  \todo Check for serial number matching in AddTool(), replace the name if it exists
  \todo Refactor ComputeCRC() and LoadToolDefinitionFile() functions
  \todo Cartesian position should be "invalid" if the tool is missing or disabled
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
            unsigned int FrameNumber;
            double ErrorRMS;
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

    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void) {};
    void Run(void);
    void Cleanup(void) {};

    size_t GetNumberOfTools(void) const {
        return ToolsMap.size();
    }
    std::string GetToolName(const unsigned int index) const;

protected:
    enum { MAX_BUFFER_SIZE = 512 };
    enum { CRC_SIZE = 4 };

    Tool * AddTool(const std::string & name, const char * serialNumber);
    Tool * AddTool(const std::string & name, const char * serialNumber, const char * toolDefinitionFile);

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
    void CommandAppend(const int command);
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
    void Beep(const mtsInt & numberOfBeeps);

    void LoadToolDefinitionFile(const char * portHandle, const char * toolDefinitionFile);
    void PortHandlesInitialize(void);
    void PortHandlesQuery(void);
    void PortHandlesEnable(void);

    void ToggleTracking(const mtsBool & track);
    void Track(void);

    osaSerialPort SerialPort;
    char SerialBuffer[MAX_BUFFER_SIZE];
    char * SerialBufferPointer;

    typedef cmnNamedMap<Tool> ToolsMapType;
    ToolsMapType ToolsMap;
    cmnNamedMap<Tool> PortToTool;

    bool IsTracking;
    char * Tool8700338;
    char * Tool8700340;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDiSerial);

#endif  //_devNDiSerial_h_
