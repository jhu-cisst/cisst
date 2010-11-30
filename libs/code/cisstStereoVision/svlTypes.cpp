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

svlRect::svlRect(int left, int top, int right, int bottom) :
    left(left),
    top(top),
    right(right),
    bottom(bottom)
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


/************************/
/*** svlPoint2D class ***/
/************************/

svlPoint2D::svlPoint2D() :
    x(0),
    y(0)
{
}

svlPoint2D::svlPoint2D(int x, int y)
{
    svlPoint2D::x = x;
    svlPoint2D::y = y;
}

void svlPoint2D::Assign(const svlPoint2D & point)
{
    x = point.x;
    y = point.y;
}

void svlPoint2D::Assign(int x, int y)
{
    svlPoint2D::x = x;
    svlPoint2D::y = y;
}


/*************************/
/*** svlTarget2D class ***/
/*************************/

svlTarget2D::svlTarget2D() :
    used(false),
    visible(false),
    conf(0),
    pos(0, 0)
{
}

svlTarget2D::svlTarget2D(bool used, bool visible, unsigned char conf, int x, int y)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    pos.Assign(x, y);
}

svlTarget2D::svlTarget2D(bool used, bool visible, unsigned char conf, svlPoint2D & pos)
{
    svlTarget2D::used    = used;
    svlTarget2D::visible = visible;
    svlTarget2D::conf    = conf;
    svlTarget2D::pos     = pos;
}

svlTarget2D::svlTarget2D(int x, int y)
{
    used    = true;
    visible = true;
    conf    = 255;
    pos.Assign(x, y);
}

svlTarget2D::svlTarget2D(svlPoint2D & pos)
{
    used             = true;
    visible          = true;
    conf             = 255;
    svlTarget2D::pos = pos;
}

void svlTarget2D::Assign(const svlTarget2D & target)
{
    used    = target.used;
    visible = target.visible;
    conf    = target.conf;
    pos     = target.pos;
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

