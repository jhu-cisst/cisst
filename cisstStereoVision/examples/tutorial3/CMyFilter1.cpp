// See license at http://www.cisst.org/cisst/license.txt


#include "CMyFilter1.h"


CMyFilter1::CMyFilter1() :
    svlFilterBase()                                                       // Call baseclass' constructor
{                                                                         //
    AddInput("input", true);                                              // Create synchronous input connector
    AddInputType("input", svlTypeImageRGB);                               // Set sample type for input connector
                                                                          //
    AddOutput("output", true);                                            // Create synchronous ouput connector
    SetAutomaticOutputType(true);                                         // Set output sample type the same as input sample type
}

int CMyFilter1::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;                                               // Pass the input sample forward to the output
    return SVL_OK;                                                        //
}

int CMyFilter1::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    _OnSingleThread(procInfo)                                             // Execute the following block on one thread only
    {                                                                     //
        svlSampleImageRGB* image = (svlSampleImageRGB*)syncInput;         // Cast input sample to RGB image type
        svlRGB* pixel = (svlRGB*)image->GetUCharPointer();                // Get RGB pixel buffer for in-place modifications
        unsigned int pixelcount = image->GetWidth() * image->GetHeight(); // Get number of pixels in image
                                                                          //
        for (unsigned int i = 0; i < pixelcount; i ++, pixel ++) {        // for each pixel ...
            pixel->r = 255 - pixel->r;                                    //   process 'red' channel
            pixel->g = 255 - pixel->g;                                    //   process 'green' channel
            pixel->b = 255 - pixel->b;                                    //   process 'blue' channel
        }                                                                 //
    }                                                                     //
                                                                          //
    syncOutput = syncInput;                                               // Pass the modified sample forward to the output
                                                                          //
    return SVL_OK;                                                        //
}

