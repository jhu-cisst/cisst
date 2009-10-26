
#ifndef _robDefines_h
#define _robDefines_h

enum robError{ SUCCESS, ERROR };

inline double Saturate( double val, double min, double max )
{ return (val<min) ? min : ( (max<val) ? max : val ); }

#endif
