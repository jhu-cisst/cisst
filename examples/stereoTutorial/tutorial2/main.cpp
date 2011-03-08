// See license at http://www.cisst.org/cisst/license.txt


#include <cisstStereoVision.h>
#include "CMyEventHandler.h"


int main()
{
    svlInitialize();                                // 1. Discover supported devices and codecs

    svlStreamManager stream;                        // 2. Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);          // 3. Instantiate video capture filter
    svlFilterImageWindow window;                    // 4. Instantiate image window filter

    CMyEventHandler event_handler;                  // 5. Instantiate our own window event handler
    window.SetEventHandler(&event_handler);         // 6. Assign event handler to image window filter

    source.SetDevice(0);                            // 7. Select first available capture device

    stream.SetSourceFilter(&source);                // 8. Assign source filter to stream
    source.GetOutput()->Connect(window.GetInput()); // 9. Connect source filter to window filter

    stream.Play();                                  // 10. Initialize and Play video stream

    char ch;
    std::cin >> ch;                                 // (Wait for key-press)

    stream.Release();                               // 11. Release stream

    return 0;
}

