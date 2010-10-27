
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaCPUAffinity.h>

#if (CISST_OS == CISST_LINUX)                       ||	\
    (CISST_OS == CISST_LINUX_XENOMAI) 
#include <unistd.h>
#include <sched.h>
#endif

int osaCPUGetCount(){
  
  int cpucount = 0;
  
#if (CISST_OS == CISST_LINUX)                       ||	\
    (CISST_OS == CISST_LINUX_XENOMAI) 
  cpucount = sysconf( _SC_NPROCESSORS_ONLN );
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

#endif

  return OSAFAILURE;
}

