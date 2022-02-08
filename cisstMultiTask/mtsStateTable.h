/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Min Yang Jung, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsStateArrayBase.h>
#include <cisstMultiTask/mtsStateArray.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>


#include <vector>
#include <iostream>

// Always include last
#include <cisstMultiTask/mtsExport.h>

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
class CISST_EXPORT mtsStateTable: public cmnGenericObject {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsCollectorState;
    friend class mtsComponent;
    friend class mtsTaskTest;
    friend class mtsStateTableTest;
    friend class mtsCollectorBaseTest;

 public:
    /*! Collection is performed by batches, this requires to save
      the state indices for begin/end. */
    class CISST_EXPORT IndexRange: public mtsGenericObject
    {
        CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
    public:
        mtsStateIndex First;
        mtsStateIndex Last;

        void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                         bool headerOnly = false, const std::string & headerPrefix = "") const;
    };

    /*! Data structure used for state table data collection.  Stores
      information related to data collection as well as methods for
      callbacks */
    class DataCollectionInfo {
    public:
        /*! True if data collection event can be triggered, this state
          table is currently collecting data (false by default). */
        bool Collecting;

        /*! Delay in second before data collection should start.  This
          is measured in seconds based on the state table Tic and Toc.
          0 means that there is no defined start time. */
        double StartTime;

        /*! Delay in second before data collection should stop.  This
          is measured in seconds based on the state table Tic and
          Toc. 0 means that there is no defined stop time. */
        double StopTime;

        /*! Range for the batch */
        mtsStateTable::IndexRange BatchRange;

        /*! Maximum number of elements to collect in one batch. */
        size_t BatchSize;

        /*! Number of elements to be collected so far. */
        size_t BatchCounter;

        /*! Number of elements collected since the last event */
        size_t CounterForEvent;

        /*! Interval between two progress events (in seconds) */
        double TimeIntervalForProgressEvent;

        /*! Time of last progress event */
        double TimeOfLastProgressEvent;

        /*! Function used to trigger event sent to state collector
          when the data collection is needed.  The payload is the
          range defined by state indices. */
        mtsFunctionWrite BatchReady;

        /*! Function used to trigger an event when the collection has
          started */
        mtsFunctionVoid CollectionStarted;

        /*! Function used to trigger an event when the collection has
          stopped */
        mtsFunctionWrite CollectionStopped;

        /*! Function used to trigger an event to indicate progress */
        mtsFunctionWrite Progress;

        /*! Default constructors */
        inline DataCollectionInfo(void):
            Collecting(false),
            StartTime(0.0),
            StopTime(0.0),
            BatchSize(0),
            BatchCounter(0),
            CounterForEvent(0)
        {}

        inline ~DataCollectionInfo() {}
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
        typedef typename mtsGenericTypes<_elementType>::FinalType value_type;
        typedef typename mtsGenericTypes<_elementType>::FinalRefType value_ref_type;
        typedef typename mtsStateTable::Accessor<_elementType> ThisType;
        const mtsStateArray<value_type> & History;
        value_ref_type * Current;

    public:
        Accessor(const mtsStateTable & table, mtsStateDataId id,
                 const mtsStateArray<value_type> * history, value_ref_type * data):
            AccessorBase(table, id), History(*history), Current(data) {}

        void ToStream(std::ostream & outputStream, const mtsStateIndex & when) const {
            History.Element(when.Index()).ToStream(outputStream);
        }

        bool Get(const mtsStateIndex & when, value_type & data) const {
            data = History.Element(when.Index());
            return Table.ValidateReadIndex(when);
        }

        //This should be used with caution because
        //the state table mechanism could override the data that the pointer is pointing to.
        const value_type * GetPointer(const mtsStateIndex & when) const {
            if (!Table.ValidateReadIndex(when))
                return 0;
            else
                return  &(History.Element(when.Index()));
        }

        bool Get(const mtsStateIndex & when, mtsGenericObject & data) const {
            value_type* pdata = dynamic_cast<value_type*>(&data);
            if (pdata) {
                return Get(when, *pdata);
            }
            value_ref_type* pref = dynamic_cast<value_ref_type*>(&data);
            if (pref) {
                return Get(when, *pref);
            }
            return false;
        }

        bool GetLatest(value_type & data) const {
            return Get(Table.GetIndexReader(), data);
        }
        bool GetLatest(mtsGenericObject & data) const {
            return Get(Table.GetIndexReader(), data);
        }

        bool GetDelayed(value_type & data) const {
            return Get(Table.GetIndexDelayed(), data);
        }
        bool GetDelayed(mtsGenericObject & data) const {
            return Get(Table.GetIndexDelayed(), data);
        }
        void SetCurrent(const value_type & data) {
            *Current = data;
        }
    };

 protected:

    /*! Flag to indicate if the table has started.  True between
      Start() and Advance() calls, false otherwise. */
    bool mStarted;

    /*! The number of rows of the state data table. */
    size_t HistoryLength;

    /*! The index of the writer in the data table. */
    size_t IndexWriter;

    /*! The index of the reader in the table. */
    size_t IndexReader;

    /*! The index of the delayed reader in the table. */
    size_t IndexDelayed;

    /*! Delay used for GetIndexDelayed and GetDelayed.  In number of
      rows in state tables. */
    size_t Delay;

    /*! Automatic advance flag.  This flag is used by the method
      AdvanceIfAutomatic to decide if this state table should advance
      or not.  The method AdvanceIsAutomatic is used by mtsTask on all
      the registered state tables.  If a user wishes to Advance the
      state table manually, he or she will have to set this flag to
      false (see SetAutomaticAdvance).  This flag is set to true by
      default. */
    bool AutomaticAdvanceFlag;

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

   /*! Periodicist Statistics */
    mtsIntervalStatistics PeriodStats;

 protected:
    /*! The sum of all the periods (time differences between
        consecutive Tic values); used to compute average period. */
    double SumOfPeriods;

    /*! The average period over the last HistoryLength samples. */
    double AveragePeriod;

    /*! The name of this state table. */
    std::string Name;

    /*! Information used for the state table data collection, see also
      mtsCollectorState. */
    DataCollectionInfo DataCollection;

    /*! Write specified data. */
    bool Write(mtsStateDataId id, const mtsGenericObject & obj);


 public:
    /*! Constructor. Constructs a state table with a default
      size of 256 rows. */
    mtsStateTable(size_t size, const std::string & name);

    /*! Default destructor. */
    ~mtsStateTable();

    /*! Method to change the size of the table*/
    bool SetSize(const size_t size);

    /*! Get a handle for data to be used by a reader.  All the const
      methods, that can be called from a reader and writer. */
    mtsStateIndex GetIndexReader(void) const;

    inline void GetIndexReader(mtsStateIndex & timeIndex) const {
        timeIndex = GetIndexReader();
    }

    /*! Get a handle for data to be used by a reader with a given
      delay.  All the const methods, that can be called from a reader
      and writer. */
    mtsStateIndex GetIndexDelayed(void) const;

    /*! Set delay in number of rows. */
    size_t SetDelay(size_t newDelay);

    /*! Verifies if the data is valid. */
    inline bool ValidateReadIndex(const mtsStateIndex &timeIndex) const {
        return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
    }

    /*! Get method for auto advance flag. See AutomaticAdvanceFlag */
    inline const bool & AutomaticAdvance(void) const {
        return this->AutomaticAdvanceFlag;
    }

    /*! Set method for auto advance flag.  See AutoAdvanceFlag. */
    inline void SetAutomaticAdvance(bool automaticAdvance) {
        this->AutomaticAdvanceFlag = automaticAdvance;
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
    void AddData(_elementType & element, const std::string & name = "") {
        NewElement(name, &element);
    }

    /*! Return pointer to the state data element specified by the id.
      This element is the same type as the state data table entry. */
    template <class _elementType>
    _elementType * GetStateDataElement(mtsStateDataId id) const {
        return StateVectorElements[id]; // WEIRD???
    }


    mtsGenericObject * GetStateVectorElement(size_t id) const {
        return StateVectorElements[id];
    }

    /*! Return pointer to accessor functions for the state data element.
      \param element Pointer to state data element (i.e., working copy)
      \returns Pointer to accessor class (0 if not found)
    */
    template<class _elementType>
    mtsStateTable::AccessorBase * GetAccessorByInstance(const _elementType & element) const;

    /*! Return pointer to accessor functions for the state data element.
      \param name Name of state data element
      \returns Pointer to accessor class (0 if not found)
    */
    //@{
    mtsStateTable::AccessorBase * GetAccessorByName(const std::string & name) const;
    mtsStateTable::AccessorBase * GetAccessorByName(const char * name) const;
    //@}

    /*! Return pointer to accessor functions for the state data element.
      \param id Id of state data element
      \returns Pointer to accessor class (0 if not found)
    */
    mtsStateTable::AccessorBase * GetAccessorById(const size_t id) const;

#ifndef SWIG
    template<class _elementType>
    CISST_DEPRECATED mtsStateTable::AccessorBase * GetAccessor(const _elementType & element) const {
        CMN_LOG_RUN_WARNING << "mtsStateTable::GetAccessor is deprecated, use GetAccessorBy{Instance,Name,Id}" << std::endl;
        return GetAccessorByInstance(element);
    }
    CISST_DEPRECATED mtsStateTable::AccessorBase * GetAccessor(const std::string & name) const {
        CMN_LOG_RUN_WARNING << "mtsStateTable::GetAccessor is deprecated, use GetAccessorBy{Instance,Name,Id}" << std::endl;
        return GetAccessorByName(name);
    }
    CISST_DEPRECATED mtsStateTable::AccessorBase * GetAccessor(const char * name) const {
        CMN_LOG_RUN_WARNING << "mtsStateTable::GetAccessor is deprecated, use GetAccessorBy{Instance,Name,Id}" << std::endl;
        return GetAccessorByName(name);
    }
    CISST_DEPRECATED mtsStateTable::AccessorBase * GetAccessor(const size_t id) const {
        CMN_LOG_RUN_WARNING << "mtsStateTable::GetAccessor is deprecated, use GetAccessorBy{Instance,Name,Id}" << std::endl;
        return GetAccessorById(id);
    }
#endif

    /*! Get a handle for data to be used by a writer */
    mtsStateIndex GetIndexWriter(void) const;

    /*! Start the current cycle. This just records the starting timestamp (Tic). */
    void Start(void);

    /*! Start if automatic advance is set and does nothing otherwise. */
    void StartIfAutomatic(void);

    /*! Check if state table is "started", i.e. between Start() and Advance() calls. */
    inline bool Started(void) const {
        return mStarted;
    }

    /*! Advance the pointers of the circular buffer. Note that since
      there is only a single writer, it is not necessary to use mutual
      exclusion primitives; the critical section can be handled by
      updating (incrementing) the write index before the read index.
    */
    void Advance(void);

    /*! Advance if automatic advance is set and does nothing otherwise. */
    void AdvanceIfAutomatic(void);

    /*! Advance for replay mode, be very careful this should only be
      used in replay mode as this method only increments the reader
      index. */
    bool ReplayAdvance(void);

    /*! Cleanup called when the task is being stopped. */
    void Cleanup(void);

    inline double GetTic(void) const {
        return this->Tic.Data;
    }

    inline double GetToc(void) const {
        return this->Toc.Data;
    }

    inline size_t GetHistoryLength(void) const {
        return this->HistoryLength;
    }

    inline size_t GetNumberOfElements(void) const {
        return this->StateVector.size();
    }

    /*! Return the moving average of the measured period (i.e., average of last
      HistoryLength values). */
    inline double GetAveragePeriod(void) const {
        return AveragePeriod;
    }

    /*! For debugging, dumps the current data table to output stream. */
    void ToStream(std::ostream & out) const;

    /*! For debugging, dumps some values of the current data table to
      output stream. */
    void Debug(std::ostream & out, unsigned int * listColumn, unsigned int number) const;

    /*! This method is to dump the state data table in the csv format,
        allowing easy import into matlab.
        Assumes that individual columns are also printed in csv format.
     By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
     value i.e, those rows that have been written to at least once.
     */
    void CSVWrite(std::ostream & out, bool nonZeroOnly = false);
    void CSVWrite(std::ostream & out, unsigned int * listColumn, unsigned int number, bool nonZeroOnly = false);

    void CSVWrite(std::ostream & out, mtsGenericObject ** listColumn, unsigned int number, bool nonZeroOnly = false);

    /*! Return the name of this state table. */
    inline const std::string & GetName(void) const { return Name; }

    /*! Determine a ratio to generate a data collection event. */
    void DataCollectionEventTriggeringRatio(const mtsDouble & eventTriggeringRatio);

    /*! Methods used to control the data collection start/stop */
    //@{
    void DataCollectionStart(const mtsDouble & delay);
    void DataCollectionStop(const mtsDouble & delay);
    //@}
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStateTable);
typedef mtsStateTable::IndexRange mtsStateTableIndexRange;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStateTableIndexRange);


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
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsGenericTypes<_elementType>::FinalRefType FinalRefType;
    mtsStateArray<FinalType> * elementHistory =
        new mtsStateArray<FinalType>(*element, HistoryLength);
    StateVector.push_back(elementHistory);
    FinalRefType *pdata = mtsGenericTypes<_elementType>::ConditionalWrap(*element);
    StateVectorElements.push_back(pdata);

    StateVectorDataNames.push_back(name);
    mtsStateDataId id = static_cast<mtsStateDataId>(StateVector.size() - 1);
    AccessorBase * accessor = new Accessor<_elementType>(*this, id, elementHistory, pdata);
    StateVectorAccessors.push_back(accessor);
    return id;
}

template <class _elementType>
mtsStateTable::AccessorBase *mtsStateTable::GetAccessorByInstance(const _elementType & element) const
{
    for (size_t i = 0; i < StateVectorElements.size(); i++) {
        if (mtsGenericTypes<_elementType>::IsEqual(element, *StateVectorElements[i]))
            return StateVectorAccessors[i];
    }
    return 0;
}

#endif // _mtsStateTable_h
