/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlDraw_h
#define _svlDraw_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstCommon/cmnConstants.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlDrawInternals;


namespace svlDraw
{
    class CISST_EXPORT Internals
    {
    public:
        Internals();
        ~Internals();
        svlDrawInternals* Get();
        void Set(svlDrawInternals* ib);
        void Release();
    private:
        svlDrawInternals* Ptr;
    };


    // RGB
    void CISST_EXPORT Pixel(svlSampleImage* image, unsigned int videoch, svlPoint2D pos, svlRGB color);
    void CISST_EXPORT Pixel(svlSampleImage* image, unsigned int videoch, int x, int y, unsigned char r, unsigned char g, unsigned char b);

    // RGB
    void CISST_EXPORT Rectangle(svlSampleImage* image, unsigned int videoch, svlRect rect, svlRGB color, bool fill = true);
    void CISST_EXPORT Rectangle(svlSampleImage* image, unsigned int videoch, int left, int top, int right, int bottom, unsigned char r, unsigned char g, unsigned char b, bool fill = true);

    // RGB
    void CISST_EXPORT Line(svlSampleImage* image, unsigned int videoch, svlPoint2D from, svlPoint2D to, svlRGB color);
    void CISST_EXPORT Line(svlSampleImage* image, unsigned int videoch, int from_x, int from_y, int to_x, int to_y, unsigned char r, unsigned char g, unsigned char b);

    // Mono8
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, svlTriangle & tri, unsigned char value, svlDraw::Internals& internals);
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, svlPoint2D corner1, svlPoint2D corner2, svlPoint2D corner3, unsigned char value, svlDraw::Internals& internals);
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, int x1, int y1, int x2, int y2, int x3, int y3, unsigned char value, svlDraw::Internals& internals);
    // RGB
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, svlTriangle & tri, svlRGB color, svlDraw::Internals& internals);
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, svlPoint2D corner1, svlPoint2D corner2, svlPoint2D corner3, svlRGB color, svlDraw::Internals& internals);
    void CISST_EXPORT Triangle(svlSampleImage* image, unsigned int videoch, int x1, int y1, int x2, int y2, int x3, int y3, svlRGB color, svlDraw::Internals& internals);

    // Mono8
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, svlQuad & quad, unsigned char value, svlDraw::Internals& internals);
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, svlPoint2D corner1, svlPoint2D corner2, svlPoint2D corner3, svlPoint2D corner4, unsigned char value, svlDraw::Internals& internals);
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned char value, svlDraw::Internals& internals);
    // RGB
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, svlQuad & quad, svlRGB color, svlDraw::Internals& internals);
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, svlPoint2D corner1, svlPoint2D corner2, svlPoint2D corner3, svlPoint2D corner4, svlRGB color, svlDraw::Internals& internals);
    void CISST_EXPORT Quad(svlSampleImage* image, unsigned int videoch, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, svlRGB color, svlDraw::Internals& internals);

    // RGB
    void CISST_EXPORT Poly(svlSampleImage* image, unsigned int videoch, const vctDynamicVectorRef<svlPoint2D> points, svlRGB color, unsigned int thickness, unsigned int start);

    // RGB
    void CISST_EXPORT Ellipse(svlSampleImage* image, unsigned int videoch, int cx, int cy, int rx, int ry, unsigned char r, unsigned char g, unsigned char b, double angle = 0.0, double from_angle = 0.0, double to_angle = 2.0*cmnPI, int thickness = 1);
    void CISST_EXPORT Ellipse(svlSampleImage* image, unsigned int videoch, int cx, int cy, int rx, int ry, svlRGB color, double angle = 0.0, double from_angle = 0.0, double to_angle = 2.0*cmnPI, int thickness = 1);
    void CISST_EXPORT Ellipse(svlSampleImage* image, unsigned int videoch, const svlEllipse & ellipse, svlRGB color, double from_angle = 0.0, double to_angle = 2.0*cmnPI, int thickness = 1);
    void CISST_EXPORT Ellipse(svlSampleImage* image, unsigned int videoch, const svlEllipse & ellipse, unsigned char r, unsigned char g, unsigned char b, double from_angle = 0.0, double to_angle = 2.0*cmnPI, int thickness = 1);

    // RGB
    void CISST_EXPORT Crosshair(svlSampleImage* image, unsigned int videoch, svlPoint2D pos, svlRGB color, unsigned int radius = 5, unsigned int thickness = 2);
    void CISST_EXPORT Crosshair(svlSampleImage* image, unsigned int videoch, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned int radius = 5, unsigned int thickness = 2);

    // RGB
    void CISST_EXPORT Text(svlSampleImage* image, unsigned int videoch, svlPoint2D pos, const std::string & text, double fontsize, svlRGB color);
    void CISST_EXPORT Text(svlSampleImage* image, unsigned int videoch, int x, int y, const std::string & text, double fontsize, unsigned char r, unsigned char g, unsigned char b);

    // Mono8 and RGB
    void CISST_EXPORT WarpTriangle(svlSampleImage* in_img,  unsigned int in_vch,  svlTriangle & in_tri,
                                   svlSampleImage* out_img, unsigned int out_vch, svlTriangle & out_tri,
                                   svlDraw::Internals& internals,
                                   unsigned int alpha = 256);
    void CISST_EXPORT WarpQuad(svlSampleImage* in_img,  unsigned int in_vch,  svlQuad & in_quad,
                               svlSampleImage* out_img, unsigned int out_vch, svlQuad & out_quad,
                               svlDraw::Internals& internals,
                               unsigned int alpha = 256);

    class CISST_EXPORT WarpMT
    {
    public:
        WarpMT(unsigned int thread_count);
        void SetThreadCount(unsigned int thread_count);

        void WarpTriangle(unsigned int thread_id,
                          svlSampleImage* in_img,  unsigned int in_vch,  svlTriangle & in_tri,
                          svlSampleImage* out_img, unsigned int out_vch, svlTriangle & out_tri,
                          unsigned int alpha = 256);
        void WarpQuad(unsigned int thread_id,
                      svlSampleImage* in_img,  unsigned int in_vch,  svlQuad & in_quad,
                      svlSampleImage* out_img, unsigned int out_vch, svlQuad & out_quad,
                      unsigned int alpha = 256);

    private:
        WarpMT(const WarpMT & other);
        vctDynamicVector<svlDraw::Internals> Internals;
    };
};

#endif // _svlDraw_h

