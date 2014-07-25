// See license at http://www.cisst.org/cisst/license.txt

#include "CMyFilter1.h"

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

int main()
{
    svlInitialize();                                 // Discover supported devices and codecs
                                                     //
    svlStreamManager stream;                         // Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);           // Instantiate video capture filter
    CMyFilter1 filter1;                              // Instantiate user-implemented filter #1
    svlFilterImageWindow window;                     // Instantiate image window filter
                                                     //
    source.SetDevice(0);                             // Select first available capture device
                                                     //
    stream.SetSourceFilter(&source);                 // Assign source filter to stream
    source.GetOutput()->Connect(filter1.GetInput()); // Connect source filter to filter #1
    filter1.GetOutput()->Connect(window.GetInput()); // Connect filter #1 to window filter
                                                     //
    stream.Play();                                   // Initialize and Play video stream
                                                     //
    char ch;                                         //
    std::cin >> ch;                                  // (Wait for key-press)
                                                     //
    stream.Release();                                // Release stream
                                                     //
    return 0;                                        //
}
