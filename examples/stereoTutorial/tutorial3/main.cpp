// See license at http://www.cisst.org/cisst/license.txt


#include <cisstStereoVision.h>
#include "CMyFilter1.h"


int main()
{
    svlInitialize();                                 // 1. Discover supported devices and codecs

    svlStreamManager stream;                         // 2. Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);           // 3. Instantiate video capture filter
    CMyFilter1 filter1;                              // 4. Instantiate user-implemented filter #1
    svlFilterImageWindow window;                     // 5. Instantiate image window filter

    source.SetDevice(0);                             // 6. Select first available capture device

    stream.SetSourceFilter(&source);                 // 7. Assign source filter to stream
    source.GetOutput()->Connect(filter1.GetInput()); // 8. Connect source filter to filter #1
    filter1.GetOutput()->Connect(window.GetInput()); // 9. Connect filter #1 to window filter

    stream.Play();                                   // 10. Initialize and Play video stream

    char ch;
    std::cin >> ch;                                  // (Wait for key-press)

    stream.Release();                                // 11. Release stream

    return 0;
}

