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


/*
In discovery mode poll the port handle for its serial and put it into int form.
Then Set up the Map like this:
int serial;
MapPortToTool[portNumber] = MapSerialToTool[serial];

To Use a Port handle to Set data for an interface:
MapPortToTool[portNumber]->SetTranslationAndPosition(x,y,z,a,b,c,d);

When Freeing a port Number:
MapPortToTool[portNumber] = NULL;


*/


#include <cisstDevices/devNDiSerial.h>

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <string>
#include <string.h> // for memcpy, strlen

#if (CISST_OS == CISST_WINDOWS)
typedef signed char int8_t;
#endif

void getBitFromShort(unsigned int in,bool* bitlist,int arraysize) {
    for (int i = 0;i<arraysize;i++) {
        bitlist[i] = ((in%2)==1);
        in = in/2;
    }
}

void forwardCharArr(char* charval,char* ctemp,int begin,int length) {
    
    //    char tempstring[length];    
    for (int i = 0;i<length;i++) {
        ctemp[i] = (charval[begin+i]);
    }
    //strcpy(ctemp,tempstring);
}

unsigned short bin2Short(char* charval,int begin) { 
    char ctemp[2];
    forwardCharArr(charval,ctemp,begin,2);
    unsigned short ret = *(reinterpret_cast<const unsigned short*>(ctemp)); 
    return ret;
}

int hexToInt(char* in, int start, int length)
{
    int tot = 0;
    for (int i = 0; i < length; i++) {
        int val = 0;
        switch(in[i]) {
        case 'A':
        case 'a':
            val = 10;
            break;
        case 'B':
        case 'b':
            val = 11;
            break;
        case 'C':
        case 'c':
            val = 12;
            break;
        case 'D':
        case 'd':
            val = 13;
            break;
        case 'E':
        case 'e':
            val = 14;
            break;
        case 'F':
        case 'f':
            val = 15;
            break;
        default:
            val = in[i]-'0';
        }
        tot = tot*16 + val;
    }
    return tot;
}

/** stringToInt(char* hex, int start, int length)
 * Returns the int representation of the char*
 * 
 * Input char* : String of int
 * Input int : Start Position
 * Input int : Length of Read
 * Output string : Integer Representation
 */
unsigned int stringToInt(char* in, int start, int length)
{
    unsigned int ret;
    std::string tempstring(in);
    std::string substring = tempstring.substr(start,length);
    std::stringstream ss(substring); 
    ss >> ret;
    return ret;
}


unsigned int stringToInt(std::string tempstring, int start, int length)
{
    unsigned int ret;
    std::string substring = tempstring.substr(start,length);
    std::stringstream ss(substring); 
    ss >> ret;
    return ret;
}


/** stringToDouble(char* hex, int start, int length)
 * Returns the double representation of the char*
 * 
 * Input char* : String of double
 * Input int : Start Position
 * Input int : Length of Read
 * Output double : Integer Representation
 */
double stringToDouble(char* in, int start, int length)
{
    double ret;
    std::string rest;
    std::string tempstring(in);
    std::string substring = tempstring.substr(start,length);
    std::stringstream ss(substring); 
    ss >> ret;
    return ret;
}


/** CalcCRC(char *CRCdata)
 *	Calculates the CRC for a Polaris from the given input
 *
 * Input char* : Command to Polaris
 * Output unsigned : CRC
 */
unsigned CalcCRC(char *CRCdata)
{
    static unsigned char oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
    unsigned Data, uCrc = 0;
    unsigned char *puch = (unsigned char *)CRCdata;
    
    while (*puch) {
        Data = (*puch ^ (uCrc & 0xff)) & 0xff;
        uCrc >>= 8;
	
        if (oddparity[Data & 0x0f] ^ oddparity[Data >> 4]) {
            uCrc ^= 0xc001;
        }
	
        Data <<= 6;
        uCrc ^= Data;
        Data <<= 1;
        uCrc ^= Data;
        puch++;
    }
    
    return uCrc;
}





#include <iostream>
using std::cout;
using std::endl;
using std::ios;
using std::string;
#include <vector>
using std::vector;
#include <algorithm>
#include <sstream>
#include <fstream>
using std::ifstream;
#include <queue>
using std::queue;
// #include "functions.h"


CMN_IMPLEMENT_SERVICES(devNDiSerial);

devNDiSerial::devNDiSerial(const string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // constructor, used to set up serial pot information, the setcommsettings is used just to make sure of equality and to up the baud
    serialPort.SetPortNumber(1);
    serialPort.SetFlowControl(osaSerialPort::FlowControlHardware);
    if (!serialPort.Open())  {
        CMN_LOG_CLASS_INIT_ERROR << "Sorry, can't open serial port: " << serialPort.GetPortName() << std::endl;
    }
    SetTimeout(8.0 * cmn_s); //Set the timeout at one second
    SetCommSettings(osaSerialPort::BaudRate115200, osaSerialPort::ParityCheckingEven,
                    osaSerialPort::StopBitsTwo, osaSerialPort::FlowControlHardware);
}



void devNDiSerial::Configure(const std::string & filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: Using file \"" << filename << "\"" << std::endl;

    /* Configure "System" Interface */
    mtsProvidedInterface * providedInterface = AddProvidedInterface("System");
    if (providedInterface) {
        // Add commands to "System" interface
        providedInterface->AddCommandVoid(&devNDiSerial::Reset, this, "Reset");
        providedInterface->AddCommandVoid(&devNDiSerial::Beep, this, "Beep");
        providedInterface->AddCommandVoid(&devNDiSerial::Track, this, "Track");
        providedInterface->AddCommandVoid(&devNDiSerial::Discover, this, "Discover");
        providedInterface->AddCommandVoid(&devNDiSerial::Stop, this, "Stop");
        // Add Events to "System" interface
        EventToolUnplugged.Bind(providedInterface->AddEventVoid("EventToolUnplugged"));
        EventToolPlugged.Bind(providedInterface->AddEventVoid("EventToolPlugged"));
        
        /* Configure Tool Interfaces */
        ParseFile(filename);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Configure: can not add provided interface \"System\"" << std::endl;
    }
}


void devNDiSerial::Reset(void)
{
    // used to link polaris class to other multitask classes.  These
    // are basically accessors reset is paired with userinit simply to
    // mask the userinit phase, may be able to use USERINIT as a
    // separate soft-reset
    UserCommandQueue.push(devNDiSerial::RESET);
    UserCommandQueue.push(devNDiSerial::USERINIT);
    UserCommandQueue.push(devNDiSerial::USRNONE);
    ProcessNextCommand(); 
}

void devNDiSerial::Beep(void)
{
    // used to link polaris class to other multitask classes.  These
    // are basically accessors
    UserCommandQueue.push(devNDiSerial::BEEP);
    ProcessNextCommand(); 
}


void devNDiSerial::Track(void)
{
    // used to link polaris class to other multitask classes.  These
    // are basically accessors
    UserCommandQueue.push(devNDiSerial::TRACK);
    ProcessNextCommand(); 
}


void devNDiSerial::Discover(void)
{
    // used to link polaris class to other multitask classes.  These
    // are basically accessors
    UserCommandQueue.push(devNDiSerial::DISCOVER);
    UserCommandQueue.push(devNDiSerial::USRNONE);
    ProcessNextCommand(); 
}


void devNDiSerial::Stop(void)
{
    // used to link polaris class to other multitask classes.  These
    // are basically accessors
    UserCommandQueue.push(devNDiSerial::USRSTOP);
    ProcessNextCommand(); 
}


void devNDiSerial::ConfigureToolInterface(Tool * tool)
{
    mtsProvidedInterface * providedInterface = AddProvidedInterface(tool->GetName());
    
    if (providedInterface) {
        //Configure getPositionCartesian
        const std::string namePositionCartesian = (tool->GetName() + "PositionCartesian");
        this->StateTable.AddData(tool->PositionCartesian, namePositionCartesian);
        providedInterface->AddCommandReadState(this->StateTable, tool->PositionCartesian, "GetPosition");
        
        //Configure getToolInformation
        const std::string nameToolInformation = (tool->GetName() + "ToolInformation");
        this->StateTable.AddData(tool->ToolInformation, nameToolInformation);
        providedInterface->AddCommandReadState(this->StateTable, tool->ToolInformation, "GetToolInformation");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureToolInterface: can not add provided interface for tool \""
                                 << tool->GetName() << "\"" << std::endl;
    }
}


void devNDiSerial::Sleep(double timeInSeconds)
{
    // ADV - this should be updated to use milliseconds.
    // uses period to approximate number of polaris cycles to the realtime duration
    this->NumWaitCycles +=
        static_cast<int>(timeInSeconds / (this->Period))+1;
}


void devNDiSerial::SetTimeout(double timeInSeconds)
{
    // uses period to approximate number of polaris cycles to the
    // realtime duration of timeout
    this->NumTimeoutCycles =
        static_cast<int>(timeInSeconds / (this->Period))+1;
    CMN_LOG_CLASS_INIT_DEBUG << "TimeOutLimit Set at: " << this->NumTimeoutCycles
                             << std::endl;
}


void devNDiSerial::ParseFile(const std::string & filename)
{
    // parse the configure file to get serial numbers of active tools
    // used, so they can be later mapped to created tools
    
    CMN_LOG_CLASS_INIT_VERBOSE << "Parsing File \"" << filename << "\"" << std::endl;
    int i;
    //Open File
    ifstream configurationFile(filename.c_str(), ios::in);
    if (!configurationFile) {
        CMN_LOG_CLASS_INIT_ERROR << "Could not find/open file \"" << filename << "\"" << std::endl;
    }
    
    // Tracker Type and Number of Tools
    string tracker;
    int number;
    configurationFile >> tracker >> number;
    CMN_LOG_CLASS_INIT_VERBOSE << "Tracker Type: " << tracker << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Number of Tools: " << number << std::endl;
    
    //Read All Tools and Configure MapSerialToTool and Tools
    string name;
    string serial;
    //Vectors for Unique Naming
    vector<string> nameVector;
    vector<string> serialVector;
    
    for (i = 0; i < number; i++) {
        configurationFile>>name>>serial;
	
        //Check if name or serial number have been used. If used no interface 
        // or tool will be created with that name and a Warning will be thrown.
        if (binary_search(nameVector.begin(), nameVector.end(), name)) {
            CMN_LOG_CLASS_INIT_WARNING << "Warning: Tool Name: " << name << " already used." << endl;
        } else if (binary_search(serialVector.begin(), serialVector.end(), serial)) {
            CMN_LOG_CLASS_INIT_WARNING << "Warning: Serial Number: " << serial << " already used." << endl;
        } else {
            CMN_LOG_CLASS_INIT_VERBOSE << "Adding Tool Name: " << name << " Serial: " << serial << endl; 
            Tools[i] = new Tool(name, serial);
            Tools[i]->SetDisabled(true);
            Tools[i]->SetInitialized(false);
            Tools[i]->SetEnabled(false);
            Tools[i]->SetMissing(false);
            Tools[i]->SetOutOfVolume(false);
            Tools[i]->SetPartiallyOutOfVolume(false);
            ConfigureToolInterface(Tools[i]);
            //Add to Vectors for Unique Naming
            nameVector.push_back(name);
            serialVector.push_back(serial);
            MapSerialToTool[serial] = Tools[i];//Add to Map
        }
    }
}


void devNDiSerial::CallInit(void)
{
    //  calls the Init function of the polaris machine soft reset?
    SendCommand("init \r");
    RunStatus = INITIALIZE;
}


void devNDiSerial::InitPortsFind()
{
    //  Finding Number of Port Handles to be initialized
    SendCommand("phsr 02\r");
    RunStatus = PORT_INITIALIZE_FIND;
}


void devNDiSerial::InitPorts(string phsrReply, int i)
{
    //  don't have an adequate redundancy check yet to see if
    //  InitPortsFind() has been called yet but that call is necessary
    //  parses the phsrReply which is a copy of ReplyBuffer and grabs
    //  the porthandle(2characters) from the appropriate part of that
    //  string depending on int i (index). gets called same # of port
    //  handles based on structure of Run()
    
    std::stringstream commandStream; 
    if (NumPorts>0) {
        commandStream << "pinit "
                      << phsrReply.substr(2 + i*5, 2) //first 2 chars are number of porthandles index 2 and 3 is porthandle 1...
                      << "\r";
        CMN_LOG_CLASS_INIT_DEBUG << phsrReply << std::endl;
        SendCommand(commandStream.str());
    }else{
        CMN_LOG_CLASS_INIT_DEBUG << "InitPorts(): No Ports" << endl;
    } 
    RunStatus = PORT_INITIALIZE;
}


void devNDiSerial::EnablePortsFind()
{
    //Finding Number of Port Handles to be enabled
    SendCommand("phsr 03\r");
    RunStatus = PORT_ENAB_FIND;
}


void devNDiSerial::GetSerialNumber(std::string phsrReply, int i)
{
    //  the command phinf has a very limited scope of usage... must be done with port initialization fully 
    std::stringstream commandStream; 
    if (NumPorts>0) {
        commandStream << "phinf "
                      << phsrReply.substr(2 + i*5, 2)
                      << "\r";
        CMN_LOG_CLASS_INIT_DEBUG << "GetSerialNumber():"<< phsrReply.substr(2 + i*5, 2) << std::endl;
        SendCommand(commandStream.str());
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GetSerialNumber(): No portHandles"<<endl;
    } 
    RunStatus = ACQUIRE_SERIAL;
}


void devNDiSerial::EnablePorts(string phsrReply, int i)
{
    //  enables ports, requires the EnablePortsFind(). Look to InitializePorts(string phsrReply, int i)
    std::stringstream commandStream; 
    if (NumPorts>0) {
        commandStream << "pena "
                      << phsrReply.substr(2 + i*5, 2)
                      << "D\r";
        CMN_LOG_CLASS_INIT_DEBUG << phsrReply << std::endl;
        SendCommand(commandStream.str());
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "EnablePorts(): No Ports"<<endl;
    } 
    RunStatus = PORT_ENAB;
}


void devNDiSerial::FreePortsFind()
{
    //Finding Number of Port Handles to be initialized
    SendCommand("phsr 01\r");
    RunStatus = PORT_FREE_FIND;
}


void devNDiSerial::FreePorts(string phsrReply, int i)
{
    //  frees ports, requires the FreePortsFind(), attempts to free unused porthandles. Look to InitializePorts(string phsrReply, int i)
    std::stringstream commandStream; 
    if (NumPorts>0) {
        commandStream << "phf "
                      << phsrReply.substr(2 + i*5, 2)
                      << "\r";
        CMN_LOG_CLASS_INIT_DEBUG << phsrReply << std::endl;
        SendCommand(commandStream.str());
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "FreePorts(): No Ports"<<endl;
    } 
    RunStatus = PORT_FREE;
}


void devNDiSerial::Startup(void) 
{
    // steps to allow for automatic tracking
    RunStatus = devNDiSerial::SYSNONE;
    SystemCommand = devNDiSerial::SYSNONE;
    UserCommandQueue.push(devNDiSerial::RESET);
    UserCommandQueue.push(devNDiSerial::USERINIT);
    UserCommandQueue.push(devNDiSerial::DISCOVER);
    UserCommandQueue.push(devNDiSerial::TRACK);
    ProcessNextCommand();
}


void devNDiSerial::ResetBreak(void) {
    serialPort.WriteBreak(0);   // hard resets are written with a serialbreak
    this->Sleep(100 * cmn_ms); // 100 ms as recommended in Polaris API doc.
    RunStatus = SERIAL_RESET;
}


void devNDiSerial::SystemBeep() {
    SendCommand("beep 2\r");  // 2 is an arbitrary pick
    RunStatus = SYSBEEP;
}


void devNDiSerial::RunBX() {
    SendCommand("bx 0001\r"); //0001 asks for all information other options less so
    RunStatus = RUNBX;
}


void devNDiSerial::RunTX() {
    SendCommand("tx 0001\r"); //0001 asks for all information other options less so
    RunStatus = RUNTX;
}


void devNDiSerial::ProcessTX() {
    
    int numtools = 0;  
    int handlenum = 0;
    double sign[8] = {1,1,1,1,1,1,1,1};
    double p[8];
    char portStatusCharacter;
    char systemStatusCharacter;
    int i;
    int j;
    
    numtools = stringToInt(ReplyBuffer, 0, 2); // number of porthandles represented in reply is located here in text representation
    int offsetindex = 2;
    for (i = 0; i < numtools; i++) {
        handlenum = stringToInt(ReplyBuffer,offsetindex,2); //first part of 0001 reply
        if (ReplyBuffer[offsetindex+2] == 'M') {
            // Tool Is Missing, may be better way to check, but this works well enough
            //Set Flags
            portStatusCharacter = ReplyBuffer[offsetindex + 9 + 6];        
            MapPortToTool[handlenum]->SetDisabled(false);
            MapPortToTool[handlenum]->SetMissing(true);
            MapPortToTool[handlenum]->SetInitialized(true);
            MapPortToTool[handlenum]->SetEnabled(true);
	    
            if (portStatusCharacter == '3') {
                // Object Can Not Be Detected At All, based on hex values of 4 bits
                MapPortToTool[handlenum]->SetOutOfVolume(false);
                MapPortToTool[handlenum]->SetPartiallyOutOfVolume(false);
            } else if (portStatusCharacter == '7') {
                // Object is close to Volume
                MapPortToTool[handlenum]->SetOutOfVolume(true);
                MapPortToTool[handlenum]->SetPartiallyOutOfVolume(false);
            } else if (portStatusCharacter == 'B') {
                // Object is even closer to Volume
                MapPortToTool[handlenum]->SetOutOfVolume(true);
                MapPortToTool[handlenum]->SetPartiallyOutOfVolume(true);
            } else {
                //"FUBAR"
            }
            offsetindex+=26; // amount of characters taken up by a missing tool
        } else if (ReplyBuffer[2+offsetindex] == 'D') {
            MapPortToTool[handlenum]->SetDisabled(true);
            offsetindex+=11; // amount of characters taken up by a disabled tool
        } else {
            MapPortToTool[handlenum]->SetDisabled(false);
            MapPortToTool[handlenum]->SetMissing(false);
            MapPortToTool[handlenum]->SetInitialized(true);
            MapPortToTool[handlenum]->SetEnabled(true);
            MapPortToTool[handlenum]->SetOutOfVolume(false);
            MapPortToTool[handlenum]->SetPartiallyOutOfVolume(false);
            for (j = 0; j < 5; j++) {
                // text replies are ugly, grabs signs from middle of ReplyBuffer, 2 forloops because spacing changes            
                if (ReplyBuffer[offsetindex+2+j*6] == '-') {
                    sign[j] = -1.0;
                } else {
                    sign[j] = 1.0;
                }
            }
            for (j = 0; j < 3; j++) {
                if (ReplyBuffer[offsetindex+33+j*7] == '-') {
                    sign[j+5] = -1.0;
                } else {
                    sign[j+5] = 1.0;
                }
            }
    
            //  changes the text representations into appropriate doubles, for loops for spacing and magnitude shifts, combine values
            //  with signs[8] to get negative positive, store in an array of doubles
            p[0] = stringToDouble(ReplyBuffer,3+offsetindex,5)/10000.0;
            for (j = 1;j<=3;j++) {
                p[j] = stringToDouble(ReplyBuffer, 3+6*j+offsetindex,5) * sign[j] / 10000.0;
            }
            for (j = 0;j<3;j++) {
                p[j+4] = stringToDouble(ReplyBuffer,27+7*j+offsetindex,6) * sign[j+4] / 100.0;
            }
            p[7] = stringToDouble(ReplyBuffer,48+offsetindex,5) * sign[7] / 10000.0;
	    
            // call the tool function to change it's values.  
            MapPortToTool[handlenum]->SetQuaternionAndTranslation(p[0],p[1],p[2],p[3],p[4],p[5],p[6]);
            MapPortToTool[handlenum]->SetError(p[7]);
            offsetindex+=70; // offset for a trackable tool
        }

    }  
    systemStatusCharacter = ReplyBuffer[offsetindex + 2];  
    // systemcharacter determines  plugged in and unplugged.
    // using hex to binary conversion table online
    
    if (systemStatusCharacter == '4'||
        systemStatusCharacter == '5'||
        systemStatusCharacter == '6'||
        systemStatusCharacter == '7'||
        systemStatusCharacter == 'C'||
        systemStatusCharacter == 'D'||
        systemStatusCharacter == 'E'||
        systemStatusCharacter == 'F') {
        StatChangePortOccupied = true;
    } else {
        StatChangePortOccupied = false;
    }

    if (systemStatusCharacter == '8'||
        systemStatusCharacter == '9'||
        systemStatusCharacter == 'A'||
        systemStatusCharacter == 'B'||
        systemStatusCharacter == 'C'||
        systemStatusCharacter == 'D'||
        systemStatusCharacter == 'E'||
        systemStatusCharacter == 'F') {
        StatChangePortUnoccupied = true;    
    } else {
        StatChangePortUnoccupied = false;
    }
       
    // letting the system know an events occured
    if (StatChangePortOccupied && !FlagAutoDiscoverSent) {
        EventToolPlugged();
        FlagAutoDiscoverSent = true;
    }
    if (StatChangePortUnoccupied) {
        EventToolUnplugged();
    }
}


void devNDiSerial::ProcessBX() {

    //unused right now, but basically same as above but no need to read the signs first

    char * currentPointer = ReplyBuffer;
    bool statByteArr[16];
    float temp;
    int i;
    int8_t numhandles;    
    int8_t handlenum;
    int8_t handlestat;
    float p[8];

    // numhandles = (unsigned int)(BX_BUFFER[6] & 0x0F);
    currentPointer += 6;
    PointerToValue(currentPointer, numhandles);
    //            currentPointer += 7;            
    for (i = 0;i<numhandles && numhandles != -1;i++) {
        PointerToValue(currentPointer, handlenum);
        PointerToValue(currentPointer, handlestat);
        if (handlestat == 1) {
            //8 value 4 byte floats
            for (int j = 0; j < 8 ; j++) {
                PointerToValue(currentPointer, p[j]);         
            }
            //cout << (int)numhandles << (int)handlenum <<  "(" <<p[4] << ", "<< p[5]<< ", "<< p[6] << ")" << endl;
            MapPortToTool[(int)handlenum]->SetQuaternionAndTranslation(p[0],p[1],p[2],p[3],p[4],p[5],p[6]);
            MapPortToTool[(int)handlenum]->SetError(p[7]);
	    
            //cout << MapPortToTool[(int)handlenum]->GetName() << ": " << "(" <<p[4] << ", "<< p[5]<< ", "<< p[6] << ")" << endl;
        } else if (handlestat == 2) {
            CMN_LOG_CLASS_RUN_WARNING << (int)handlenum << "MISSING" << endl;
        } else if (handlestat == 4) {
            CMN_LOG_CLASS_RUN_DEBUG << (int)handlenum << "DISABLED" << endl;
        } else{
            CMN_LOG_CLASS_INIT_ERROR << "RunBX(): handlestat is wrong or too many handles: "<< (int)handlestat << "  " <<(int)handlenum << endl; 
        }
        PointerToValue(currentPointer,temp);   
        PointerToValue(currentPointer,temp); 
    }
        
    //grabbing status bits
    unsigned short statusBytes = 0;
    PointerToValue(currentPointer,statusBytes); 
    getBitFromShort(statusBytes, statByteArr,16);    
    StatChangePortOccupied = statByteArr[6];
    StatChangePortUnoccupied = statByteArr[7];
    if (StatChangePortOccupied == 1) {
        cout << "crap plugged in" << endl;
    }      
}


bool devNDiSerial::Reply(int expectedLength) {    
    // only returns true if the reply has received the appropriate amount of bytes
    // expected length is sent for definate lengths else it assumes it knows where the appropriate information is found
    // FlagBXREPLY was set and the BX reply did hold information on how many bytes it was sending
    // TX reply does not carry information on how long the reply will be, just hope that your frequency isn't too fast

    int Replysize = serialPort.Read(SerialBuffer,256);
        
    if (FlagBXREPLY) {
        if (ReplyIndex >4)
            IndefinateReplyLength = bin2Short(ReplyBuffer,2)+8;
        CMN_LOG_CLASS_INIT_ERROR << "indefinate " << (IndefinateReplyLength -ReplyIndex) <<endl;
    } else {
        if (expectedLength > -1) {
            IndefinateReplyLength = expectedLength; 
        } else {
            if (ReplyIndex != 0) {
                IndefinateReplyLength = (hexToInt(ReplyBuffer,0,2)*5) +6;
		
            }
        }    
    }
    
    
    if (Replysize == 0 && ReplyIndex == 0) { //this could lead to a stallout, may be frequency is too fast for machine
        //    CMN_LOG_CLASS_RUN_ERROR << "Reply(): Replysize == 0 && ReplyIndex == 0 trouble " << endl;
    } else {
        if (Replysize != 0) {
            // if serialport.read has read anything in, throw it to the ReplyBuffer and change the index in ReplyIndex to match total 
            // length of string
            memcpy(ReplyBuffer+ReplyIndex,SerialBuffer,Replysize);
            ReplyIndex += Replysize;
        } else if ((ReplyIndex >= IndefinateReplyLength)) {
            return true;
        } else if (FlagTXREPLY) {
            return true;
        }
    }
    return false;
}


bool devNDiSerial::SetCommSettings(osaSerialPort::BaudRateType baud,
                                   osaSerialPort::ParityCheckingType parity,
                                   osaSerialPort::StopBitsType stopBits,
                                   osaSerialPort::FlowControlType flowControl) {
    
    char commbuf[12] = "comm 00000\r";
    char buf[256];
    bool comsettrue;
    if (baud == osaSerialPort::BaudRate9600) commbuf[5] = '0';
    if (baud == osaSerialPort::BaudRate19200) commbuf[5] = '2';
    if (baud == osaSerialPort::BaudRate38400) commbuf[5] = '3';
    if (baud == osaSerialPort::BaudRate57600) commbuf[5] = '4';
    if (baud == osaSerialPort::BaudRate115200) commbuf[5] = '5';

    if (parity == osaSerialPort::ParityCheckingNone) commbuf[7] = '0';
    if (parity == osaSerialPort::ParityCheckingEven) commbuf[7] = '1';
    if (parity == osaSerialPort::ParityCheckingOdd) commbuf[7] = '2';

    if (stopBits == osaSerialPort::StopBitsTwo)
        commbuf[8] = '1';
    else
        commbuf[8] = '0';

    if (flowControl == osaSerialPort::FlowControlHardware) 
        commbuf[9] = '1';
    else
        commbuf[9] = '0';

    SendCommand(commbuf);
    getResponse(buf,256);
    
    if (buf[0] != 'O') {
        cout << "fubar";
        return false;
    } else {
        comsettrue = true;
        osaSleep(100 * cmn_ms);
        serialPort.SetBaudRate(baud);
        serialPort.SetParityChecking(parity);
        serialPort.SetStopBits(stopBits);
        serialPort.SetFlowControl(flowControl);
        return (comsettrue && serialPort.Configure());
    }
}


void devNDiSerial::StartTrack() {
    SendCommand("tstart \r");
    RunStatus = STARTTRACK;
    SystemStatusInstance = TRACKING;
}


void devNDiSerial::StopTrack() {
    SendCommand("tstop \r");
    RunStatus = STOPTRACK;
    SystemStatusInstance = SETUP;
}


void devNDiSerial::ProcessNextCommand() {
    // assesses the appropriate steps to take in the next iteration of Run

    UserCommand userCommand;

    if (UserCommandQueue.empty()) {
        userCommand = USRNONE;
    } else {
        userCommand = UserCommandQueue.front();
        UserCommandQueue.pop();
    }

    switch (userCommand) {
        //translates usercommands to SystemCommands
    case USERINIT:
        SystemCommandQueue.push(devNDiSerial::INITIALIZE);
        break;
    case RESET:
        SystemCommandQueue.push(devNDiSerial::SERIAL_RESET);
        break;
    case CHANGECOMM:
        SystemCommandQueue.push(devNDiSerial::COMMSETTINGS);
        break;
    case TRACK:
        SystemCommandQueue.push(devNDiSerial::STARTTRACK);
        SystemCommandQueue.push(devNDiSerial::RUNTX);
        break;
    case USRSTOP:
        SystemCommandQueue.push(devNDiSerial::STOPTRACK);
        break;
    case DISCOVER:
        SystemCommandQueue.push(devNDiSerial::STOPTRACK);
        SystemCommandQueue.push(devNDiSerial::PORT_FREE_FIND);      
        SystemCommandQueue.push(devNDiSerial::PORT_FREE);      
        SystemCommandQueue.push(devNDiSerial::PORT_INITIALIZE_FIND);
        SystemCommandQueue.push(devNDiSerial::PORT_INITIALIZE);
        SystemCommandQueue.push(devNDiSerial::ACQUIRE_SERIAL);
        SystemCommandQueue.push(devNDiSerial::PORT_ENAB_FIND);
        SystemCommandQueue.push(devNDiSerial::PORT_ENAB);
        break;  
    case BEEP:
        SystemCommandQueue.push(devNDiSerial::SYSBEEP);      
        break;  
    case USRNONE:
        break;
    }

    if (!SystemCommandQueue.empty()) {
        SystemCommand = SystemCommandQueue.front();  
        SystemCommandQueue.pop();
        FlagBXREPLY = false;        
        FlagTXREPLY = false;
    } else {
        SystemCommand = SYSNONE;
    }

  
    switch (SystemCommand) {
        // calls appropriate functions and sets flags, assesses flags etc.
    case ACQUIRE_SERIAL:
        PortIndex = 0;
        GetSerialNumber(PortHandleResponseBuffer,PortIndex);
        break;

    case SERIAL_RESET:
        ResetBreak();               
        break;

    case COMMSETTINGS:
        SetCommSettings(osaSerialPort::BaudRate115200,
                        osaSerialPort::ParityCheckingNone,
                        osaSerialPort::StopBitsOne,
                        osaSerialPort::FlowControlHardware);
        break;

    case INITIALIZE:
        CallInit();
        break;

    case PORT_INITIALIZE_FIND:
        InitPortsFind();
        break;

    case PORT_INITIALIZE:
        PortIndex = 0;
        InitPorts(PortHandleResponseBuffer, PortIndex);
        break;

    case PORT_FREE_FIND:
        FreePortsFind();
        break;

    case PORT_FREE:
        PortIndex = 0;
        FreePorts(PortHandleResponseBuffer, PortIndex);
        break;

    case PORT_ENAB_FIND:
        EnablePortsFind();
        break;

    case PORT_ENAB:
        PortIndex = 0;
        EnablePorts(PortHandleResponseBuffer, PortIndex);
        break;

    case SYSBEEP:
        SystemBeep();
        if (SystemCommandQueue.empty() && (SystemStatusInstance == TRACKING)) {
            SystemCommandQueue.push(devNDiSerial::RUNTX);
        }
        break;

    case RUNBX:
        FlagBXREPLY = true;
        RunBX();
        if (SystemCommandQueue.empty() && (SystemStatusInstance == TRACKING)) {
            SystemCommandQueue.push(devNDiSerial::RUNBX);
        }
        break;

    case RUNTX:
        FlagTXREPLY = true;
        RunTX();
        if (SystemCommandQueue.empty() && (SystemStatusInstance == TRACKING)&& (UserCommandQueue.empty())) {
            SystemCommandQueue.push(devNDiSerial::RUNTX);
        }
        break;

    case SYSNONE:
        FlagSTANDBY = true;
        RunStatus = SYSNONE;
        break;

    case STARTTRACK:
        if (SystemStatusInstance == SETUP || SystemStatusInstance == DIAG) {      
            StartTrack();
            FlagAutoDiscoverSent = false;
        } else {
            ProcessNextCommand();
        }
        break;

    case STOPTRACK:
        if (SystemStatusInstance == TRACKING || SystemStatusInstance == DIAG) {
            StopTrack();
        } else {
            ProcessNextCommand();
        }
        break;
    }
    ReplyIndex = 0; // important, could also throw in SendCommand, preps for new Reply(int i) call
}


bool devNDiSerial::ResponseOKAY(char* SerialBuffer) {
    // checks that the response quite literally contains okay
    if ((SerialBuffer[0] == 'O')&&(SerialBuffer[1] == 'K')&&(SerialBuffer[2] == 'A')&&(SerialBuffer[3] == 'Y'))
        return true;
    else
        return false;
}


void devNDiSerial::Run(void) {
    // basic structure is, wait for completion of reply, if complete, assess reply, set variables and then call ProcessNextCommand()
    const mtsStateIndex now = StateTable.GetIndexWriter();
    ProcessQueuedCommands();
    TimeoutCounter++;
    if (FlagSTANDBY) {
        ProcessNextCommand();
        FlagSTANDBY = false;
    } else {
        if (TimeoutCounter>NumTimeoutCycles) {
            CMN_LOG_CLASS_INIT_ERROR << "TIMEOUT DETECTED" << endl;
            abort();
        }
	
        if (NumWaitCycles > 0) {
            NumWaitCycles--;
            CMN_LOG_CLASS_RUN_DEBUG << "sleeping: " << NumWaitCycles << endl;
            return;
        }   
        if (NumWaitCyclesTester > 0) {
            NumWaitCyclesTester--;
        } else {/*
                  NumWaitCyclesTester = 100;
                  UserCommandQueue.push(devNDiSerial::BEEP);
                  UserCommandQueue.push(devNDiSerial::DISCOVER);
                  UserCommandQueue.push(devNDiSerial::TRACK);*/  
        } 
	

        switch(RunStatus) { 
        case SYSNONE:
            TimeoutCounter--;
            break;
	
        case SERIAL_RESET:
            if (Reply(L_POL_RESET)) {
                serialPort.SetBaudRate(osaSerialPort::BaudRate9600);
                //BX only works with 8 bit + private, can't touch
                //serialPort.SetCharacterSize(osaSerialPort::CharacterSize);
                serialPort.SetParityChecking(osaSerialPort::ParityCheckingNone);
                serialPort.SetStopBits(osaSerialPort::StopBitsOne);
                serialPort.SetFlowControl(osaSerialPort::FlowControlSoftware);
                if (serialPort.Configure()) {
                    CMN_LOG_CLASS_INIT_DEBUG << "Serial port Reset result: \"" << ReplyBuffer << "\"" << std::endl;
                    ProcessNextCommand();
                } else {
                    CMN_LOG_CLASS_INIT_ERROR << "CRASH" << endl;
                }  
            }
            break;
	    
        case INITIALIZE:
            if (Reply(L_POL_OK)) {
                if (ResponseOKAY(ReplyBuffer)) {
		    
                    CMN_LOG_CLASS_INIT_DEBUG << "init(): INITIALIZE Response: "<< PortIndex << " OKAY" <<endl; 
                    ProcessNextCommand();    
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "init(): INITIALIZE Response: "<< PortIndex << " FAIL" <<endl;    
                }
            }
            break;

        case ACQUIRE_SERIAL:
            if (NumPorts == 0) {
                ProcessNextCommand();
            } else {
                if (Reply(-1)) {
                    string ret(ReplyBuffer);
                    ret = ret.substr(23,8);
                    string phsr(PortHandleResponseBuffer);
                    CMN_LOG_CLASS_INIT_DEBUG << "SerialNumber:::" << ret << endl;
                    MapPortToTool[stringToInt(phsr.substr(2 + PortIndex*5, 2),0,2)] = MapSerialToTool[ret];
                    CMN_LOG_CLASS_INIT_DEBUG << MapPortToTool[stringToInt(phsr.substr(2 + PortIndex*5, 2),0,2)]->GetName() << endl;
                    if (PortIndex < NumPorts-1) {
                        PortIndex++;
                        GetSerialNumber(PortHandleResponseBuffer,PortIndex);
                        ReplyIndex = 0;
                    } else {
                        ProcessNextCommand();  
                    }
                }
            }
            break;

        case PORT_INITIALIZE:
            if (NumPorts == 0) {
                ProcessNextCommand();
            } else {
                if (Reply(L_POL_OK)) {  
                    if (ResponseOKAY(ReplyBuffer)) {
                        CMN_LOG_CLASS_INIT_DEBUG << "InitPorts(): Port INITIALIZE Response: "<< PortIndex << " OKAY" <<endl;     
                    } else {
                        CMN_LOG_CLASS_INIT_WARNING << "InitPorts(): Port INITIALIZE Response: "<< PortIndex << " FAIL" <<endl;     
                    }
                    if (PortIndex < NumPorts-1) {
                        PortIndex++;
                        InitPorts(PortHandleResponseBuffer,PortIndex);
                        ReplyIndex = 0;
                    } else{
                        ProcessNextCommand();  
                    }
                }
            }
            break;
	    
        case PORT_ENAB:
            if (NumPorts == 0) {
                ProcessNextCommand();
            } else {
                if (Reply(L_POL_OK)) {
                    if (ResponseOKAY(ReplyBuffer)) {
                        CMN_LOG_CLASS_INIT_DEBUG << "EnablePorts(): Port ENAB Response: "<< PortIndex << " OKAY" <<endl;     
                    } else {
                        CMN_LOG_CLASS_INIT_WARNING << "EnablePorts(): Port ENAB Response: "<< PortIndex << " FAIL" <<endl;     
                    }
                    if (PortIndex < NumPorts-1) {
                        PortIndex++;
                        EnablePorts(PortHandleResponseBuffer,PortIndex);
                        ReplyIndex = 0;
                    } else {
                        ProcessNextCommand();
                    }
                }
            }
            break;
	    
        case PORT_FREE:
            if (NumPorts == 0) {
                ProcessNextCommand();
            } else {
                if (Reply(L_POL_OK)) {
                    if (ResponseOKAY(ReplyBuffer)) {
                        CMN_LOG_CLASS_INIT_DEBUG << "FreePorts(): Port FREE Response: "<< PortIndex << " OKAY" <<endl;     
                    } else {
                        CMN_LOG_CLASS_INIT_WARNING << "FreePorts(): Port FREE Response: "<< PortIndex << " FAIL" <<endl;     
                    }
                    if (PortIndex < NumPorts-1) {
                        PortIndex++;
                        FreePorts(PortHandleResponseBuffer,PortIndex);
                        ReplyIndex = 0;
                    } else {
                        ProcessNextCommand();  
                    }
                }
            }
            break;
	    
        case STARTTRACK:
            if (Reply(L_POL_OK)) {
                if (ResponseOKAY(ReplyBuffer)) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "StartTrack(): StartTrack Response: OKAY" <<endl;  
                } else {
                    CMN_LOG_CLASS_INIT_ERROR << "StartTrack(): StartTrack Response: FAIL" <<endl;     
                }
                ProcessNextCommand();
            }
            break;
	    
        case STOPTRACK:
            if (Reply(L_POL_OK)) {
                if (ResponseOKAY(ReplyBuffer)) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "StartTrack(): StopTrack Response: OKAY" <<endl;     
                } else {
                    CMN_LOG_CLASS_INIT_ERROR << "StartTrack(): StopTrack Response: FAIL" <<endl;     
                }
                ProcessNextCommand();
            }
            break;
	    
        case RUNBX:
            if (Reply(0)) {
                ProcessBX();        
                ProcessNextCommand();
            }
            break;

        case RUNTX:
            if (Reply(0)) {
                ProcessTX();        
                ProcessNextCommand();
            }
            break;
	    
        case COMMSETTINGS:
            if (Reply(L_POL_OK)) {
                if (ResponseOKAY(ReplyBuffer)) {
                    CMN_LOG_CLASS_INIT_DEBUG << "commSetting(): commSet Response: OKAY" <<endl;     
                }else{
                    CMN_LOG_CLASS_INIT_ERROR << "commSetting(): commSet Response: FAIL" <<endl;     
                }
                ProcessNextCommand();
            }
            break;
	    
        case PORT_INITIALIZE_FIND:
            // numports necesarry for calls to portinitialize()
            if (Reply(-1)) {
                memcpy(PortHandleResponseBuffer, ReplyBuffer, 256);
                NumPorts = stringToInt(PortHandleResponseBuffer, 0, 2);
                ProcessNextCommand();
            }
            break;
	    
        case PORT_FREE_FIND:
            if (Reply(-1)) {
                memcpy(PortHandleResponseBuffer, ReplyBuffer, 256);
                NumPorts = stringToInt(PortHandleResponseBuffer, 0, 2);
                ProcessNextCommand();
            }
            break;
	    
        case PORT_ENAB_FIND:
            if (Reply(-1)) {
                memcpy(PortHandleResponseBuffer, ReplyBuffer, 256);
                NumPorts = stringToInt(PortHandleResponseBuffer, 0, 2);
                CMN_LOG_CLASS_INIT_VERBOSE << "Found " << NumPorts << " ports" << std::endl;
                ProcessNextCommand();
            }
            break;
	    
        case SYSBEEP:
            if (Reply(4)) {
                ProcessNextCommand();
            }
            break;
        }
    }
}


cmnBool devNDiSerial::getResponse(char *buf, int maxLen)
{
    // black magic
    int gotLen = 0, i, done = 0, newLen;
    char CRCStr[5], CRCChk[5];
    cmnULong start;
    
    while (!done) {													
        newLen = 0;

        while (newLen == 0) {
            newLen = serialPort.Read(buf+gotLen, maxLen-gotLen);
            gotLen += newLen;
	    
        }
	
        for (i=gotLen-1; i>=0; i--) {
            if (buf[i] == 13 && i >= 4) {
                done = 1;
                break;
            }
        }
    }
  
    //Perform CRC check on buf response
    CRCStr[0] = buf[i-4];
    CRCStr[1] = buf[i-3];
    CRCStr[2] = buf[i-2];
    CRCStr[3] = buf[i-1];
    CRCStr[4] = 0;
    buf[i-4] = 0;
    sprintf(CRCChk, "%04X", CalcCRC(buf));
    
    if (strncmp(CRCStr,CRCChk,4) != 0) {
        return false;
    } else {
        return true;
    }
}




// #include "Tool.h"

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;
using std::ios;

Tool::Tool(void) {}

Tool::Tool(string name, string serial)
{
    this->SetName(name);
    this->SetSerialNumber(serial);
}

void Tool::SetName(string name)
{ 
    ToolInformation.SetName(name);
}

string Tool::GetName(void) 
{ 
    string name;
    ToolInformation.GetName(name); 
    return name;
}

void Tool::SetSerialNumber(string serial)
{ 
    ToolInformation.SetSerialNumber(serial);
}
string Tool::GetSerialNumber(void) 
{
    string serial;
    ToolInformation.GetSerialNumber(serial); 
    return serial; 
}

void Tool::SetDisabled(bool flag)
{
    ToolInformation.SetDisabled(flag); 
}
bool Tool::GetDisabled(void)
{
    bool flag;
    ToolInformation.GetDisabled(flag); 
    return flag;
}

void Tool::SetInitialized(bool flag)
{
    ToolInformation.SetInitialized(flag); 
}

bool Tool::GetInitialized(void)
{
    bool flag;
    ToolInformation.GetInitialized(flag); 
    return flag;
}

void Tool::SetEnabled(bool flag)
{
    ToolInformation.SetEnabled(flag); 
}

bool Tool::GetEnabled(void)
{
    bool flag;
    ToolInformation.GetEnabled(flag); 
    return flag;
}

void Tool::SetMissing(bool flag)
{
    ToolInformation.SetMissing(flag); 
}

bool Tool::GetMissing(void)
{
    bool flag;
    ToolInformation.GetMissing(flag); 
    return flag;
}

void Tool::SetOutOfVolume(bool flag)
{
    ToolInformation.SetOutOfVolume(flag); 
}

bool Tool::GetOutOfVolume(void)
{
    bool flag;
    ToolInformation.GetOutOfVolume(flag); 
    return flag;
}

void Tool::SetPartiallyOutOfVolume(bool flag)
{
    ToolInformation.SetPartiallyOutOfVolume(flag); 
}
bool Tool::GetPartiallyOutOfVolume(void)
{
    bool flag;
    ToolInformation.GetPartiallyOutOfVolume(flag); 
    return flag;
}

prmPositionCartesianGet Tool::GetPosition(void)
{
    return PositionCartesian;
}

void Tool::SetPosition(prmPositionCartesianGet position)
{
    PositionCartesian = position;
}

void Tool::SetTranslation(double x, double y, double z)
{
    vctDoubleFrm3 temp;
    PositionCartesian.GetPosition(temp);
    temp.Translation().Assign(x,y,z);
    PositionCartesian.SetPosition(temp);
}

void Tool::SetRotation(double a, double b, double c, double d)
{
    vctDoubleFrm3 temp;
    PositionCartesian.GetPosition(temp);
    temp.Rotation().Assign(a,b,c,d);
    PositionCartesian.SetPosition(temp);
}

#if 0 // ADV
void Tool::SetTranslationAndQuaternion(double x, double y, double z, double a, double b, double c, double d)
{
    //TODO Fix this
    vctDoubleFrm3 temp;
    PositionCartesian.Data.GetPosition(temp);
    temp.Translation().Assign(x,y,z);
    temp.Rotation().Assign(a,b,c,d);
    PositionCartesian.Data.SetPosition(temp);
}
#endif

void Tool::SetQuaternionAndTranslation(double a, double b, double c, double d, double x, double y, double z)
{
    vctDoubleFrm3 temp;
    PositionCartesian.GetPosition(temp);
    temp.Translation().Assign(x,y,z);
    temp.Rotation().FromRaw(vctQuatRot3(b,c,d,a,VCT_DO_NOT_NORMALIZE));
    PositionCartesian.SetPosition(temp);
}

void Tool::SetError(double error)
{
    ToolInformation.SetError(error);
}

double Tool::GetError(void)
{
    double error;
    ToolInformation.GetError(error); 
    return error; 
}
