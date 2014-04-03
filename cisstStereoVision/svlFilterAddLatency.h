/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Obenshain, Thomas Tantillo, Anton Deguet
  Created on: 2010

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterAddLatency_h
#define _svlFilterAddLatency_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstOSAbstraction/osaTimeServer.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

/*!
  \todo Check if we could use an stl container to manager the list of samples
  \todo Change API to define latency in seconds
  \todo Remove up/down by a hard coded number of samples
  \todo In Release, make sure memory is freed for linked list
  \todo Is memcpy the best way to copy the in/out samples, is there a more generic way?
  \todo Add interface provided to set latency
  \todo Add method to report memory size used
*/

class CISST_EXPORT svlFilterAddLatency : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    svlFilterAddLatency(void);
    virtual ~svlFilterAddLatency();

    void UpLatency(void); //increase latency by 100 ms
    void DownLatency(void); //decrease latency by 100 ms
    void SetFrameDelayed(const unsigned int numberOfFrames);

 protected:
    virtual int Initialize(svlSample * syncInput, svlSample * & syncOutput);
    virtual int Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample * & syncOutput);
    virtual int Release(void);

 private:

    int Add(svlSampleImage * in);
    int Remove(svlSampleImage * out);

    svlSampleImage * OutputImage;

    int FramesDelayed;
    int Length;

    // todo, should we use a std::list at that point?
    typedef struct NodeStruct {
        NodeStruct * Next;
        svlSampleImage * Data;
    } NodeType;

    NodeType * Head, * Tail;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterAddLatency)

#endif // _svlFilterAddLatency_h
