
/*
  Author(s): Tae Soo Kim
  Created on: 2012-04-03

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

*/

#ifndef osaDate_h
#define osaDate_h

#include <time.h>
#include <string>
#include "osaTimeData.h"
#include <stdexcept>
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
		\param t osaTimeData object that will be converted to a osaDate object
	*/
	osaDate(osaTimeData &t);
	/*!Default destructor*/
	~osaDate(){} 
	/*! Creates a std::string representation of this osaDate object.
	*/
	std::string ToString() const;

    /*! Creates an osaDate object from a given osaTimeData object
        \param timeData osaTimeData object to be converted
     */
    void From(osaTimeData &timeData);

    /* Creates an osaTimeData object from this osaDate object
        \pram timeData the osaTimeData object to convert this osaDate object to
     */
    void To(osaTimeData &timeData) const;

    unsigned int& GetYear(void);
    const unsigned int& GetYear(void) const;

    unsigned int& GetMonth(void);
    const unsigned int& GetMonth(void) const;
    
    unsigned int& GetDay(void);
    const unsigned int& GetDay(void) const;

    unsigned int& GetHour(void);
    const unsigned int& GetHour(void) const;
   
    unsigned int& GetMinute(void);
    const unsigned int& GetMinute(void) const;

    unsigned int& GetSecond(void);
    const unsigned int& GetSecond(void) const;

    int_type& GetNanoSecond(void);
private:
protected:
	unsigned int YearMember;
 	unsigned int MonthMember;
	unsigned int DayMember;
	unsigned int HourMember;
	unsigned int MinuteMember;
	unsigned int SecondMember;
    int_type NanoSeconds;
};

#endif
