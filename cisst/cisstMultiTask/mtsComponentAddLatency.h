/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2011-12-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsComponentAddLatency_h
#define _mtsComponentAddLatency_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentAddLatency: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    struct DelayedRead
    {
        bool Valid;
        std::string Name;
        mtsFunctionRead Function;
        mtsGenericObject * PlaceHolder;
    };

    struct DelayedVoid
    {
        std::string Name;
        mtsFunctionVoid Function;
        void Method(void);
    };

    struct DelayedWrite
    {
        std::string Name;
        mtsFunctionWrite Function;
        mtsCommandWriteBase * Command;
        void Method(const mtsGenericObject & data);
    };

    typedef std::list<DelayedRead *> DelayedReadList;
    DelayedReadList DelayedReads;

 public:
    // constructor
    mtsComponentAddLatency(const std::string & componentName, double periodInSeconds);
    virtual ~mtsComponentAddLatency();

    // methods defined as virtual in base class
    void Configure(const std::string & filename);
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    double SetLatency(double latencyInSeconds);

 protected:

    /*! Add the following data to the state table used to store all
      data and introduce the latency.  The provided and required
      interfaces must be created before using the methods
      AddInterfaceRequired and AddInterfaceProvided.  The command name
      should match the command name from the source component provided
      interface.  By default the provided interface with delay will
      use the same command name.  The data object must not be deleted
      for the life of the component, i.e. make sure this is not a
      local variable. */
    template <class _elementType>
    bool AddCommandReadDelayed(_elementType & data,
                               mtsInterfaceRequired * interfaceRequired,
                               const std::string & commandRequiredName,
                               mtsInterfaceProvided * interfaceProvided,
                               const std::string & commandProvidedName = "") {
        this->LatencyStateTable.AddData(data);
        interfaceProvided->AddCommandReadStateDelayed(this->LatencyStateTable,
                                                      data,
                                                      commandProvidedName == "" ? commandRequiredName : commandProvidedName);
        this->AddCommandReadInternal(data, interfaceRequired, commandRequiredName);
        return true;
    }

    bool AddCommandVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                               const std::string & commandRequiredName,
                               mtsInterfaceProvided * interfaceProvided,
                               const std::string & commandProvidedName = "");

    /*! Data object is provided just creation of an argument
      prototype.  It can be a locally defined object. */
    bool AddCommandWriteDelayed(const mtsGenericObject & data,
                                mtsInterfaceRequired * interfaceRequired,
                                const std::string & commandRequiredName,
                                mtsInterfaceProvided * interfaceProvided,
                                const std::string & commandProvidedName = "");

    bool AddEventVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                             const std::string & eventRequiredName,
                             mtsInterfaceProvided * interfaceProvided,
                             const std::string & eventProvidedName = "");

    double Latency;
    mtsStateTable LatencyStateTable;

 private:
    bool AddCommandReadInternal(mtsGenericObject & data,
                                mtsInterfaceRequired * interfaceRequired,
                                const std::string & commandRequiredName);
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentAddLatency)


#endif // _mtsComponentAddLatency_h

