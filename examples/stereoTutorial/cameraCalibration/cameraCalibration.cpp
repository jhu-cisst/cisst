/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cameraCalibration.cpp 2426 2011-05-21 00:53:58Z wliu25 $
  
  Author(s):  Wen P. Liu
  Created on: 2011
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.
  
--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.
    
--- end cisst license ---
*/

#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlTypes.h>
#include <limits>

using namespace std;
bool debug = false;

////////////////////////////////////////
//     Window event handler class     //
////////////////////////////////////////

class CViewerEventHandler : public svlWindowEventHandlerBase
{
public:
    CViewerEventHandler() :
            svlWindowEventHandlerBase()
            ,Source(0)
            ,Rectifier()
            ,MousePressed(false)
            ,MousePressedCount(0)
            ,ImageIndex(0)
            ,Invalid(0)
    {
        CameraCalibration = new svlCCCameraCalibration(BoardWidth,BoardHeight,SquareSize,OriginDetectorColorModeFlag);
        Images = new std::vector<svlSampleImageRGB>();
        OriginalImages = new std::vector<svlSampleImageRGB>();
        OriginIndicators = new std::vector<svlOverlayStaticEllipse>();      
    }
    
    void CalibrateAllImages()
    {
        std::cout << "Calling svlExCameraCalibration " << ImageDirectory << " "<< ImagePrefix << " " << ImageType << " ";
        std::cout << StartIndex << " " << StopIndex << " " << BoardWidth << " " << BoardHeight << std::endl;
        bool ok = CameraCalibration->Process(ImageDirectory,ImagePrefix,ImageType,StartIndex,StopIndex);
        if(ok && CameraCalibration->RunCameraCalibration(RunHandEye) && CameraCalibration->images.size() > 0)
        {
            CameraCalibration->PrintCalibrationParameters();
            CameraCalibration->WriteToFileCalibrationParameters(ImageDirectory);
            CameraCalibration->SetRectifier(Rectifier);
            for(int i=0;i<CameraCalibration->images.size();i++)
                Images->at(i) = CameraCalibration->images.at(i);
        }
    }

    void CalibrateImage()
    {        
        vctDynamicVector<vctInt2> indicators;
        if(OriginIndicators->at(0).GetVisible()&&OriginIndicators->at(1).GetVisible()&&OriginIndicators->at(2).GetVisible())
        {
            indicators.resize(3);
            for(int i=0;i<OriginIndicators->size();i++)
            {
                indicators[i] = vctInt2(OriginIndicators->at(i).GetCenter().x*2,OriginIndicators->at(i).GetCenter().y*2);
            }
        }
        bool ok = CameraCalibration->ProcessImage(ImageDirectory,ImagePrefix,ImageType,StartIndex+(*ImageIndex),indicators);
        
        if(ok && CameraCalibration->images.size() > 0)
        {
            CameraCalibration->SetRectifier(Rectifier,StartIndex+(*ImageIndex));
            Images->at(*ImageIndex) = CameraCalibration->images.back();
            Source->SetImageOverwrite(Images->at(*ImageIndex));
            (*Valid)++;
             cout << "Showing Image# " << (*ImageIndex)+1 << " of " << Images->size()<< "Valid: " << *Valid << endl;
        }else
        {
            Invalid++;
        }
    }

    void ResetOriginIndicators()
    {
        MousePressedCount = 0;
        for(int i=0;i<3;i++)
        {
            OriginIndicators->at(i).SetVisible(false);
        }

    }

    void LoadImage(int index)
    {
        //process images
        svlSampleImageRGB image;

        std::stringstream path;
        std::string currentFileName;

        // image file
        path << ImageDirectory;
        path << ImagePrefix;
        path.fill('0');
        path << std::setw(3) << index << std::setw(1);
        path << "." << ImageType;

        std::cout << "Attempting to load image: " << path.str() << std::endl;

        bool ok = svlImageIO::Read(image, 0, path.str());
        if(ok != SVL_OK)
        {
            std::cout << "ERROR: svl Failed to load image: " << path.str() << std::endl;
        }else
        {
            (*ImageIndex) = Images->size();
            Images->push_back(image);
            OriginalImages->push_back(image);
            Source->SetImageOverwrite(Images->back());
            cout << "Showing Image# " << (*ImageIndex)+1 << " of " << Images->size()<< "Valid: " << *Valid << endl;
        }
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        double gamma;

        // handling user inputs
        if (ascii) {
            switch (eventid) {
            case 'a':
                if (CameraCalibration) {
                    CalibrateAllImages();
                    ResetOriginIndicators();
                }
                break;
            case 'c':
                if (CameraCalibration) {
                    CalibrateImage();
                    ResetOriginIndicators();
                }
                break;
            case 'l':
                LoadImage(StartIndex+Images->size());
                break;
            case 'o':
                cout << endl << " >>> > optimize camera calibration" << std::endl;
                if(CameraCalibration->RunCameraCalibration(RunHandEye) && CameraCalibration->images.size() > 0)
                {
                    CameraCalibration->PrintCalibrationParameters();
                    CameraCalibration->WriteToFileCalibrationParameters(ImageDirectory);
                    CameraCalibration->SetRectifier(Rectifier);
                }
                break;
            case 'r':
                cout << endl << " >>> Set Image invalid at " << StartIndex+*ImageIndex << std::endl;
                CameraCalibration->SetGridValid(*ImageIndex,false);
                Images->at(*ImageIndex) = OriginalImages->at(*ImageIndex);
                Source->SetImage(Images->at(*ImageIndex));
                (*Valid)--;
                cout << "Showing Image# " << (*ImageIndex)+1 << " of " << Images->size()<< "Valid: " << *Valid << endl;
                break;
            case 'u':
                cout << endl << " >>> Undo previous optimization" << std::endl;
                CameraCalibration->Reset();
                break;
            default:
                (*ImageIndex)++;
                if((*ImageIndex) >= Images->size())
                    (*ImageIndex) = 0;
                Source->SetImage(Images->at(*ImageIndex));
                cout << "Showing Image# " << (*ImageIndex)+1 << " of " << Images->size()<< "Valid: " << *Valid << endl;
                return;
            }

        }else {

            switch (eventid) {
            case winInput_LBUTTONDOWN:
                if (!MousePressed) {
                    MousePressed = true;
                    if(MousePressedCount >= 3)
                    {
                        cout << endl << " >>> > 3 mouse clicks resetting to zero" << std::endl;
                        ResetOriginIndicators();
                    }else
                    {
                        int mouseOriginX, mouseOriginY;
                        GetMousePos(mouseOriginX, mouseOriginY);
                        OriginIndicators->at(MousePressedCount).SetCenter(svlPoint2D(mouseOriginX,mouseOriginY));
                        OriginIndicators->at(MousePressedCount).SetVisible(true);
                        //cout << endl << " >>> Mouse press # << " << MousePressedCount+1<< " : " <<  mouseOriginX << "," << mouseOriginY << endl;
                        MousePressedCount++;
                    }
                }
                break;
                case winInput_LBUTTONUP:
                if (MousePressed) {
                    MousePressed = false;
                }
                break;
            }
        }
    }

    svlFilterSourceDummy* Source;
    svlFilterImageRectifier* Rectifier;
    svlCCCameraCalibration* CameraCalibration;
    bool RunHandEye;
    string ImageDirectory;
    string ImagePrefix;
    string ImageType;
    int StartIndex;
    int StopIndex;
    int BoardWidth;
    int BoardHeight;
    float SquareSize;
    int OriginDetectorColorModeFlag;
    bool MousePressed;
    std::vector<svlSampleImageRGB>* Images;
    std::vector<svlSampleImageRGB>* OriginalImages;
    std::vector<svlOverlayStaticEllipse> * OriginIndicators;
    int MousePressedCount;
    int* ImageIndex;
    int* Valid;
    FILE* OriginIndicatorFile;
    int Invalid;
};

//////////////////////////////////
//             main             //
//////////////////////////////////

int ParseNumber(char* string, unsigned int maxlen)
{
    if (string == 0 || maxlen == 0) return -1;

    int ivalue, j;
    char ch;

    // parse number
    j = 0;
    ivalue = 0;
    ch = string[j];
    // 4 digits max
    while (ch != 0 && j < (int)maxlen) {
        // check if number
        ch -= '0';
        if (ch > 9 || ch < 0) {
            ivalue = -1;
            break;
        }
        ivalue = ivalue * 10 + ch;
        // step to next digit
        j ++;
        ch = string[j];
    }
    if (j == 0) ivalue = -1;

    return ivalue;
}

int CameraCalibration(string imageDirectory
                      ,string imagePrefix
                      ,string imageType
                      ,int startIndex
                      ,int stopIndex
                      ,int boardWidth
                      ,int boardHeight
                      ,float squareSize
                      ,int originDetectorColorModeFlag)
{

    svlInitialize();

    // Creating SVL objects
    svlStreamManager stream(2); // number of threads per stream
    svlFilterSourceDummy source;
    svlFilterImageResizer resizer, resizer2;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;
    svlFilterSplitter splitter;
    svlFilterImageUnsharpMask filtering;
    svlFilterImageWindow window2;
    svlFilterImageRectifier rectifier;
    svlCCCameraCalibration cameraCalibration;
    CViewerEventHandler window_eh;
    std::vector<svlSampleImageRGB> images;
    std::vector<svlSampleImageRGB> originalImages;
    std::vector<svlOverlayStaticEllipse> originIndicators;
    int imageIndex=0;
    int valid=0;

    // Setup dummy video source
    source.SetTargetFrequency(30.0);

    // Setup image resizer
    // (Tip: enable OpenCV in CMake for higher performance)
    resizer.SetOutputRatio(0.5, 0.5, SVL_LEFT);
    resizer.SetOutputRatio(0.5, 0.5, SVL_RIGHT);
    resizer.SetInterpolation(true);
    resizer2.SetOutputRatio(0.5, 0.5, SVL_LEFT);
    resizer2.SetOutputRatio(0.5, 0.5, SVL_RIGHT);
    resizer2.SetInterpolation(true);

    // Setup window
    window.SetTitle("Rectified");

    // Setup unsharp masking
    // (Tip: enable OpenCV in CMake for higher performance)
    filtering.SetAmount(200);
    filtering.SetRadius(3);

    bool ok = false;
    bool runHandEye = true;
    int ch;

    // Setup branch window
    // setup image window
    window_eh.Source = &source;
    window_eh.Rectifier = &rectifier;
    window_eh.CameraCalibration = &cameraCalibration;
    window_eh.RunHandEye = runHandEye;
    window_eh.ImageDirectory = imageDirectory;
    window_eh.ImagePrefix = imagePrefix;
    window_eh.ImageType = imageType;
    window_eh.StartIndex = startIndex;
    window_eh.StopIndex = stopIndex;
    window_eh.BoardWidth = boardWidth;
    window_eh.BoardHeight = boardHeight;
    window_eh.SquareSize = squareSize;
    window_eh.OriginDetectorColorModeFlag = originDetectorColorModeFlag;
    window_eh.Images = & images;
    window_eh.OriginalImages = &originalImages;
    window_eh.OriginIndicators = &originIndicators;
    window_eh.ImageIndex = &imageIndex;
    window_eh.Valid = &valid;
    window2.SetEventHandler(&window_eh);
    window2.SetTitle("Original");

    // Add new output to splitter
    splitter.AddOutput("output2");

    //Overlays
    originIndicators.resize(3);
    originIndicators.at(0).SetColor(svlRGB(0,0,255));
    originIndicators.at(1).SetColor(svlRGB(0,255,0));
    originIndicators.at(2).SetColor(svlRGB(0,100,100));

    for(int i=0;i<3;i++)
    {
        originIndicators.at(i).SetRadius(3);
        originIndicators.at(i).SetVisible(false);
        overlay.AddOverlay(originIndicators.at(i));
    }

    //process images
    svlSampleImageRGB image;
    for(int index=startIndex;index<=stopIndex;index++)
    {
        std::stringstream path;
        std::string currentFileName;

        // image file
        path << imageDirectory;
        path << imagePrefix;
        path.fill('0');
        path << std::setw(3) << index << std::setw(1);
        path << "." << imageType;

        std::cout << "Attempting to load image: " << path.str() << std::endl;

        ok = svlImageIO::Read(image, 0, path.str());
        if(ok != SVL_OK)
        {
            std::cout << "ERROR: svl Failed to load image: " << path.str() << std::endl;
        }else
        {
            images.push_back(image);
            originalImages.push_back(image);
        }
    }

    if(images.size() > 0)
    {
        imageIndex=0;
        source.SetImage(images.at(imageIndex));
    }
    else
    {
        goto labError;
    }
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(splitter.GetInput());
    splitter.GetOutput()->Connect(filtering.GetInput());
    filtering.GetOutput()->Connect(rectifier.GetInput());
    rectifier.GetOutput()->Connect(resizer.GetInput());
    resizer.GetOutput()->Connect(window.GetInput());
    splitter.GetOutput("output2")->Connect(resizer2.GetInput());
    resizer2.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window2.GetInput());

    cout << "Streaming is just about to start." << endl;
    cout << "Press any key to toggle images..." << endl;
    cout << "Press 'q' to stop stream..." << endl;
    cout << "Showing Image# " << imageIndex+1 << " of " << images.size()<< endl;

    // Initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    // Wait for user input
    do
    {
        ch = cmnGetChar();
        imageIndex++;
        if(imageIndex >= images.size())
            imageIndex = 0;
        source.SetImage(images.at(imageIndex));
        cout << "Showing Image# " << imageIndex+1 << " of " << images.size() << " Valid: " << valid << endl;

    }while (ch != 'q');

    // Safely stopping and deconstructing stream before de-allocation
    stream.Stop();
    stream.Release();

    cout << "Success... Quitting." << endl;
    return 1;

    labError:
    cout << "Error occured... Quitting." << endl;

    return 1;
}

int main(int argc, char** argv)
{
    cerr << "svlExCameraCalibration - cisstStereoVision example by Wen P. Liu" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'svlExCameraCalibration-?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int j, options;
    options = argc - 1;
    string imageDirectory = "./Images/SD/";
    string imagePrefix = "image";
    string imageType = "png";
    int startIndex = 0;
    int stopIndex = 9;
    int boardWidth = 18;
    int boardHeight = 16;
    float squareSize = 2.0;
    int originDetectorColorModeFlag = svlCCOriginDetector::RGY;

    if (argc == 3)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
    }
    else if(argc == 6)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
        imageType = argv[3];
        startIndex = atoi(argv[4]);
        stopIndex = atoi(argv[5]);
    }

    for (j = 1; j <= options; j ++) {
        if (argv[j][0] != '-') continue;

        switch (argv[j][1]) {
        case '?':
            cout << "Command line format:" << endl;
            cout << "     svlExCameraCalibration imageDirectory imagePrefix " << endl;
            cout << "     ex: images should be in format image00X.png" << endl;
            cout << "     OPTIONAL [imageType startIndex stopIndex]" << endl;
            cout << "     (defaults [png 0 9])" << endl;
            cout << "Examples:" << endl;
            cout << "     svlExCameraCalibration ./Images/SD/ image " << endl;
            cout << "     svlExCameraCalibration ./Images/SD/ image png 0 9 " << endl;
            return 1;
            break;

        default:
            // NOP
            break;
        }
    }

    //////////////////////////////
    // starting camera calibration

    CameraCalibration(imageDirectory
                      ,imagePrefix
                      ,imageType
                      ,startIndex
                      ,stopIndex
                      ,boardWidth
                      ,boardHeight
                      ,squareSize
                      ,originDetectorColorModeFlag);

    cerr << "Quit" << endl;
    return 1;
}

