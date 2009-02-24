/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

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
#include <cisstMultiTask/mtsStateArrayBase.h>
#include <cisstMultiTask/mtsStateArray.h>
#include <cisstMultiTask/mtsStateIndex.h>


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
  cmnGenericObject.  The state data table also resolves conflicts
  between reads and writes to the state, by ensuring that the reader
  head is always one behind the write head. To ensure this we have an
  assumption here that there is only one writer, though there can be
  multiple readers. State Data Table is also refered as Data Table or
  State Table elsewhere in the documentation.
 */
class CISST_EXPORT mtsStateTable {
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
    std::vector<cmnGenericObject *> StateVectorElements;
    
	/*! The columns entries can be accessed by name. This vector
	  stores the names corresponding to the columns. */
	std::vector<std::string> StateVectorDataNames;

#if 0  // PK: I believe the following is obsolete
	/*! This vector of boolean indicates if the data in the
	   corresponding column under the write head is valid. */
	std::vector<bool> DataValid;

	/*! This sets the default policy on copying the previous data on
	  the advance of the write head, if no valid data was written. If
	  true data gets copied */
	std::vector<bool> Copy;
#endif

	/*! The vector contains the time stamp in counts or ticks per
	  period of the task that the state table is associated with. */
	std::vector<mtsStateIndex::TimeTicksType> Ticks;

    /*! The start/end times for the current row of data. We could use
        mtsStateData<cmnDouble> instead of (TicId, Tic) and (TocId, Toc). */
    mtsStateDataId TicId, TocId;
    cmnDouble Tic, Toc;

    /*! The measured task period (difference between current Tic and
        previous Tic). */
    mtsStateDataId PeriodId;
    cmnDouble Period;

    /*! The time server used to provide absolute and relative times. */
    const osaTimeServer *TimeServer;

    /*! The sum of all the periods (time differences between
        consecutive Tic values); used to compute average period. */
    double SumOfPeriods;

    /*! The average period over the last HistoryLength samples. */
    double AvgPeriod;

 public:
	/*! Constructor. Constructs a state table with a default
	  size of 256 rows. */
	mtsStateTable(int size = 256);

	/*! Default destructor. Does nothing */
	~mtsStateTable() {}

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
    
	/*! Read specified data to be used by a reader */
	bool ReadFromReader(mtsStateDataId id, const mtsStateIndex &timeIndex,
                        cmnGenericObject &obj) const;
    
	/*! Read specified data to be used by a reader (PK: do we need this?) */
	bool ReadFromReader(const std::string & name, const mtsStateIndex & timeIndex,
                        cmnGenericObject & obj);

	/*! Read specified data vector to be used by a reader */
    bool ReadVectorFromReader(mtsStateDataId id, const mtsStateIndex & timeIndexStart, 
                              const mtsStateIndex & timeIndexEnd, cmnGenericObject & obj) const;

	/*! Add an element to the table. Should be called during startup
	    of a real time loop.  All the non-const methods, that can be
	    called from a writer only. Returns index of data within state data table.
        Note: the copy parameter is obsolete. */
	template <class _elementType>
    mtsStateDataId NewElement(const std::string & name = "", _elementType * element = 0 /* ADV: ask PKAZ if still needed, bool copy = true*/);

    /*! Return pointer to the state data element specified by the id.
      This element is the same type as the state data table entry. */
    template<class _elementType>
    _elementType * GetStateDataElement(mtsStateDataId id) const {
        return StateVectorElements[index];
    }

	/*! Get a handle for data to be used by a writer */
	mtsStateIndex GetIndexWriter(void);

	/*! Read specified data to be used by a writer */
	bool ReadFromWriter(mtsStateDataId id, const mtsStateIndex & timeIndex, cmnGenericObject & obj);

	/*! Write specified data. Please note our assumption that there is
      only one writer for the table. PK: this is now obsolete (write done by Advance). */
	bool Write(mtsStateDataId id, const cmnGenericObject &obj);

    /*! Start the current cycle. This just records the starting timestamp (Tic). */
    void Start(void);

	/*! Advance the pointers of the circular buffer. Note that since there is only a single
        writer, it is not necessary to use mutual exclusion primitives; the critical section
        can be handled by updating (incrementing) the write index before the read index.
	 */
	void Advance(void);

    double GetTic() const { return Tic.Data; }
    double GetToc() const { return Toc.Data; }

    /*! Return the moving average of the measured period (i.e., average of last
        HistoryLength values). */
    double GetAveragePeriod() const { return AvgPeriod; }

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

};


// overload mtsObjectName to provide the class name
inline std::string mtsObjectName(const mtsStateTable * CMN_UNUSED(object))
{
    return "mtsStateTable";
}


template <class _elementType>
mtsStateDataId mtsStateTable::NewElement(const std::string & name, _elementType * element /* ADV: ask PKAZ if still needed, bool copy*/) {
    mtsStateArray<_elementType> * elementHistory =
        new mtsStateArray<_elementType>(*element,
                                        HistoryLength);
    StateVector.push_back(elementHistory);
    NumberStateData = StateVector.size();
    StateVectorElements.push_back(element); 
#if 0
    DataValid.push_back(false);
    Copy.push_back(copy);
#endif
    StateVectorDataNames.push_back(name);
    return NumberStateData-1;
}

#endif // _mtsStateTable_h

