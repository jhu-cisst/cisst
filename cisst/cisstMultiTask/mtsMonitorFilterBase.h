/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorFilterBase.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines the base class of filters that are used for fault detection and 
         diagnosis
*/

#ifndef _mtsMonitorFilterBase_h
#define _mtsMonitorFilterBase_h

#include <cisstCommon/cmnGenericObject.h>

#include <vector>
#include <iostream>
#include <sstream>

/*!
  \ingroup cisstMultiTask
*/

class mtsMonitorFilterBase : public cmnGenericObject
{
public:
    /*! Names of input signals.  A filter may need more than one signal to run 
        its filtering algorithm. */
    typedef std::vector<std::string> SignalNamesType;

private:    
    /*! ID of this filter, which is unique within the process.  This allows multiple 
        filters of the same type to be used in the same state table.  */
    static int UID;

protected:
    /*! UID of this filter */
    int FilterUID;

    /*! Name of this filter.  Set by derived filters */
    const std::string FilterName;

    /*! If this filter is active */
    bool Enabled;

    /*! Names of input signals required by this filter */
    SignalNamesType InputNames;

    /*! Names of output signals that this filter generates */
    SignalNamesType OutputNames;

    /*! Placeholder for output signals
        MJ TEMP: Use double type for now.  Can be extended to arbitrary type later */
    double * OutputSignals;

public:
    /*! Constructors and destructor */
    mtsMonitorFilterBase(void); 
    mtsMonitorFilterBase(const std::string & filterName);
    virtual ~mtsMonitorFilterBase();

    /*! Getters and setters */
    //@{
    inline int GetFilterUID(void) const {
        return FilterUID;
    }
    inline const std::string & GetFilterName(void) const {
        return this->FilterName;
    }
    inline bool IsEnabled(void) const {
        return this->Enabled;
    }
    inline void Enable(bool enable = true) {
        this->Enabled = enable;
    }
    inline size_t GetNumberOfInputs(void) const {
        return InputNames.size();
    }
    inline size_t GetNumberOfOutputs(void) const {
        return OutputNames.size();
    }
    inline SignalNamesType GetNamesOfInputs(void) const {
        return InputNames;
    }
    inline SignalNamesType GetNamesOfOutputs(void) const {
        return OutputNames;
    }
    inline double * const GetOutputs(void) const {
        return OutputSignals;
    }
    //@}

    /*! To support human readable outputs (for debugging purpose) */
    virtual std::string ToString(void) const  {
        std::stringstream ss;
        ToStream(ss);
        return ss.str();
    };
    virtual void ToStream(std::ostream & outputStream) const = 0;

};

inline std::ostream & operator << (std::ostream & outputStream, const mtsMonitorFilterBase & filter)
{
    filter.ToStream(outputStream);
    return outputStream;
}

#endif // _mtsMonitorFilterBase_h

