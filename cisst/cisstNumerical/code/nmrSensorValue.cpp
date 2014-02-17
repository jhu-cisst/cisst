#include <cisstNumerical/nmrSensorValue.h>

CMN_IMPLEMENT_SERVICES(nmrSensorValue);

/*! Constructor
\param v A pointer to a vector of sensor values
*/
nmrSensorValue::nmrSensorValue(vctDynamicVector<double> * v)
{
	values = v;
}

//! Gets the sensor values. 
/*! getValues
\return vctDynamicVector<double> The sensor values
*/
vctDynamicVector<double> nmrSensorValue::getValues()
{
	return *values;
}