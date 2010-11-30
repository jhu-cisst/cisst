/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _svlOverlayObjects_h
#define _svlOverlayObjects_h

#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlDraw.h>
#include <cisstOSAbstraction/osaStopwatch.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlOverlay
{
friend class svlFilterImageOverlay;

public:
    svlOverlay();
    svlOverlay(unsigned int videoch,
               bool visible);
    virtual ~svlOverlay();

    void SetVideoChannel(unsigned int videoch);
    void SetVisible(bool visible);
    unsigned int GetVideoChannel() const;
    bool GetVisible() const;
    bool IsUsed() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input) = 0;

private:
    void Draw(svlSampleImage* bgimage, svlSample* input);

protected:
    unsigned int VideoCh;
    bool Visible;

private:
    svlOverlay* Next;
    svlOverlay* Prev;
    bool Used;
};


class CISST_EXPORT svlOverlayInput
{
friend class svlFilterImageOverlay;

public:
    svlOverlayInput();
    svlOverlayInput(const std::string & inputname);
    virtual ~svlOverlayInput();

    void SetInputName(const std::string & inputname);
    const std::string& GetInputName() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype) = 0;

protected:
    std::string   InputName;

private:
    svlFilterInput* Input;
    svlSample* SampleCache;
};


class CISST_EXPORT svlOverlayImage : public svlOverlay, public svlOverlayInput
{
public:
    svlOverlayImage();
    svlOverlayImage(unsigned int videoch,
                    bool visible,
                    const std::string & inputname,
                    unsigned int inputch,
                    vctInt2 pos,
                    unsigned char alpha);
    virtual ~svlOverlayImage();

    void SetInputChannel(unsigned int inputch);
    void SetPosition(vctInt2 pos);
    void SetAlpha(unsigned char alpha);

    unsigned int GetInputChannel() const;
    vctInt2 GetPosition() const;
    unsigned char GetAlpha() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    unsigned int InputCh;
    vctInt2 Pos;
    unsigned char Alpha;
};


class CISST_EXPORT svlOverlayTargets : public svlOverlay, public svlOverlayInput
{
public:
    svlOverlayTargets();
    svlOverlayTargets(unsigned int videoch,
                      bool visible,
                      const std::string & inputname,
                      unsigned int inputch,
                      bool confcoloring,
                      bool crosshair,
                      unsigned int size);
    virtual ~svlOverlayTargets();

    void SetInputChannel(unsigned int inputch);
    void SetConfidenceColoring(bool enable);
    void SetCrosshair(bool enable);
    void SetSize(unsigned int size);

    unsigned int GetInputChannel() const;
    bool GetConfidenceColoring() const;
    bool GetCrosshair() const;
    unsigned int GetSize() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    unsigned int InputCh;
    bool ConfidenceColoring;
    bool Crosshair;
    unsigned int TargetSize;
};


class CISST_EXPORT svlOverlayStaticText : public svlOverlay
{
public:
    svlOverlayStaticText();
    svlOverlayStaticText(unsigned int videoch,
                         bool visible,
                         const std::string & text,
                         svlRect rect,
                         double fontsize,
                         svlRGB txtcolor);
    svlOverlayStaticText(unsigned int videoch,
                         bool visible,
                         const std::string & text,
                         svlRect rect,
                         double fontsize,
                         svlRGB txtcolor,
                         svlRGB bgcolor);
    virtual ~svlOverlayStaticText();

    void SetText(const std::string & text);
    void SetRect(svlRect rect);
    void SetRect(int left, int top, int right, int bottom);
    void SetFontSize(double size);
    void SetTextColor(svlRGB txtcolor);
    void SetBackgroundColor(svlRGB bgcolor);
    void SetBackground(bool enable);

    const std::string & GetText() const;
    svlRect GetRect() const;
    void GetRect(int & left, int & top, int & right, int & bottom) const;
    double GetFontSize() const;
    svlRGB GetTextColor() const;
    svlRGB GetBackgroundColor() const;
    bool GetBackground() const;

    svlRect GetTextSize(const std::string & text);

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    std::string Text;
    svlRect Rect;
    double FontSize;
    svlRGB TxtColor;
    svlRGB BGColor;
    bool Background;
#if CISST_SVL_HAS_OPENCV
    CvFont Font;
#endif // CISST_SVL_HAS_OPENCV
    bool FontChanged;
    int Baseline;
};


class CISST_EXPORT svlOverlayText : public svlOverlayStaticText, public svlOverlayInput
{
public:
    svlOverlayText();
    svlOverlayText(unsigned int videoch,
                   bool visible,
                   const std::string & inputname,
                   svlRect rect,
                   double fontsize,
                   svlRGB txtcolor);
    svlOverlayText(unsigned int videoch,
                   bool visible,
                   const std::string & inputname,
                   svlRect rect,
                   double fontsize,
                   svlRGB txtcolor,
                   svlRGB bgcolor);
    virtual ~svlOverlayText();

    void SetRect(svlRect rect);
    void SetRect(int left, int top, int right, int bottom);
    void SetFontSize(double size);
    void SetTextColor(svlRGB txtcolor);
    void SetBackgroundColor(svlRGB bgcolor);
    void SetBackground(bool enable);

    svlRect GetRect() const;
    void GetRect(int & left, int & top, int & right, int & bottom) const;
    double GetFontSize() const;
    svlRGB GetTextColor() const;
    svlRGB GetBackgroundColor() const;
    bool GetBackground() const;

    svlRect GetTextSize(const std::string & text);

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlRect Rect;
    double FontSize;
    svlRGB TxtColor;
    svlRGB BGColor;
    bool Background;
#if CISST_SVL_HAS_OPENCV
    CvFont Font;
#endif // CISST_SVL_HAS_OPENCV
    bool FontChanged;
    int Baseline;
};


class CISST_EXPORT svlOverlayStaticRect : public svlOverlay
{
public:
    svlOverlayStaticRect();
    svlOverlayStaticRect(unsigned int videoch,
                         bool visible,
                         svlRect rect,
                         svlRGB color,
                         bool fill = true);
    virtual ~svlOverlayStaticRect();

    void SetRect(svlRect rect);
    void SetRect(int left, int top, int right, int bottom);
    void SetColor(svlRGB color);
    void SetFill(bool fill);

    svlRect GetRect() const;
    void GetRect(int & left, int & top, int & right, int & bottom) const;
    svlRGB GetColor() const;
    bool GetFill() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlRect Rect;
    svlRGB Color;
    bool Fill;
};


class CISST_EXPORT svlOverlayStaticEllipse : public svlOverlay
{
public:
    svlOverlayStaticEllipse();
    svlOverlayStaticEllipse(unsigned int videoch,
                            bool visible,
                            const svlPoint2D center,
                            int radius_horiz,
                            int radius_vert,
                            double angle,
                            svlRGB color,
                            bool fill = true);
    svlOverlayStaticEllipse(unsigned int videoch,
                            bool visible,
                            const svlPoint2D center,
                            int radius,
                            svlRGB color,
                            bool fill = true);
    virtual ~svlOverlayStaticEllipse();

    void SetCenter(const svlPoint2D center);
    void SetRadius(const int radius_horiz, const int radius_vert);
    void SetRadius(const int radius);
    void SetAngle(const double angle);
    void SetColor(svlRGB color);
    void SetFill(bool fill);

    svlPoint2D GetCenter() const;
    void GetRadius(int & radius_horiz, int & radius_vert) const;
    double GetAngle() const;
    svlRGB GetColor() const;
    bool GetFill() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlPoint2D Center;
    int RadiusHoriz;
    int RadiusVert;
    double Angle;
    svlRGB Color;
    bool Fill;
};


class CISST_EXPORT svlOverlayStaticTriangle : public svlOverlay
{
public:
    svlOverlayStaticTriangle();
    svlOverlayStaticTriangle(unsigned int videoch,
                             bool visible,
                             const svlPoint2D corner1,
                             const svlPoint2D corner2,
                             const svlPoint2D corner3,
                             svlRGB color,
                             bool fill = true);
    svlOverlayStaticTriangle(unsigned int videoch,
                             bool visible,
                             const int x1, const int y1,
                             const int x2, const int y2,
                             const int x3, const int y3,
                             svlRGB color,
                             bool fill = true);
    virtual ~svlOverlayStaticTriangle();

    void SetCorners(const svlPoint2D corner1,
                    const svlPoint2D corner2,
                    const svlPoint2D corner3);
    void SetCorners(const int x1, const int y1,
                    const int x2, const int y2,
                    const int x3, const int y3);
    void SetColor(svlRGB color);
    void SetFill(bool fill);

    void GetCorners(svlPoint2D& corner1,
                    svlPoint2D& corner2,
                    svlPoint2D& corner3) const;
    void GetCorners(int& x1, int& y1,
                    int& x2, int& y2,
                    int& x3, int& y3) const;
    svlRGB GetColor() const;
    bool GetFill() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlPoint2D Corner1;
    svlPoint2D Corner2;
    svlPoint2D Corner3;
    svlRGB Color;
    bool Fill;
    svlDraw::Internals* DrawInternals;
};


class CISST_EXPORT svlOverlayStaticPoly : public svlOverlay
{
public:
    typedef vctDynamicVector<svlPoint2D> Type;
    typedef vctDynamicVectorRef<svlPoint2D> TypeRef;

    svlOverlayStaticPoly();
    svlOverlayStaticPoly(unsigned int videoch,
                         bool visible,
                         const TypeRef poly,
                         svlRGB color,
                         unsigned int start = 0);
    virtual ~svlOverlayStaticPoly();

    void SetPoints(const TypeRef points);
    void SetColor(svlRGB color);
    void SetStart(unsigned int start);

    TypeRef GetPoints();
    svlRGB GetColor() const;
    unsigned int GetStart() const;

    unsigned int AddPoint(svlPoint2D point);
    unsigned int AddPoint(int x, int y);
    int SetPoint(unsigned int idx, svlPoint2D point);
    int SetPoint(unsigned int idx, vctInt2 point);
    int SetPoint(unsigned int idx, int x, int y);
    int GetPoint(unsigned int idx, svlPoint2D & point) const;
    int GetPoint(unsigned int idx, vctInt2 & point) const;
    int GetPoint(unsigned int idx, int & x, int & y) const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    Type Poly;
    svlRGB Color;
    unsigned int Start;
};


class CISST_EXPORT svlOverlayStaticBar : public svlOverlay
{
public:
    svlOverlayStaticBar();
    svlOverlayStaticBar(unsigned int videoch,
                        bool visible,
                        vct2 range,
                        double value,
                        bool vertical,
                        svlRect rect,
                        svlRGB color,
                        svlRGB bgcolor,
                        unsigned int borderwidth,
                        svlRGB bordercolor);
    svlOverlayStaticBar(unsigned int videoch,
                        bool visible,
                        vct2 range,
                        double value,
                        bool vertical,
                        svlRect rect,
                        svlRGB color,
                        svlRGB bgcolor);
    virtual ~svlOverlayStaticBar();

    void SetRange(const vct2 range);
    void SetRange(const double from, const double to);
    void SetValue(const double value);
    void SetDirection(const bool vertical);
    void SetRect(svlRect rect);
    void SetRect(int left, int top, int right, int bottom);
    void SetColor(svlRGB color);
    void SetBackgroundColor(svlRGB bgcolor);
    void SetBorderWidth(const unsigned int pixels);
    void SetBorderColor(svlRGB bordercolor);

    vct2 GetRange() const;
    void GetRange(double & from, double & to) const;
    double GetValue() const;
    bool GetDirection() const;
    svlRect GetRect() const;
    void GetRect(int & left, int & top, int & right, int & bottom) const;
    svlRGB GetColor() const;
    svlRGB GetBackgroundColor() const;
    unsigned int GetBorderWidth() const;
    svlRGB GetBorderColor() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    vct2 Range;
    double Value;
    bool Vertical;
    svlRect Rect;
    svlRGB Color;
    svlRGB BGColor;
    unsigned int BorderWidth;
    svlRGB BorderColor;
};


class CISST_EXPORT svlOverlayFramerate : public svlOverlayStaticText
{
public:
    svlOverlayFramerate();
    svlOverlayFramerate(unsigned int videoch,
                        bool visible,
                        svlFilterBase* filter,
                        svlRect rect,
                        double fontsize,
                        svlRGB txtcolor);
    svlOverlayFramerate(unsigned int videoch,
                        bool visible,
                        svlFilterBase* filter,
                        svlRect rect,
                        double fontsize,
                        svlRGB txtcolor,
                        svlRGB bgcolor);
    virtual ~svlOverlayFramerate();

    void SetUpdateRate(double seconds);
    double GetUpdateRate() const;
    void Reset();

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlFilterBase* Filter;
    unsigned int PrevSampleCount;
    double PrevSampleTime;
    osaStopwatch StopWatch;
    double UpdateRate;
    bool ResetFlag;
};


class CISST_EXPORT svlOverlayTimestamp : public svlOverlayStaticText
{
public:
    svlOverlayTimestamp();
    svlOverlayTimestamp(unsigned int videoch,
                        bool visible,
                        svlFilterBase* filter,
                        svlRect rect,
                        double fontsize,
                        svlRGB txtcolor);
    svlOverlayTimestamp(unsigned int videoch,
                        bool visible,
                        svlFilterBase* filter,
                        svlRect rect,
                        double fontsize,
                        svlRGB txtcolor,
                        svlRGB bgcolor);
    virtual ~svlOverlayTimestamp();

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlFilterBase* Filter;
};


class CISST_EXPORT svlOverlayAsyncOutputProperties : public svlOverlayStaticText
{
public:
    svlOverlayAsyncOutputProperties();
    svlOverlayAsyncOutputProperties(unsigned int videoch,
                                    bool visible,
                                    svlFilterOutput* output,
                                    svlRect rect,
                                    double fontsize,
                                    svlRGB txtcolor);
    svlOverlayAsyncOutputProperties(unsigned int videoch,
                                    bool visible,
                                    svlFilterOutput* output,
                                    svlRect rect,
                                    double fontsize,
                                    svlRGB txtcolor,
                                    svlRGB bgcolor);
    virtual ~svlOverlayAsyncOutputProperties();

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlFilterOutput* Output;
};


#endif // _svlOverlayObjects_h

