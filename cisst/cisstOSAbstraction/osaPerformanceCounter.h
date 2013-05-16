

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Tae Soo Kim
  Created on: 2012-04-10

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

*/
#ifndef osaPerformanceCounter_h
#define osaPerformanceCounter_h

#include "osaTimeData.h"

/*!
\brief Implementation of a reliable performance counter.
\ingroup cisstOSAbstraction

	The osaPerformanceCounter provides a reliable and an easy to use high performance relative counter. It is likely to drift and thus this class contains methods to realign and correct the drift.
*/
class osaPerformanceCounter
{
public:
	/*! Default constructor*/
	osaPerformanceCounter()
	{
		isRunning = false;
	}
	/*!Default destructor*/
	~osaPerformanceCounter(){}
	/*! Reset all the counters to zero and stop the watch*/
	void Reset();
	/*! Sets the origin of this osaPerformanceCounter to a given osaTimeData*/
	void SetOrigin(osaTimeData &origin);
	/*! Start the stopwatch */
	void Start();
	/*! Stop the stopwatch at current counter state*/
	void Stop();
	/*! Returns the state of the counter.
	\return true if counter is running, false if counter is stopped*/
	bool IsRunning();
	/*! Return the current read of the counter*/
	osaTimeData GetElapsedTime() ;
	/*! Delays/Sleeps the system by the time represented by osaTimeData*/
	void delay(osaTimeData delayAmount);
protected:
	osaTimeData origin;
	osaTimeData end;
	bool HasHighPerformanceCounter;
	bool isRunning;
	
private:
};

#endif
