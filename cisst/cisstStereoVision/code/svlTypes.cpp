/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlTypes.h>


/*************************/
/*** svlRGB class ********/
/*************************/

svlRGB::svlRGB() :
    b(0), g(0), r(0)
{
}

svlRGB::svlRGB(unsigned char r_, unsigned char g_, unsigned char b_) :
    b(b_), g(g_), r(r_)
{
}

void svlRGB::Assign(const svlRGB & color)
{
    r = color.r;
    g = color.g;
    b = color.b;
}

void svlRGB::Assign(unsigned char r_, unsigned char g_, unsigned char b_)
{
    r = r_;
    g = g_;
    b = b_;
}


/*************************/
/*** svlRGBA class *******/
/*************************/

svlRGBA::svlRGBA() :
    b(0), g(0), r(0), a(0)
{
}

svlRGBA::svlRGBA(const svlRGB & rgb, unsigned char a_) :
    a(a_)
{
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
}

svlRGBA::svlRGBA(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_) :
    b(b_), g(g_), r(r_), a(a_)
{
}

void svlRGBA::Assign(const svlRGBA & color)
{
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
}

void svlRGBA::Assign(const svlRGB & rgb, unsigned char a_)
{
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
    a = a_;
}

void svlRGBA::Assign(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_)
{
    r = r_;
    g = g_;
    b = b_;
    a = a_;
}


/*************************/
/*** svlBlob class *******/
/*************************/

svlBlob::svlBlob() :
    ID(0),
    used(false),
    left(0),
    right(0),
    top(0),
    bottom(0),
    center_x(0),
    center_y(0),
    area(0),
    circumference(0),
    label(0)
{
}

void svlBlob::Assign(const svlBlob & blob)
{
    ID            = blob.ID;
    used          = blob.used;
    left          = blob.left;
    right         = blob.right;
    top           = blob.top;
    bottom        = blob.bottom;
    center_x      = blob.center_x;
    center_y      = blob.center_y;
    area          = blob.area;
    circumference = blob.circumference;
    label         = blob.label;
}


/*********************/
/*** svlRect class ***/
/*********************/

svlRect::svlRect() :
    left(0),
    top(0),
    right(0),
    bottom(0)
{
}

svlRect::svlRect(int _left, int _top, int _right, int _bottom) :
    left(_left),
    top(_top),
    right(_right),
    bottom(_bottom)
{
}

void svlRect::Assign(const svlRect & rect)
{
    left = rect.left;
    top = rect.top;
    right = rect.right;
    bottom = rect.bottom;
}

void svlRect::Assign(int left, int top, int right, int bottom)
{
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
}

void svlRect::Normalize()
{
    int temp;
    if (left > right) {
        temp = right;
        right = left;
        left = temp;
    }
    if (top > bottom) {
        temp = bottom;
        bottom = top;
        top = temp;
    }
}

void svlRect::Trim(int minx, int maxx, int miny, int maxy)
{
    if (left < minx) left = minx;
    if (left > maxx) left = maxx;
    if (right < minx) right = minx;
    if (right > maxx) right = maxx;
    if (top < miny) top = miny;
    if (top > maxy) top = maxy;
    if (bottom < miny) bottom = miny;
    if (bottom > maxy) bottom = maxy;
}


/*************************/
/*** svlTriangle class ***/
/*************************/

svlTriangle::svlTriangle() :
    x1(0),
    y1(0),
    x2(0),
    y2(0),
    x3(0),
    y3(0)
{
}

svlTriangle::svlTriangle(int _x1, int _y1, int _x2, int _y2, int _x3, int _y3) :
    x1(_x1),
    y1(_y1),
    x2(_x2),
    y2(_y2),
    x3(_x3),
    y3(_y3)
{
}

void svlTriangle::Assign(const svlTriangle & triangle)
{
    x1 = triangle.x1;
    y1 = triangle.y1;
    x2 = triangle.x2;
    y2 = triangle.y2;
    x3 = triangle.x3;
    y3 = triangle.y3;
}

void svlTriangle::Assign(int x1, int y1, int x2, int y2, int x3, int y3)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    this->x3 = x3;
    this->y3 = y3;
}


/*********************/
/*** svlQuad class ***/
/*********************/

svlQuad::svlQuad() :
    x1(0),
    y1(0),
    x2(0),
    y2(0),
    x3(0),
    y3(0),
    x4(0),
    y4(0)
{
}

svlQuad::svlQuad(int _x1, int _y1, int _x2, int _y2, int _x3, int _y3, int _x4, int _y4) :
    x1(_x1),
    y1(_y1),
    x2(_x2),
    y2(_y2),
    x3(_x3),
    y3(_y3),
    x4(_x4),
    y4(_y4)
{
}

void svlQuad::Assign(const svlQuad & quad)
{
    x1 = quad.x1;
    y1 = quad.y1;
    x2 = quad.x2;
    y2 = quad.y2;
    x3 = quad.x3;
    y3 = quad.y3;
    x4 = quad.x4;
    y4 = quad.y4;
}

void svlQuad::Assign(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    this->x3 = x3;
    this->y3 = y3;
    this->x4 = x4;
    this->y4 = y4;
}


/************************/
/*** svlPoint2D class ***/
/************************/

svlPoint2D::svlPoint2D() :
    x(0),
    y(0)
{
}

svlPoint2D::svlPoint2D(int _x, int _y) :
    x(_x),
    y(_y)
{
}

void svlPoint2D::Assign(const svlPoint2D & point)
{
    x = point.x;
    y = point.y;
}

void svlPoint2D::Assign(int x, int y)
{
    this->x = x;
    this->y = y;
}


/*************************/
/*** svlTarget2D class ***/
/*************************/

svlTarget2D::svlTarget2D() :
    used(false),
    visible(false),
    conf(0),
    pos(0, 0),
    feature_quality(-1)
{
}

svlTarget2D::svlTarget2D(bool used, bool visible, unsigned char conf, int x, int y)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    pos.Assign(x, y);
    feature_quality      = -1;
}

svlTarget2D::svlTarget2D(bool used, bool visible, unsigned char conf, svlPoint2D & pos)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    svlTarget2D::pos     = pos;
    feature_quality      = -1;
}

svlTarget2D::svlTarget2D(int x, int y)
{
    used             = true;
    visible          = true;
    conf             = 255;
    pos.Assign(x, y);
    feature_quality  = -1;
}

svlTarget2D::svlTarget2D(svlPoint2D & pos)
{
    used             = true;
    visible          = true;
    conf             = 255;
    svlTarget2D::pos = pos;
    feature_quality  = -1;
}

void svlTarget2D::Assign(const svlTarget2D & target)
{
    used            = target.used;
    visible         = target.visible;
    conf            = target.conf;
    pos             = target.pos;
    feature_quality = target.feature_quality;
    feature_data.SetSize(target.feature_data.size());
    memcpy(feature_data.Pointer(), target.feature_data.Pointer(), feature_data.size());
}

void svlTarget2D::Assign(bool used, bool visible, unsigned char conf, int x, int y)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    pos.Assign(x, y);
}

void svlTarget2D::Assign(bool used, bool visible, unsigned char conf, svlPoint2D & pos)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    svlTarget2D::pos     = pos;
}

void svlTarget2D::Assign(int x, int y)
{
    used    = true;
    visible = true;
    conf    = 255;
    pos.Assign(x, y);
}

void svlTarget2D::Assign(svlPoint2D & pos)
{
    used             = true;
    visible          = true;
    conf             = 255;
    svlTarget2D::pos = pos;
}

