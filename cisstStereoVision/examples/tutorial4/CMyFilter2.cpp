// See license at http://www.cisst.org/cisst/license.txt


#include "CMyFilter2.h"


CMyFilter2::CMyFilter2() :
    svlFilterBase()                                                         // Call baseclass' constructor
{                                                                           //
    AddInput("input", true);                                                // Create synchronous input connector
    AddInputType("input", svlTypeImageRGB);                                 // Set sample type for input connector
                                                                            //
    AddOutput("output", true);                                              // Create synchronous output connector
    SetAutomaticOutputType(true);                                           // Set output sample type the same as input sample type
}

int CMyFilter2::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;                                                 // Pass the input sample forward to the output
    return SVL_OK;                                                          //
}

int CMyFilter2::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImageRGB* image = dynamic_cast<svlSampleImageRGB*>(syncInput); // Cast input sample to RGB image type
    svlRGB* pixel = reinterpret_cast<svlRGB*>(image->GetUCharPointer());    // Get RGB pixel buffer for in-place modifications
    unsigned int pixelcount = image->GetWidth() * image->GetHeight();       // Get number of pixels in image
    unsigned int i, res;                                                    //
                                                                            //
    // for (i = 0; i < pixelcount; i ++)
    _ParallelLoop(procInfo, i, pixelcount)                                  //
    {                                                                       //
        res  = pixel[i].r;                                                  // process data
        res += pixel[i].g;                                                  //
        res += pixel[i].b;                                                  //
        res /= 3;                                                           //
        pixel[i].r = pixel[i].g = pixel[i].b = res;                         //
    }                                                                       //
                                                                            //
    syncOutput = syncInput;                                                 // Pass the modified sample forward to the output
                                                                            //
    return SVL_OK;                                                          //
}

