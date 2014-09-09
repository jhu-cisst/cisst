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

#include <cisstStereoVision/svlDraw.h>
#include "svlDrawHelper.h"


/*****************************/
/*** svlDraw namespace *******/
/*****************************/

void svlDraw::Pixel(svlSampleImage* image,
                    unsigned int videoch,
                    svlPoint2D pos,
                    svlRGB color)
{
    Pixel(image, videoch, pos.x, pos.y, color.r, color.g, color.b);
}

void svlDraw::Pixel(svlSampleImage* image,
                    unsigned int videoch,
                    int x,
                    int y,
                    unsigned char r,
                    unsigned char g,
                    unsigned char b)
{
    if (!image || videoch >= image->GetVideoChannels() ||
        x < 0 || x >= static_cast<int>(image->GetWidth(videoch)) ||
        y < 0 || y >= static_cast<int>(image->GetHeight(videoch))) return;

    unsigned char* img = image->GetUCharPointer(videoch, x, y);

    *img = b; img ++;
    *img = g; img ++;
    *img = r;
}

void svlDraw::Rectangle(svlSampleImage* image,
                        unsigned int videoch,
                        svlRect rect,
                        svlRGB color,
                        bool fill)
{
    Rectangle(image, videoch, rect.left, rect.top, rect.right, rect.bottom, color.r, color.g, color.b, fill);
}

void svlDraw::Rectangle(svlSampleImage* image,
                        unsigned int videoch,
                        int left,
                        int top,
                        int right,
                        int bottom,
                        unsigned char r,
                        unsigned char g,
                        unsigned char b,
                        bool fill)
{
    if (!image || videoch >= image->GetVideoChannels()) return;

    const int width  = image->GetWidth(videoch);
    const int height = image->GetHeight(videoch);
    int i, j, fromx, fromy, tox, toy, endstride;
    unsigned char* img;

    if (fill) {
        fromx = left;
        if (fromx < 0) fromx = 0;
        fromy = top;
        if (fromy < 0) fromy = 0;
        tox = right;
        if (tox > width) tox = width;
        toy = bottom;
        if (toy > height) toy = height;

        img = image->GetUCharPointer(videoch, fromx, fromy);
        endstride = (width - (tox - fromx)) * 3;

        for (j = fromy; j < toy; j ++) {
            for (i = fromx; i < tox; i ++) {
                *img = b; img ++;
                *img = g; img ++;
                *img = r; img ++;
            }
            img += endstride;
        }
    }
    else {
        Line(image, videoch, left, top, right - 1, top, r, g, b);
        Line(image, videoch, left, top, left, bottom - 1, r, g, b);
        Line(image, videoch, right - 1, top, right - 1, bottom - 1, r, g, b);
        Line(image, videoch, left, bottom - 1, right - 1, bottom - 1, r, g, b);
    }
}

void svlDraw::Line(svlSampleImage* image,
                   unsigned int videoch,
                   svlPoint2D from,
                   svlPoint2D to,
                   svlRGB color)
{
    Line(image, videoch, from.x, from.y, to.x, to.y, color.r, color.g, color.b);
}

void svlDraw::Line(svlSampleImage* image,
                   unsigned int videoch,
                   int from_x,
                   int from_y,
                   int to_x,
                   int to_y,
                   unsigned char r,
                   unsigned char g,
                   unsigned char b)
{
    if (!image || videoch >= image->GetVideoChannels()) return;

    const int width  = image->GetWidth(videoch);
    const int height = image->GetHeight(videoch);
    unsigned char *tbuf, *img = image->GetUCharPointer(videoch);

    if (from_x == to_x && from_y == to_y) {
        if (from_x >= 0 && from_x < width &&
            from_y >= 0 && from_y < height) {
            tbuf = img + (from_y * width + from_x) * 3;
            *tbuf = b; tbuf ++;
            *tbuf = g; tbuf ++;
            *tbuf = r;
        }
        return;
    }

    int x, y, eps = 0;

    if (to_x <= from_x) {
        x = to_x;
        to_x = from_x;
        from_x = x;
        y = to_y;
        to_y = from_y;
        from_y = y;
    }

    int dx = to_x - from_x;
    int dy = to_y - from_y;

    y = from_y;
    x = from_x;

    if (dy > 0) {
        if (dx >= dy) {
            for (x = from_x; x <= to_x; x ++) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    tbuf = img + (y * width + x) * 3;
                    *tbuf = b; tbuf ++;
                    *tbuf = g; tbuf ++;
                    *tbuf = r;
                }
                eps += dy;
                if ((eps << 1) >= dx) {
                    y ++;
                    eps -= dx;
                }
            }
        }
        else {
            for (y = from_y; y <= to_y; y ++) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    tbuf = img + (y * width + x) * 3;
                    *tbuf = b; tbuf ++;
                    *tbuf = g; tbuf ++;
                    *tbuf = r;
                }
                eps += dx;
                if ((eps << 1) >= dy) {
                    x ++;
                    eps -= dy;
                }
            }
        }
    }
    else {
        if (dx >= abs(dy)) {
            for (x = from_x; x <= to_x; x ++) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    tbuf = img + (y * width + x) * 3;
                    *tbuf = b; tbuf ++;
                    *tbuf = g; tbuf ++;
                    *tbuf = r;
                }
                eps += dy;
                if ((eps << 1) <= -dx) {
                    y --;
                    eps += dx;
                }
            }
        }
        else {
            for (y = from_y; y >= to_y; y --) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    tbuf = img + (y * width + x) * 3;
                    *tbuf = b; tbuf ++;
                    *tbuf = g; tbuf ++;
                    *tbuf = r;
                }
                eps += dx;
                if ((eps << 1) >= -dy) {
                    x ++;
                    eps -= -dy;
                }
            }
        }
    }
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       svlTriangle & tri,
                       unsigned char value,
                       svlDraw::Internals& internals)
{
    Triangle(image, videoch, tri.x1, tri.y1, tri.x2, tri.y2, tri.x3, tri.y3, value, internals);
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       svlPoint2D corner1,
                       svlPoint2D corner2,
                       svlPoint2D corner3,
                       unsigned char value,
                       svlDraw::Internals& internals)
{
    Triangle(image, videoch, corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y, value, internals);
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       int x1, int y1,
                       int x2, int y2,
                       int x3, int y3,
                       unsigned char value,
                       svlDraw::Internals& internals)
{
    if (image->GetPixelType() != svlPixelMono8) return;
    svlDrawHelper::ShapeInternals* triangledrawer = dynamic_cast<svlDrawHelper::ShapeInternals*>(internals.Get());
    if (triangledrawer == 0) {
        triangledrawer = new svlDrawHelper::ShapeInternals;
        internals.Set(triangledrawer);
    }
    if (!triangledrawer->SetImage(image, videoch)) return;

    triangledrawer->DrawMono8(x1, y1, x2, y2, x3, y3, value);
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       svlTriangle & tri,
                       svlRGB color,
                       svlDraw::Internals& internals)
{
    Triangle(image, videoch, tri.x1, tri.y1, tri.x2, tri.y2, tri.x3, tri.y3, color, internals);
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       svlPoint2D corner1,
                       svlPoint2D corner2,
                       svlPoint2D corner3,
                       svlRGB color,
                       svlDraw::Internals& internals)
{
    Triangle(image, videoch, corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y, color, internals);
}

void svlDraw::Triangle(svlSampleImage* image,
                       unsigned int videoch,
                       int x1, int y1,
                       int x2, int y2,
                       int x3, int y3,
                       svlRGB color,
                       svlDraw::Internals& internals)
{
    if (image->GetPixelType() != svlPixelRGB) return;
    svlDrawHelper::ShapeInternals* triangledrawer = dynamic_cast<svlDrawHelper::ShapeInternals*>(internals.Get());
    if (triangledrawer == 0) {
        triangledrawer = new svlDrawHelper::ShapeInternals;
        internals.Set(triangledrawer);
    }
    if (!triangledrawer->SetImage(image, videoch)) return;

    triangledrawer->DrawRGB(x1, y1, x2, y2, x3, y3, color);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   svlQuad & quad,
                   unsigned char value,
                   svlDraw::Internals& internals)
{
    Quad(image, videoch, quad.x1, quad.y1, quad.x2, quad.y2, quad.x3, quad.y3, quad.x4, quad.y4, value, internals);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   svlPoint2D corner1,
                   svlPoint2D corner2,
                   svlPoint2D corner3,
                   svlPoint2D corner4,
                   unsigned char value,
                   svlDraw::Internals& internals)
{
    Quad(image, videoch, corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y, corner4.x, corner4.y, value, internals);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3,
                   int x4, int y4,
                   unsigned char value,
                   svlDraw::Internals& internals)
{
    if (image->GetPixelType() != svlPixelMono8) return;
    svlDrawHelper::ShapeInternals* quaddrawer = dynamic_cast<svlDrawHelper::ShapeInternals*>(internals.Get());
    if (quaddrawer == 0) {
        quaddrawer = new svlDrawHelper::ShapeInternals;
        internals.Set(quaddrawer);
    }
    if (!quaddrawer->SetImage(image, videoch)) return;

    quaddrawer->DrawMono8(x1, y1, x2, y2, x3, y3, x4, y4, value);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   svlQuad & quad,
                   svlRGB color,
                   svlDraw::Internals& internals)
{
    Quad(image, videoch, quad.x1, quad.y1, quad.x2, quad.y2, quad.x3, quad.y3, quad.x4, quad.y4, color, internals);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   svlPoint2D corner1,
                   svlPoint2D corner2,
                   svlPoint2D corner3,
                   svlPoint2D corner4,
                   svlRGB color,
                   svlDraw::Internals& internals)
{
    Quad(image, videoch, corner1.x, corner1.y, corner2.x, corner2.y, corner3.x, corner3.y, corner4.x, corner4.y, color, internals);
}

void svlDraw::Quad(svlSampleImage* image,
                   unsigned int videoch,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3,
                   int x4, int y4,
                   svlRGB color,
                   svlDraw::Internals& internals)
{
    if (image->GetPixelType() != svlPixelRGB) return;
    svlDrawHelper::ShapeInternals* quaddrawer = dynamic_cast<svlDrawHelper::ShapeInternals*>(internals.Get());
    if (quaddrawer == 0) {
        quaddrawer = new svlDrawHelper::ShapeInternals;
        internals.Set(quaddrawer);
    }
    if (!quaddrawer->SetImage(image, videoch)) return;

    quaddrawer->DrawRGB(x1, y1, x2, y2, x3, y3, x4, y4, color);
}

void svlDraw::Poly(svlSampleImage* image,
                   unsigned int videoch,
                   const vctDynamicVectorRef<svlPoint2D> points,
                   svlRGB color,
                   unsigned int thickness,
                   unsigned int start)
{
    if (!image || videoch >= image->GetVideoChannels()) return;

    const unsigned int size = static_cast<unsigned int>(points.size());

    if (size < 1) return;
    if (size == 1) Pixel(image, videoch, points[0], color);

    unsigned int i, end = start + 1;

    for (i = 1; i < size; i ++) {
        if (end >= size) end = 0;
        if (thickness == 1) {
            Line(image, videoch, points[start], points[end], color);
        }
        else {
#if CISST_SVL_HAS_OPENCV
            if (thickness <= 255) {
                cvLine(image->IplImageRef(videoch),
                       cvPoint(points[start].x, points[start].y),
                       cvPoint(points[end].x,   points[end].y),
                       cvScalar(color.r, color.g, color.b),
                       thickness);
            }
#else // CISST_SVL_HAS_OPENCV
            // TO DO: line thickness to be implemented
            Line(image, videoch, points[start], points[end], color);
#endif // CISST_SVL_HAS_OPENCV
        }
        start = end;
        end ++;
    }
}

void svlDraw::Ellipse(svlSampleImage* image,
                      unsigned int videoch,
                      int cx,
                      int cy,
                      int rx,
                      int ry,
                      unsigned char r,
                      unsigned char g,
                      unsigned char b,
                      double from_angle,
                      double to_angle,
                      double angle,
                      int thickness)
{
    Ellipse(image, videoch, svlEllipse(cx, cy, rx, ry, angle), r, g, b, from_angle, to_angle, thickness);
}                      

void svlDraw::Ellipse(svlSampleImage* image,
                      unsigned int videoch,
                      int cx,
                      int cy,
                      int rx,
                      int ry,
                      svlRGB color,
                      double from_angle,
                      double to_angle,
                      double angle,
                      int thickness)
{
    Ellipse(image, videoch, svlEllipse(cx, cy, rx, ry, angle), color.r, color.g, color.b, from_angle, to_angle, thickness);
}                      

void svlDraw::Ellipse(svlSampleImage* image,
                      unsigned int videoch,
                      const svlEllipse & ellipse,
                      svlRGB color,
                      double from_angle,
                      double to_angle,
                      int thickness)
{
    Ellipse(image, videoch, ellipse, color.r, color.g, color.b, from_angle, to_angle, thickness);
}

#if CISST_SVL_HAS_OPENCV

void svlDraw::Ellipse(svlSampleImage* image,
                      unsigned int videoch,
                      const svlEllipse & ellipse,
                      unsigned char r,
                      unsigned char g,
                      unsigned char b,
                      double from_angle,
                      double to_angle,
                      int thickness)
{
    if (!image || videoch >= image->GetVideoChannels()) return;
    if (ellipse.rx < 0 || ellipse.ry < 0 || thickness > 255) return;

    cvEllipse(image->IplImageRef(videoch),
              cvPoint(ellipse.cx, ellipse.cy),
              cvSize(ellipse.rx, ellipse.ry),
              ellipse.angle * 180.0f / 3.14159265358979f,
              from_angle * 180.0f / 3.14159265358979f,
              to_angle * 180.0f / 3.14159265358979f,
              cvScalar(r, g, b),
              thickness);
}

#else // CISST_SVL_HAS_OPENCV

void svlDraw::Ellipse(svlSampleImage* CMN_UNUSED(image),
                      unsigned int CMN_UNUSED(videoch),
                      const svlEllipse & CMN_UNUSED(center),
                      unsigned char CMN_UNUSED(r),
                      unsigned char CMN_UNUSED(g),
                      unsigned char CMN_UNUSED(b),
                      double CMN_UNUSED(from_angle),
                      double CMN_UNUSED(to_angle),
                      int CMN_UNUSED(thickness))
{
    // To be implemented
}

#endif // CISST_SVL_HAS_OPENCV

void svlDraw::Crosshair(svlSampleImage* image,
                        unsigned int videoch,
                        svlPoint2D pos,
                        svlRGB color,
                        unsigned int radius,
                        unsigned int thickness)
{
    Crosshair(image, videoch, pos.x, pos.y, color.r, color.g, color.g, radius, thickness);
}

void svlDraw::Crosshair(svlSampleImage* image,
                        unsigned int videoch,
                        int x,
                        int y,
                        unsigned char r,
                        unsigned char g,
                        unsigned char b,
                        unsigned int radius,
                        unsigned int thickness)
{
    if (!image || videoch >= image->GetVideoChannels()) return;

    const int in_rad = radius / 3 + 1;
    const int thickness_half1 = thickness >> 1;
    const int thickness_half2 = thickness - thickness_half1;

    Rectangle(image, videoch, x - radius, y - thickness_half1, x - in_rad + 1, y + thickness_half2, r, g, b);
    Rectangle(image, videoch, x + in_rad, y - thickness_half1, x + radius + 1, y + thickness_half2, r, g, b);
    Rectangle(image, videoch, x - thickness_half1, y - radius, x + thickness_half2, y - in_rad + 1, r, g, b);
    Rectangle(image, videoch, x - thickness_half1, y + in_rad, x + thickness_half2, y + radius + 1, r, g, b);
    Pixel(image, videoch, x - in_rad + 1, y, r, g, b);
    Pixel(image, videoch, x + in_rad - 1, y, r, g, b);
    Pixel(image, videoch, x, y - in_rad + 1, r, g, b);
    Pixel(image, videoch, x, y + in_rad - 1, r, g, b);
}

#if CISST_SVL_HAS_OPENCV

void svlDraw::Text(svlSampleImage* image,
                   unsigned int videoch,
                   svlPoint2D pos,
                   const std::string & text,
                   double fontsize,
                   svlRGB color)
{
    if (!image || videoch >= image->GetVideoChannels()) return;

    CvFont font;
    cvInitFont(&font,
               CV_FONT_HERSHEY_PLAIN,
               fontsize / SVL_OCV_FONT_SCALE,
               fontsize / SVL_OCV_FONT_SCALE,
               0, 1, 4);
    cvPutText(image->IplImageRef(videoch),
              text.c_str(),
              cvPoint(pos.x, pos.y),
              &font,
              cvScalar(color.r, color.g, color.b));
}

#else // CISST_SVL_HAS_OPENCV

void svlDraw::Text(svlSampleImage* CMN_UNUSED(image),
                   unsigned int CMN_UNUSED(videoch),
                   svlPoint2D CMN_UNUSED(pos),
                   const std::string & CMN_UNUSED(text),
                   double CMN_UNUSED(fontsize),
                   svlRGB CMN_UNUSED(color))
{
    // To be implemented
}

#endif // CISST_SVL_HAS_OPENCV

void svlDraw::Text(svlSampleImage* image,
                   unsigned int videoch,
                   int x,
                   int y,
                   const std::string & text,
                   double fontsize,
                   unsigned char r,
                   unsigned char g,
                   unsigned char b)
{
    Text(image, videoch, svlPoint2D(x, y), text, fontsize, svlRGB(r, g, b));
}

void svlDraw::WarpTriangle(svlSampleImage* in_img,  unsigned int in_vch,  svlTriangle & in_tri,
                           svlSampleImage* out_img, unsigned int out_vch, svlTriangle & out_tri,
                           svlDraw::Internals& internals,
                           unsigned int alpha)
{
    svlDrawHelper::WarpInternals* trianglewarper = dynamic_cast<svlDrawHelper::WarpInternals*>(internals.Get());
    if (trianglewarper == 0) {
        trianglewarper = new svlDrawHelper::WarpInternals(3);
        internals.Set(trianglewarper);
    }
    if (!trianglewarper->SetInputImage(in_img, in_vch) ||
        !trianglewarper->SetOutputImage(out_img, out_vch)) return;

    trianglewarper->Draw(1, 0,
                         in_tri.x1,  in_tri.y1,  in_tri.x2,  in_tri.y2,  in_tri.x3,  in_tri.y3,
                         out_tri.x1, out_tri.y1, out_tri.x2, out_tri.y2, out_tri.x3, out_tri.y3,
                         alpha);
}

void svlDraw::WarpQuad(svlSampleImage* in_img,  unsigned int in_vch,  svlQuad & in_quad,
                       svlSampleImage* out_img, unsigned int out_vch, svlQuad & out_quad,
                       svlDraw::Internals& internals,
                       unsigned int alpha)
{
    svlDrawHelper::WarpInternals* quadwarper = dynamic_cast<svlDrawHelper::WarpInternals*>(internals.Get());
    if (quadwarper == 0) {
        quadwarper = new svlDrawHelper::WarpInternals(4);
        internals.Set(quadwarper);
    }
    if (!quadwarper->SetInputImage(in_img, in_vch) ||
        !quadwarper->SetOutputImage(out_img, out_vch)) return;

    quadwarper->Draw(1, 0,
                     in_quad.x1,  in_quad.y1,  in_quad.x2,  in_quad.y2,  in_quad.x3,  in_quad.y3,  in_quad.x4,  in_quad.y4,
                     out_quad.x1, out_quad.y1, out_quad.x2, out_quad.y2, out_quad.x3, out_quad.y3, out_quad.x4, out_quad.y4,
                     alpha);
}


/********************************/
/*** svlDraw::Internals class ***/
/********************************/

svlDraw::Internals::Internals() :
    Ptr(0)
{
}

svlDraw::Internals::~Internals()
{
    Release();
}

svlDrawInternals* svlDraw::Internals::Get()
{
    return Ptr;
}

void svlDraw::Internals::Set(svlDrawInternals* ib)
{
    Release();
    Ptr = ib;
}

void svlDraw::Internals::Release()
{
    if (Ptr) delete Ptr;
    Ptr = 0;
}


/*****************************/
/*** svlDraw::WarpMT class ***/
/*****************************/

svlDraw::WarpMT::WarpMT(unsigned int thread_count)
{
    SetThreadCount(thread_count);
}

void svlDraw::WarpMT::SetThreadCount(unsigned int thread_count)
{
    if (thread_count > Internals.size()) Internals.SetSize(thread_count);
}

void svlDraw::WarpMT::WarpTriangle(unsigned int thread_id,
                                   svlSampleImage* in_img,  unsigned int in_vch,  svlTriangle & in_tri,
                                   svlSampleImage* out_img, unsigned int out_vch, svlTriangle & out_tri,
                                   unsigned int alpha)
{
    const unsigned int thread_count = static_cast<unsigned int>(Internals.size());
    if (thread_id >= thread_count) return;

    svlDrawHelper::WarpInternals* trianglewarper = dynamic_cast<svlDrawHelper::WarpInternals*>(Internals[thread_id].Get());
    if (trianglewarper == 0) {
        trianglewarper = new svlDrawHelper::WarpInternals(3);
        Internals[thread_id].Set(trianglewarper);
    }
    if (!trianglewarper->SetInputImage(in_img, in_vch) ||
        !trianglewarper->SetOutputImage(out_img, out_vch)) return;

    trianglewarper->Draw(thread_count, thread_id,
                         in_tri.x1,  in_tri.y1,  in_tri.x2,  in_tri.y2,  in_tri.x3,  in_tri.y3,
                         out_tri.x1, out_tri.y1, out_tri.x2, out_tri.y2, out_tri.x3, out_tri.y3,
                         alpha);
}

void svlDraw::WarpMT::WarpQuad(unsigned int thread_id,
                               svlSampleImage* in_img,  unsigned int in_vch,  svlQuad & in_quad,
                               svlSampleImage* out_img, unsigned int out_vch, svlQuad & out_quad,
                               unsigned int alpha)
{
    const unsigned int thread_count = static_cast<unsigned int>(Internals.size());
    if (thread_id >= thread_count) return;

    svlDrawHelper::WarpInternals* quadwarper = dynamic_cast<svlDrawHelper::WarpInternals*>(Internals[thread_id].Get());
    if (quadwarper == 0) {
        quadwarper = new svlDrawHelper::WarpInternals(4);
        Internals[thread_id].Set(quadwarper);
    }
    if (!quadwarper->SetInputImage(in_img, in_vch) ||
        !quadwarper->SetOutputImage(out_img, out_vch)) return;

    quadwarper->Draw(thread_count, thread_id,
                     in_quad.x1,  in_quad.y1,  in_quad.x2,  in_quad.y2,  in_quad.x3,  in_quad.y3,  in_quad.x4,  in_quad.y4,
                     out_quad.x1, out_quad.y1, out_quad.x2, out_quad.y2, out_quad.x3, out_quad.y3, out_quad.x4, out_quad.y4,
                     alpha);
}

