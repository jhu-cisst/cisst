/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devNDiSerial.h 192 2009-03-25 22:12:25Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2009-03-27

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devSartoriusSerial_h
#define _devSartoriusSerial_h

//includes from cisstlibraries
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstMultiTask/mtsTaskContinuous.h>

// Always include last
#include <cisstDevices/devExport.h>

/*!
  \brief Device wrapper for Sartorius scale (model GC 2502)
  \ingroup cisstDevices
*/

class CISST_EXPORT devSartoriusSerial: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:

    /*! Set serial port parameters to match scale's defaults */
    void SetSerialPortDefaults(void);

    /*! Setup interface and state table */
    void SetupInterface(void);

    /*! "System" Commands: */
    void SendPrintToggle(void);
    bool GetWeight(double & weightInGrams, bool & stable);
    bool GetModel(std::string & modelName);
    bool ProcessBuffer(void);
    typedef const char * const_char_pointer;
    void UpdateStateTable(const const_char_pointer & buffer);

    /*! Placeholder for last weigth read */
    cmnDouble Weight;

    /*! Replies are limited to 16 chars, to be tested */
    enum {BUFFER_SIZE = 512};
    char BytesReadSoFar[BUFFER_SIZE];
    unsigned int NbBytesReadSoFar;
    char TempBuffer[BUFFER_SIZE];

    /*! Serial port instance */
    osaSerialPort SerialPort;

public:
    
    devSartoriusSerial(const std::string & taskName,
                       const std::string & serialPortName);
    devSartoriusSerial(const std::string & taskName,
                       unsigned int serialPortNumber);

    // all four methods are pure virtual in mtsTaskContinous
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {}; // user defined cleanup
    
};

CMN_DECLARE_SERVICES_INSTANTIATION(devSartoriusSerial);

#endif //_devSartoriusSerial_h_
