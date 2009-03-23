/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: client.cpp,v 1.4 2007/05/31 21:05:46 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2005-05-04
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


// include what is needed from cisst
#include <cisstCommon.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstOSAbstraction/osaSleep.h>

// system includes
#include <iostream>


// main function
int main(void) {
    // add cout for all log
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, 10);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(10);
    cmnClassRegister::SetLoD("osaSerialPort", 10);

    osaSerialPort serialPort;
    // serialPort.SetPortNumber(1);
    serialPort.SetPortName("/dev/tty.KeySerial1");
    serialPort.SetBaudRate(osaSerialPort::BaudRate1200);
    serialPort.SetCharacterSize(osaSerialPort::CharacterSize7);
    serialPort.SetParityChecking(osaSerialPort::ParityCheckingOdd);
    serialPort.SetTwoStopBits(false);
    serialPort.SetHardwareFlowControl(false);

    if (!serialPort.Open()) {
        std::cout << "Sorry, can't open serial port: " << serialPort.GetPortName() << std::endl;
        return 0;
    }

    char buffer[512];
    serialPort.Write("\x1bP", 2);
    osaSleep(1.0 * cmn_s);
    serialPort.Write("\x1bP", 2);
    while (true) {
        osaSleep(100.0 * cmn_ms);
        serialPort.Read(buffer, 512);
    }
    return 0;
}

