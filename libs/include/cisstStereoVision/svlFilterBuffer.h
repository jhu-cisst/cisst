/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterBuffer.h 1218 2010-02-22 16:08:09Z adeguet1 $

  Author(s):	Daniel Mirota
  Created on:	2010-03-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _svlFilterBuffer_h
#define _svlFilterBuffer_h

#include <cisstStereoVision.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

/*!
 Abstract class that provides the structure for implementing interfaces between
 the video capture devices and other components interface a video buffer
*/


#define VBsize 10


typedef vctDynamicNArrayRef<unsigned char,3> NumpyNArrayType;
typedef NumpyNArrayType::nsize_type SizeType;
typedef NumpyNArrayType::nindex_type IndexType;
typedef NumpyNArrayType::nstride_type StrideType;


class CISST_EXPORT svlFilterBuffer : public svlFilterBase
{
public:



    /*!
     Constructor
    */
    svlFilterBuffer();

    /*!
     Destructor
    */
    virtual ~svlFilterBuffer();
    
    unsigned int GetWidth(unsigned int videoch = 0);
    unsigned int GetHeight(unsigned int videoch = 0);
    unsigned int GetDataChannels();

    vctDynamicMatrixRef<unsigned char> GetCurrentFrame();
    int GetCurrentFrameNArray(NumpyNArrayType matrix_in);


private:
    int VideoBindex;
    vctDynamicMatrix<unsigned char> VideoBuffer[VBsize];
    double VideoTimeStamp[VBsize];
    osaThreadSignal IsFrameSetEvent;
    bool IsFrameSet;

    NumpyNArrayType svlBufferNArrayRef;
    NumpyNArrayType numpyNArrayRef;
    svlSample* OutputData;

    virtual int Initialize(svlSample* inputdata, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* inputdata, svlSample* &syncOutput);

    //Warning: These are quick subs
    virtual int  UpdateTypes(svlFilterInput &input, svlStreamType type){
        return SVL_OK;
    };
    /*virtual int  Initialize(svlSample* syncInput, svlSample* &syncOutput){
        return Initialize(syncInput);
    };*/
    virtual int  OnStart(unsigned int procCount){
        return SVL_OK;}
    /*virtual  int  Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput){
        return ProcessFrame(procInfo,syncInput);
    }*/
    virtual void OnStop(void){}
    virtual int  Release(void){return SVL_OK;}

};

#endif // _svlFilterCallback_h

