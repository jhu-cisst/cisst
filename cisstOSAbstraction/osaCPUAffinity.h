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

/*!
  \file
  \brief Functions for multi-core CPUs
  \ingroup cisstOSAbstraction
 */

#ifndef _osaCPUAffinity_h
#define _osaCPUAffinity_h

// Always include last
#include <cisstOSAbstraction/osaExport.h>

typedef unsigned short osaCPUMask;

const osaCPUMask OSA_CPUANY = 0x0000;
const osaCPUMask OSA_CPU1   = 0x0001;
const osaCPUMask OSA_CPU2   = 0x0002;
const osaCPUMask OSA_CPU3   = 0x0004;
const osaCPUMask OSA_CPU4   = 0x0008;
const osaCPUMask OSA_CPU5   = 0x0010;
const osaCPUMask OSA_CPU6   = 0x0020;
const osaCPUMask OSA_CPU7   = 0x0040;
const osaCPUMask OSA_CPU8   = 0x0080;
const osaCPUMask OSA_CPU9   = 0x0100;
const osaCPUMask OSA_CPU10  = 0x0200;
const osaCPUMask OSA_CPU11  = 0x0400;
const osaCPUMask OSA_CPU12  = 0x0800;
const osaCPUMask OSA_CPU13  = 0x1000;
const osaCPUMask OSA_CPU14  = 0x2000;
const osaCPUMask OSA_CPU15  = 0x4000;
const osaCPUMask OSA_CPU16  = 0x8000;

const osaCPUMask OSA_CPU_LAST = OSA_CPU16;

// Why not use a bool instead?
enum osaErrno { OSAFAILURE=-1, OSASUCCESS=0 };

//! Return the number of available CPUs
/**
   On SMP architectures, return the number of available CPUs.
   \return The number of available CPUs. 
 */
CISST_EXPORT int osaCPUGetCount();

//! Set the affinity of the current thread
/**
   On SMP architectures, sets the affinity of the current thread to one or
   several CPU. The mask can represent more than one CPU in which case the OS
   will decide which CPU of a set to run the thread.
   \param mask A bitfield mask of CPUs. 
   \return OSASUCCESS if the operation succeeded. OSAERROR otherwise.
 */
CISST_EXPORT osaErrno osaCPUSetAffinity( osaCPUMask mask );

#endif
