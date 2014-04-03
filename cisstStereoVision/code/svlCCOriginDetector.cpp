/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#include <math.h> 
#include <string.h> 
#include <cisstStereoVision/svlCCOriginDetector.h>

svlCCOriginDetector::svlCCOriginDetector(int colorModeFlag)
{
    originColorModeFlag = colorModeFlag;
    reset();
}

void svlCCOriginDetector::reset()
{
    originDetectionFlag = NO_ORIGIN;
    colorBlobs.clear();
}

/**************************************************************************************************
* findOriginByColor()					
*	Find origin by Red, Green, Blue, or Red, Green, Yellow circles
*	Known issue with Blue and Yellow printer barrel bleeds as well as camera artifacts,
*	lends to addition check and resampling of these color circles.
*
* Input:
*	matImage	cv::Mat						- Matrix representation of the OpenCV image
*	
* Output:
*	void							
*
***********************************************************************************************************/
void svlCCOriginDetector::findOriginByColor( IplImage* iplImage) 
{ 
    cv::Point2f predictedBlob;
    // find blobs
    int blobIndexFlag[] = {0,0,0,0};
    int blobThresholds[] = {29,29,29,29};
    bool blobsFound = findColorBlobs(iplImage, (float)std::max(iplImage->width,iplImage->height),blobIndexFlag,blobThresholds);
    // find intersection of blobs

    if(blobsFound)
    {
        //find RGB blob intersection for origin
        if(originColorModeFlag == RGY)
            origin = intersectionByColorBlobs(colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);
        else if(originColorModeFlag == RGB)
            origin = intersectionByColorBlobs(colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);

        float distanceBetweenGreenRed = distanceBetweenTwoPoints(colorBlobs.at(RED_INDEX).x, colorBlobs.at(RED_INDEX).y, colorBlobs.at(GREEN_INDEX).x, colorBlobs.at(GREEN_INDEX).y);
        predictedBlob = cv::Point2f((float)(origin.x+0.75*(colorBlobs.at(RED_INDEX).y-colorBlobs.at(GREEN_INDEX).y)/2),(float)(origin.y-0.75*(colorBlobs.at(RED_INDEX).x-colorBlobs.at(GREEN_INDEX).x)/2));

        if(originColorModeFlag == RGY)
        {
            //fix yellow blob if its not in proximty to green and red
            if(distanceBetweenTwoPoints(predictedBlob.x,predictedBlob.y,colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y) > distanceBetweenGreenRed/4)
            {
                colorBlobs[YELLOW_INDEX] = predictedBlob;
                //if(debug)
                std::cout << "updated yellow blob at: " << predictedBlob.x << "," << predictedBlob.y << std::endl;
                blobIndexFlag[YELLOW_INDEX] = 1;
                findColorBlobs(iplImage,2*distanceBetweenTwoPoints(origin.x,origin.y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y),blobIndexFlag,blobThresholds);
                predictedBlob = cv::Point2f((float)(origin.x+0.75*(colorBlobs.at(RED_INDEX).y-colorBlobs.at(GREEN_INDEX).y)/2),(float)(origin.y-0.75*(colorBlobs.at(RED_INDEX).x-colorBlobs.at(GREEN_INDEX).x)/2));

                //sanity check
                if(distanceBetweenTwoPoints(origin.x, origin.y, colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y) > distanceBetweenGreenRed)
                    colorBlobs[YELLOW_INDEX] = predictedBlob;
                else
                    originDetectionFlag = COLOR;
            }else{
                originDetectionFlag = COLOR;
            }
        }else if(originColorModeFlag == RGB)
        {
            //fix blue blob if its not in proximty to green and red
            if(distanceBetweenTwoPoints(predictedBlob.x,predictedBlob.y,colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y) > distanceBetweenGreenRed/4)
            {
                //float originRadius = distanceBetweenTwoPoints(predictedOriginRG.x,predictedOriginRG.y, line_x1, line_y1);
                //intersect_x = colorBlobs.at(GREEN_INDEX).x+(colorBlobs.at(RED_INDEX).x-colorBlobs.at(GREEN_INDEX).x)/2;
                //intersect_y = colorBlobs.at(GREEN_INDEX).y+(colorBlobs.at(RED_INDEX).y-colorBlobs.at(GREEN_INDEX).y)/2;
                //intersect_x = intersect_y-y_intercept/slope_of_line;
                //new_line_y_intercept = y_intercept - intersect_x*(perpendicular_slope - slope_of_line);
                //point_y = new_line_y_intercept + perpendicular_slope * (intersect_x+sqrt(originRadius*originRadius+(;

                colorBlobs[BLUE_INDEX] = predictedBlob;
                if(debug)
                    std::cout << "updated blue blob at: " << predictedBlob.x << "," << predictedBlob.y << std::endl;
                blobIndexFlag[BLUE_INDEX] = 1;
                findColorBlobs(iplImage,2*distanceBetweenTwoPoints(origin.x,origin.y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y),blobIndexFlag,blobThresholds);
                predictedBlob = cv::Point2f((float)(origin.x+0.75*(colorBlobs.at(RED_INDEX).y-colorBlobs.at(GREEN_INDEX).y)/2),(float)(origin.y-0.75*(colorBlobs.at(RED_INDEX).x-colorBlobs.at(GREEN_INDEX).x)/2));

                //sanity check
                if(distanceBetweenTwoPoints(origin.x, origin.y, colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y) > distanceBetweenGreenRed)
                    colorBlobs[BLUE_INDEX] = predictedBlob;
                else
                    originDetectionFlag = COLOR;
            }else{
                originDetectionFlag = COLOR;
            }
        }

        blobIndexFlag[RED_INDEX] = 1;
        blobIndexFlag[GREEN_INDEX] = 1;
        blobIndexFlag[BLUE_INDEX] = 1;
        findColorBlobs(iplImage,2*distanceBetweenTwoPoints(origin.x,origin.y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y),blobIndexFlag,blobThresholds);

    }
}


cv::Point2f svlCCOriginDetector::intersectionByColorBlobs(float point_x, float point_y, float line_x1, float line_y1, float line_x2, float line_y2)
{

    //find RG blob intersection for origin
    cv::Point2f predictedOriginRGB, predictedOriginRG;
    predictedOriginRG = cv::Point2f(colorBlobs.at(GREEN_INDEX).x+(colorBlobs.at(RED_INDEX).x-colorBlobs.at(GREEN_INDEX).x)/2, colorBlobs.at(GREEN_INDEX).y+(colorBlobs.at(RED_INDEX).y-colorBlobs.at(GREEN_INDEX).y)/2);

    // find the slope
    float slope_of_line = (line_y1 - line_y2) / (line_x1 - line_x2);

    // find the perpendicular slope
    float perpendicular_slope = (line_x1 - line_x2) / (line_y1 - line_y2) * -1;

    // find the y_intercept of line BC
    float y_intercept = line_y2 - slope_of_line * line_x2;

    // find the y_intercept of line AX
    float new_line_y_intercept = point_y - perpendicular_slope * point_x;

    // get the x_coordinate of point X
    // equation of BC is    y = slope_of_line * x + y_intercept;
    // equation of AX is    y = perpendicular_slope * x + new_line_y_intercept;
    //   perpendicular_slope * x + new_line_y_intercept == slope_of_line * x + y_intercept;
    //   perpendicular_slope * x == slope_of_line * x + y_intercept - new_line_y_intercept;
    //   (perpendicular_slope - slope_of_line) * x == (y_intercept - new_line_y_intercept);
    // get the y_coordinate of point X
    float intersect_x = (y_intercept - new_line_y_intercept) / (perpendicular_slope - slope_of_line);//(intersect_y-y_intercept)/slope_of_line;//

    float intersect_y = slope_of_line * intersect_x + y_intercept;//slope_of_line/(slope_of_line-perpendicular_slope)*(new_line_y_intercept-perpendicular_slope/slope_of_line*y_intercept);//

    predictedOriginRGB = cv::Point2f(intersect_x, intersect_y);

    // measure the distance between A and X
    //return distanceBetweenTwoPoints(point_x, point_y, intersect_x, intersect_y);
    return predictedOriginRG;
}

bool svlCCOriginDetector::findColorBlobs(IplImage* iplImage, float radius, int flag[], int thresholds[])
{
    int i,j,k;
    int height,width,step,channels;
    //int stepr, channelsr;
    //int temp=0;
    uchar *data; //,*datar;

    i=j=k=0;
    colorBlobs.resize(4);

    height = iplImage->height;
    width = iplImage->width;
    step = iplImage->widthStep;
    channels = iplImage->nChannels;
    data = (uchar *)iplImage->imageData;

    int g0, g1, g2;
    int b0, b1, b2;
    int r0, r1, r2;
    int y0, y1, y2;
    int rx,ry,gx,gy,bx,by,yx,yy;
    rx=ry=gx=gy=bx=by=yx=yy=0;
    int rn, gn, bn, yn;
    rn=gn=bn=yn=0;
    int nThreshold = 0;
    //green
    g0 = 1;
    g1 = 2;
    g2 = 0;
    //blue
    b0 = 0;
    b1 = 1;
    b2 = 2;
    //red
    r0 = 2;
    r1 = 0;
    r2 = 1;
    //yellow
    y0 = 2;
    y1 = 1;
    y2 = 0;

    //Use previous measurements for thresholds
    if(flag[BLUE_INDEX])
    {
        j = (int)colorBlobs.at(BLUE_INDEX).x;
        i = (int)colorBlobs.at(BLUE_INDEX).y;
        int newThreshold = ((data[i*step+j*channels+b0] - data[i*step+j*channels+b1]) + (data[i*step+j*channels+b0] - data[i*step+j*channels+b2]))/2;
        //min of precomputed threshold and avg of differences with R,G
        if(newThreshold > 5)
            thresholds[BLUE_INDEX] = std::min(thresholds[BLUE_INDEX],newThreshold);
    }
    if(flag[YELLOW_INDEX])
    {
        j = (int)colorBlobs.at(YELLOW_INDEX).x;
        i = (int)colorBlobs.at(YELLOW_INDEX).y;
        int newThreshold = ((data[i*step+j*channels+y0] - data[i*step+j*channels+y2]) + (data[i*step+j*channels+y0] - data[i*step+j*channels+y2]))/2;
        //min of precomputed threshold and avg of differences with R,G
        if(newThreshold > 5)
            thresholds[YELLOW_INDEX] = std::min(thresholds[YELLOW_INDEX],newThreshold);
    }
    if(flag[RED_INDEX])
    {
        j = (int)colorBlobs.at(RED_INDEX).x;
        i = (int)colorBlobs.at(RED_INDEX).y;
        int newThreshold = ((data[i*step+j*channels+r0] - data[i*step+j*channels+r1]) + (data[i*step+j*channels+r0] - data[i*step+j*channels+r2]))/2;
        //min of precomputed threshold and avg of differences with R,G
        if(newThreshold > 5)
            thresholds[RED_INDEX] = std::min(thresholds[RED_INDEX],newThreshold);
    }
    if(flag[GREEN_INDEX])
    {
        j = (int)colorBlobs.at(GREEN_INDEX).x;
        i = (int)colorBlobs.at(GREEN_INDEX).y;
        int newThreshold = ((data[i*step+j*channels+g0] - data[i*step+j*channels+g1]) + (data[i*step+j*channels+g0] - data[i*step+j*channels+g2]))/2;
        //min of precomputed threshold and avg of differences with R,G
        if(newThreshold > 5)
            thresholds[GREEN_INDEX] = std::min(thresholds[GREEN_INDEX],newThreshold);
    }


    // select pixels with more than this threshold
    // of one channel versus the other two

    for(i=0;i < (height);i++) for(j=0;j <(width);j++)
    {
        if(distanceBetweenTwoPoints((float)j,(float)i,origin.x,origin.y) < radius)
        {
            if(((data[i*step+j*channels+r0]) > (thresholds[RED_INDEX]+data[i*step+j*channels+r1]))
                && ((data[i*step+j*channels+r0]) > (thresholds[RED_INDEX]+data[i*step+j*channels+r2])))
                {
                if(!flag[RED_INDEX] || (flag[RED_INDEX] && distanceBetweenTwoPoints(colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,(float)j,(float)i) < radius/4))
                {
                    //red
                    rx = rx + j;
                    ry = ry + i;
                    rn++;
                }
            }else if(((data[i*step+j*channels+g0]) > (thresholds[GREEN_INDEX]+data[i*step+j*channels+g1]))
                && ((data[i*step+j*channels+g0]) > (thresholds[GREEN_INDEX]+data[i*step+j*channels+g2])))
                {
                if(!flag[GREEN_INDEX] || (flag[GREEN_INDEX] && distanceBetweenTwoPoints(colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y,(float)j,(float)i) < radius/4))
                {
                    //green
                    gx = gx + j;
                    gy = gy + i;
                    gn++;
                }
            }else if(((data[i*step+j*channels+b0]) > (thresholds[BLUE_INDEX]+data[i*step+j*channels+b1]))
                && ((data[i*step+j*channels+b0]) > (thresholds[BLUE_INDEX]+data[i*step+j*channels+b2])))
                {
                //blue
                if(!flag[BLUE_INDEX] || (flag[BLUE_INDEX] && distanceBetweenTwoPoints(colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y,(float)j,(float)i) < radius/4))
                {
                    bx = bx + j;
                    by = by + i;
                    bn++;
                    if(flag[BLUE_INDEX])
                        colorBlobs[BLUE_INDEX] = cv::Point2f(colorBlobs.at(BLUE_INDEX).x+(bx/bn - colorBlobs.at(BLUE_INDEX).x)/2, colorBlobs.at(BLUE_INDEX).y+(by/bn-colorBlobs.at(BLUE_INDEX).y)/2);
                }
            }else if(((data[i*step+j*channels+y0]) > (thresholds[YELLOW_INDEX]+data[i*step+j*channels+y2]))
                && ((data[i*step+j*channels+y1]) > (thresholds[YELLOW_INDEX]+data[i*step+j*channels+y2])))
                {
                //yellow
                if(!flag[YELLOW_INDEX] || (flag[YELLOW_INDEX] && distanceBetweenTwoPoints(colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y,(float)j,(float)i) < radius/4))
                {
                    yx = yx + j;
                    yy = yy + i;
                    yn++;
                    if(flag[YELLOW_INDEX])
                        colorBlobs[YELLOW_INDEX] = cv::Point2f(colorBlobs.at(YELLOW_INDEX).x+(yx/yn - colorBlobs.at(YELLOW_INDEX).x)/2, colorBlobs.at(YELLOW_INDEX).y+(yy/yn-colorBlobs.at(YELLOW_INDEX).y)/2);
                }
            }
        }
    }

    if(rn > nThreshold)
    {
        rx = rx/rn;
        ry = ry/rn;
    }

    if(gn > nThreshold)
    {
        gx = gx/gn;
        gy = gy/gn;
    }

    if(bn > nThreshold)
    {
        bx = bx/bn;
        by = by/bn;
    }

    if(yn > nThreshold)
    {
        yx = yx/yn;
        yy = yy/yn;
    }

    //save color blobs
    colorBlobs[RED_INDEX] = cv::Point2f((float)rx,(float)ry);
    colorBlobs[GREEN_INDEX] = cv::Point2f((float)gx,(float)gy);
    if(originColorModeFlag == RGB)
        colorBlobs[BLUE_INDEX] = cv::Point2f((float)bx,(float)by);
    else if(originColorModeFlag == RGY)
        colorBlobs[YELLOW_INDEX] = cv::Point2f((float)yx,(float)yy);

    //find RGB blob intersection for origin
    if(originColorModeFlag == RGY)
        origin = intersectionByColorBlobs(colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);
    else if(originColorModeFlag == RGB)
        origin = intersectionByColorBlobs(colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);

    return((rn>nThreshold) &&(gn>nThreshold)&&(((originColorModeFlag == RGB)&&(bn>nThreshold))||((yn>nThreshold)&&(originColorModeFlag == RGY))));
}

float svlCCOriginDetector::distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2)
{
    return sqrt( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) ) ;
}

void svlCCOriginDetector::drawColorBlobs(IplImage* iplImage)
{

    if(debug)
        std::cout << "drawColorBlobs start, total: " <<colorBlobs.size() << std::endl;

    for(unsigned int i = 0; i < colorBlobs.size(); i++ )
    {
        if(i==0)
        {
            if(debug)
                std::cout << "Found red blob at " << colorBlobs.at(i).x << "," << colorBlobs.at(i).y << std::endl;
            cvCircle( iplImage, cvPoint((int)colorBlobs.at(i).x,(int)colorBlobs.at(i).y), 5, CV_RGB(255,0,0), -1, 8, 0 );
        }else if(i==1)
        {
            if(debug)
                std::cout << "Found green blob at " << colorBlobs.at(i).x << "," << colorBlobs.at(i).y << std::endl;
            cvCircle( iplImage, cvPoint((int)colorBlobs.at(i).x,(int)colorBlobs.at(i).y), 5, CV_RGB(0,255,0), -1, 8, 0 );
        }else if(i==2 && originColorModeFlag == RGB)
        {
            if(debug)
                std::cout << "Found blue blob at " << colorBlobs.at(i).x << "," << colorBlobs.at(i).y << std::endl;
            cvCircle( iplImage, cvPoint((int)colorBlobs.at(i).x,(int)colorBlobs.at(i).y), 5, CV_RGB(0,0,255), -1, 8, 0 );
        }else if(i==3 && originColorModeFlag == RGY)
        {
            if(debug)
                std::cout << "Found yellow blob at " << colorBlobs.at(i).x << "," << colorBlobs.at(i).y << std::endl;
            cvCircle( iplImage, cvPoint((int)colorBlobs.at(i).x,(int)colorBlobs.at(i).y), 5, CV_RGB(255,255,0), -1, 8, 0 );
        }
    }

    cvLine(iplImage,cvPoint((int)colorBlobs.at(RED_INDEX).x,(int)colorBlobs.at(RED_INDEX).y),cvPoint((int)colorBlobs.at(GREEN_INDEX).x,(int)colorBlobs.at(GREEN_INDEX).y),CV_RGB(0,0,255));
    if(originColorModeFlag == RGB)
        cvLine(iplImage,cvPoint((int)colorBlobs.at(BLUE_INDEX).x,(int)colorBlobs.at(BLUE_INDEX).y),origin,CV_RGB(0,0,255));
    else if(originColorModeFlag == RGY)
        cvLine(iplImage,cvPoint((int)colorBlobs.at(YELLOW_INDEX).x,(int)colorBlobs.at(YELLOW_INDEX).y),origin,CV_RGB(0,0,255));

    //if(debug)
    std::cout << "Calculated origin at " << origin.x << "," << origin.y << std::endl;
    cvCircle( iplImage, cvPoint((int)origin.x,(int)origin.y), 5, CV_RGB(255,0,0), -1, 8, 0 );
    cvCircle( iplImage, cvPoint((int)origin.x,(int)origin.y), 5, CV_RGB(255,255,255), -1, 8, 0 );
}


/**************************************************************************************************
* detectOrigin()					
*	Find the origin and orientation of a chessboard image
*
* Input:
*	IplImage* iplImage					- IplImage* iplImage
*	
* Output:
*	void							
*	
*	Older versions had skeleton code, i.e. not tested for the following:
*	 1). findOriginByTemplate
*	 2). findOriginByCircle
*
***********************************************************************************************************/
void svlCCOriginDetector::detectOrigin(IplImage* iplImage, vctDynamicVector<vctInt2> inputBlobs)
{
    reset();
    if(!inputBlobs.empty())
    {
        if(colorBlobs.size() == 0)
            colorBlobs.resize(4);
         //find RGB blob intersection for origin
        if(originColorModeFlag == RGY)
        {
            colorBlobs[RED_INDEX] = cv::Point2f((float)inputBlobs.at(0).X(),(float)inputBlobs.at(0).Y());
            colorBlobs[GREEN_INDEX] = cv::Point2f((float)inputBlobs.at(1).X(),(float)inputBlobs.at(1).Y());
            colorBlobs[YELLOW_INDEX] = cv::Point2f((float)inputBlobs.at(2).X(),(float)inputBlobs.at(2).Y());
            origin = intersectionByColorBlobs(colorBlobs.at(YELLOW_INDEX).x,colorBlobs.at(YELLOW_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);
        }else if(originColorModeFlag == RGB)
        {
            colorBlobs[RED_INDEX] = cv::Point2f((float)inputBlobs.at(0).X(),(float)inputBlobs.at(0).Y());
            colorBlobs[GREEN_INDEX] = cv::Point2f((float)inputBlobs.at(1).X(),(float)inputBlobs.at(1).Y());
            colorBlobs[BLUE_INDEX] = cv::Point2f((float)inputBlobs.at(2).X(),(float)inputBlobs.at(2).Y());
            origin = intersectionByColorBlobs(colorBlobs.at(BLUE_INDEX).x,colorBlobs.at(BLUE_INDEX).y,colorBlobs.at(RED_INDEX).x,colorBlobs.at(RED_INDEX).y,colorBlobs.at(GREEN_INDEX).x,colorBlobs.at(GREEN_INDEX).y);
        }
        originDetectionFlag = COLOR;
    }
    if(originDetectionFlag == NO_ORIGIN)
    {
        origin = cv::Point2f((float)iplImage->width/2,(float)iplImage->height/2);
        findOriginByColor(iplImage);
    }

    // draw color blobs
    //if(debug)
    drawColorBlobs(iplImage);

}


