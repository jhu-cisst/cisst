// See license at http://www.cisst.org/cisst/license.txt

#include "CMyEventHandler.h"

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

int main()
{
    svlInitialize();                                // Discover supported devices and codecs

    svlStreamManager stream;                        // Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);          // Instantiate video capture filter
    svlFilterImageWindow window;                    // Instantiate image window filter

    CMyEventHandler event_handler;                  // Instantiate our own window event handler
    window.SetEventHandler(&event_handler);         // Assign event handler to image window filter

    source.SetDevice(0);                            // Select first available capture device

    stream.SetSourceFilter(&source);                // Assign source filter to stream
    source.GetOutput()->Connect(window.GetInput()); // Connect source filter to window filter

    stream.Play();                                  // Initialize and Play video stream

    char ch;
    std::cin >> ch;                                 // (Wait for key-press)

    stream.Release();                               // Release stream

    return 0;
}
