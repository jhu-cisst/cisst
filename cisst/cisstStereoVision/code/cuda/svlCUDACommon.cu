/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung & Balazs Vagvolgyi
  Created on: 2010-05-24

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <stdio.h>
#include <cutil_inline.h>


// CUDA configuration
const int ThreadCountPerBlock = 1 << 7; // 128 threads per block
//const int BlockCount = 1 << 5; // Warp size: 32

extern "C" bool cisst_cudaInitialize()
{
    int count = 0;

    cudaGetDeviceCount(&count);
    if (count == 0) {
        printf("There is no device supporting CUDA.\n");
        return false;
    }

    int i = 0;
    for (i = 0; i < count; i++) {
        cudaDeviceProp prop;
        if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
            if (prop.major >= 1) {
                break;
            }
        }
    }

    if (i == count) {
        printf("There is no device supporting CUDA.\n");
        return false;
    }
    cudaSetDevice(i);

    return true;
}

extern "C" int cisst_cudaGetBlockSize(void)
{
	int device_count;
	int blocksize = 0;
	cudaDeviceProp device_prop;
	
	cudaGetDeviceCount(&device_count);

	// No Cuda Device Found
	if (device_count == 0)
		return 0;

    cudaGetDeviceProperties(&device_prop, 0);

	// Limited By Shared Memory
	// 5 Blocks per MP
	// (2588+28) * 5 ==  13080 < (16384 shared memory of per MP)
	blocksize = ThreadCountPerBlock * 5 * device_prop.multiProcessorCount;

	return blocksize;
}


//////////////////////////////
// MEMORY HANDLER FUNCTIONS //
//////////////////////////////

extern "C" void* cisst_cudaMalloc(unsigned int size)
{
    void* ptr = 0;
    if (cudaMalloc(&ptr, size) == cudaSuccess) return ptr;
    return 0;
}

extern "C" bool cisst_cudaMemset(void* devPtr, unsigned char value, unsigned int size)
{
    if (devPtr == 0) return false;
    if (size == 0) return true;
    if (cudaMemset(devPtr, (int)value, size) == cudaSuccess) return true;
    return false;
}

extern "C" bool cisst_cudaMemcpyHostToDevice(void* devDestPtr, const void* hostSrcPtr, unsigned int size)
{
    if (devDestPtr == 0 || hostSrcPtr == 0) return false;
    if (size == 0) return true;
    if (cudaMemcpy(devDestPtr, hostSrcPtr, size, cudaMemcpyHostToDevice) != cudaSuccess) return false;
    return true;
}

extern "C" bool cisst_cudaMemcpyDeviceToDevice(void* devDestPtr, const void* devSrcPtr, unsigned int size)
{
    if (devDestPtr == 0 || devSrcPtr == 0) return false;
    if (size == 0) return true;
    if (cudaMemcpy(devDestPtr, devSrcPtr, size, cudaMemcpyDeviceToDevice) != cudaSuccess) return false;
    return true;
}

extern "C" bool cisst_cudaMemcpyDeviceToHost(void* hostDestPtr, const void* devSrcPtr, unsigned int size)
{
    if (hostDestPtr == 0 || devSrcPtr == 0) return false;
    if (size == 0) return true;
    if (cudaMemcpy(hostDestPtr, devSrcPtr, size, cudaMemcpyDeviceToHost) != cudaSuccess) return false;
    return true;
}

extern "C" bool cisst_cudaFree(void* devPtr)
{
    if (devPtr == 0) return false;
    cudaFree(devPtr);
    cudaThreadExit();
    return true;
}

