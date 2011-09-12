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
  \brief cisst component for NDI surgical trackers with serial interface.
  \ingroup cisstComponents

  \bug Missing support for 14400bps, 921600bps and 1228739bps baud rates in osaSerialPort.

  \todo CMaking for component example, suitable both for internal and external build.
  \todo Cleanup of CISST_QT4_WRAP_UI CMake macro.

  \todo Consider deriving from mtsTaskContinuous using an "adaptive" sleep.
  \todo Verify the need for existing sleep times.
  \todo Enable/disable individual tools on-the-fly (or dynamically disable their interfaces).
  \todo Move CalibratePivot to cisstNumerical?
  \todo Handle other main types of tools (besides pointer, reference, etc.).
  \todo Parse port/system status, in order to get "partially out of volume", etc..
  \todo Refactor ComputeCRC and implement a CRC check in CommandSend (move CRC check to osaSerialPort?).
  \todo Every sscanf should check if valid number of items have been read (wrapper for sscanf?).
  \todo Error handling for all strncpy.
  \todo Check for buffer overflow in CommandAppend.
  \todo Implement a timeout for CheckResponse (integrate Ryan's patch).
  \todo Support for the extra features of newer Polaris versions.
  \todo Pretty print for SerialNumber, to extract date, etc..
  \todo Overload Tool class to have a stream.
  \todo Use frame number to decide if timestamp should be refreshed.
  \todo Use a map to convert Tool's MainType to human readable text.
  \todo Strategies for error recovery, send an event with a human readable payload, implement in CheckResponse.
*/

#ifndef _mtsNDISerial_h
#define _mtsNDISerial_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstNDISerial/cisstNDISerialExport.h>  // always include last


class CISST_EXPORT mtsNDISerial : public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:
    class Tool
    {
     public:
        Tool(void);
        ~Tool(void) {};

        std::string Name;
        unsigned int FrameNumber;
        double ErrorRMS;
        mtsInterfaceProvided * Interface;
        prmPositionCartesianGet TooltipPosition;
        prmPositionCartesianGet MarkerPosition;

        char PortHandle[3];
        // PHINF 0001
        char MainType[3];
        char ManufacturerID[13];
        char ToolRevision[4];
        char SerialNumber[9];
        // PHINF 0004
        char PartNumber[21];

        vct3 TooltipOffset;
    };

 public:
    mtsNDISerial(const std::string & taskName, const double period);
    ~mtsNDISerial(void) {};

    void Configure(const std::string & filename = "");
    void Startup(void) {};
    void Run(void);
    void Cleanup(void);

    size_t GetNumberOfTools(void) const {
        return Tools.size();
    }
    std::string GetToolName(const unsigned int index) const;

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

    void LoadToolDefinitionFile(const char * portHandle, const char * filePath);
    Tool * CheckTool(const char * serialNumber);
    Tool * AddTool(const std::string & name, const char * serialNumber);
    Tool * AddTool(const std::string & name, const char * serialNumber, const char * toolDefinitionFile);

    void PortHandlesInitialize(void);
    void PortHandlesQuery(void);
    void PortHandlesEnable(void);

    void ToggleTracking(const mtsBool & track);
    void Track(void);
    void CalibratePivot(const mtsStdString & toolName);
    void ReportStrayMarkers(void);

    osaSerialPort SerialPort;
    char SerialBuffer[MAX_BUFFER_SIZE];
    char * SerialBufferPointer;

    typedef cmnNamedMap<Tool> ToolsType;
    ToolsType Tools;
    cmnNamedMap<Tool> PortToTool;

    mtsBool IsTracking;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsNDISerial);

#endif  //_mtsNDISerial_h
