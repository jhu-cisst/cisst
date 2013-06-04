
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
	typedef long long int int_type;

	/*! Default constructor, construct the osaTimeData of current time*/
	osaTimeData();

	/*! Initialize all the elements to the given values 
	  \param seconds number of seconds used to set seconds of this object
	  \param nseconds number of nano seconds used to set nseconds of this object
	  \param positive_flag the boolean flag to determine whether this osaTimeData is positive or negative. Set to true by default. True = positive*/
	osaTimeData(const int_type seconds,const int_type nseconds, bool positive_flag=true);
	/*! Initializes all the elements of the osaTimeData object from seconds
	\param seconds the number of seconds in double
	*/
	osaTimeData(double dseconds);

	/*! Destructor*/
	~osaTimeData(){}

	//Utility
//	osaTimeData Copy() const;
	/*!Set the objects time to equal the given time
	\param newTime the osaTimeData object to set to*/
	void SetTime(const osaTimeData &newTime);
	/*!Normalize the osaTimeData object*/
	void Normalize();

	/*!Determines whether the two objects are equal
	\param compareTo
	\return true if the objects are equal */
	bool Equals(const osaTimeData &compareTo) const;
	
	/*!Returns the number of seconds that this osaTimeData object represents
	\return the number of seconds in double */
	double ToSeconds(); 

	/*!Create osaTimeData object from seconds
	\param seconds the number of seconds in double
	\return this object that is converted from given number of seconds*/
	osaTimeData From(double seconds);

	/*!Addition of two osaTimeData objects
	\param rhs the osaTimeData object to add*/
	void Add(const osaTimeData &rhs);
	/*!Subtraction of two osaTimeData objects
	\param rhs the osaTimeData object to subtract
	*/
	void Subtract(const osaTimeData &rhs);
	/*!Sum of two osaTimeData objects*/
	void SumOf(osaTimeData &first, osaTimeData &second);
	
	std::string ToString() const; // string representation YYYY/MM/DD, HH:MM:SS.DDDD
	
    /*!Returns true if the osaTimeData is positive
    \return true if the object is positive
    */
    bool IsPositive(void) const;
    /*!sets the positive flag of osaTimeData
    \param flag if true, the osaTimeData is set to positive, else, negative
    */
    void SetPositive(bool flag);
	//gets,sets
	/*!Returns the seconds of this osaTimeData object.
	\return the number seconds this object represents
	*/
	long long GetSeconds() const;
	/*!Sets the seconds of this osaTimeData object to a specified number of seconds
	\param sec number of seconds to set to
	*/
	void SetSeconds(long long sec);
	/*!Returns the nano seconds of this osaTimeData object.
	\return the number of nano-seconds this object holds
	*/
	long long GetNanoSeconds() const;
	/*!Sets the nano-seconds of this osaTimeData object to a specified number of nanoseconds
	\return nsec number of nano seconds to set to
	*/
	void SetNanoSeconds(long long nsec);
	/*!Returns the resolution of the osaTimeData object in nano seconds
	\return the resolution of the osaTimeData object in nanoseconds
	*/
//	long long GetResolution();

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
	int_type Seconds;
	/*!Number of nano seconds*/
	int_type NanoSeconds;	
	/*!Boolean to indicate whether this osaTimeData is negatve or positive. Positive = true, negative = false*/
	bool Positive;
private:
	/*! Given a double variable, split it into two integers (whole part and fraction part)
	\param doubleToSplit the double value to split
	\param fractionpart the address of where the int value of the fraction part will be stored
	\return the whole part of the doubleToSplit
	*/
	void SplitDoubles(const double &seconds, int_type &fullSeconds, int_type &nanoSeconds);
}
;


/*! sets the properties of the osaTimeData object to current time
*/
osaTimeData osaTimeNow(void);
#endif
