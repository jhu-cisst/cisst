/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cmath>
#include <cisstStereoVision/svlTypes.h>
#include <cmath>


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
    svlShape(),
    left(0),
    top(0),
    right(0),
    bottom(0)
{
}

svlRect::svlRect(int _left, int _top, int _right, int _bottom) :
    svlShape(),
    left(_left),
    top(_top),
    right(_right),
    bottom(_bottom)
{
}

svlShape* svlRect::Clone()
{
    return new svlRect(*this);
}

bool svlRect::IsWithin(int x, int y) const
{
    if (x < left || x >= right ||
        y < top  || y >= bottom) {
        return false;
    }
    return true;
}

void svlRect::GetBoundingRect(svlRect & rect) const
{
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
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
    svlShape(),
    x1(0),
    y1(0),
    x2(0),
    y2(0),
    x3(0),
    y3(0)
{
}

svlTriangle::svlTriangle(int _x1, int _y1, int _x2, int _y2, int _x3, int _y3) :
    svlShape(),
    x1(_x1),
    y1(_y1),
    x2(_x2),
    y2(_y2),
    x3(_x3),
    y3(_y3)
{
}

svlShape* svlTriangle::Clone()
{
    return new svlTriangle(*this);
}

bool svlTriangle::IsWithin(int x, int y) const
{
    // Test bounding rectangle first; that's fast
    svlRect bounding;
    GetBoundingRect(bounding);
    if (bounding.IsWithin(x, y) == false) return false;

    // Perform proper test next
    vctFloat2 v0(x3 - x1, y3 - y1);
    vctFloat2 v1(x2 - x1, y2 - y1);
    vctFloat2 v2(x - x1, y - y1);

    const float dp00 = v0.DotProduct(v0);
    const float dp01 = v0.DotProduct(v1);
    const float dp02 = v0.DotProduct(v2);
    const float dp11 = v1.DotProduct(v1);
    const float dp12 = v1.DotProduct(v2);

    float det = dp00 * dp11 - dp01 * dp01;
    if (det == 0.0f) return false;
    det = 1.0f / det;

    const float u = (dp11 * dp02 - dp01 * dp12) * det;
    const float v = (dp00 * dp12 - dp01 * dp02) * det;

    if (u >= 0.0f && v >= 0.0f && (u + v) < 1.0f) return true;
    return false;
}

void svlTriangle::GetBoundingRect(svlRect & rect) const
{
    rect.left = std::min(std::min(x1, x2), x3);
    rect.top = std::min(std::min(y1, y2), y3);
    rect.right = std::max(std::max(x1, x2), x3) + 1;
    rect.bottom = std::max(std::max(y1, y2), y3) + 1;
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
    svlShape(),
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
    svlShape(),
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

svlQuad::svlQuad(const svlRect & rect) :
    svlShape(),
    x1(rect.left),
    y1(rect.top),
    x2(rect.right + 1),
    y2(rect.top),
    x3(rect.right + 1),
    y3(rect.bottom + 1),
    x4(rect.left),
    y4(rect.bottom + 1)
{
}

svlShape* svlQuad::Clone()
{
    return new svlQuad(*this);
}

bool svlQuad::IsWithin(int x, int y) const
{
    svlTriangle tri1(x1, y1, x2, y2, x3, y3);
    svlTriangle tri2(x3, y3, x4, y4, x1, y1);

    // Test convex case
    if (tri1.IsWithin(x, y) || tri2.IsWithin(x, y)) return true;

    // TO DO: concave implementation
    return false;
}

void svlQuad::GetBoundingRect(svlRect & rect) const
{
    rect.left = std::min(std::min(x1, x2), std::min(x3, x4));
    rect.top = std::min(std::min(y1, y2), std::min(y3, y4));
    rect.right = std::max(std::max(x1, x2), std::max(x3, x4)) + 1;
    rect.bottom = std::max(std::max(y1, y2), std::max(y3, y4)) + 1;
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

void svlQuad::Assign(const svlRect & rect)
{
    x1 = rect.left;
    y1 = rect.top;
    x2 = rect.right + 1;
    y2 = rect.top;
    x3 = rect.right + 1;
    y3 = rect.bottom + 1;
    x4 = rect.left;
    y4 = rect.bottom + 1;
}


/************************/
/*** svlEllipse class ***/
/************************/

svlEllipse::svlEllipse() :
    svlShape(),
    cx(0),
    cy(0),
    rx(0),
    ry(0),
    angle(0.0)
{
}

svlEllipse::svlEllipse(int _cx, int _cy, int _rx, int _ry, double _angle) :
    svlShape(),
    cx(_cx),
    cy(_cy),
    rx(_rx),
    ry(_ry),
    angle(_angle)
{
}

svlShape* svlEllipse::Clone()
{
    return new svlEllipse(*this);
}

bool svlEllipse::IsWithin(int x, int y) const
{
    // Test bounding rectangle first; that's fast
    int maxr = std::max(rx, ry);
    if (x < (cx - maxr) || x > (cx + maxr) ||
        y < (cy - maxr) || y > (cy + maxr)) {
        return false;
    }

    // Perform proper test next
    if (angle == 0.0) {
        double dx = x - cx;
        double dy = y - cy;
        double dist = dx * dx / (rx * rx) + dy * dy / (ry * ry);

        if (dist <= 1) return true;
        else return false;
    }
    else {
        double sa = sin(-angle);
        double ca = cos(-angle);
        double dx = x - cx;
        double dy = y - cy;
        double dxr = dx * ca - dy * sa;
        double dyr = dx * sa + dy * ca;
        double dist = dxr * dxr / (rx * rx) + dyr * dyr / (ry * ry);

        if (dist <= 1) return true;
        else return false;
    }
}

void svlEllipse::GetBoundingRect(svlRect & rect) const
{
    double ta = tan(angle);
    double cta = 1.0 / ta;
    double sa = sin(angle);
    double ca = cos(angle);
    double anx = -atan((double)ry * ta / rx);
    double any = atan((double)ry * cta / rx);
    int x = cx + (int)std::ceil(std::abs(rx * cos(anx) * ca - ry * sin(anx) * sa));
    int y = cy + (int)std::ceil(std::abs(rx * cos(any) * sa + ry * sin(any) * ca));
    rect.left = (cx << 1) - x;
    rect.top = (cy << 1) - y;
    rect.right = x + 1;
    rect.bottom = y + 1;
}

void svlEllipse::Assign(const svlEllipse & ellipse)
{
    cx    = ellipse.cx;
    cy    = ellipse.cy;
    rx    = ellipse.rx;
    ry    = ellipse.ry;
    angle = ellipse.angle;
}

void svlEllipse::Assign(int cx, int cy, int rx, int ry, double angle)
{
    this->cx    = cx;
    this->cy    = cy;
    this->rx    = rx;
    this->ry    = ry;
    this->angle = angle;
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
    image_data.SetSize(target.image_data.size());
    memcpy(image_data.Pointer(), target.image_data.Pointer(), image_data.size());
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

