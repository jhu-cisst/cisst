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

#include <cisstCommon/cmnStrings.h>
#include <cisstCommon/cmnXMLPath.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstOSAbstraction/osaSleep.h>
#if CISST_HAS_CISSTNETLIB
    #include <cisstNumerical/nmrLSSolver.h>
#endif
#include <cisstDevices/devNDISerial.h>

CMN_IMPLEMENT_SERVICES(devNDISerial);


devNDISerial::devNDISerial(const std::string & taskName, const double period) :
    mtsTaskPeriodic(taskName, period, false, 5000),
    IsTracking(false)
{
    mtsProvidedInterface * provided = AddProvidedInterface("Controller");
    if (provided) {
        StateTable.AddData(IsTracking, "IsTracking");

        provided->AddCommandWrite(&devNDISerial::Beep, this, "Beep", mtsInt());
        provided->AddCommandVoid(&devNDISerial::PortHandlesInitialize, this, "PortHandlesInitialize");
        provided->AddCommandVoid(&devNDISerial::PortHandlesQuery, this, "PortHandlesQuery");
        provided->AddCommandVoid(&devNDISerial::PortHandlesEnable, this, "PortHandlesEnable");
        provided->AddCommandWrite(&devNDISerial::CalibratePivot, this, "CalibratePivot", mtsStdString());
        provided->AddCommandWrite(&devNDISerial::ToggleTracking, this, "ToggleTracking", mtsBool());
        provided->AddCommandReadState(StateTable, IsTracking, "IsTracking");
    }

    memset(SerialBuffer, 0, MAX_BUFFER_SIZE);
    SerialBufferPointer = SerialBuffer;
}


void devNDISerial::Configure(const std::string & filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: using " << filename << std::endl;

    cmnXMLPath config;
    config.SetInputSource(filename);

    // initialize serial port
    std::string serialPort;
    config.GetXMLValue("/config/device", "@port", serialPort);

    SerialPort.SetPortName(serialPort);
    if (!SerialPort.Open()) {
        CMN_LOG_CLASS_INIT_ERROR << "Configure: failed to open serial port: " << SerialPort.GetPortName() << std::endl;
    }
    ResetSerialPort();
    SetSerialPortSettings(osaSerialPort::BaudRate115200,
                          osaSerialPort::CharacterSize8,
                          osaSerialPort::ParityCheckingNone,
                          osaSerialPort::StopBitsOne,
                          osaSerialPort::FlowControlNone);

    // initialize NDI controller
    CommandSend("INIT ");
    ResponseRead("OKAY");

    // add tools
    int maxNumTools = 100;
    std::string toolName;
    bool toolEnabled;
    std::string toolSerial, toolSerialLast;
    std::string toolDefinition;

    for (int i = 0; i < maxNumTools; i++) {
        std::stringstream context;
        context << "/config/tools/tool[" << i << "]";
        config.GetXMLValue(context.str().c_str(), "@name", toolName);
        config.GetXMLValue(context.str().c_str(), "@enabled", toolEnabled);
        config.GetXMLValue(context.str().c_str(), "@serial", toolSerial);
        config.GetXMLValue(context.str().c_str(), "@definition", toolDefinition);
        if (toolSerial != toolSerialLast) {
            toolSerialLast = toolSerial;
            if (toolEnabled) {
                if (toolDefinition == "") {
                    AddTool(toolName, toolSerial.c_str());
                } else {
                    AddTool(toolName, toolSerial.c_str(), toolDefinition.c_str());
                }
            }
        }
    }
}


void devNDISerial::Run(void)
{
    ProcessQueuedCommands();

    if (IsTracking) {
        Track();
    }
}


void devNDISerial::Cleanup(void)
{
    ToggleTracking(false);
    if (!SerialPort.Close()) {
        CMN_LOG_CLASS_INIT_ERROR << "Cleanup: failed to close serial port" << std::endl;
    }
}


void devNDISerial::CommandInitialize(void)
{
    SerialBufferPointer = SerialBuffer;
}


void devNDISerial::CommandAppend(const char command)
{
    *SerialBufferPointer = command;
    SerialBufferPointer++;
}


void devNDISerial::CommandAppend(const char * command)
{
    const size_t size = strlen(command);
    strncpy(SerialBufferPointer, command, size);
    SerialBufferPointer += size;
}


void devNDISerial::CommandAppend(const int command)
{
    SerialBufferPointer += cmn_snprintf(SerialBufferPointer, GetSerialBufferAvailableSize(), "%d", command);
}


unsigned int devNDISerial::ComputeCRC(const char * data)
{
    static unsigned char oddParity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
    unsigned char * dataPointer = (unsigned char *)data;
    unsigned int temp = 0;
    unsigned int crc = 0;

    while (*dataPointer) {
        temp = (*dataPointer ^ (crc & 0xff)) & 0xff;
        crc >>= 8;

        if (oddParity[temp & 0x0f] ^ oddParity[temp >> 4]) {
            crc ^= 0xc001;
        }
        temp <<= 6;
        crc ^= temp;
        temp <<= 1;
        crc ^= temp;
        dataPointer++;
    }
    return crc;
}


bool devNDISerial::CommandSend(void)
{
    CommandAppend('\r');
    CommandAppend('\0');

    const size_t bytesToSend = strlen(SerialBuffer);
    const size_t bytesSent = SerialPort.Write(SerialBuffer, bytesToSend);
    if (bytesSent != bytesToSend) {
        CMN_LOG_CLASS_RUN_ERROR << "SendCommand: sent only " << bytesSent << " of " << bytesToSend
                                << " for command \"" << SerialBuffer << "\"" << std::endl;
        return false;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "SendCommand: successfully sent command \"" << SerialBuffer << "\"" << std::endl;
    return true;
}


bool devNDISerial::ResponseRead(void)
{
    SerialBufferPointer = SerialBuffer;
    do {
        SerialBufferPointer += SerialPort.Read(SerialBufferPointer, GetSerialBufferAvailableSize());
    } while (*(SerialBufferPointer - 1) != '\r');

    if (!ResponseCheckCRC()) {
        return false;
    }
    return true;
}


bool devNDISerial::ResponseRead(const char * expectedMessage)
{
    ResponseRead();

    if (strncmp(expectedMessage, SerialBuffer, GetSerialBufferStringSize()) != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "ResponseRead: expected \"" << expectedMessage
                                << "\", but received \"" << SerialBuffer << "\"" << std::endl;
        return false;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "ResponseRead: received expected response" << std::endl;
    return true;
}


bool devNDISerial::ResponseCheckCRC(void)
{
    char receivedCRC[CRC_SIZE + 1];
    char computedCRC[CRC_SIZE + 1];
    char * crcPointer = SerialBufferPointer - (CRC_SIZE + 1);  // +1 for '\r'

    // extract CRC from buffer
    strncpy(receivedCRC, crcPointer, CRC_SIZE);
    receivedCRC[CRC_SIZE] = '\0';
    *crcPointer = '\0';
    SerialBufferPointer = crcPointer + 1;

    // compute CRC
    sprintf(computedCRC, "%04X", ComputeCRC(SerialBuffer));
    computedCRC[CRC_SIZE] = '\0';

    // compare CRCs
    if (strncmp(receivedCRC, computedCRC, CRC_SIZE) != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "ResponseCheckCRC: received \"" << SerialBuffer << receivedCRC
                                << "\", but computed \"" << computedCRC << "\" for CRC" << std::endl;
        return false;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "ResponseCheckCRC: CRC check was successful for \"" << SerialBuffer << "\"" << std::endl;
    return true;
}


bool devNDISerial::ResetSerialPort(void)
{
    SerialPort.WriteBreak(0.5 * cmn_s);
    osaSleep(200.0 * cmn_ms);

    SerialPort.SetBaudRate(osaSerialPort::BaudRate9600);
    SerialPort.SetCharacterSize(osaSerialPort::CharacterSize8);
    SerialPort.SetParityChecking(osaSerialPort::ParityCheckingNone);
    SerialPort.SetStopBits(osaSerialPort::StopBitsOne);
    SerialPort.SetFlowControl(osaSerialPort::FlowControlNone);
    SerialPort.Configure();

    if (!ResponseRead("RESET")) {
        CMN_LOG_CLASS_INIT_ERROR << "ResetSerialPort: failed to reset" << std::endl;
        return false;
    }
    return true;
}


bool devNDISerial::SetSerialPortSettings(osaSerialPort::BaudRateType baudRate,
                                         osaSerialPort::CharacterSizeType characterSize,
                                         osaSerialPort::ParityCheckingType parityChecking,
                                         osaSerialPort::StopBitsType stopBits,
                                         osaSerialPort::FlowControlType flowControl)
{
    CommandInitialize();
    CommandAppend("COMM ");

    switch (baudRate) {
        case osaSerialPort::BaudRate9600:
            CommandAppend('0');
            break;
        case osaSerialPort::BaudRate19200:
            CommandAppend('2');
            break;
        case osaSerialPort::BaudRate38400:
            CommandAppend('3');
            break;
        case osaSerialPort::BaudRate57600:
            CommandAppend('4');
            break;
        case osaSerialPort::BaudRate115200:
            CommandAppend('5');
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "SetSerialPortSettings: invalid baud rate" << std::endl;
            return false;
            break;
    }

    switch (characterSize) {
        case osaSerialPort::CharacterSize8:
            CommandAppend('0');
            break;
        case osaSerialPort::CharacterSize7:
            CommandAppend('1');
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "SetSerialPortSettings: invalid character size" << std::endl;
            return false;
            break;
    }

    switch (parityChecking) {
        case osaSerialPort::ParityCheckingNone:
            CommandAppend('0');
            break;
        case osaSerialPort::ParityCheckingOdd:
            CommandAppend('1');
            break;
        case osaSerialPort::ParityCheckingEven:
            CommandAppend('2');
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "SetSerialPortSettings: invalid parity checking" << std::endl;
            return false;
            break;
    }

    switch (stopBits) {
        case osaSerialPort::StopBitsOne:
            CommandAppend('0');
            break;
        case osaSerialPort::StopBitsTwo:
            CommandAppend('1');
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "SetSerialPortSettings: invalid stop bits" << std::endl;
            return false;
            break;
    }

    switch (flowControl) {
        case osaSerialPort::FlowControlNone:
            CommandAppend('0');
            break;
        case osaSerialPort::FlowControlHardware:
            CommandAppend('1');
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "SetSerialPortSettings: invalid flow control" << std::endl;
            return false;
            break;
    }

    if (!CommandSend()) {
        return false;
    }

    if (ResponseRead("OKAY")) {
        osaSleep(200.0 * cmn_ms);
        SerialPort.SetBaudRate(baudRate);
        SerialPort.SetCharacterSize(characterSize);
        SerialPort.SetParityChecking(parityChecking);
        SerialPort.SetStopBits(stopBits);
        SerialPort.SetFlowControl(flowControl);
        SerialPort.Configure();
        return true;
    }
    return false;
}


void devNDISerial::Beep(const mtsInt & numberOfBeeps)
{
    if (numberOfBeeps.Data < 1 || numberOfBeeps.Data > 9) {
        CMN_LOG_CLASS_RUN_ERROR << "Beep: invalid input: " << numberOfBeeps << ", must be between 0-9" << std::endl;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "Beep: beeing " << numberOfBeeps << " times" << std::endl;
    do {
        CommandInitialize();
        CommandAppend("BEEP ");
        CommandAppend(numberOfBeeps.Data);
        CommandSend();
        osaSleep(100.0 * cmn_ms);
        if (!ResponseRead()) {
            return;
        }
    } while (strncmp("0", SerialBuffer, 1) == 0);

    if (strncmp("1", SerialBuffer, 1) != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Beep: unknown response received: " << SerialBuffer << std::endl;
    }
}


void devNDISerial::LoadToolDefinitionFile(const char * portHandle, const char * filePath)
{
    std::ifstream toolDefinitionFile(filePath, std::ios::binary);
    if (!toolDefinitionFile.is_open()) {
        CMN_LOG_CLASS_INIT_ERROR << "LoadToolDefinitionFile: could not open " << filePath << std::endl;
        return;
    }

    toolDefinitionFile.seekg(0, std::ios::end);
    size_t fileSize = toolDefinitionFile.tellg();
    size_t definitionSize = fileSize * 2;
    size_t paddingSize = 128 - (definitionSize % 128);
    size_t numChunks = (definitionSize + paddingSize) / 128;
    toolDefinitionFile.seekg(0, std::ios::beg);

    if (fileSize > 960) {
        CMN_LOG_CLASS_INIT_ERROR << "LoadToolDefinitionFile: " << filePath << " of size "
                                 << fileSize << " bytes exceeds the 960 bytes limit" << std::endl;
        return;
    }

    char input[65] = { 0 };
    input[64] = '\0';
    char output[129];
    output[128] = '\0';
    char address[5];
    address[4] = '\0';

    for (unsigned int i = 0; i < numChunks; i++) {
        toolDefinitionFile.read(input, 64);
        for (unsigned int j = 0; j < 64; j++) {
            sprintf(&output[j*2], "%02X", static_cast<unsigned char>(input[j]));
        }
        sprintf(address, "%04X", i * 64);
        CommandInitialize();
        CommandAppend("PVWR ");
        CommandAppend(portHandle);
        CommandAppend(address);
        CommandAppend(output);
        CommandSend();
        ResponseRead("OKAY");
    }
}


devNDISerial::Tool * devNDISerial::CheckTool(const char * serialNumber)
{
    const ToolsType::const_iterator end = Tools.end();
    ToolsType::const_iterator toolIterator;
    for (toolIterator = Tools.begin(); toolIterator != end; ++toolIterator) {
        if (strncmp(toolIterator->second->SerialNumber, serialNumber, 8) == 0) {
            CMN_LOG_CLASS_INIT_DEBUG << "CheckTool: found existing tool for serial number: " << serialNumber << std::endl;
            return toolIterator->second;
        }
    }
    return 0;
}


devNDISerial::Tool * devNDISerial::AddTool(const std::string & name, const char * serialNumber)
{
    Tool * tool = CheckTool(serialNumber);

    if (tool) {
        CMN_LOG_CLASS_INIT_WARNING << "AddTool: " << tool->Name << " already exists, renaming it to " << name << " instead" << std::endl;
        tool->Name = name;
    } else {
        tool = new Tool();
        tool->Name = name;
        strncpy(tool->SerialNumber, serialNumber, 8);

        if (!Tools.AddItem(tool->Name, tool, CMN_LOG_LOD_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddTool: no tool created, duplicate name exists: " << name << std::endl;
            delete tool;
            return 0;
        }
        CMN_LOG_CLASS_INIT_VERBOSE << "AddTool: created tool \"" << name << "\" with serial number: " << serialNumber << std::endl;

        // create an interface for tool
        tool->Interface = AddProvidedInterface(name);
        if (tool->Interface) {
            StateTable.AddData(tool->TooltipPosition, name + "Position");
            tool->Interface->AddCommandReadState(StateTable, tool->TooltipPosition, "GetPositionCartesian");
        }
    }
    return tool;
}


devNDISerial::Tool * devNDISerial::AddTool(const std::string & name, const char * serialNumber, const char * toolDefinitionFile)
{
    char portHandle[3];
    portHandle[2] = '\0';

    // request port handle for wireless tool
    CommandSend("PHRQ *********1****");
    ResponseRead();
    sscanf(SerialBuffer, "%2c", portHandle);

    LoadToolDefinitionFile(portHandle, toolDefinitionFile);
    return AddTool(name, serialNumber);
}


std::string devNDISerial::GetToolName(const unsigned int index) const
{
    ToolsType::const_iterator toolIterator = Tools.begin();
    if (index >= Tools.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetToolName: requested index is out of range" << std::endl;
        return "";
    }
    for (unsigned int i = 0; i < index; i++) {
        toolIterator++;
    }
    return toolIterator->first;
}


void devNDISerial::PortHandlesInitialize(void)
{
    char * parsePointer;
    unsigned int numPortHandles = 0;
    std::vector<vctChar3> portHandles;

    // are there port handles to be freed?
    CommandSend("PHSR 01");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &numPortHandles);
    parsePointer += 2;
    portHandles.resize(numPortHandles);
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        sscanf(parsePointer, "%2c%*3c", portHandles[i].Pointer());
        parsePointer += 5;
        portHandles[i][2] = '\0';
    }
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        CommandInitialize();
        CommandAppend("PHF ");
        CommandAppend(portHandles[i].Pointer());
        CommandSend();
        ResponseRead("OKAY");
        CMN_LOG_CLASS_RUN_DEBUG << "PortHandlesInitialize: freed port handle: " << portHandles[i].Pointer() << std::endl;
    }

    // are there port handles to be initialized?
    CommandSend("PHSR 02");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &numPortHandles);
    parsePointer += 2;
    portHandles.resize(numPortHandles);
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        sscanf(parsePointer, "%2c%*3c", portHandles[i].Pointer());
        parsePointer += 5;
        portHandles[i][2] = '\0';
    }
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        CommandInitialize();
        CommandAppend("PINIT ");
        CommandAppend(portHandles[i].Pointer());
        CommandSend();
        ResponseRead("OKAY");
        CMN_LOG_CLASS_RUN_DEBUG << "PortHandlesInitialize: initialized port handle: " << portHandles[i].Pointer() << std::endl;
    }
}


void devNDISerial::PortHandlesQuery(void)
{
    char * parsePointer;
    unsigned int numPortHandles = 0;
    std::vector<vctChar3> portHandles;

    CommandSend("PHSR 00");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &numPortHandles);
    parsePointer += 2;
    CMN_LOG_CLASS_INIT_DEBUG << "PortHandlesQuery: " << numPortHandles << " tools are plugged in" << std::endl;
    portHandles.resize(numPortHandles);
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        sscanf(parsePointer, "%2c%*3c", portHandles[i].Pointer());
        parsePointer += 5;
        portHandles[i][2] = '\0';
    }

    Tool * tool;
    std::string toolKey;
    PortToTool.clear();
    char mainType[3];
    mainType[2] = '\0';
    char serialNumber[9];
    serialNumber[8] = '\0';

    for (unsigned int i = 0; i < portHandles.size(); i++) {
        CommandInitialize();
        CommandAppend("PHINF ");
        CommandAppend(portHandles[i].Pointer());
        CommandAppend("0005");
        CommandSend();
        ResponseRead();
        sscanf(SerialBuffer, "%2c%*1X%*1X%*2c%*2c%*12c%*3c%8c%*2c%*20c",
               mainType, serialNumber);

        // check if tool exists, generate a name and add it otherwise
        tool = CheckTool(serialNumber);
        if (!tool) {
            std::string name = std::string(mainType) + '-' + std::string(serialNumber);
            tool = AddTool(name, serialNumber);
        }

        // update tool information
        sscanf(SerialBuffer, "%2c%*1X%*1X%*2c%*2c%12c%3c%*8c%*2c%20c",
               tool->MainType, tool->ManufacturerID, tool->ToolRevision, tool->PartNumber);
        strncpy(tool->PortHandle, portHandles[i].Pointer(), 2);

        // associate the tool to its port handle
        toolKey = portHandles[i].Pointer();
        CMN_LOG_CLASS_INIT_VERBOSE << "PortHandlesQuery: associating " << tool->Name << " to port handle " << tool->PortHandle << std::endl;
        PortToTool.AddItem(toolKey, tool, CMN_LOG_LOD_INIT_ERROR);

        CMN_LOG_CLASS_INIT_DEBUG << "PortHandlesQuery:\n"
                                 << " * Port Handle: " << tool->PortHandle << "\n"
                                 << " * Main Type: " << tool->MainType << "\n"
                                 << " * Manufacturer ID: " << tool->ManufacturerID << "\n"
                                 << " * Tool Revision: " << tool->ToolRevision << "\n"
                                 << " * Serial Number: " << tool->SerialNumber << "\n"
                                 << " * Part Number: " << tool->PartNumber << std::endl;
    }
}


void devNDISerial::PortHandlesEnable(void)
{
    char * parsePointer;
    unsigned int numPortHandles = 0;
    std::vector<vctChar3> portHandles;

    CommandSend("PHSR 03");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &numPortHandles);
    parsePointer += 2;
    portHandles.resize(numPortHandles);
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        sscanf(parsePointer, "%2c%*3c", portHandles[i].Pointer());
        parsePointer += 5;
        portHandles[i][2] = '\0';
    }
    for (unsigned int i = 0; i < portHandles.size(); i++) {
        CommandInitialize();
        CommandAppend("PENA ");
        CommandAppend(portHandles[i].Pointer());

        Tool * tool;
        std::string toolKey = portHandles[i].Pointer();
        tool = PortToTool.GetItem(toolKey);
        if (!tool) {
            CMN_LOG_CLASS_RUN_ERROR << "PortHandlesEnable: no tool for port handle: " << toolKey << std::endl;
            return;
        }

        if (strncmp(tool->MainType, "01", 2) == 0) {
            CommandAppend("S");
        } else if (strncmp(tool->MainType, "02", 2) == 0) {
            CommandAppend("D");
        } else if (strncmp(tool->MainType, "03", 2) == 0) {
            CommandAppend("B");
        } else if (strncmp(tool->MainType, "0A", 2) == 0) {
            CommandAppend("D");
        } else {
            CMN_LOG_CLASS_RUN_ERROR << "PortHandlesEnable: unknown tool of main type: " << tool->MainType << std::endl;
            return;
        }
        CommandSend();
        ResponseRead("OKAY");
        CMN_LOG_CLASS_RUN_DEBUG << "PortHandlesEnable: enabled port handle: " << portHandles[i].Pointer() << std::endl;
    }
}


void devNDISerial::ToggleTracking(const mtsBool & track)
{
    if (track.Data) {
        IsTracking = true;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is on" << std::endl;
        CommandSend("TSTART 80");
    } else {
        IsTracking = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is off" << std::endl;
        CommandSend("TSTOP ");
    }
    ResponseRead("OKAY");
    osaSleep(0.5 * cmn_s);
}


void devNDISerial::Track(void)
{
    char * parsePointer;
    unsigned int numPortHandles = 0;
    char portHandle[3];
    portHandle[2] = '\0';
    std::string toolKey;
    Tool * tool;
    vctQuatRot3 toolOrientation;
    vct3 toolPosition;
    vctFrm3 tooltipPosition;

    CommandSend("TX 0001");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &numPortHandles);
    parsePointer += 2;
    CMN_LOG_CLASS_RUN_DEBUG << "Track: tracking " << numPortHandles << " tools" << std::endl;
    for (unsigned int i = 0; i < numPortHandles; i++) {
        sscanf(parsePointer, "%2c", portHandle);
        parsePointer += 2;
        toolKey = portHandle;
        tool = PortToTool.GetItem(toolKey);
        if (!tool) {
            CMN_LOG_CLASS_RUN_ERROR << "Track: no tool for port handle: " << toolKey << std::endl;
            return;
        }

        if (strncmp(parsePointer, "MISSING", 7) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Track: " << tool->Name << " is missing" << std::endl;
            tool->TooltipPosition.SetValid(false);
            parsePointer += 7;
            parsePointer += 8;  // skip Port Status
        } else if (strncmp(parsePointer, "DISABLED", 8) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Track: " << tool->Name << " is disabled" << std::endl;
            tool->TooltipPosition.SetValid(false);
            parsePointer += 8;
            parsePointer += 8;  // skip Port Status
        } else if (strncmp(parsePointer, "UNOCCUPIED", 10) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Track: " << tool->Name << " is unoccupied" << std::endl;
            tool->TooltipPosition.SetValid(false);
            parsePointer += 10;
            parsePointer += 8;  // skip Port Status
        } else {
            sscanf(parsePointer, "%6lf%6lf%6lf%6lf%7lf%7lf%7lf%6lf%*8X",
                   &(toolOrientation.W()), &(toolOrientation.X()), &(toolOrientation.Y()), &(toolOrientation.Z()),
                   &(toolPosition.X()), &(toolPosition.Y()), &(toolPosition.Z()),
                   &(tool->ErrorRMS));
            parsePointer += (4 * 6) + (3 * 7) + 6 + 8;

            toolOrientation.Divide(10000.0);
            tooltipPosition.Rotation().FromRaw(toolOrientation);
            toolPosition.Divide(100.0);
            tooltipPosition.Translation() = toolPosition;
            tool->ErrorRMS /= 10000.0;
            tool->MarkerPosition.Position() = tooltipPosition;

            tooltipPosition.Translation() += tooltipPosition.Rotation() * tool->TooltipOffset;  // apply tooltip offset
            tool->TooltipPosition.Position() = tooltipPosition;
            tool->TooltipPosition.SetValid(true);

            CMN_LOG_CLASS_RUN_DEBUG << "Track: " << tool->Name << " is at:\n" << tooltipPosition << std::endl;
        }
        sscanf(parsePointer, "%08X", &(tool->FrameNumber));
        parsePointer += 8;
        CMN_LOG_CLASS_RUN_DEBUG << "Track: frame number: " << tool->FrameNumber << std::endl;
        if (*parsePointer != '\n') {
            CMN_LOG_CLASS_RUN_ERROR << "Track: line feed expected, received: " << *parsePointer << std::endl;
            return;
        }
        parsePointer += 1;  // skip '\n'
    }
    parsePointer += 4;  // skip System Status
}


void devNDISerial::CalibratePivot(const mtsStdString & toolName)
{
#if CISST_HAS_CISSTNETLIB
    const unsigned int numPoints = 500;

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibrating " << toolName.Data << std::endl;
    Tool * tool = Tools.GetItem(toolName.Data);
    tool->TooltipOffset.SetAll(0.0);

    CommandSend("TSTART 80");
    ResponseRead("OKAY");

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: starting calibration in 5 seconds" << std::endl;
    osaSleep(5.0 * cmn_s);
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibration stopped" << std::endl;
    Beep(2);

    vctMat A(3 * numPoints, 6, VCT_COL_MAJOR);
    vctMat b(3 * numPoints, 1, VCT_COL_MAJOR);
    std::vector<vctFrm3> frames(numPoints);

    for (unsigned int i = 0; i < numPoints; i++) {
        Track();
        frames[i] = tool->MarkerPosition.Position();

        vctDynamicMatrixRef<double> rotation(3, 3, 1, numPoints*3, A.Pointer(i*3, 0));
        rotation.Assign(tool->MarkerPosition.Position().Rotation());

        vctDynamicMatrixRef<double> identity(3, 3, 1, numPoints*3, A.Pointer(i*3, 3));
        identity.Assign(-vctRot3::Identity());

        vctDynamicVectorRef<double> translation(3, b.Pointer(i*3, 0));
        translation.Assign(tool->MarkerPosition.Position().Translation());
    }

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibration started" << std::endl;
    CommandSend("TSTOP ");
    ResponseRead("OKAY");

    nmrLSSolver calibration(A, b);
    calibration.Solve(A, b);

    vct3 tooltip;
    vct3 pivot;
    for (unsigned int i = 0; i < 3; i++) {
        tooltip.Element(i) = -b.at(i, 0);
        pivot.Element(i) = -b.at(i+3, 0);
    }
    tool->TooltipOffset = tooltip;

    vct3 error;
    double errorSquareSum = 0.0;
    for (int i = 0; i < numPoints; i++) {
        error = (frames[i] * tooltip) - pivot;
        CMN_LOG_CLASS_RUN_DEBUG << error << std::endl;
        errorSquareSum += error.NormSquare();
    }
    double errorRMS = sqrt(errorSquareSum / numPoints);

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot:\n"
                              << " * tooltip offset: " << tooltip << "\n"
                              << " * pivot position: " << pivot << "\n"
                              << " * error RMS: " << errorRMS << std::endl;
#else
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: requires cisstNetlib" << std::endl;
#endif
}


devNDISerial::Tool::Tool(void) :
    TooltipOffset(0.0)
{
    PortHandle[2] = '\0';
    MainType[2] = '\0';
    ManufacturerID[12] = '\0';
    ToolRevision[3] = '\0';
    SerialNumber[8] = '\0';
    PartNumber[20] = '\0';
}
