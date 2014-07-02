// See license at http://www.cisst.org/cisst/license.txt

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageUnsharpMask.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

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
    svlStreamManager stream;                                      // 2. Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);                        // 3. Instantiate video capture filter
    svlFilterSplitter splitter;                                   // 4. Instantiate stream splitter filter
    svlFilterImageResizer resizer;                                // 5. Instantiate image resizer filter
    svlFilterImageUnsharpMask sharpen;                            // 6. Instantiate unsharp masking filter
    svlFilterImageWindow window;                                  // 7. Instantiate first image window filter
    svlFilterImageWindow window2;                                 // 8. Instantiate second image window filter

    source.SetDevice(0);                                          // 9. Select first available capture device

    splitter.AddOutput("async_out");                              // 10. Add 2nd output to stream slitter filter

    resizer.SetOutputSize(400, 300);                              // 11. Setup image resizer filter

    sharpen.SetAmount(200);                                       // 12. Setup unsharp masking filter
    sharpen.SetRadius(5);
    sharpen.SetThreshold(2);

    stream.SetSourceFilter(&source);                              // 15. Assign source filter to stream
    source.GetOutput()->Connect(splitter.GetInput());             // 16. Connect source filter to splitter filter
    splitter.GetOutput()->Connect(resizer.GetInput());            // 17. Connect splitter output #1 to resizer filter
    resizer.GetOutput()->Connect(window.GetInput());              // 18. Connect resizer filter to 1st window filter

    splitter.GetOutput("async_out")->Connect(sharpen.GetInput()); // 19. Connect splitter output #2 to unsharp masking filter
    sharpen.GetOutput()->Connect(window2.GetInput());             // 20. Connect unsharp masking filter to 2nd window filter

    stream.Play();                                                // 21. Initialize and Play video stream

    char ch;
    std::cin >> ch;                                               // (Wait for key-press)

    stream.Release();                                             // 22. Release stream
}


int main()
{
    svlInitialize();     // 1. Discover supported devices and codecs

    //SimpleStream();
    ProcessingStream();

    return 0;
}
