/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devNDiSerial.cpp 192 2009-03-25 22:12:25Z adeguet1 $

  Author(s): Eric Lin, Joseph Vidalis
  Created on: 2008-09-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstDevices/devSartoriusSerial.h>
#include <cisstOSAbstraction/osaSleep.h>


CMN_IMPLEMENT_SERVICES(devSartoriusSerial);


devSartoriusSerial::devSartoriusSerial(const std::string & taskName,
                                       const std::string & serialPortName):
    mtsTaskContinuous(taskName, 500)
{
    this->SerialPort.SetPortName(serialPortName);
    this->SetSerialPortDefaults();
    this->SetupInterface();
}


devSartoriusSerial::devSartoriusSerial(const std::string & taskName,
                                       unsigned int serialPortNumber):
    mtsTaskContinuous(taskName, 500)
{
    this->SerialPort.SetPortNumber(serialPortNumber);
    this->SetSerialPortDefaults();
    this->SetupInterface();
}


void devSartoriusSerial::SetSerialPortDefaults(void)
{
    this->SerialPort.SetBaudRate(osaSerialPort::BaudRate1200);
    this->SerialPort.SetCharacterSize(osaSerialPort::CharacterSize7);
    this->SerialPort.SetParityChecking(osaSerialPort::ParityCheckingOdd);
    this->SerialPort.SetStopBits(osaSerialPort::StopBitsOne);
    this->SerialPort.SetFlowControl(osaSerialPort::FlowControlHardware);
}


void devSartoriusSerial::SetupInterface(void)
{
    // add weight to state table
    StateTable.AddData(this->Weight, "Weight");
    // add one interface, this will create an mtsTaskInterface
    mtsProvidedInterface * providedInterface = AddProvidedInterface("Scale");
    if (providedInterface) {
        // add command to access state table values to the interface
        providedInterface->AddCommandReadState(this->StateTable, this->Weight, "GetWeight");
    }
}


bool devSartoriusSerial::GetWeight(double & weightInGrams, bool & stable)
{
    const unsigned int bytesToRead = 16;
    unsigned int nbTrials = 10; // 10, each read is blocking for
    unsigned int bytesRead = 0;
    while ((bytesRead < bytesToRead) && (nbTrials > 0)) {
        nbTrials--;
        bytesRead += this->SerialPort.Read(this->CharBuffer + bytesRead,
                                           bytesToRead - bytesRead);
        std::cerr << "\"" << this->CharBuffer << "\"  \"" << bytesRead << std::endl;
        osaSleep(1.0 * cmn_ms); // tiny sleep
    }
    if (bytesRead < bytesToRead) {
        CMN_LOG_CLASS(6) << "GetWeight failed, not enought data" << std::endl;
        return false;
    }
    this->CharBuffer[bytesToRead] = '\0';
    std::cout << this->CharBuffer << std::endl;
    return true;
}


void devSartoriusSerial::Startup(void)
{
    if (!this->SerialPort.Open()) {
        CMN_LOG_CLASS(1) << "Startup: can't open serial port \""
                         << this->SerialPort.GetPortName() << "\"" << std::endl;
    }
}


void devSartoriusSerial::Run(void)
{
    this->ProcessQueuedCommands();

    bool stable;
    this->GetWeight(this->Weight.Data, stable); 
}
