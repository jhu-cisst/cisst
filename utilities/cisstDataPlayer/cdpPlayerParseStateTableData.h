/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Joshua Chuang
  Created on: 2011-06-01

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _cdpPlayerParseStateTableData_h
#define _cdpPlayerParseStateTableData_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstOSAbstraction.h>

#include <cisstMultiTask.h>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>
// Always include last
#include "cdpExport.h"

class CISST_EXPORT cdpPlayerParseStateTableData: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:  
    cdpPlayerParseStateTableData(const std::string & name="FileParser");
    ~cdpPlayerParseStateTableData();

    //call this after tasks connections are made
    void Configure(const std::string & filename = ""){};
    void Startup(void){};
    void Run(void);
    void Cleanup(void) {};


    // read header
    void ParseHeader(std::string);

    // We need to generate index for each input file
    // it generates a index per 1MB.
    // getnerateIndex from data file
    void GenerateIndex(void);
    // write Index to File
    void WriteIndexToFile(std::string);
    // Load Data into buffer by time index
    // Load a range of data, we may use a ping-pong buffer. Range should be time or data size. I am not sure yet
    // Compare TimeStampForSearch with TimeField which is set from user and load data & time to Data,Time respectively
    void GetBoundary(double TimeStampForSearch, double &TopBoundary, double &LowBoundary);
    void GetBoundary(vctPlot2DBase::Trace *  TraceHandle, double &TopBoundary, double &LowBoundary);
    //void LoadDataFromFile(double TimeStampForSearch, std::vector <double> &Data, std::vector <double> &Time ); 
    //void LoadDataFromFile(vctPlot2DBase::Trace *  TraceHandle ,double TimeStampForSearch, bool ResetTraceBuffer);
    void LoadDataFromFile(vctPlot2DBase::Trace *  TraceHandle ,double TimeStampForSearch, double VisualRange=0.0,  bool ResetTraceBuffer=false);
    void TriggerLoadDataFromFile(vctPlot2DBase::Trace *  TraceHandle ,double TimeStampForSearch, double VisualRange=0.0,  bool ResetTraceBuffer=false);
    void SetDataFieldForSearch(std::string DataField);
    void SetTimeFieldForSearch(std::string TimeField);
    void GetBeginEndTime(double &Begin, double &End);
    size_t GetDataPoisitionFromFile(double Data, int FieldNumber);

    // Test Function, remove later
    void TestIndex(void);
private:
    typedef vctDynamicVector<double> TimeField;
    typedef vctDynamicVector<double> DataField;
    
    std::string DataFieldForSearch;
    std::string TimeFieldForSearch;
    unsigned int IndexOfTimeField;
    unsigned int IndexOfDataField;

    void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

    //structure for parsing header
    struct{
        std::string FilePath;
        std::string Date;
        std::string FileFormat;
        std::string Delimiter;
        unsigned int NumberOfTotalFields;
        unsigned int NumberOfTimeFields;
        unsigned int NumverOfDataFields;
        // retrieve type and dataName and write to Fields, which is a 2D vector
        // string is worked for CSV file, but for binary, we have to use other ways
        std::vector <std::string> Fields;
    }Header;

    // structure for generating index file
    struct IndexElement{
        // How far from beginning
        unsigned int ByteCount;
        // TimeStamp at the point
        std::string LineAtIndex;
    };

    std::vector <struct IndexElement> Index;

    //Data to be sent from above application
    vctPlot2DBase::Trace *  TracePointer;
    double MiddleTime;
    double TimeRange;
    bool ResetBuffer;
    bool StartRun;
    double TimeBaseOffset;
    osaCriticalSection CS; 

};

CMN_DECLARE_SERVICES_INSTANTIATION(cdpPlayerParseStateTableData);

#endif // _cdpPlayerParseStateTableData_h
