/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Eric Lin, Joseph Vidalis
  Created on: 2008-09-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devNDiSerial_h
#define _devNDiSerial_h

//includes from cisstlibraries
#include <cisstMultiTask.h>
#include <cisstCommon.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstParameterTypes.h>

//includes from c++ standard libraries
#include <string>
#include <queue>
#include <vector>
#include <map>


#define MAX_TOOLS 20
#define L_POL_RESET 10
#define L_POL_OK 9


// Forward declaration for this stupid class to remove
class Tool;


/*!
  \brief A class non-block Polaris task
  \ingroup cisstTracker
  This class is an extension of the mtsTaskPeriodic class. Its goal
  is to provide an user a nonblocking way to retrieve positional data
  from a Polaris tracking device.
*/

class devNDiSerial: public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
    enum SystemStatus {
        SETUP,
        DIAG,
        TRACKING,
    };

    // here we list out the various usercommands that the user has access to, 2nd level commands
    enum UserCommand {
        RESET,
        CHANGECOMM,
        TRACK,
        DISCOVER,
        USERINIT,
        BEEP,
        USRSTOP,
        USRNONE,
    };

    // here we list out all the individual actions that the system can take, 1st level interactions
    enum SystemCommandStatus {
        ACQUIRE_SERIAL,
        SERIAL_RESET,
        COMMSETTINGS,
        INITIALIZE,
        PORT_INITIALIZE,
        PORT_FREE,
        PORT_ENAB,
        PORT_INITIALIZE_FIND,
        PORT_FREE_FIND,
        PORT_ENAB_FIND,
        SYSBEEP,
        RUNBX,
        RUNTX,
        STARTTRACK,
        STOPTRACK,
        SYSNONE,  
    };

    //currently unused datamemeber
    enum dataStatus {
        VALID,
        OUTOFSPACE,
        OCCUPIED,
        INITIALIZED,
        ENABLED,
        NEW,
    };

    
protected:

    /* "System" Interface: */
    /* "System" Commands: */
    void Reset(void);
    void Beep(void);
    void Track(void);
    void Discover(void);
    void Stop(void);
    /* "System" Events*/
    mtsFunctionVoid EventToolUnplugged;
    mtsFunctionVoid EventToolPlugged;
    
private:
    //used in the BX response line, unused because BX replies drop characters
    template <class _elementType>
	static inline void PointerToValue(char * & pointer, _elementType & placeHolder) {
        placeHolder = *(reinterpret_cast<_elementType *>(pointer));
        pointer += sizeof(_elementType);
    }
    
    // used in and modified by Reply(int i)
    char ReplyBuffer[1024];               // generic character buffer for storing responses from tracker, buffer is overestimating max
    char SerialBuffer[256];               // buffer used in the reply class for copying from serial port at each instance
    char PortHandleResponseBuffer[256];   // used to store the replies in the cases of port handling commands: pinit,phf,pena. 
                                          // necessary because of reply-run structure
    int ReplyIndex;                       // used in Reply to keep track of cumulative size of response between calls to Reply(int)
    int NumPorts;                         // keeps track of number of ports during port handling commands, reply-run structure
    int PortIndex;                        // keeps track of current port handle num
    int IndefinateReplyLength;            // keeps track of size of Reply(int) we are expecting
  

    // status flags
    bool StatChangePortOccupied;          // keeps track of status bit after each TX
    bool StatChangePortUnoccupied;        // keeps track of status bit after each TX
    bool FlagSTANDBY;                     // keeps track of status of machine, interacts to help timeoutCounter function properly
    bool FlagBXREPLY;                     // indicates whether or not BX reply is called for
    bool FlagTXREPLY;                     // indicates whether or not TX reply is called for
    bool FlagAutoDiscoverSent;            // tries to make sure Discover only gets sent once per AutoDiscover

    // used for Timeout determination
    int TimeoutCounter;                   // running TimeOut counter, incremented every run, reset every SendCommand
    int NumTimeoutCycles;                 // number of timeoutcycles before timeout is declared, set by SetTimeout
    int NumWaitCycles;
    int NumWaitCyclesTester;
    
    // state machine data members
    std::queue<UserCommand> UserCommandQueue;             // queue of commands at the User level, only touched by ProcessNextCommand()
    std::queue<SystemCommandStatus> SystemCommandQueue;   // queue of commands at the System level, only touched by ProcessNextCommand()
    SystemStatus SystemStatusInstance;                    // keeps track of the Status of the System: Diagnostic, Tracking, Setup
    SystemCommandStatus SystemCommand;                    // keeps track of SystemCommands, needs to be set to sysnone
    SystemCommandStatus RunStatus;                        // keeps track of the runstate between methods
    
    // serial port
    osaSerialPort serialPort;     //used to communicate with serial port

    // task functions
    void Sleep(double timeInSeconds);                     // standard sleep, non-blocking
    void SetTimeout(double timeInSeconds);                // sets number of cycles before timeout
    void ProcessNextCommand();                            // determines next state of statemachine
  
    // command functions
    void CallInit();                                      // sends init command and changes run state
    void FreePorts(std::string phsrReply, int i);         // sends phf command and changes run state
    void FreePortsFind();                                 // sends phsr command and determines number of ports to free
    void InitPorts(std::string phsrReply, int i);         // sends pinit command and changes run state
    void InitPortsFind();                                 // sends phsr command and determines number of ports to init
    void EnablePorts(std::string phsrReply, int i);       // sends pena command and changes run state
    void EnablePortsFind();                               // sends phsr command and determines number of ports to enable
    void ResetBreak(void);                                // sends a serial break and changes run state
    void GetSerialNumber(std::string phsrReply, int i);   // reconciles port handle to device serial
    void StartTrack();                                    // sends tstart command and changes runstate
    void StopTrack();                                     // sends tstop command and changes runstate
    void RunBX();                                         // not used, sends BX command and changes runstate, also changes flag
    void ProcessBX();                                     // processes reply after a RunBX call
    void RunTX();                                         // sends TX command and changes runstate, also changes flag
    void ProcessTX();                                     // processes reply after a RunTX call

    bool Reply(int expected);                             // waits for serial port read to finish, non-blocking, stores in ReplyBuffer

    bool ResponseOKAY(char* serialBuf);                   // determines whether or not reply is OKAY, used for freeports, initports and 
                                                          // Enableports, StartTrack
    bool SetCommSettings(osaSerialPort::BaudRateType baud,
                         osaSerialPort::ParityCheckingType parity,
                         osaSerialPort::StopBitsType stopBits,
                         osaSerialPort::FlowControlType flowControl);

    void SystemBeep();                                    // sends a beep of length 2, if you want more, write another 
                                                          // function and enum and link it all up

    cmnBool getResponse(char *buffer, int maxLen);        // unique blocking reply, used in setCommSettings
  
    inline void SendCommand(const std::string & command) {    // used to mask the serialport write command
        TimeoutCounter = 0;
        this->serialPort.Write(command);
    }

    void ParseFile(const std::string & filename);       // used in port handle map to tool and other things, parses config file
    void ConfigureToolInterface(Tool *tool);          // used in port handle map to tool
    
    // Private Variables
    Tool *Tools[MAX_TOOLS];
    std::map<std::string, Tool*> MapSerialToTool;
    std::map<int, Tool*> MapPortToTool;

public:
    
    // provide a name for the Polaris and define the frequency (time
    // interval between calls to the periodic Run).  Also used to
    // populate the interface(s)
    devNDiSerial(const std::string & polarisName, double period);
    // all four methods are pure virtual in mtsPolaris
    void Configure(const std::string & filename);
    void Startup(void);    // Set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup
    
};


CMN_DECLARE_SERVICES_INSTANTIATION(devNDiSerial);





// include for the whole cisstMultiTask library
#include <cisstMultiTask.h>
#include <cisstCommon.h>
#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstParameterTypes.h>	

#include <cisstDevices/devNDiSerialToolInformationGet.h>

#include <string>

class Tool
{
public:
    Tool(std::string name, std::string serial);  
    Tool(void);
    
    // Position Information
    prmPositionCartesianGet PositionCartesian;
    // Position Information Data
    void SetTranslation(double x, double y, double z);
    void SetRotation(double a, double b, double c, double d);
    // ADV - not used. had bugs.  void SetTranslationAndQuaternion(double x, double y, double z, double a, double b, double c, double d);
    void SetQuaternionAndTranslation(double a, double b, double c, double d, double x, double y, double z);
    prmPositionCartesianGet GetPosition(void);
    void SetPosition(prmPositionCartesianGet position);
    
    // ToolInformation
    devNDiSerialToolInformationGet ToolInformation;
    // Tool Identifing information
    void SetName(std::string name);
    std::string GetName(void);
    void SetSerialNumber(std::string serial);
    std::string GetSerialNumber(void);
    // Tool Information Data
    void SetError(double error);
    double GetError(void);
    // Tool Information Flags
    void SetDisabled(bool flag);
    bool GetDisabled(void);
    void SetInitialized(bool flag);
    bool GetInitialized(void);
    void SetEnabled(bool flag);
    bool GetEnabled(void);
    void SetMissing(bool flag);
    bool GetMissing(void);
    void SetOutOfVolume(bool flag);
    bool GetOutOfVolume(void);
    void SetPartiallyOutOfVolume(bool flag);
    bool GetPartiallyOutOfVolume(void);

    
protected:
};

#endif //_devNDiSerial_h_

