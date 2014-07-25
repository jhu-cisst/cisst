// See license at http://www.cisst.org/cisst/license.txt

#include "CMyEventHandler2.h"

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
  
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_DARWIN)
    const std::string MediaDir = "../";
#else
    const std::string MediaDir = "";
#endif

int main()
{
    svlInitialize();


    ////////////////////
    // Setup overlays //
    ////////////////////

    // Rectangle overlay
    svlOverlayStaticRect rect_overlay(0,                           // video channel
                                      true,                        // visible
                                      svlRect(400, 185, 474, 234), // rectangle size and position
                                      svlRGB(128, 64, 64),         // color
                                      true);                       // filled

    // Ellipse overlay
    svlOverlayStaticEllipse ellipse_overlay(0,                    // video channel
                                            true,                 // visible
                                            svlPoint2D(440, 327), // center position
                                            48,                   // radius horizontally
                                            32,                   // radius vertically
                                            60.0,                 // angle
                                            svlRGB(64, 128, 64),  // color
                                            true);                // filled

    // Triangle overlay
    svlOverlayStaticTriangle tri_overlay(0,                   // video channel
                                         true,                // visible
                                         200, 250,            // triangle corners
                                         350, 231,
                                         254, 303,
                                         svlRGB(64, 64, 200), // color
                                         true);               // filled

    // Text overlay
    svlOverlayStaticText text_overlay(0,                         // video channel
                                      true,                      // visible
                                      "",                        // text
                                      svlRect(250, 10, 330, 26), // bounding rectangle
                                      14.0,                      // font size
                                      svlRGB(255, 255, 255),     // text color
                                      svlRGB(32, 32, 32));       // background color

    // Still image overlay
    svlSampleImageRGB img;
    svlImageIO::Read(img, 0, MediaDir + "wall.bmp");
    svlOverlayStaticImage image_overlay(0,                // video channel
                                        true,             // visible
                                        img,              // image sample
                                        vctInt2(20, 260), // position
                                        255);             // alpha (transparency)

    // Live image overlay (picture in picture)
    svlOverlayImage image_overlay_live(0,               // video channel
                                       true,            // visible
                                       "image",         // image input name
                                       0,               // image input channel
                                       vctInt2(15, 15), // position
                                       255);            // alpha (transparency)

    // Path overlay
    svlOverlayStaticPoly::Type path;
    svlOverlayStaticPoly path_overlay(0,                  // video channel
                                      true,               // visible
                                      path,               // initial path
                                      svlRGB(0, 255, 0)); // color


    ///////////////
    // Stream #1 //
    ///////////////

    svlStreamManager stream;
    svlFilterSourceVideoCapture source(1);
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;

    // Setup video capture source
    source.SetDevice(0);

    // Add items to overlay filter
    overlay.AddOverlay(rect_overlay);
    overlay.AddOverlay(ellipse_overlay);
    overlay.AddOverlay(tri_overlay);
    overlay.AddOverlay(text_overlay);
    overlay.AddOverlay(image_overlay);
    overlay.AddOverlay(image_overlay_live);
    overlay.AddOverlay(path_overlay);
    overlay.AddInputImage("image");
    overlay.AddQueuedItems(); // Don't wait till `Play`; add right now

    // Setup window event handler
    CMyEventHandler2 event_handler;
    event_handler.TextBox = &text_overlay;
    event_handler.Path = &path_overlay;
    window.SetEventHandler(&event_handler);

    // Assemble stream #1
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    ///////////////
    // Stream #2 //
    ///////////////

    svlStreamManager stream2;
    svlFilterSourceVideoFile source2(1);
    svlFilterImageResizer resizer;

    // Setup video file source
    source2.SetFilePath(MediaDir + "xray.avi");

    // Setup image resizer filter
    resizer.SetOutputRatio(0.6, 0.6);

    // Assemble stream #2
    stream2.SetSourceFilter(&source2);
    source2.GetOutput()->Connect(resizer.GetInput());
    resizer.GetOutput()->Connect(overlay.GetInput("image"));


    stream.Play();
    stream2.Play();

    char ch;
    std::cin >> ch;

    stream.Release();
    stream2.Release();

    return 0;
}
