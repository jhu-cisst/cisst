/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines the state data table.
*/

#ifndef _mtsStateTable_h
#define _mtsStateTable_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsStateArrayBase.h>
#include <cisstMultiTask/mtsStateArray.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstMultiTask/mtsHistory.h>

#include <vector>
#include <iostream>

// Always include last
#include <cisstMultiTask/mtsExport.h>

#define MTS_STATE_TABLE_DEFAULT_NAME "StateTable"

// Forward declaration
class osaTimeServer;


/*! mtsStateDataId.  Unique identifier for the columns of the State
  Data Table.  Typedef'ed to an int */
typedef int mtsStateDataId;

/*!
  \ingroup cisstMultiTask

  The state data table is the storage for the state of the task that
  the table is associated with. It is a heterogenous circular buffer
  and can contain data of any type so long as it is derived from
  mtsGenericObject.  The state data table also resolves conflicts
  between reads and writes to the state, by ensuring that the reader
  head is always one behind the write head. To ensure this we have an
  assumption here that there is only one writer, though there can be
  multiple readers. State Data Table is also refered as Data Table or
  State Table elsewhere in the documentation.
 */
class CISST_EXPORT mtsStateTable {

    friend class mtsCollectorState;
    friend class mtsTaskTest;
    friend class mtsStateTableTest;
    friend class mtsCollectorBaseTest;

    class DataCollectionInfoStruct {
    public:
        /* True if data collection event can be triggered (false by default). */
        bool TriggerEnabled;

        /*! Number of data that are newly generated and are to be fetched by the 
        data collection tool. */
        unsigned int NewDataCount;

        /*! If NewDataCount becomes greater than this vaule, an event for data collection
            is generated. Though this value is redundant in some respect (because
            EventTriggeringRatio is already defined), this value is kept for the purpose 
            of efficiency. */
        unsigned int EventTriggeringLimit;

        DataCollectionInfoStruct() : TriggerEnabled(false), NewDataCount(0), EventTriggeringLimit(0)
        {}

        ~DataCollectionInfoStruct() {}
    };

public:
    class AccessorBase {
    protected:
        const mtsStateTable &Table;
        mtsStateDataId Id;   // Not currently used
    public:
        AccessorBase(const mtsStateTable &table, mtsStateDataId id): Table(table), Id(id) {}
        virtual ~AccessorBase() {}
        virtual void ToStream(std::ostream & outputStream, const mtsStateIndex & when) const = 0;
    };
    
    template <class _elementType>
    class Accessor : public AccessorBase {
        typedef _elementType value_type;
        typedef typename mtsStateTable::Accessor<value_type> ThisType;
        const mtsStateArray<value_type> &History;
        value_type * Current;

    public:
        Accessor(const mtsStateTable & table, mtsStateDataId id, 
                 const mtsStateArray<value_type> * history, value_type * data):
            AccessorBase(table, id), History(*history), Current(data) {}

        void ToStream(std::ostream & outputStream, const mtsStateIndex & when) const {
            History.Element(when.Index()).ToStream(outputStream);
        }
        
        bool Get(const mtsStateIndex & when, value_type & data) const { 
            data = History.Element(when.Index());
            return Table.ValidateReadIndex(when);
        }

        bool GetLatest(value_type & data) const {
            return Get(Table.GetIndexReader(), data);
        }
        
        void SetCurrent(const value_type & data) {
            *Current = data;
        }
        
        // Get a vector of data, starting and ending at the specified time indices (inclusive).
        // For now, set the start index based on the vector size. In the future, we
        // should define a new parameter type that consists of a pair of mtsStateIndex.
        bool GetHistory(const mtsStateIndex & end, mtsHistory<value_type> & data) const {
            bool ret = false;
            if (data.size() > 0) {
                mtsStateIndex start = end;
                start -= (data.size()-1);
                if (Table.ValidateReadIndex(start) && Table.ValidateReadIndex(end)) {
                    ret = History.GetHistory(start.Index(), end.Index(), data);
                    // If GetHistory succeeded, then check if the data is still valid (has not been overwritten).
                    // Here it is sufficient to just check the oldest index (start).
                    if (ret)
                        ret = Table.ValidateReadIndex(start);
                }
                else
                    CMN_LOG_INIT_ERROR << "ReadVectorFromReader: data not available" << std::endl;
            }
            return ret;
        }
    };

 protected:

	/*! The number of rows of the state data table. */
	unsigned int HistoryLength;
	
	/*! The number of columns of the data table. */
	unsigned int NumberStateData;
	
	/*! The index of the writer in the data table. */
	unsigned int IndexWriter;
	
	/*! The index of the reader in the table. */
	unsigned int IndexReader;
	
	/*! The vector contains pointers to individual columns. */
	std::vector<mtsStateArrayBase *> StateVector;

    /*! The vector contains pointers to the current values
      of elements that are to be added to the state when we
      advance.
      */
    std::vector<mtsGenericObject *> StateVectorElements;
    
	/*! The columns entries can be accessed by name. This vector
	  stores the names corresponding to the columns. */
	std::vector<std::string> StateVectorDataNames;

    /*! The vector contains pointers to the accessor methods
      (e.g., Get, GetLatest) from which command objects are created. */
    std::vector<AccessorBase *> StateVectorAccessors;

	/*! The vector contains the time stamp in counts or ticks per
	  period of the task that the state table is associated with. */
	std::vector<mtsStateIndex::TimeTicksType> Ticks;

    /*! The state table indices for Tic, Toc, and Period. */
    mtsStateDataId TicId, TocId;
    mtsStateDataId PeriodId;

    /*! The time server used to provide absolute and relative times. */
    const osaTimeServer * TimeServer;

public:

    /* The start/end times for the current row of data. */
    mtsDouble Tic, Toc;

    /*! The measured task period (difference between current Tic and
        previous Tic). */
    mtsDouble Period;

 protected:
    /*! The sum of all the periods (time differences between
        consecutive Tic values); used to compute average period. */
    double SumOfPeriods;

    /*! The average period over the last HistoryLength samples. */
    double AvgPeriod;

    /*! The name of this state table. */
    std::string StateTableName;

    /*! Data collection event handler. */
    mtsCommandVoidBase * DataCollectionEventHandler;

    DataCollectionInfoStruct DataCollectionInfo;

	/*! Write specified data. */
	bool Write(mtsStateDataId id, const mtsGenericObject &obj);

 public:
    /*! Constructor. Constructs a state table with a default
      size of 256 rows. */
    mtsStateTable(int size = 256, const std::string & stateTableName = MTS_STATE_TABLE_DEFAULT_NAME);
    
    /*! Default destructor. */
    ~mtsStateTable();

    /*! Get a handle for data to be used by a reader.  All the const
      methods, that can be called from a reader and writer. */
    mtsStateIndex GetIndexReader(void) const;

    inline void GetIndexReader(mtsStateIndex & timeIndex) const {
        timeIndex = GetIndexReader();
    }

    /*! Verifies if the data is valid. */
    inline bool ValidateReadIndex(const mtsStateIndex &timeIndex) const {
        return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
    }
    
    /*! Check if the signal has been registered. */
    int GetStateVectorID(const std::string & dataName) const;

    /*! Add an element to the table. Should be called during startup
      of a real time loop.  All the non-const methods, that can be
      called from a writer only. Returns index of data within state
      data table. */
    template <class _elementType>
    mtsStateDataId NewElement(const std::string & name = "", _elementType * element = 0);

    /*! Add an element to the table (alternative to NewElement). */
    template <class _elementType>
    void AddData(_elementType &element, const std::string & name = "") {
        NewElement(name, &element);
    }

    /*! Return pointer to the state data element specified by the id.
      This element is the same type as the state data table entry. */
    template<class _elementType>
    _elementType * GetStateDataElement(mtsStateDataId id) const {
        return StateVectorElements[id]; // WEIRD???
    }

    /*! Return pointer to accessor functions for the state data element.
        \param element Pointer to state data element (i.e., working copy)
        \returns Pointer to accessor class (0 if not found)
        \note This method is overloaded to accept the element pointer or string name.
    */
    mtsStateTable::AccessorBase *GetAccessor(const mtsGenericObject &element) const;

    /*! Return pointer to accessor functions for the state data element.
        \param name Name of state data element
        \returns Pointer to accessor class (0 if not found)
        \note This method is overloaded to accept the element pointer or string name.
    */
    mtsStateTable::AccessorBase *GetAccessor(const std::string &name) const;

    /*! Get a handle for data to be used by a writer */
    mtsStateIndex GetIndexWriter(void) const;

    /*! Start the current cycle. This just records the starting timestamp (Tic). */
    void Start(void);

    /*! Advance the pointers of the circular buffer. Note that since
      there is only a single writer, it is not necessary to use mutual
      exclusion primitives; the critical section can be handled by
      updating (incrementing) the write index before the read index.
    */
    void Advance(void);

    double GetTic(void) const { return Tic.Data; }
    double GetToc(void) const { return Toc.Data; }

    /*! Return the moving average of the measured period (i.e., average of last
      HistoryLength values). */
    double GetAveragePeriod(void) const { return AvgPeriod; }

    /*! For debugging, dumps the current data table to output
      stream. */
    void ToStream(std::ostream& out) const;

    /*! For debugging, dumps some values of the current data table to
      output stream. */
    void Debug(std::ostream& out, unsigned int * listColumn, unsigned int number) const;

    /*! This method is to dump the state data table in the csv format, 
        allowing easy import into matlab.
        Assumes that individual columns are also printed in csv format.
     By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
     value i.e, those rows that have been written to at least once.
     */
    void CSVWrite(std::ostream& out, bool nonZeroOnly = false);
    void CSVWrite(std::ostream& out, unsigned int * listColumn, unsigned int number, bool nonZeroOnly = false);

    void CSVWrite(std::ostream& out, mtsGenericObject ** listColumn, unsigned int number, bool nonZeroOnly = false);
    
    /*! A base column index of StateTable for a signal registered by user. */
    static int StateVectorBaseIDForUser;
    
    //-------------------------------------------------------------------------
    //  Data Collection
    //-------------------------------------------------------------------------
    /*! Fetch state table data. */
    //void GetStateTableHistory(mtsDoubleVecHistory & history,
    //                          const unsigned int signalIndex,
    //                          const unsigned int lastFetchIndex);
    
    /*! Return the name of this state table. */
    const std::string GetName(void) const { return StateTableName; }
    
    /*! Enable data collection event trigger. */
    void ResetDataCollectionTrigger(void) { 
        DataCollectionInfo.TriggerEnabled = true;
    }
    
    /*! Set an event handler to inform the data collector about the
      event that data in this state table is populated. */
    void SetDataCollectionEventHandler(mtsCollectorState * collector);
    
    /*! Determine a ratio to generate a data collection event. */
    void SetDataCollectionEventTriggeringRatio(const double eventTriggeringRatio);
    
    void GenerateDataCollectionEvent(void);
};


// overload mtsObjectName to provide the class name
inline std::string mtsObjectName(const mtsStateTable * object)
{
    return object->GetName();
}

// overload mtsObjectName for mtsStateTable::Accessor
template <class _elementType>
inline std::string mtsObjectName(const mtsStateTable::Accessor<_elementType> * CMN_UNUSED(accessor)) {
    return "mtsStateTable::Accessor";
}


template <class _elementType>
mtsStateDataId mtsStateTable::NewElement(const std::string & name, _elementType * element) {
    mtsStateArray<_elementType> * elementHistory =
        new mtsStateArray<_elementType>(*element,
                                        HistoryLength);
    StateVector.push_back(elementHistory);
    NumberStateData = StateVector.size();
    StateVectorElements.push_back(element); 
    StateVectorDataNames.push_back(name);
    AccessorBase * accessor = new Accessor<_elementType>(*this, NumberStateData-1, elementHistory, element);
    StateVectorAccessors.push_back(accessor);
    return NumberStateData-1;
}

#endif // _mtsStateTable_h

