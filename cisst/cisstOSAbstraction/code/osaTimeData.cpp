


#include <math.h>
#include <iomanip>
#include "../osaTimeData.h"


osaTimeData::osaTimeData()
{
    Seconds = 0;
    Nano_seconds=0;
	Positive = true;
}

osaTimeData::osaTimeData(unsigned int seconds, unsigned int nseconds, bool positive_flag)
{
    Seconds = seconds;
    Nano_seconds = nseconds;
    Positive = positive_flag ; 
	Normalize();
    struct timespec res;
    clock_getres( CLOCK_REALTIME, &res);
    Resolution = static_cast<long long>(res.tv_nsec);
}

osaTimeData::osaTimeData(double dseconds)
{
	bool flag = true;
	if(dseconds < 0 )
	{
		flag = false;
		dseconds = dseconds * -1;
	}
	long long fractpart ;      
	Seconds = SplitDoubles(dseconds,&fractpart);
	Nano_seconds = fractpart;
	Positive = flag;
	Normalize();
	struct timespec res;
	clock_getres(CLOCK_REALTIME, &res);
	Resolution = static_cast<long long>(res.tv_nsec); 
}
/*returns the osaTimeData object of current time*/
void osaTimeData::Now()
{
	timespec res;
	timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	Seconds = static_cast<long long> (now.tv_sec);
	Nano_seconds = static_cast<long long> (now.tv_nsec); 
	Resolution = static_cast<long long>(res.tv_nsec);
}


void osaTimeData::SetTime(const osaTimeData &newTime)
{
	Seconds = newTime.Seconds;
	Nano_seconds = newTime.Nano_seconds;
	Positive = newTime.Positive;	
}

void osaTimeData::Normalize()
{
	while(Nano_seconds < 0)
	{
		Nano_seconds = Nano_seconds + 1000000000;
		Seconds--;
	}
	while(this->Nano_seconds >= 1000000000)
	{
		this->Nano_seconds = this->Nano_seconds - 1000000000;
		this->Seconds++;
	}	
	if(Seconds < 0 )
	{
		if(Positive) // if it was a positive time, make it negative
			Positive = false;
		else  // if it was a negative time, make it positive
			Positive = true;
		Seconds = Seconds * -1;
	}
}

bool osaTimeData::Equals(const osaTimeData &compareTo) const
{
	
	if(Seconds==compareTo.Seconds && Nano_seconds==compareTo.Nano_seconds && Positive == compareTo.Positive )
		return true;
	else
		return false;
}


double osaTimeData::ToSeconds()
{
	if(Positive)
		return Seconds+(Nano_seconds/1000000000.0);
	else
		return -1*(Seconds+Nano_seconds/1000000000.0);
}

osaTimeData osaTimeData::From(double doubleSeconds)
{
	bool flag = true;
	if(doubleSeconds < 0 )
	{
		flag = false;
		doubleSeconds = doubleSeconds * -1;
	}
	long long fractpart , intpart ;      
	intpart = SplitDoubles(doubleSeconds,&fractpart);
	return osaTimeData(intpart,fractpart,flag);
}

void osaTimeData::Add(const osaTimeData &rhs)
{
	Seconds = Seconds + rhs.Seconds;
	Nano_seconds = Nano_seconds + rhs.Nano_seconds;	
	
	Normalize();
}

void osaTimeData::Subtract(const osaTimeData &rhs)
{
	if( (!Positive && rhs.Positive ) ||  (Positive && !rhs.Positive)) // lhs is negative, rhs is positive
	{
		Seconds = Seconds + rhs.Seconds;
		Nano_seconds = Nano_seconds + rhs.Nano_seconds;
	}
	else if(Positive && rhs.Positive)
	{
		Seconds = Seconds - rhs.Seconds;
		Nano_seconds = Nano_seconds - rhs.Nano_seconds;
	}
	else // both negatives
	{
		if(Seconds < rhs.Seconds)
		{
			Seconds =rhs.Seconds - Seconds ;
			Nano_seconds = rhs.Nano_seconds - Nano_seconds ;
			Positive = true; // it is now positive
		}
		else
		{
			Seconds = Seconds - rhs.Seconds;
			Nano_seconds = Nano_seconds - rhs.Nano_seconds;
		}
	}
	Normalize();

}


void osaTimeData::SumOf(osaTimeData &first, osaTimeData &second)
{
	osaTimeData temp(first);
	temp.Add(second);
	Seconds = temp.Seconds;
	Nano_seconds = temp.Nano_seconds;
}

std::string osaTimeData::ToString()
{	
	std::stringstream ss;
   	long long tempArray[9];
 
	if(!Positive)
		ss<<"-"; 
	ss<<Seconds<<".";

	long long temp =  Nano_seconds ;
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

long long osaTimeData::GetSeconds()
{
	return Seconds;
}

long long osaTimeData::GetNanoSeconds()
{
	return Nano_seconds;
}

void osaTimeData::SetSeconds(long long s)
{
	Seconds = s;	
}

void osaTimeData::SetNanoSeconds(long long s)
{
	Nano_seconds = s;
}
long long osaTimeData::GetResolution()
{
	return Resolution;
}

long long osaTimeData::SplitDoubles(double doubleToSplit, long long *fractionPart)
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
	if(!(this->Equals(rhs))	)
	{
		SetTime(rhs);
	}
	return *this;
}

osaTimeData osaTimeData::operator+(const osaTimeData &rhs)
{
	Add(rhs);
	return *this;
}


osaTimeData osaTimeData::operator-(const osaTimeData &rhs)
{
	Subtract(rhs);
	return *this;	
}

bool osaTimeData::operator==(const osaTimeData &rhs) const
{
	return this->Equals(rhs);
}

bool osaTimeData::operator>(const osaTimeData &rhs) const
{
	if(this->Seconds > rhs.Seconds)
		return true;
	else if(this->Seconds == rhs.Seconds && this->Nano_seconds > rhs.Nano_seconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator<(const osaTimeData &rhs) const
{
	if(this->Seconds < rhs.Seconds)
		return true;
	else if(this->Seconds == rhs.Seconds && this->Nano_seconds < rhs.Nano_seconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator>=(const osaTimeData &rhs) const
{
	if(this->Seconds > rhs.Seconds)
		return true;
	else if(this->Seconds == rhs.Seconds && this->Nano_seconds >= rhs.Nano_seconds)
		return true;
	else
		return false;
}
bool osaTimeData::operator<=(const osaTimeData &rhs) const
{
	if(this->Seconds <  rhs.Seconds)
		return true;
	else if(this->Seconds == rhs.Seconds && this->Nano_seconds <= rhs.Nano_seconds)
		return true;
	else
		return false;
}
osaTimeData osaTimeData::operator*(const double &rhs) 
{
	double me = ToSeconds();
	return From(me*rhs);	
}
osaTimeData osaTimeData::operator/(const double &rhs) 
{
	double me = ToSeconds();
	return From(me/rhs);	
}


