


#ifndef _osaCPUAffinity_h
#define _osaCPUAffinity_h

// Always include last
#include <cisstOSAbstraction/osaExport.h>

typedef unsigned short osaCPUMask;

#define OSA_CPUANY 0x0000
#define OSA_CPU1   0x0001
#define OSA_CPU2   0x0002
#define OSA_CPU3   0x0004
#define OSA_CPU4   0x0008
#define OSA_CPU5   0x0010
#define OSA_CPU6   0x0020
#define OSA_CPU7   0x0040
#define OSA_CPU8   0x0080
#define OSA_CPU9   0x0100
#define OSA_CPU10  0x0200
#define OSA_CPU11  0x0400
#define OSA_CPU12  0x0800
#define OSA_CPU13  0x1000
#define OSA_CPU14  0x2000
#define OSA_CPU15  0x4000
#define OSA_CPU16  0x8000

#define OSA_CPU_LAST  OSA_CPU16

enum osaErrno { OSAFAILURE=-1, OSASUCCESS=0 };

//! Return the number of available CPUs
/**
   On SMP architectures, return the number of available CPUs.
   \return The number of available CPUs. 
 */
int osaCPUGetCount();

//! Set the affinity of the current thread
/**
   On SMP architectures, sets the affinity of the current thread to one or
   several CPU. The mask can represent more than one CPU in which case the OS
   will decide which CPU of a set to run the thread.
   \param mask A bitfield mask of CPUs. 
   \return OSASUCCESS if the operation succeeded. OSAERROR otherwise.
 */
osaErrno osaCPUSetAffinity( osaCPUMask mask );

#endif
