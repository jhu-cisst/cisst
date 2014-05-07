// See license at http://www.cisst.org/cisst/license.txt


#include "CMyFilter3.h"
#include <cisstStereoVision/svlImageProcessing.h>
#include <cisstOSAbstraction/osaSleep.h>


CMyFilter3::CMyFilter3() :
    svlFilterBase()                                                         // Call baseclass' constructor
{                                                                           //
    AddInput("input", true);                                                // Create synchronous input connector
    AddInputType("input", svlTypeImageRGB);                                 // Set sample type for input connector
                                                                            //
    AddOutput("output", true);                                              // Create synchronous ouput connector
    SetAutomaticOutputType(true);                                           // Set output sample type the same as input sample type
}

int CMyFilter3::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;                                                 // Pass the input sample forward to the output
    return SVL_OK;                                                          //
}

int CMyFilter3::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImageRGB* image = dynamic_cast<svlSampleImageRGB*>(syncInput); // Cast input sample to RGB image type
    svlRGB* pixel = reinterpret_cast<svlRGB*>(image->GetUCharPointer());    // Get RGB pixel buffer for in-place modifications
    unsigned int pixelcount = image->GetWidth() * image->GetHeight();       // Get number of pixels in image
    unsigned int i, val;                                                    //
                                                                            //
    _OnSingleThread(procInfo)                                               // Execute the following block on one thread only
    {                                                                       //
        osaSleep(1.0);
        
        MaxVal = 0;                                                         // Find highest intensity pixel
        for (i = 0; i < pixelcount; i ++) {                                 //
            val = (unsigned int)pixel[i].r + pixel[i].g + pixel[i].b;       //
            if (val >= MaxVal) MaxVal = val;                                //
        }                                                                   //
        MaxVal /= 3;                                                        //
    }                                                                       //
                                                                            //
    _SynchronizeThreads(procInfo);                                          // All threads wait until finished
                                                                            //
    if (MaxVal > 0) {                                                       //

        svlSampleImage* subimage = image->GetSubImage(procInfo);
        
        _CriticalSection(procInfo)
        {
            std::cerr << procInfo->ID << ", "
                      << subimage->GetWidth() << ", " << subimage->GetHeight() << ", "
                      << subimage->GetDataChannels() << ", " << subimage->GetBPP()
                      << std::endl;
        }
        
        svlImageProcessing::Deinterlace(subimage, 0, svlImageProcessing::DI_Blending);
        
        delete subimage;
    }                                                                       //
                                                                            //
    syncOutput = syncInput;                                                 // Pass the modified sample forward to the output
                                                                            //
    return SVL_OK;                                                          //
}

