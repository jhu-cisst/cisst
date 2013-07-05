/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTimeServerTest.cpp 3615 2012-04-25 00:46:07Z adeguet1 $

  Author(s): Tae Soo Kim
  Created on: 2013-06-01

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _osaDate_h
#define _osaDate_h

#include <time.h>
#include <string>
#include <cisstOSAbstraction/osaTimeData.h>

/*! \brief Implementation of a simple object to represent date
  \ingroup cisstOSAbstraction

  The osaDate provides an easy way to visualize and store date information
*/
class osaDate
{
public:
	typedef long long int int_type;
	/*! Default contructor*/
	osaDate();

	/*! Create an osaDate object from a given osaTimeData object 
      \param t osaTimeData object that will be converted to a osaDate object */
	osaDate(osaTimeData & timeData);

    /*! Create an osaDate object from the specified years, months, days,
      hours, minutes, seconds, and nano seconds. */
    osaDate(unsigned int years, unsigned int months, unsigned int days, unsigned int hours, 
            unsigned int minutes, unsigned int seconds, int_type nanoSeconds);
    
	/*!Default destructor*/
	~osaDate() {} 

	/*! Creates a std::string representation of this osaDate object. */
	std::string ToString(void) const;

    /*! Creates an osaDate object from a given osaTimeData object
      \param timeData osaTimeData object to be converted. */
    void From(osaTimeData & timeData);

    /* Creates an osaTimeData object from this osaDate object
       \param timeData the osaTimeData object to convert this osaDate object to */
    void To(osaTimeData & timeData) const;

    unsigned int & Years(void);
    const unsigned int & Years(void) const;

    unsigned int & Months(void);
    const unsigned int & Months(void) const;
    
    unsigned int & Days(void);
    const unsigned int & Days(void) const;

    unsigned int & Hours(void);
    const unsigned int & Hours(void) const;
   
    unsigned int & Minutes(void);
    const unsigned int & Minutes(void) const;

    unsigned int & Seconds(void);
    const unsigned int & Seconds(void) const;

    int_type & NanoSeconds(void);
    const int_type & NanoSeconds(void) const;

protected:
	unsigned int Years_;
 	unsigned int Months_;
	unsigned int Days_;
	unsigned int Hours_;
	unsigned int Minutes_;
	unsigned int Seconds_;
    int_type NanoSeconds_;
};

osaDate osaDateNow(void);

#endif // _osaDate_h
