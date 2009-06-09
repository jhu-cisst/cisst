/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSartoriusSerial.cpp 192 2009-03-25 22:12:25Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2009-04-01

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

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
    this->NbBytesReadSoFar = 0;
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
    CMN_LOG_CLASS_RUN_DEBUG << "GetWeight: entering method" << std::endl;
    unsigned int nbTrials;
    nbTrials = 10;
    bool enoughData = false;

    while ((!enoughData) && (nbTrials > 0)) {
        nbTrials--;
        // add whatever we can read to main buffer, read potentialy as
        // many characters as space left in buffer
        if (this->NbBytesReadSoFar < BUFFER_SIZE) {
            this->NbBytesReadSoFar +=
                this->SerialPort.Read(this->BytesReadSoFar + this->NbBytesReadSoFar,
                                      BUFFER_SIZE - this->NbBytesReadSoFar);
            CMN_LOG_CLASS_RUN_DEBUG << "GetWeight: buffer now contains "
                                    << this->NbBytesReadSoFar << " characters"
                                    << std::endl;
            enoughData = this->ProcessBuffer();
        } else {
            CMN_LOG_CLASS_RUN_ERROR << "GetWeight: buffer is full" << std::endl;
        }
        osaSleep(1.0 * cmn_ms); // tiny sleep
    }
    if (!enoughData) {
        CMN_LOG_RUN_VERBOSE << "GetWeight failed, not enought data" << std::endl;
        return false;
    }
    return true;
}


bool devSartoriusSerial::ProcessBuffer(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "ProcessBuffer: entering method" << std::endl;
	static char toProcess[16];
    // we need at least 16 characters to start any processing
    if (this->NbBytesReadSoFar < 16) {
        return false;
    }
    // look for eof as "CR" (\r) "LF" (\n)
    int index;
    int notYetProcessedBytes;
    bool eolFound;
    
    eolFound = false;
    index = 0;
    
    // try to find 'eol'
    while ((index < (this->NbBytesReadSoFar - 1))
           && (!eolFound)) {
        if ((this->BytesReadSoFar[index] == '\r')
            && (this->BytesReadSoFar[index + 1] == '\n')) {
            // found eol, process if enough characters before eol
            // and in any case, remove the characters up to 'eol'
            // just found.  process is enough characters
            eolFound = true;
            CMN_LOG_CLASS_RUN_DEBUG << "ProcessBuffer: found eol in buffer at position " << index << std::endl;
            // test if the buffer already has 16 bytes
            if (index < 14) {
                CMN_LOG_RUN_VERBOSE << "ProcessBuffer: partial message found in buffer, this should not happen except during initialization"
                                    << std::endl;
            } else {
                // get all 16 chars to be processed and removed from buffer
                memcpy(toProcess, /* destination buffer */
                       this->BytesReadSoFar + index - 14, /* begining of message from index */
                       16 * sizeof(char));
                CMN_LOG_CLASS_RUN_DEBUG << "ProcessBuffer: bytes to process \"" << toProcess << "\"" << std::endl;
                UpdateStateTable(toProcess);
            }
            // copy what's left from buffer in temporary buffer
            notYetProcessedBytes = this->NbBytesReadSoFar - (index + 2);
            if (notYetProcessedBytes < 0) {
                CMN_LOG_CLASS_RUN_ERROR << "ProcessBuffer: negative number of bytes in buffer ("
                                        << notYetProcessedBytes << "), this should never happen" << std::endl;
                
            } else {
                memcpy(this->TempBuffer,
                       this->BytesReadSoFar + index + 2, /* start after end of message */
                       notYetProcessedBytes);
                // copy back and update number of bytes read
                memcpy(this->BytesReadSoFar, this->TempBuffer, notYetProcessedBytes);
                this->NbBytesReadSoFar = notYetProcessedBytes;
            }
        }
        index++;
    }
    return false;
}


void devSartoriusSerial::UpdateStateTable(const const_char_pointer & buffer)
{
    if (buffer[11] == 'g') {
        // weight is stable
    }
    std::stringstream stdBuffer;
    stdBuffer << buffer + 1;
    stdBuffer >> this->Weight;
    if (buffer[0] == '-') {
        this->Weight = - this->Weight;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "UpdateStateTable: found weight : " << this->Weight << std::endl;
}


void devSartoriusSerial::Startup(void)
{
    if (!this->SerialPort.Open()) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: can't open serial port \""
                                 << this->SerialPort.GetPortName() << "\"" << std::endl;
    }
}


void devSartoriusSerial::Run(void)
{
    this->ProcessQueuedCommands();

    bool stable;
    this->GetWeight(this->Weight.Data, stable); 
    osaSleep(1.0 * cmn_ms); // way smaller than delay from scale
}
