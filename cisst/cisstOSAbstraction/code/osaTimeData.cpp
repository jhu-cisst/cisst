


#include <math.h>
#include <iomanip>
#include "../osaTimeData.h"

/*returns the osaTimeData object of current time*/
void osaTimeData::now()
{
	timespec res;
	timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	seconds = static_cast<long long> (now.tv_sec);
	nseconds = static_cast<long long> (now.tv_nsec); 
	resolution = static_cast<long long>(res.tv_nsec);
}


void osaTimeData::setTime(const osaTimeData &newTime)
{
	seconds = newTime.seconds;
	nseconds = newTime.nseconds;
	positive_flag = newTime.positive_flag;	
}

void osaTimeData::normalize()
{
	while(nseconds < 0)
	{
		nseconds = nseconds + 1000000000;
		seconds--;
	}
	while(this->nseconds >= 1000000000)
	{
		this->nseconds = this->nseconds - 1000000000;
		this->seconds++;
	}	
	if(seconds < 0 )
	{
		if(positive_flag) // if it was a positive time, make it negative
			positive_flag = false;
		else  // if it was a negative time, make it positive
			positive_flag = true;
		seconds = seconds * -1;
	}
}

bool osaTimeData::equals(const osaTimeData &compareTo) const
{
	
	if(seconds==compareTo.seconds && nseconds==compareTo.nseconds && positive_flag == compareTo.positive_flag )
		return true;
	else
		return false;
}


double osaTimeData::toSeconds()
{
	if(positive_flag)
		return seconds+(nseconds/1000000000.0);
	else
		return -1*(seconds+nseconds/1000000000.0);
}

osaTimeData osaTimeData::from(double doubleSeconds)
{
	bool flag = true;
	if(doubleSeconds < 0 )
	{
		flag = false;
		doubleSeconds = doubleSeconds * -1;
	}
	long long fractpart , intpart ;      
	intpart = splitDoubles(doubleSeconds,&fractpart);
	return osaTimeData(intpart,fractpart,flag);
}

void osaTimeData::add(const osaTimeData &rhs)
{
	seconds = seconds + rhs.seconds;
	nseconds = nseconds + rhs.nseconds;	
	
	normalize();
}

void osaTimeData::subtract(const osaTimeData &rhs)
{
	if( (!positive_flag && rhs.positive_flag ) ||  (positive_flag && !rhs.positive_flag)) // lhs is negative, rhs is positive
	{
		seconds = seconds + rhs.seconds;
		nseconds = nseconds + rhs.nseconds;
	}
	else if(positive_flag && rhs.positive_flag)
	{
		seconds = seconds - rhs.seconds;
		nseconds = nseconds - rhs.nseconds;
	}
	else // both negatives
	{
		if(seconds < rhs.seconds)
		{
			seconds =rhs.seconds - seconds ;
			nseconds = rhs.nseconds - nseconds ;
			positive_flag = true; // it is now positive
		}
		else
		{
			seconds = seconds - rhs.seconds;
			nseconds = nseconds - rhs.nseconds;
		}
	}
	normalize();

}


void osaTimeData::sumOf(osaTimeData &first, osaTimeData &second)
{
	osaTimeData temp(first);
	temp.add(second);
	seconds = temp.seconds;
	nseconds = temp.nseconds;
}

std::string osaTimeData::toString()
{	
	std::stringstream ss;
   	long long tempArray[9];
 
	if(!positive_flag)
		ss<<"-"; 
	ss<<seconds<<".";

	long long temp =  nseconds ;
	for(int i = 0 ; i < 9 ; i++)
	{
		tempArray[8-i] = (temp % 10 );
		temp = temp/10;
	}
	for(int i = 0 ; i < 9 ; i++)
	{
		ss<<tempArray[i];
	}

	return ss.str();
}

long long osaTimeData::getSeconds()
{
	return seconds;
}

long long osaTimeData::getNSeconds()
{
	return nseconds;
}

void osaTimeData::setSeconds(long long s)
{
	seconds = s;	
}

void osaTimeData::setNSeconds(long long s)
{
	nseconds = s;
}
long long osaTimeData::getResolution()
{
	return resolution;
}

long long osaTimeData::splitDoubles(double doubleToSplit, long long *fractionPart)
{

	long long temp = static_cast<long long> ( doubleToSplit * 1000000000);
	long long computation=0;
	for(int i = 0 ; i < 9 ; i++)
	{
		computation = computation+  (temp % 10 ) * pow(10,i);
		temp = temp/10;
	}
	*fractionPart = computation;
	return temp;
}

const osaTimeData osaTimeData::operator=(const osaTimeData &rhs)
{
	if(!(this->equals(rhs))	)
	{
		setTime(rhs);
	}
	return *this;
}

osaTimeData osaTimeData::operator+(const osaTimeData &rhs)
{
	add(rhs);
	return *this;
}


osaTimeData osaTimeData::operator-(const osaTimeData &rhs)
{
	subtract(rhs);
	return *this;	
}

bool osaTimeData::operator==(const osaTimeData &rhs) const
{
	return this->equals(rhs);
}

bool osaTimeData::operator>(const osaTimeData &rhs) const
{
	if(this->seconds > rhs.seconds)
		return true;
	else if(this->seconds == rhs.seconds && this->nseconds > rhs.nseconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator<(const osaTimeData &rhs) const
{
	if(this->seconds < rhs.seconds)
		return true;
	else if(this->seconds == rhs.seconds && this->nseconds < rhs.nseconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator>=(const osaTimeData &rhs) const
{
	if(this->seconds > rhs.seconds)
		return true;
	else if(this->seconds == rhs.seconds && this->nseconds >= rhs.nseconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator<=(const osaTimeData &rhs) const
{
	if(this->seconds <  rhs.seconds)
		return true;
	else if(this->seconds == rhs.seconds && this->nseconds <= rhs.nseconds)
		return true;
	else
		return false;
}
osaTimeData osaTimeData::operator*(const double &rhs) 
{
	double me = toSeconds();
	return from(me*rhs);	
}
osaTimeData osaTimeData::operator/(const double &rhs) 
{
	double me = toSeconds();
	return from(me/rhs);	
}


