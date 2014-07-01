// See license at http://www.cisst.org/cisst/license.txt

#include "CMyFilter2.h"
#include "CMyFilter3.h"

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

int main()
{
    svlInitialize();                                                // Discover supported devices and codecs
                                                                    //
    svlStreamManager stream(2);                                     // Instantiate SVL Stream
    svlFilterSourceVideoCapture source(1);                          // Instantiate video capture filter
    svlFilterSplitter splitter;                                     // Instantiate splitter filter
    CMyFilter2 filter2;                                             // Instantiate user-implemented filter #2
    CMyFilter3 filter3;                                             // Instantiate user-implemented filter #3
    svlFilterImageWindow window1;                                   // Instantiate image window filter #1
    svlFilterImageWindow window2;                                   // Instantiate image window filter #2
                                                                    //
    source.SetDevice(0);                                            // Select first available capture device
                                                                    //
    splitter.AddOutput("asyncbranch", 7, 3);                        // Add second output (async) to splitter
                                                                    //
    window1.SetTitle("Main branch");                                // Assign title text to window filter #1
    window2.SetTitle("Side branch");                                // Assign title text to window filter #2
                                                                    //
    stream.SetSourceFilter(&source);                                // Assign source filter to stream
    source.GetOutput()->Connect(splitter.GetInput());               // Connect source filter to splitter filter
    splitter.GetOutput()->Connect(filter2.GetInput());              // Connect splitter filter to filter #2
    splitter.GetOutput("asyncbranch")->Connect(filter3.GetInput()); // Connect splitter filter to filter #3
    filter2.GetOutput()->Connect(window1.GetInput());               // Connect filter #2 to window #1
    filter3.GetOutput()->Connect(window2.GetInput());               // Connect filter #3 to window #2
                                                                    //
    stream.Play();                                                  // Initialize and Play video stream
                                                                    //
    char ch;                                                        //
    std::cin >> ch;                                                 // (Wait for key-press)
                                                                    //
    stream.Release();                                               // Release stream
                                                                    //
    return 0;                                                       //
}
