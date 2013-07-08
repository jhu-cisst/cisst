/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Tae Soo Kim
  Created on: 2013-05-31

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iomanip>
#include <cisstOSAbstraction/osaTimeData.h>
#include <cmath>
#include <iostream>


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
#include <sys/time.h>

#elif (CISST_OS == CISST_WINDOWS || CISST_OS == CISST_CYGWIN )
#include <cmath>
#include <windows.h>
#endif


osaTimeData::osaTimeData(void):
    Seconds_(0),
    NanoSeconds_(0),
    Positive(true)
{
}

osaTimeData::osaTimeData(int_type seconds, int_type nseconds, bool positive_flag)
{
    Seconds_ = seconds;
    NanoSeconds_ = nseconds;
    Positive = positive_flag ;
    Normalize();
}

osaTimeData::osaTimeData(double dseconds)
{
    if (dseconds < 0) {
        Positive = false;
        dseconds = -dseconds;
    } else {
        Positive = true;
    }
    SplitDoubles(dseconds, Seconds_, NanoSeconds_);
    Normalize();
}

void osaTimeData::SetTime(const osaTimeData & newTime)
{
    Seconds_ = newTime.Seconds_;
    NanoSeconds_ = newTime.NanoSeconds_;
    Positive = newTime.Positive;
}

void osaTimeData::Normalize(void)
{
    while (NanoSeconds_ < 0) {
        NanoSeconds_ += 1000000000;
        --Seconds_;
    }
    while (this->NanoSeconds_ >= 1000000000) {
        NanoSeconds_ -= 1000000000;
        ++Seconds_;
    }
    if (Seconds_ < 0) {
        // if it was a positive time, make it negative
        if (Positive) {
            Positive = false;
        }
        // if it was a negative time, make it positive
        else {
            Positive = true;
            Seconds_ = -Seconds_;
        }
    }
}

bool osaTimeData::Equals(const osaTimeData & compareTo) const
{

    return (Seconds_ == compareTo.Seconds_)
        && (NanoSeconds_ == compareTo.NanoSeconds_)
        && (Positive == compareTo.Positive);
}

double osaTimeData::ToSeconds(void) const
{
    if (Positive) {
        return Seconds_ + (NanoSeconds_ / 1000000000.0);
    } else {
        return -1.0 * (Seconds_ + NanoSeconds_ / 1000000000.0);
    }
}

osaTimeData & osaTimeData::From(double doubleSeconds)
{
    if (doubleSeconds < 0) {
        Positive = false;
        doubleSeconds = -doubleSeconds;
    } else {
        Positive = true;
    }
    SplitDoubles(doubleSeconds, Seconds_, NanoSeconds_);
    Normalize();
    return *this;
}

void osaTimeData::Add(const osaTimeData & rhs)
{
    Seconds_ += rhs.Seconds_;
    NanoSeconds_ += rhs.NanoSeconds_;
    Normalize();
}

void osaTimeData::Subtract(const osaTimeData & rhs)
{
    // lhs is negative, rhs is positive
    if ((!Positive && rhs.Positive)
        || (Positive && !rhs.Positive))   {
        Seconds_ += rhs.Seconds_;
        NanoSeconds_ += rhs.NanoSeconds_;
    }
    else if(Positive && rhs.Positive) {
        Seconds_ -= rhs.Seconds_;
        NanoSeconds_ -= rhs.NanoSeconds_;
    }
    // both negatives
    else {
        if (Seconds_ < rhs.Seconds_) {
            Seconds_ = rhs.Seconds_ - Seconds_ ;
            NanoSeconds_ = rhs.NanoSeconds_ - NanoSeconds_ ;
            Positive = true; // it is now positive
        }
        else {
            Seconds_ -= rhs.Seconds_;
            NanoSeconds_ -= rhs.NanoSeconds_;
        }
    }
    Normalize();
}

void osaTimeData::SumOf(const osaTimeData & first, const osaTimeData & second)
{
    *this = first;
    this->Add(second);
}

std::string osaTimeData::ToString(void) const
{
    std::stringstream ss;
    int_type tempArray[9];

    if (!Positive) {
        ss << "-";
    }
    ss << Seconds_ << ".";

    int_type temp =  NanoSeconds_ ;
    for (unsigned int i = 0 ; i < 9 ; i++) {
        tempArray[8-i] = (temp % 10 );
        temp = temp / 10;
    }
    for (unsigned int i = 0 ; i < 9 ; i++) {
        ss << tempArray[i];
    }
    return ss.str();
}

bool osaTimeData::IsPositive(void) const
{
    return Positive;
}

void osaTimeData::SetPositive(bool flag)
{
    Positive = flag ;
}

const osaTimeData::int_type & osaTimeData::Seconds(void) const
{
    return Seconds_;
}

const osaTimeData::int_type & osaTimeData::NanoSeconds(void) const
{
    return NanoSeconds_;
}

void osaTimeData::SetSeconds(int_type s)
{
    Seconds_ = s;
}

void osaTimeData::SetNanoSeconds(int_type s)
{
    NanoSeconds_ = s;
}

void osaTimeData::SplitDoubles(const double & seconds, int_type & fullSeconds, int_type & nanoSeconds)
{

    int_type temp = static_cast<int_type>(seconds * 1000000000);
    int_type computation = 0;
    for (unsigned int i = 0 ; i < 9 ; i++) {
        computation = computation + (temp % 10 ) * pow(10.0, i);
        temp = temp / 10;
    }
    nanoSeconds = computation;
    fullSeconds = temp;
}

const osaTimeData osaTimeData::operator=(const osaTimeData & rhs)
{
    SetTime(rhs);
    return *this;
}

osaTimeData osaTimeData::operator+(const osaTimeData & rhs)
{
    osaTimeData result(*this);
    result.Add(rhs);
    return result;
}

osaTimeData osaTimeData::operator-(const osaTimeData & rhs)
{
    osaTimeData result(*this);
    result.Subtract(rhs);
    return result;
}

bool osaTimeData::operator==(const osaTimeData & rhs) const
{
    return this->Equals(rhs);
}

bool osaTimeData::operator>(const osaTimeData & rhs) const
{
    if (this->IsPositive() && !rhs.IsPositive()) {
        return true;
    }
    else if (!this->IsPositive() && rhs.IsPositive()) {
        return false;
    }
    else if (!this->IsPositive() && !rhs.IsPositive()) {
        if (this->Seconds_ < rhs.Seconds_) {
            return true;
        }
        else if (this->Seconds_ == rhs.Seconds_ && this->NanoSeconds_ < rhs.NanoSeconds_) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if (this->Seconds_ > rhs.Seconds_) {
            return true;
        }
        else if (this->Seconds_ == rhs.Seconds_ && this->NanoSeconds_ > rhs.NanoSeconds_) {
            return true;
        }
        else {
            return false;
        }
    }
}

bool osaTimeData::operator<(const osaTimeData & rhs) const
{
    return (rhs > *this);
}

bool osaTimeData::operator>=(const osaTimeData & rhs) const
{
    return ((*this == rhs)
            || (*this > rhs));
}

bool osaTimeData::operator<=(const osaTimeData & rhs) const
{
    return ((*this == rhs)
            || (*this < rhs));
}

osaTimeData osaTimeData::operator*(const double & rhs)
{
    osaTimeData result(this->ToSeconds() * rhs);
    return result;
}

osaTimeData osaTimeData::operator/(const double & rhs)
{
    osaTimeData result(this->ToSeconds() / rhs);
    return result;
}

/*returns the osaTimeData object of current time
  \param indicator 0-real time, 1-monotonic*/
osaTimeData osaTimeNow(int indicator)
{
    typedef osaTimeData::int_type int_type;
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
    timeval now;
    windows_gettime(&now);
    const int_type seconds = static_cast<int_type>(now.tv_sec);
    const int_type nanoSeconds = static_cast<int_type>(now.tv_usec);
    return osaTimeData(seconds, nanoSeconds);
#elif (CISST_OS == CISST_DARWIN)
    struct timeval now;
    gettimeofday(&now, NULL);
    const int_type seconds = now.tv_sec;
    const int_type nanoSeconds = now.tv_usec * 1000;
    return osaTimeData(seconds, nanoSeconds);
#else
    //   timespec res;
    timespec now;
    if (indicator == 0) {
        clock_gettime(CLOCK_REALTIME, &now);
    }
    else {
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
    const int_type seconds = static_cast<int_type>(now.tv_sec);
    const int_type nanoSeconds = static_cast<int_type>(now.tv_nsec);
    //   Resolution = static_cast<long long>(res.tv_nsec);
    return osaTimeData(seconds, nanoSeconds);
#endif
}

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
LARGE_INTEGER getFILETIMEoffset(void)
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

void windows_gettime(struct timeval *vtv)
{
    LARGE_INTEGER t;
    FILETIME f;
    double microseconds;
    static LARGE_INTEGER offset;
    static double frequencyToMicroseconds;
    static int initialized = 0;
    static BOOL usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);

        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        }
        else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) {
        QueryPerformanceCounter(&t);
    }
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart |= f.dwLowDateTime;
    }
    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;

    return ;
}

#endif

std::ostream & operator << (std::ostream & outputStream,
                            const osaTimeData & timeData) {
    outputStream << (timeData.IsPositive() ? '+' : '-') << timeData.Seconds() << "s" << timeData.NanoSeconds() << "ns";
    return outputStream;
}
