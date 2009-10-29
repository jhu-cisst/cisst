/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-13

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstDevices/devNDiSerial.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <cstdio>

CMN_IMPLEMENT_SERVICES(devNDiSerial);


devNDiSerial::devNDiSerial(const std::string & taskName, const std::string & serialPort) :
    mtsTaskContinuous(taskName, 5000)
{
    mtsProvidedInterface * provided = AddProvidedInterface("ProvidesNDISerialController");
    if (provided) {
        provided->AddCommandWrite(&devNDiSerial::Beep, this, "Beep");
        provided->AddCommandVoid(&devNDiSerial::PortHandlesInitialize, this, "PortHandlesInitialize");
        provided->AddCommandVoid(&devNDiSerial::PortHandlesQuery, this, "PortHandlesQuery");
        provided->AddCommandVoid(&devNDiSerial::PortHandlesEnable, this, "PortHandlesEnable");
        provided->AddCommandWrite(&devNDiSerial::ToggleTracking, this, "ToggleTracking");
    }

    memset(SerialBuffer, 0, MAX_BUFFER_SIZE);
    SerialBufferPointer = SerialBuffer;

    IsTracking = false;

    SerialPort.SetPortName(serialPort);
    if (!SerialPort.Open()) {
        CMN_LOG_CLASS_INIT_ERROR << "devNDiSerial: failed to open serial port: " << SerialPort.GetPortName() << std::endl;
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
    //PortHandlesInitialize();
    //PortHandlesQuery();
}


void devNDiSerial::Configure(const std::string & CMN_UNUSED(filename))
{
    AddTool("TraxtalProbe", "3091280C");
}


devNDiSerial::Tool * devNDiSerial::AddTool(const std::string & name, const char * serialNumber)
{
    Tool * tool = new Tool();
    tool->Name = name;
    strncpy(tool->SerialNumber, serialNumber, 8);
    if (!ToolsMap.AddItem(tool->Name, tool, CMN_LOG_LOD_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddTool: no tool created, duplicate name exists: " << name << std::endl;
        delete tool;
        return 0;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "AddTool: created tool \"" << name << "\" with serial number: " << serialNumber << std::endl;

    // create an interface for tool
    StateTable.AddData(tool->Position, name + "Position");
    tool->Interface = AddProvidedInterface(name);
    if (tool->Interface) {
        tool->Interface->AddCommandReadState(StateTable, tool->Position, "GetPositionCartesian");
    }

    return tool;
}


std::string devNDiSerial::GetToolName(const unsigned int index) const
{
    const ToolsMapType::const_iterator end = ToolsMap.end();
    ToolsMapType::const_iterator toolIterator = ToolsMap.begin();
    if (index >= ToolsMap.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetToolName: requested index is out of range" << std::endl;
        return "";
    }
    for (unsigned int i = 0; i < index; i++) {
        toolIterator++;
    }
    return toolIterator->first;
}


void devNDiSerial::CommandInitialize(void)
{
    SerialBufferPointer = SerialBuffer;
}


void devNDiSerial::CommandAppend(const char command)
{
    *SerialBufferPointer = command;
    SerialBufferPointer++;
}


void devNDiSerial::CommandAppend(const char * command)
{
    const size_t size = strlen(command);
    strncpy(SerialBufferPointer, command, size);
    SerialBufferPointer += size;
}


void devNDiSerial::CommandAppend(const int command)
{
    SerialBufferPointer += _snprintf(SerialBufferPointer, GetSerialBufferAvailableSize(), "%d", command);
}


bool devNDiSerial::CommandSend(void)
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


unsigned int devNDiSerial::ComputeCRC(const char * data)
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


bool devNDiSerial::ResponseRead(void)
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


bool devNDiSerial::ResponseRead(const char * expectedMessage)
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



bool devNDiSerial::ResponseCheckCRC(void)
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


bool devNDiSerial::ResetSerialPort(void)
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


bool devNDiSerial::SetSerialPortSettings(osaSerialPort::BaudRateType baudRate,
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
            //! \todo Log error
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
            //! \todo Log error
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
            //! \todo Log error
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
            //! \todo Log error
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
            //! \todo Log error
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


void devNDiSerial::Beep(const mtsInt & numberOfBeeps)
{
    //! \todo Check that the value is between 1 and 9. Warn and limit to bounds if not.

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


void devNDiSerial::LoadToolDefinition(const char * portHandle, const char * toolDefinition)
{
    const unsigned int nChunks = strlen(toolDefinition) / 128;
    char data[129];
    data[128] = '\0';
    char address[5];
    address[4] = '\0';

    for (unsigned int i = 0; i < nChunks; i++ ) { 
        sscanf(toolDefinition + (i * 128), "%128c", data);
        sprintf(address, "%04X", i * 64);
        CommandInitialize();
        CommandAppend("PVWR ");
        CommandAppend(portHandle);
        CommandAppend(address);
        CommandAppend(data);
        CommandSend();
        ResponseRead("OKAY");
    }
}


void devNDiSerial::PortHandlesInitialize(void)
{
    char * parsePointer;
    unsigned int nPortHandles = 0;
    std::vector<vctChar3> portHandles;

    // are there port handles to be freed?
    CommandSend("PHSR 01");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &nPortHandles);
    parsePointer += 2;
    portHandles.resize(nPortHandles);
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

    // do I need a handle for a port?
//    CommandSend("PHRQ *********1****");
//    ResponseRead();
//    portHandles.resize(1);
//    sscanf(SerialBuffer, "%2c", portHandles[0].Pointer());
//    portHandles[0][2] = '\0';

    // do I need to load a tool definition file?
//    const char * tool8700338 = "4E4449008C110000010000000000000100000000011480345A00000004000000040000000000403F000000000000000000000000000000000000000000000000000020410000000000000000000000000000000000000000713DCA413333814100000000000000000000824200000000333395C148E1EC410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000803F00000000000000000000803F00000000000000000000803F00000000000000000000803F000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010203000000000000000000000000000000001F1F1F1F090000004E4449000000000000000000383730303333380000000000000000000000000009010101010000000000000000000000000000000001010101000000000000000000000000000000008000290000000000000000000080BF00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
//    const char * tool8700340 = "4E444900D00D0000010000000100000200000000012480345A00000004000000040000000000403F0000000000000000000000000000000000000000000000000000204100000000000000000000000000000000000000000000000000004842000000000000C8410000C842000000000000C8C1000007430000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000803F00000000000000000000803F00000000000000000000803F00000000000000000000803F000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010203000000000000000000000000000000001F1F1F1F090000004E44490000000000000000003837303033343000000000000000000000000000090101010100000000000000000000000000000000010101010000000000000000000000000000000080002900000000000000000000803F00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
//    LoadToolDefinition(portHandles[0].Pointer(), tool8700340);

    // are there port handles to be initialized?
    CommandSend("PHSR 02");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &nPortHandles);
    parsePointer += 2;
    portHandles.resize(nPortHandles);
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


void devNDiSerial::PortHandlesQuery(void)
{
    char * parsePointer;
    unsigned int nPortHandles = 0;
    std::vector<vctChar3> portHandles;

    CommandSend("PHSR 00");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &nPortHandles);
    parsePointer += 2;
    CMN_LOG_CLASS_INIT_DEBUG << "PortHandlesQuery: " << nPortHandles << " tools are plugged in" << std::endl;
    portHandles.resize(nPortHandles);
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

        // find the tool with this serial number if it exists, or create one
        tool = 0;
        const ToolsMapType::const_iterator end = ToolsMap.end();
        ToolsMapType::const_iterator toolIterator;
        for (toolIterator = ToolsMap.begin(); toolIterator != end; ++toolIterator) {
            if (strncmp((toolIterator->second)->SerialNumber, serialNumber, 8) == 0) {
                tool = toolIterator->second;
                CMN_LOG_CLASS_INIT_DEBUG << "PortHandlesQuery: found existing tool for serial number: " << serialNumber << std::endl;
            }
        }
        if (!tool) {
            std::string name;
            name = std::string(mainType) + '-' + std::string(serialNumber);
            tool = AddTool(name, serialNumber);
        }

        // update tool information
        sscanf(SerialBuffer, "%2c%*1X%*1X%*2c%*2c%12c%3c%*8c%*2c%20c",
               tool->MainType, tool->ManufacturerID, tool->ToolRevision, tool->PartNumber);
        strncpy(tool->PortHandle, portHandles[i].Pointer(), 2);

        // associate the tool to its port handle
        toolKey = portHandles[i].Pointer();
        CMN_LOG_CLASS_INIT_VERBOSE << "PortHandlesQuery: associating tool " << tool->Name << " to port handle " << tool->PortHandle << std::endl;
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


void devNDiSerial::PortHandlesEnable(void)
{
    char * parsePointer;
    unsigned int nPortHandles = 0;
    std::vector<vctChar3> portHandles;

    CommandSend("PHSR 03");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &nPortHandles);
    parsePointer += 2;
    portHandles.resize(nPortHandles);
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
            //! \todo Do error recovery
            CMN_LOG_CLASS_RUN_ERROR << "PortHandlesEnable: no tool for port handle: " << toolKey << std::endl;
            return;
        }

        if (strncmp(tool->MainType, "01", 2) == 0) {
            CommandAppend("S");
        } else if (strncmp(tool->MainType, "02", 2) == 0) {
            CommandAppend("D");
        } else if (strncmp(tool->MainType, "03", 2) == 0) {
            CommandAppend("B");
        } else {
            //!< \todo Handle other main types of tools
            CMN_LOG_CLASS_RUN_ERROR << "PortHandlesEnable: unknown tool of main type: " << tool->MainType << std::endl;
            return;
        }
        CommandSend();
        ResponseRead("OKAY");
        CMN_LOG_CLASS_RUN_DEBUG << "PortHandlesEnable: enabled port handle: " << portHandles[i].Pointer() << std::endl;
    }
}


void devNDiSerial::ToggleTracking(const mtsBool & track)
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
}


void devNDiSerial::Track(void)
{
    char * parsePointer;
    unsigned int nPortHandles = 0;
    char portHandle[3];
    portHandle[2] = '\0';
    std::string toolKey;
    Tool * tool;
    vctQuatRot3 toolOrientation;
    vct3 toolPosition;

    CommandSend("TX 0001");
    ResponseRead();
    parsePointer = SerialBuffer;
    sscanf(parsePointer, "%02X", &nPortHandles);
    parsePointer += 2;
    CMN_LOG_CLASS_RUN_DEBUG << "Track: tracking " << nPortHandles << " tools" << std::endl;
    for (unsigned int i = 0; i < nPortHandles; i++) {
        sscanf(parsePointer, "%2c", portHandle);
        parsePointer += 2;
        toolKey = portHandle;
        tool = PortToTool.GetItem(toolKey);
        if (!tool) {
            //! \todo Do error recovery
            CMN_LOG_CLASS_RUN_ERROR << "Track: no tool for port handle: " << toolKey << std::endl;
            return;
        }

        if (strncmp(parsePointer, "MISSING", 7) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << parsePointer << std::endl;
            CMN_LOG_CLASS_RUN_WARNING << "Track: tool " << tool->Name << " is missing" << std::endl;
            tool->Position.SetValid(false);
            parsePointer += 7;
            parsePointer += 8;  // skip Port Status
        } else if (strncmp(parsePointer, "DISABLED", 8) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Track: tool " << tool->Name << " is disabled" << std::endl;
            tool->Position.SetValid(false);
            parsePointer += 8;
            parsePointer += 8;  // skip Port Status
        } else if (strncmp(parsePointer, "UNOCCUPIED", 10) == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Track: tool " << tool->Name << " is unoccupied" << std::endl;
            tool->Position.SetValid(false);
            parsePointer += 10;
            parsePointer += 8;  // skip Port Status
        } else {
            //! \todo Parse Port Status here, to get "partially out of volume", etc.
            sscanf(parsePointer, "%6lf%6lf%6lf%6lf%7lf%7lf%7lf%6lf%*8X",
                   &(toolOrientation.W()), &(toolOrientation.X()), &(toolOrientation.Y()), &(toolOrientation.Z()),
                   &(toolPosition.X()), &(toolPosition.Y()), &(toolPosition.Z()),
                   &(tool->ErrorRMS));
            parsePointer += (4 * 6) + (3 * 7) + 6 + 8;
            toolOrientation.Divide(10000.0);
            tool->Position.Position().Rotation().FromRaw(toolOrientation);
            toolPosition.Divide(100.0);
            tool->Position.Position().Translation().Assign(toolPosition);
            tool->ErrorRMS /= 10000.0;

            CMN_LOG_CLASS_RUN_DEBUG << "Track: orientation (rad): " << vctAxAnRot3(toolOrientation, VCT_DO_NOT_NORMALIZE) << std::endl;
            CMN_LOG_CLASS_RUN_DEBUG << "Track: translation (mm): " << toolPosition << std::endl;
        }
        sscanf(parsePointer, "%08X", &(tool->FrameNumber));
        parsePointer += 8;
        CMN_LOG_CLASS_RUN_DEBUG << "Track: frame number: " << tool->FrameNumber << std::endl;
        if (*parsePointer != '\n') {
            CMN_LOG_CLASS_RUN_ERROR << "Track: line feed expected, received: " << *parsePointer << std::endl;
            return;
        }
        parsePointer += 1;  // skip '\n'
        //! \todo Parse System Status here
        parsePointer += 4;  // skip System Status
    }
}


void devNDiSerial::Run(void)
{
    ProcessQueuedCommands();

    if (IsTracking) {
        Track();
    }
}


devNDiSerial::Tool::Tool(void)
{
    PortHandle[2] = '\0';
    MainType[2] = '\0';
    ManufacturerID[12] = '\0';
    ToolRevision[3] = '\0';
    SerialNumber[8] = '\0';
    PartNumber[20] = '\0';
}
