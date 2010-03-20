/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-02-25

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

/*!
  \file
  \brief A data collection tool
*/

#ifndef _mtsCollectorBase_h
#define _mtsCollectorBase_h

#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnNamedMap.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsHistory.h>

#include <string>
#include <stdexcept>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class provides a way to collect data from state table in
  real-time.  Collected data can be either saved as a log file or
  displayed in GUI like an oscilloscope.
*/
class CISST_EXPORT mtsCollectorBase: public mtsTaskFromSignal
{
    friend class mtsCollectorBaseTest;

    //-------------------- Auxiliary class definition -----------------------//
public:
    typedef enum {
        COLLECTOR_FILE_FORMAT_PLAIN_TEXT,
        COLLECTOR_FILE_FORMAT_BINARY,
        COLLECTOR_FILE_FORMAT_CSV,
        COLLECTOR_FILE_FORMAT_UNDEFINED
    } CollectorFileFormat;

    //------------------------- Protected Members ---------------------------//
protected:

    /*! State definition of the collector */
    typedef enum {
        COLLECTOR_STOP,         // nothing happens.
        COLLECTOR_WAIT_START,   // Start(void) has been called. Wait for some time elapsed.
        COLLECTOR_COLLECTING,   // Currently collecting data
        COLLECTOR_WAIT_STOP     // Stop(void) has been called. Wait for some time elapsed.
    } CollectorStatus;

    CollectorStatus Status;
    CollectorFileFormat FileFormat;

    class SignalMapElement {
    public:
        mtsTask * Task;
        mtsHistoryBase * History;

        SignalMapElement(void) {}
        ~SignalMapElement(void) {}
    };

    /*! Container definition for tasks and signals.
        TaskMap:   (taskName, SignalMap*)
        SignalMap: (SignalName, SignalMapElement)
    */
    typedef cmnNamedMap<SignalMapElement> SignalMapType;
    typedef cmnNamedMap<SignalMapType> TaskMapType;
    TaskMapType TaskMap;

    /*! Static member variables */
    static unsigned int CollectorCount;
    static mtsTaskManager * TaskManager;

    /*! Flag to determine if the collector is connected.  Once the
      collector is connected, it becomes impossible to change the
      observed state table, components, ... */
    bool ConnectedFlag;

    /*! Flag for PrintHeader() method. */
    bool FirstRunningFlag;

    /*! Output file name. */
    std::string OutputFileName;

    /*! Pointer on output stream, can be create and managed by this
      class or provided by a user. */
    std::ostream * OutputStream;
    std::ofstream * OutputFile;

    /*! Delimiter used in a log file. Set by the constructor according
      to mtsCollectorBase::CollectorLogFormat. */
    char Delimiter;

    /*! String stream buffer for serialization. */
    std::stringstream StringStreamBufferForSerialization;

    /*! Serializer for binary logging. DeSerializer is used only at
      ConvertBinaryToText() method so we don't define it here. */
    cmnSerializer * Serializer;

    /*! Update the delimiter used in output files based on file
      format.  Should be used everytime FileFormat is set. */
    void SetDelimiter(void);

    /*! Default control interface and methods used for the provided commands. */
    mtsProvidedInterface * ControlInterface;

    /*! Methods used to populate the component provided interface. */
    inline void StartCollectionCommand(void) {
        this->StartCollection(mtsDouble(0.0));
    }
    inline void StartCollectionInCommand(const mtsDouble & delayInSeconds) {
        this->StartCollection(delayInSeconds);
    }
    inline void StopCollectionCommand(void) {
        this->StopCollection(mtsDouble(0.0));
    }
    inline void StopCollectionInCommand(const mtsDouble & delayInSeconds) {
        this->StopCollection(delayInSeconds);
    }
    //@}

    /*! Initialize this collector instance */
    void Init(void);

    /*! Clean up internal data. */
    void Cleanup(void);

    /*! Clear TaskMap */
    void ClearTaskMap(void);

    /*! Set some initial values */
    virtual void Startup(void) = 0;

public:
    mtsCollectorBase(const std::string & collectorName, const CollectorFileFormat fileFormat);

    virtual ~mtsCollectorBase(void);

    /*! Generate default file name, without the prefix (txt, csv, cdat) */
    virtual std::string GetDefaultOutputName(void) = 0;

    /*! Define the output file and format.  If a file is already in
      use, this method will close the current one. */
    void SetOutput(const std::string & fileName, const CollectorFileFormat fileFormat);

    /*! Define the output using an existing ostream, the collector
      will not open nor close the stream. */
    void SetOutput(std::ostream & outputStream, const CollectorFileFormat fileFormat);

    /*! Define the output using an existing ostream, the collector
      will not open nor close the stream.  If this method is called
      for the first time, the format will be
      COLLECTOR_FILE_FORMAT_CSV, otherwise it will use the previously
      used format. */
    void SetOutput(std::ostream & outputStream);

    /*! Creates a default file name using the task name, table name
      and date.  The suffix depends on the file format. */
    void SetOutputToDefault(const CollectorFileFormat fileFormat);

    /*! Creates a default file name using the task name, table name
      and date.  If this method is called for the first time, the
      format will be COLLECTOR_FILE_FORMAT_CSV, otherwise it will use
      the previously used format. */
    void SetOutputToDefault(void);

    /*! Begin collecting data. Data collection will begin after delayedStart
    second(s). If it is zero (by default), it means 'start now'. */
    virtual void StartCollection(const mtsDouble & delayInSeconds) = 0;

    /*! End collecting data. Data collection will end after delayedStop
    second(s). If it is zero (by default), it means 'stop now'. */
    virtual void StopCollection(const mtsDouble & delayInSeconds) = 0;


    //---------------------- Miscellaneous functions ------------------------//
    inline static unsigned int GetCollectorCount(void) { return CollectorCount; }

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorBase)

#endif // _mtsCollectorBase_h

