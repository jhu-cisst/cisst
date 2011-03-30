/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterSplitter_h
#define _svlFilterSplitter_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSplitter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef struct _OutputInfo {
        std::string  name;
        unsigned int threadcount;
        unsigned int buffersize;
    } OutputInfo;

public:
    svlFilterSplitter();

    int AddOutput(const std::string &name, const unsigned int threadcount = 1, const unsigned int buffersize = 3);

protected:
    virtual int UpdateTypes(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    vctDynamicVector<svlFilterOutput*> AsyncOutputs;

protected:
    virtual void CreateInterfaces();
    virtual void AddOutputCommand(const OutputInfo & output);
};

typedef mtsGenericObjectProxy<svlFilterSplitter::OutputInfo> mts_svlFilterSplitter_OutputInfo;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(mts_svlFilterSplitter_OutputInfo);

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSplitter)


CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSplitter::OutputInfo & objref);


#endif // _svlFilterSplitter_h

