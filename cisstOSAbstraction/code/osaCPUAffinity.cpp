/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaCPUAffinity.h>

#if (CISST_OS == CISST_LINUX)                       ||	\
    (CISST_OS == CISST_LINUX_XENOMAI) 
#include <unistd.h>
#include <sched.h>
#elif (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

int osaCPUGetCount(){
  
  int cpucount = 0;
  
#if (CISST_OS == CISST_LINUX)                       ||	\
    (CISST_OS == CISST_LINUX_XENOMAI) 
    cpucount = sysconf( _SC_NPROCESSORS_ONLN );
#elif (CISST_OS == CISST_WINDOWS)
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	cpucount = sysinfo.dwNumberOfProcessors;
#endif

  return cpucount;
}

osaErrno osaCPUSetAffinity( osaCPUMask mask ){

#if (CISST_OS == CISST_LINUX)                       ||	\
    (CISST_OS == CISST_LINUX_XENOMAI) 

  cpu_set_t set;
  CPU_ZERO( &set );
  
  // Let the OS do the load balancing
  if( mask == OSA_CPUANY ) 
    { return OSASUCCESS; }
  
  // Go one CPU at the time and test for the affinity
  for( int cpu=0; cpu<OSA_CPU_LAST; cpu++ ){
    if( mask & 0x0001 )
      { CPU_SET( cpu, &set ); }
    mask >>= 1;
  }

  if( sched_setaffinity( 0, sizeof( set ), &set ) == 0 )
    { return OSASUCCESS; }
  else
    { return OSAFAILURE; }

#elif (CISST_OS == CISST_WINDOWS)

  // Let the OS do the load balancing
  if( mask == OSA_CPUANY ) 
    { return OSASUCCESS; }

  if( SetThreadAffinityMask( GetCurrentThread(), mask ) != 0 )
    { return OSASUCCESS; }
  else
    { return OSAFAILURE; }

#endif

  return OSAFAILURE;
}
