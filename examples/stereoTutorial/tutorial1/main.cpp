// See license at http://www.cisst.org/cisst/license.txt


#include <cisstStereoVision.h>


void SimpleStream()
{
    svlStreamManager stream;                        // 2. Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);          // 3. Instantiate video capture filter
    svlFilterImageWindow window;                    // 4. Instantiate image window filter

    source.SetDevice(0);                            // 5. Select first available capture device

    stream.SetSourceFilter(&source);                // 6. Assign source filter to stream
    source.GetOutput()->Connect(window.GetInput()); // 7. Connect source filter to window filter

    stream.Play();                                  // 8. Initialize and Play video stream

    char ch;
    std::cin >> ch;                                 // (Wait for key-press)

    stream.Release();                               // 9. Release stream
}


void ProcessingStream()
{
    svlStreamManager stream;                                      // 2.
    svlFilterSourceVideoCapture source(1);                        // 3.
    svlFilterSplitter splitter;                                   // 4.
    svlFilterImageResizer resizer;                                // 5.
    svlFilterImageUnsharpMask sharpen;                            // 6.
    svlFilterImageWindow window;                                  // 7.
    svlFilterImageWindow window2;                                 // 8.

    source.SetDevice(0);                                          // 9.

    splitter.AddOutput("async_out");                              // 10.

    resizer.SetOutputSize(400, 300);                              // 11.

    sharpen.SetAmount(200);                                       // 12.
    sharpen.SetRadius(5);                                         // 13.
    sharpen.SetThreshold(2);                                      // 14.

    stream.SetSourceFilter(&source);                              // 15.
    source.GetOutput()->Connect(splitter.GetInput());             // 16.
    splitter.GetOutput()->Connect(resizer.GetInput());            // 17.
    resizer.GetOutput()->Connect(window.GetInput());              // 18.

    splitter.GetOutput("async_out")->Connect(sharpen.GetInput()); // 19.
    sharpen.GetOutput()->Connect(window2.GetInput());             // 20.

    stream.Play();                                                // 21. Initialize and Play video stream

    char ch;
    std::cin >> ch;                                               // (Wait for key-press)

    stream.Release();                                             // 22. Release stream
}


int main()
{
    svlInitialize();     // 1. Discover supported devices and codecs

    SimpleStream();
    //ProcessingStream();

    return 0;
}

