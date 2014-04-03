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

#ifndef _svlOverlayObjects_h
#define _svlOverlayObjects_h

#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlDraw.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlBufferImage;


class CISST_EXPORT svlOverlay
{
friend class svlFilterImageOverlay;

typedef enum { _DoNotRemove, _Remove, _RemoveAndDelete } RemoveState;

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

    void SetTransform(const vct3x3 & transform, const double timestamp = -1.0);
    double GetTransformTimestamp() const;
    void SetTransformID(int ID);
    int GetTransformID() const;
    bool IsTransformed() const;
    void SetTransformSynchronized(bool transform_synchronized);
    bool GetTransformSynchronized() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input) = 0;

private:
    void Draw(svlSampleImage* bgimage, svlSample* input);

protected:
    unsigned int VideoCh;
    bool         Visible;
    vct3x3       Transform;
    double       TransformTimestamp;
    int          TransformID;
    bool         Transformed;
    bool         TransformSynchronized;

private:
    svlOverlay*  Next;
    svlOverlay*  Prev;
    bool         Used;
    RemoveState  MarkedForRemoval;
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
    void SetInputSynchronized(bool input_synchronized);
    bool GetInputSynchronized() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype) = 0;

protected:
    std::string InputName;
    bool InputSynchronized;

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
    void SetPosition(int x, int y);
    void SetAlpha(unsigned char alpha);
    void SetEnableQuadMapping(bool enable);
    void SetQuadMapping(vctInt2 ul, vctInt2 ur, vctInt2 ll, vctInt2 lr);
    void SetQuadMapping(int xul, int yul, int xur, int yur, int xll, int yll, int xlr, int ylr);

    unsigned int GetInputChannel() const;
    vctInt2 GetPosition() const;
    unsigned char GetAlpha() const;
    bool GetEnableQuadMapping() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    unsigned int InputCh;
    vctInt2 Pos;
    unsigned char Alpha;
    bool QuadMappingEnabled;
    bool QuadMappingSet;
    vctInt2 QuadUL, QuadUR, QuadLL, QuadLR;
    svlDraw::Internals WarpInternals;
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


class CISST_EXPORT svlOverlayBlobs : public svlOverlay, public svlOverlayInput
{
public:
    svlOverlayBlobs();
    svlOverlayBlobs(unsigned int videoch,
                    bool visible,
                    const std::string & inputname,
                    unsigned int inputch,
                    bool draw_id = false);
    virtual ~svlOverlayBlobs();

    void SetInputChannel(unsigned int inputch);
    void SetDrawID(bool enable);

    unsigned int GetInputChannel() const;
    bool GetDrawID() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    unsigned int InputCh;
    bool DrawID;
};


class CISST_EXPORT svlOverlayToolTips : public svlOverlay, public svlOverlayInput
{
public:
    svlOverlayToolTips();
    svlOverlayToolTips(unsigned int videoch,
                       bool visible,
                       const std::string & inputname,
                       unsigned int inputch,
                       unsigned int thickness,
                       unsigned int length,
                       svlRGB color);
    virtual ~svlOverlayToolTips();

    void SetInputChannel(unsigned int inputch);
    void SetThickness(unsigned int thickness);
    void SetLength(unsigned int length);
    void SetColor(svlRGB color);

    unsigned int GetInputChannel() const;
    unsigned int GetThickness() const;
    unsigned int GetLength() const;
    svlRGB GetColor() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    unsigned int InputCh;
    unsigned int Thickness;
    unsigned int Length;
    svlRGB Color;
};


class CISST_EXPORT svlOverlayStaticImage : public svlOverlay
{
public:
    svlOverlayStaticImage();
    svlOverlayStaticImage(unsigned int videoch,
                          bool visible,
                          const svlSampleImageRGB & image,
                          vctInt2 pos,
                          unsigned char alpha);
    svlOverlayStaticImage(unsigned int videoch,
                          bool visible,
                          const svlSampleImageRGBStereo & image,
                          unsigned int imagech,
                          vctInt2 pos,
                          unsigned char alpha);
    virtual ~svlOverlayStaticImage();

    void SetImage(const svlSampleImageRGB & image);
    void SetImage(const svlSampleImageRGBStereo & image, unsigned int imagech);
    void SetPosition(vctInt2 pos);
    void SetPosition(int x, int y);
    void SetAlpha(unsigned char alpha);
    void SetEnableQuadMapping(bool enable);
    void SetQuadMapping(vctInt2 ul, vctInt2 ur, vctInt2 ll, vctInt2 lr);
    void SetQuadMapping(int xul, int yul, int xur, int yur, int xll, int yll, int xlr, int ylr);

    vctInt2 GetPosition() const;
    unsigned char GetAlpha() const;
    bool GetEnableQuadMapping() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlBufferImage* Buffer;
    vctInt2 Pos;
    unsigned char Alpha;
    bool QuadMappingEnabled;
    bool QuadMappingSet;
    vctInt2 QuadUL, QuadUR, QuadLL, QuadLR;
    svlDraw::Internals WarpInternals;
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
                         const svlRect & rect,
                         const svlRGB & color,
                         bool fill = true);
    virtual ~svlOverlayStaticRect();

    void SetRect(const svlRect & rect);
    void SetRect(int left, int top, int right, int bottom);
    void SetColor(const svlRGB & color);
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
    svlDraw::Internals* DrawInternals;
};


class CISST_EXPORT svlOverlayStaticEllipse : public svlOverlay
{
public:
    svlOverlayStaticEllipse();
    svlOverlayStaticEllipse(unsigned int videoch,
                            bool visible,
                            const svlEllipse & ellipse,
                            const svlRGB & color,
                            bool fill = true);
    svlOverlayStaticEllipse(unsigned int videoch,
                            bool visible,
                            const svlPoint2D & center,
                            int radius_horiz,
                            int radius_vert,
                            double angle,
                            const svlRGB & color,
                            bool fill = true);
    svlOverlayStaticEllipse(unsigned int videoch,
                            bool visible,
                            const svlPoint2D & center,
                            int radius,
                            const svlRGB & color,
                            bool fill = true);
    virtual ~svlOverlayStaticEllipse();

    void SetEllipse(const svlEllipse & ellipse);

    template <class _Type>
    void SetCenter(const vctFixedSizeVector<_Type, 2> & center)
    {
        Ellipse.cx = static_cast<int>(center[0]);
        Ellipse.cy = static_cast<int>(center[1]);
    }

    void SetCenter(const svlPoint2D & center);
    void SetRadius(const int radius_horiz, const int radius_vert);
    void SetRadius(const int radius);
    void SetAngle(const double angle);
    void SetThickness(unsigned int thickness);
    void SetColor(const svlRGB & color);
    void SetFill(bool fill);

    svlEllipse GetEllipse() const;
    svlPoint2D GetCenter() const;
    void GetRadius(int & radius_horiz, int & radius_vert) const;
    double GetAngle() const;
    unsigned int GetThickness();
    svlRGB GetColor() const;
    bool GetFill() const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlEllipse Ellipse;
    unsigned int Thickness;
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


class CISST_EXPORT svlOverlayPoly : public svlOverlay, public svlOverlayInput
{
public:
    svlOverlayPoly();
    svlOverlayPoly(unsigned int videoch,
                   bool visible,
                   const std::string & inputname,
                   svlRGB color);
    svlOverlayPoly(unsigned int videoch,
                   bool visible,
                   const std::string & inputname,
                   svlRGB color,
                   unsigned int thickness);
    virtual ~svlOverlayPoly();

    void SetColor(svlRGB color);
    void SetThickness(unsigned int thickness);

    svlRGB GetColor() const;
    unsigned int GetThickness() const;

protected:
    virtual bool IsInputTypeValid(svlStreamType inputtype);
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    vctDynamicVector<svlPoint2D> Poly;
    svlRGB Color;
    unsigned int Thickness;
};


class CISST_EXPORT svlOverlayStaticPoly : public svlOverlay
{
public:
    typedef vctDynamicVector<svlPoint2D> Type;
    typedef vctDynamicVectorRef<svlPoint2D> TypeRef;

    svlOverlayStaticPoly();
    svlOverlayStaticPoly(unsigned int videoch,
                         bool visible,
                         svlRGB color);
    svlOverlayStaticPoly(unsigned int videoch,
                         bool visible,
                         svlRGB color,
                         unsigned int thickness);
    svlOverlayStaticPoly(unsigned int videoch,
                         bool visible,
                         const TypeRef poly,
                         svlRGB color);
    svlOverlayStaticPoly(unsigned int videoch,
                         bool visible,
                         const TypeRef poly,
                         svlRGB color,
                         unsigned int thickness,
                         unsigned int start);
    virtual ~svlOverlayStaticPoly();

    void SetPoints();
    void SetPoints(const TypeRef points);
    void SetPoints(const TypeRef points, unsigned int start);
    void SetColor(svlRGB color);
    void SetThickness(unsigned int thickness);
    void SetStart(unsigned int start);

    TypeRef GetPoints();
    svlRGB GetColor() const;
    unsigned int GetThickness() const;
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
    Type PolyXF;
    svlRGB Color;
    unsigned int Thickness;
    unsigned int Start;
    osaCriticalSection CS;
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

    int GetValueInImagePos(double value, int & imagepos) const;
    int GetImagePosInValue(int imagepos, double & value) const;

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    vct2 Range;
    double Value;
    bool Vertical;
    svlRect Rect;
    svlRGB Color;
    svlRGB BGColor;
    int BorderWidth;
    svlRGB BorderColor;
    svlDraw::Internals* DrawInternals;
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

