
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Tae Soo Kim
  Created on: 2012-02-27

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

*/

#ifndef osaTimeData_h
#define osaTimeData_h

#include <cmath>
#include <sys/time.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>

/*! \brief Implementation of a simple object to represent relative time.
	\ingroup cisstOSAbstraction
	
	The osaTimeData provides an easy way to manipulate time and methods for algebraic operations on time. 
*/
class osaTimeData 
{
public:
		
	/*! Default constructor, construct the osaTimeData of current time*/
	osaTimeData()
	{
		now();
		positive_flag = true;
		normalize();
	}

	/*! Initialize all the elements to the given values 
	  \param _seconds number of seconds used to set seconds of this object
	  \param _nseconds number of nano seconds used to set nseconds of this object
	  \param _positive_flag the boolean flag to determine whether this osaTimeData is positive or negative. Set to true by default. True = positive*/
	osaTimeData(unsigned int _seconds, unsigned int _nseconds, bool _positive_flag=true): seconds(_seconds),nseconds(_nseconds),positive_flag(_positive_flag)
	{
		normalize();	
		struct timespec res;
		clock_getres( CLOCK_REALTIME, &res); 
		resolution = static_cast<long long>(res.tv_nsec);
		
	}
	/*! Initializes all the elements of the osaTimeData object from seconds
	\param seconds the number of seconds in double
	*/
	osaTimeData(double dseconds)
	{
		bool flag = true;
		if(dseconds < 0 )
		{
			flag = false;
			dseconds = dseconds * -1;
		}
		long long fractpart ;      
		seconds = splitDoubles(dseconds,&fractpart);
		nseconds = fractpart;
		positive_flag = flag;
		normalize();
		struct timespec res;
		clock_getres(CLOCK_REALTIME, &res);
		resolution = static_cast<long long>(res.tv_nsec); 
	}

	/*! Destructor*/
	~osaTimeData(){}

	//Utility
	osaTimeData copy() const;
	/*!Set the objects time to equal the given time
	\param newTime the osaTimeData object to set to*/
	void setTime(const osaTimeData &newTime);
	/*!Normalize the osaTimeData object*/
	void normalize();

	/*!Determines whether the two objects are equal
	\param compareTo
	\return true if the objects are equal */
	bool equals(const osaTimeData &compareTo) const;
	
	/*!Returns the number of seconds that this osaTimeData object represents
	\return the number of seconds in double */
	double toSeconds(); 

	/*!Create osaTimeData object from seconds
	\param seconds the number of seconds in double
	\return this object that is converted from given number of seconds*/
	osaTimeData from(double seconds);

	/*!Addition of two osaTimeData objects
	\param rhs the osaTimeData object to add*/
	void add(const osaTimeData &rhs);
	/*!Subtraction of two osaTimeData objects
	\param rhs the osaTimeData object to subtract
	*/
	void subtract(const osaTimeData &rhs);
	/*!Sum of two osaTimeData objects*/
	void sumOf(osaTimeData &first, osaTimeData &second);
	
	std::string toString(); // string representation YYYY/MM/DD, HH:MM:SS.DDDD
	
	//gets,sets
	/*!Returns the seconds of this osaTimeData object.
	\return the number seconds this object represents
	*/
	long long getSeconds();
	/*!Sets the seconds of this osaTimeData object to a specified number of seconds
	\param sec number of seconds to set to
	*/
	void setSeconds(long long sec);
	/*!Returns the nano seconds of this osaTimeData object.
	\return the number of nano-seconds this object holds
	*/
	long long getNSeconds();
	/*!Sets the nano-seconds of this osaTimeData object to a specified number of nanoseconds
	\return nsec number of nano seconds to set to
	*/
	void setNSeconds(long long nsec);
	/*!Returns the resolution of the osaTimeData object in nano seconds
	\return the resolution of the osaTimeData object in nanoseconds
	*/
	long long getResolution();

	//ALGEBRA
	const osaTimeData operator=(const osaTimeData &rhs);
	osaTimeData operator+(const osaTimeData &rhs);
	osaTimeData operator-(const osaTimeData &rhs);
	bool operator==(const osaTimeData &rhs) const;
	bool operator>(const osaTimeData &rhs) const;
	bool operator<(const osaTimeData &rhs) const;
	bool operator>=(const osaTimeData &rhs) const;
	bool operator<=(const osaTimeData &rhs) const;
	osaTimeData operator*(const double &rhs) ;
	osaTimeData operator/(const double &rhs) ;

protected:
	/*!Number of seconds*/
	long long seconds;
	/*!Number of nano seconds*/
	long long nseconds;	
	/*!Resolution of this time object in nanoseconds  */
	long long resolution;
	/*!Boolean to indicate whether this osaTimeData is negatve or positive. Positive = true, negative = false*/
	bool positive_flag;
private:
	/*! Given a double variable, split it into two integers (whole part and fraction part)
	\param doubleToSplit the double value to split
	\param fractionpart the address of where the int value of the fraction part will be stored
	\return the whole part of the doubleToSplit
	*/
	long long splitDoubles(double doubleToSplit, long long *fractionPart);
	/*! sets the properties of the osaTimeData object to current time
	*/
	void now();
}
;
#endif
