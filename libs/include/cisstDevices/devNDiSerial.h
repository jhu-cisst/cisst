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
  \brief C++ wrapper for NDI Polaris optical tracker
  \ingroup cisstDevices

  This class is an extension of the mtsTaskPeriodic class. Its goal
  is to provide an user a nonblocking way to retrieve positional data
  from a Polaris tracking device.

  In discovery mode poll the port handle for its serial and put it into int form.
  Then Set up the Map like this:
  int serial;
  MapPortToTool[portNumber] = MapSerialToTool[serial];

  To Use a Port handle to Set data for an interface:
  MapPortToTool[portNumber]->SetTranslationAndPosition(x,y,z,a,b,c,d);

  When Freeing a port Number:
  MapPortToTool[portNumber] = NULL;

  \todo Support for 14400bps, 921600bps and 1228739bps baud rates in osaSerialPort might be required
  \todo Buffer overflow checking for SendCommand()
  \todo Handle returning error values?
  \todo Compare the strings themselves as well as their CRCs
  \todo Need another CheckResponse(unsigned int n numberOfCharacters), returns true if read n characters and CRCs are good (need a timeout).
        SerialPortBuffer is populated with return value. The current CheckResponse calls this.

  \bug Don't use _snprintf
*/

#ifndef _devNDiSerial_h
#define _devNDiSerial_h

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstMultiTask/mtsTaskContinuous.h>


class devNDiSerial : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    devNDiSerial(const std::string & polarisName, const std::string & serialPort);

    void Configure(const std::string & filename) {};
    void Startup(void) {};
    void Run(void);
    void Cleanup(void) {};

protected:
    enum { BUFFER_SIZE = 512 };

    bool ResetSerialPort(void);
    unsigned int ComputeCRC(const char * data);
    bool CheckResponse(const char * expectedResponse, double timeOut = 3.0 * cmn_s);

    void CommandInitialize(void);
    void CommandAppend(const char command);
    void CommandAppend(const char * command);
    void CommandAppend(const unsigned int command);
    bool CommandSend(void);

    bool SetSerialPortSettings(osaSerialPort::BaudRateType baudRate,
                               osaSerialPort::CharacterSizeType characterSize,
                               osaSerialPort::ParityCheckingType parityChecking,
                               osaSerialPort::StopBitsType stopBits,
                               osaSerialPort::FlowControlType flowControl);
    bool Beep(unsigned int numberOfBeeps);

    osaSerialPort SerialPort;
    char SerialPortBuffer[BUFFER_SIZE];
    char * SerialPortBufferPointer;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDiSerial);

#endif  //_devNDiSerial_h_
