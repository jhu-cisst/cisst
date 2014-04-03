/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFilterImageTranslation_h
#define _svlFilterImageTranslation_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageTranslation : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageTranslation();

    int SetTranslation(const int h_translation, const int v_translation, unsigned int videoch = SVL_LEFT);
    int SetTranslationHoriz(const int translation, unsigned int videoch = SVL_LEFT);
    int SetTranslationVert(const int translation, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSampleImage* OutputImage;

    vctFixedSizeVector<int, SVL_MAX_CHANNELS> HorizTranslation;
    vctFixedSizeVector<int, SVL_MAX_CHANNELS> VertTranslation;

    void Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert);

protected:
    virtual void CreateInterfaces();
    virtual void SetTranslationLCommand(const vctInt2 & translation);
    virtual void SetTranslationRCommand(const vctInt2 & translation);
    virtual void GetTranslationLCommand(vctInt2 & translation) const;
    virtual void GetTranslationRCommand(vctInt2 & translation) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageTranslation)

#endif // _svlFilterImageTranslation_h

