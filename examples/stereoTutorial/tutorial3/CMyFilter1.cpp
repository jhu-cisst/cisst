// See license at http://www.cisst.org/cisst/license.txt


#include "CMyFilter1.h"


CMyFilter1::CMyFilter1() :
    svlFilterBase()
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int CMyFilter1::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int CMyFilter1::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;

    _OnSingleThread(procInfo)
    {
        svlSampleImageRGB* image = (svlSampleImageRGB*)syncInput;
        const unsigned int pixelcount = image->GetWidth() * image->GetHeight();
        svlRGB* pixel = (svlRGB*)image->GetUCharPointer();

        for (unsigned int i = 0; i < pixelcount; i ++, pixel ++) {
            pixel->r = 255 - pixel->r;
            pixel->g = 255 - pixel->g;
            pixel->b = 255 - pixel->b;
        }
    }

    return SVL_OK;
}

