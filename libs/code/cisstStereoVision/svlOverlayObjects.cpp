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

#include <cisstStereoVision/svlOverlayObjects.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlBufferImage.h>


/****************************/
/*** svlOverlay class *******/
/****************************/

svlOverlay::svlOverlay() :
    VideoCh(0),
    Visible(true),
    Transform(vct3x3::Eye()),
    Transformed(false),
    Next(0),
    Prev(0),
    Used(false)
{
}

svlOverlay::svlOverlay(unsigned int videoch,
                       bool visible) :
    VideoCh(videoch),
    Visible(visible),
    Transform(vct3x3::Eye()),
    Transformed(false),
    Next(0),
    Prev(0),
    Used(false)
{
}

svlOverlay::~svlOverlay()
{
}

void svlOverlay::SetVideoChannel(unsigned int videoch)
{
    VideoCh = videoch;
}

void svlOverlay::SetVisible(bool visible)
{
    Visible = visible;
}

unsigned int svlOverlay::GetVideoChannel() const
{
    return VideoCh;
}

bool svlOverlay::GetVisible() const
{
    return Visible;
}

bool svlOverlay::IsUsed() const
{
    return Used;
}

void svlOverlay::SetTransform(const vct3x3 & transform)
{
    Transform.Assign(transform);
    if (Transform != vct3x3::Eye()) Transformed = true;
    else Transformed = false;
}

void svlOverlay::Draw(svlSampleImage* bgimage, svlSample* input)
{
    if (bgimage && VideoCh < bgimage->GetVideoChannels() && Visible) {
        DrawInternal(bgimage, input);
    }
}


/*********************************/
/*** svlOverlayInput class *******/
/*********************************/

svlOverlayInput::svlOverlayInput() :
    Input(0),
    SampleCache(0)
{
}

svlOverlayInput::svlOverlayInput(const std::string & inputname) :
    InputName(inputname),
    InputSynchronized(false),
    Input(0),
    SampleCache(0)
{
}

svlOverlayInput::~svlOverlayInput()
{
}

void svlOverlayInput::SetInputName(const std::string & inputname)
{
    InputName = inputname;
}

const std::string& svlOverlayInput::GetInputName() const
{
    return InputName;
}

void svlOverlayInput::SetInputSynchronized(bool inputsynchronized)
{
    InputSynchronized = inputsynchronized;
}

bool svlOverlayInput::GetInputSynchronized() const
{
    return InputSynchronized;
}


/*********************************/
/*** svlOverlayImage class *******/
/*********************************/

svlOverlayImage::svlOverlayImage() :
    svlOverlay(),
    svlOverlayInput(),
    InputCh(0),
    Pos(0, 0),
    Alpha(255)
{
}

svlOverlayImage::svlOverlayImage(unsigned int videoch,
                                 bool visible,
                                 const std::string & inputname,
                                 unsigned int inputch,
                                 vctInt2 pos,
                                 unsigned char alpha) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    InputCh(inputch),
    Pos(pos),
    Alpha(alpha)
{
}

svlOverlayImage::~svlOverlayImage()
{
}

void svlOverlayImage::SetInputChannel(unsigned int inputch)
{
    InputCh = inputch;
}

void svlOverlayImage::SetPosition(vctInt2 pos)
{
    Pos = pos;
}

void svlOverlayImage::SetPosition(int x, int y)
{
    Pos[0] = x;
    Pos[1] = y;
}

void svlOverlayImage::SetAlpha(unsigned char alpha)
{
    Alpha = alpha;
}

unsigned int svlOverlayImage::GetInputChannel() const
{
    return InputCh;
}

vctInt2 svlOverlayImage::GetPosition() const
{
    return Pos;
}

unsigned char svlOverlayImage::GetAlpha() const
{
    return Alpha;
}

bool svlOverlayImage::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeImageRGB || inputtype == svlTypeImageRGBStereo) return true;
    return false;
}

void svlOverlayImage::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleImage* ovrlimage = dynamic_cast<svlSampleImage*>(input);

    // Check video inputs
    if (!ovrlimage ||
        InputCh >= ovrlimage->GetVideoChannels() ||
        bgimage->GetBPP() != ovrlimage->GetBPP()) return;

    int i, ws, hs, wo, ho, xs, ys, xo, yo, copylen, linecount;

    // Prepare for data copy
    ws = static_cast<int>(bgimage->GetWidth(VideoCh) * bgimage->GetBPP());
    hs = static_cast<int>(bgimage->GetHeight(VideoCh));
    wo = static_cast<int>(ovrlimage->GetWidth(InputCh) * bgimage->GetBPP());
    ho = static_cast<int>(ovrlimage->GetHeight(InputCh));

    copylen = wo;
    linecount = ho;
    xs = Pos.X() * bgimage->GetBPP();
    ys = Pos.Y();
    xo = yo = 0;

    // If overlay position reaches out of the background on the left
    if (xs < 0) {
        copylen += xs;
        xo -= xs;
        xs = 0;
    }
    // If overlay position reaches out of the background on the right
    if ((xs + copylen) > ws) {
        copylen += ws - (xs + copylen);
    }
    // If overlay is outside the background boundaries
    if (copylen <= 0) return;

    // If overlay position reaches out of the background on the top
    if (ys < 0) {
        linecount += ys;
        yo -= ys;
        ys = 0;
    }
    // If overlay position reaches out of the background on the bottom
    if ((ys + linecount) > hs) {
        linecount += hs - (ys + linecount);
    }
    // If overlay is outside the background boundaries
    if (linecount <= 0) return;

    unsigned char *bgdata = bgimage->GetUCharPointer(VideoCh) + (ys * ws) + xs;
    unsigned char *ovrldata = ovrlimage->GetUCharPointer(InputCh) + (yo * wo) + xo;

    if (Alpha == 255) {
        for (i = 0; i < linecount; i ++) {
            memcpy(bgdata, ovrldata, copylen);
            bgdata += ws;
            ovrldata += wo;
        }
    }
    else {
        // TO DO
    }
}


/***********************************/
/*** svlOverlayTargets class *******/
/***********************************/

svlOverlayTargets::svlOverlayTargets() :
    svlOverlay(),
    svlOverlayInput(),
    InputCh(0),
    ConfidenceColoring(true),
    Crosshair(false),
    TargetSize(5)
{
}

svlOverlayTargets::svlOverlayTargets(unsigned int videoch,
                                 bool visible,
                                 const std::string & inputname,
                                 unsigned int inputch,
                                 bool confcoloring,
                                 bool crosshair,
                                 unsigned int size) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    InputCh(inputch),
    ConfidenceColoring(confcoloring),
    Crosshair(crosshair),
    TargetSize(size)
{
}

svlOverlayTargets::~svlOverlayTargets()
{
}

void svlOverlayTargets::SetInputChannel(unsigned int inputch)
{
    InputCh = inputch;
}

void svlOverlayTargets::SetConfidenceColoring(bool enable)
{
    ConfidenceColoring = enable;
}

void svlOverlayTargets::SetCrosshair(bool enable)
{
    Crosshair = enable;
}

void svlOverlayTargets::SetSize(unsigned int size)
{
    TargetSize = size;
}

unsigned int svlOverlayTargets::GetInputChannel() const
{
    return InputCh;
}

bool svlOverlayTargets::GetConfidenceColoring() const
{
    return ConfidenceColoring;
}

bool svlOverlayTargets::GetCrosshair() const
{
    return Crosshair;
}

unsigned int svlOverlayTargets::GetSize() const
{
    return TargetSize;
}

bool svlOverlayTargets::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeTargets) return true;
    return false;
}

void svlOverlayTargets::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleTargets* ovrltargets = dynamic_cast<svlSampleTargets*>(input);
    if (!ovrltargets || InputCh >= ovrltargets->GetChannels()) return;

    const unsigned int maxtargets = ovrltargets->GetMaxTargets();
    vctDynamicVectorRef<int> confidence;
    vctDynamicMatrixRef<int> position;
    unsigned char r, g, b;
    unsigned int j;
    int conf, x, y;
    vct3 pos, pos_xf;

    confidence.SetRef(maxtargets, ovrltargets->GetConfidencePointer(InputCh));
    position.SetRef(2, maxtargets, ovrltargets->GetPositionPointer(InputCh));

    for (j = 0; j < maxtargets; j ++) {
        if (ovrltargets->GetFlag(j) > 0) {

            if (ConfidenceColoring) {
                conf = confidence[j];
                if (conf < 0) conf = 0; else if (conf > 255) conf = 255;
                r = 255 - conf; g = conf; b = 0;
            }
            else {
                r = g = b = 160;
            }

            x = position.Element(0, j);
            y = position.Element(1, j);

            if (Transformed) {
                pos[0] = x;
                pos[1] = y;
                pos[2] = 1.0;
                pos_xf.ProductOf(Transform, pos);
                x = pos_xf[0];
                y = pos_xf[1];
            }

            if (Crosshair) {
                svlDraw::Crosshair(bgimage, VideoCh, x, y, r, g, b, TargetSize);
            }
            else {
                svlDraw::Rectangle(bgimage, VideoCh,
                                   x - TargetSize + 1, y - TargetSize + 1, x + TargetSize, y + TargetSize,
                                   r, g, b);
            }
        }
    }
}


/***********************************/
/*** svlOverlayBlobs class *********/
/***********************************/

svlOverlayBlobs::svlOverlayBlobs() :
    svlOverlay(),
    svlOverlayInput(),
    InputCh(0),
    DrawID(false)
{
}

svlOverlayBlobs::svlOverlayBlobs(unsigned int videoch,
                                 bool visible,
                                 const std::string & inputname,
                                 unsigned int inputch,
                                 bool draw_id) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    InputCh(inputch),
    DrawID(draw_id)
{
}

svlOverlayBlobs::~svlOverlayBlobs()
{
}

void svlOverlayBlobs::SetInputChannel(unsigned int inputch)
{
    InputCh = inputch;
}

void svlOverlayBlobs::SetDrawID(bool enable)
{
    DrawID = enable;
}

unsigned int svlOverlayBlobs::GetInputChannel() const
{
    return InputCh;
}

bool svlOverlayBlobs::GetDrawID() const
{
    return DrawID;
}

bool svlOverlayBlobs::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeBlobs) return true;
    return false;
}

void svlOverlayBlobs::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleBlobs* ovrlblobs = dynamic_cast<svlSampleBlobs*>(input);
    if (!ovrlblobs || InputCh >= ovrlblobs->GetChannelCount()) return;

    svlBlob *blob = ovrlblobs->GetBlobsPointer(InputCh);
    const unsigned int maxblobs = ovrlblobs->GetBufferUsed(InputCh);

    for (unsigned int j = 0; j < maxblobs; j ++) {
        if (blob->used) {
            svlDraw::Rectangle(bgimage, VideoCh,
                               blob->left   - 1,
                               blob->top    - 1,
                               blob->right  + 1,
                               blob->bottom + 1,
                               20, 128, 20,
                               false);
        }
        blob ++;
    }
}


/***********************************/
/*** svlOverlayToolTips class ******/
/***********************************/

svlOverlayToolTips::svlOverlayToolTips() :
    svlOverlay(),
    svlOverlayInput(),
    InputCh(0),
    Thickness(10),
    Length(100),
    Color(255, 255, 255)
{
}

svlOverlayToolTips::svlOverlayToolTips(unsigned int videoch,
                                       bool visible,
                                       const std::string & inputname,
                                       unsigned int inputch,
                                       unsigned int thickness,
                                       unsigned int length,
                                       svlRGB color) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    InputCh(inputch),
    Thickness(thickness),
    Length(length),
    Color(color)
{
}

svlOverlayToolTips::~svlOverlayToolTips()
{
}

void svlOverlayToolTips::SetInputChannel(unsigned int inputch)
{
    InputCh = inputch;
}

void svlOverlayToolTips::SetThickness(unsigned int thickness)
{
    Thickness = thickness;
}

void svlOverlayToolTips::SetLength(unsigned int length)
{
    Length = length;
}

void svlOverlayToolTips::SetColor(svlRGB color)
{
    Color = color;
}

unsigned int svlOverlayToolTips::GetInputChannel() const
{
    return InputCh;
}

unsigned int svlOverlayToolTips::GetThickness() const
{
    return Thickness;
}

unsigned int svlOverlayToolTips::GetLength() const
{
    return Length;
}

svlRGB svlOverlayToolTips::GetColor() const
{
    return Color;
}

bool svlOverlayToolTips::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeMatrixDouble) return true;
    return false;
}

void svlOverlayToolTips::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleMatrixDouble* matrix_smpl = dynamic_cast<svlSampleMatrixDouble*>(input);
    if (!matrix_smpl) return;

    // Tool tracking results format:
    //  - each result is a 3x3 matrix
    //  - video channels are placed side-by-side horizontally
    //  - different tools stacked vertically
    // Example (stereo video and 3 tools):
    //    [Tool_1_left][Tool_1_right]
    //    [Tool_2_left][Tool_2_right]
    //    [Tool_3_left][Tool_3_right]

    vctDynamicMatrix<double>& matrix = matrix_smpl->GetDynamicMatrixRef();
    const unsigned int tool_count = matrix.rows() / 3;
    const unsigned int vch_count  = matrix.cols() / 3;
    if (tool_count < 1 || InputCh >= vch_count) return;

    vctFixedSizeMatrix<double, 3, 4> vertices, xf_vertices;
    vctDynamicMatrixRef<double> sub_matrix;
    const double radius = 0.5 * Thickness;
    unsigned int i, j;
    vct3x3 frm;

    vertices.Column(0).Assign(Length, -radius, 1.0);
    vertices.Column(1).Assign(0, -radius, 1.0);
    vertices.Column(2).Assign(0, radius, 1.0);
    vertices.Column(3).Assign(Length, radius, 1.0);

    for (i = 0; i < tool_count; i ++) {
        // Extract tool transformation from input matrix
        sub_matrix.SetRef(matrix, i * 3, InputCh * 3, 3, 3);
        frm.Assign(sub_matrix);

        // Transform vertices from tool coordinates to image coordinates
        xf_vertices.ProductOf(frm, vertices);

        if (Transformed) {
            // Transform vertices if overlay transform is available
            vertices.ProductOf(Transform, xf_vertices);

            // Draw poly line
            for (j = 1; j < vertices.cols(); j ++) {
                svlDraw::Line(bgimage, VideoCh,
                              vertices.Element(0, j - 1),
                              vertices.Element(1, j - 1),
                              vertices.Element(0, j),
                              vertices.Element(1, j),
                              Color.r, Color.g, Color.b);
            }
        }
        else {
            // Draw poly line
            for (j = 1; j < vertices.cols(); j ++) {
                svlDraw::Line(bgimage, VideoCh,
                              xf_vertices.Element(0, j - 1),
                              xf_vertices.Element(1, j - 1),
                              xf_vertices.Element(0, j),
                              xf_vertices.Element(1, j),
                              Color.r, Color.g, Color.b);
            }
        }
    }
}


/***************************************/
/*** svlOverlayStaticImage class *******/
/***************************************/

svlOverlayStaticImage::svlOverlayStaticImage() :
    svlOverlay(),
    Buffer(0),
    Pos(0, 0),
    Alpha(255)
{
}

svlOverlayStaticImage::svlOverlayStaticImage(unsigned int videoch,
                                             bool visible,
                                             const svlSampleImageRGB & image,
                                             vctInt2 pos,
                                             unsigned char alpha) :
    svlOverlay(videoch, visible),
    Buffer(0),
    Pos(pos),
    Alpha(alpha)
{
    SetImage(image);
}

svlOverlayStaticImage::svlOverlayStaticImage(unsigned int videoch,
                                             bool visible,
                                             const svlSampleImageRGBStereo & image,
                                             unsigned int imagech,
                                             vctInt2 pos,
                                             unsigned char alpha) :
    svlOverlay(videoch, visible),
    Buffer(0),
    Pos(pos),
    Alpha(alpha)
{
    SetImage(image, imagech);
}

svlOverlayStaticImage::~svlOverlayStaticImage()
{
    if (Buffer) delete Buffer;
}

void svlOverlayStaticImage::SetImage(const svlSampleImageRGB & image)
{
    const unsigned int width  = image.GetWidth();
    const unsigned int height = image.GetHeight();

    if (Buffer) {
        if (width != Buffer->GetWidth() || height != Buffer->GetHeight()) return;
    }
    else {
        if (width < 1 || height < 1) return;
        Buffer = new svlBufferImage(width, height);
    }

    Buffer->Push(image.GetUCharPointer(), image.GetDataSize(), false);
}

void svlOverlayStaticImage::SetImage(const svlSampleImageRGBStereo & image, unsigned int imagech)
{
    if (imagech >= image.GetVideoChannels()) return;

    const unsigned int width  = image.GetWidth(imagech);
    const unsigned int height = image.GetHeight(imagech);

    if (Buffer) {
        if (width != Buffer->GetWidth() || height != Buffer->GetHeight()) return;
    }
    else {
        if (width < 1 || height < 1) return;
        Buffer = new svlBufferImage(width, height);
    }

    Buffer->Push(image.GetUCharPointer(imagech), image.GetDataSize(imagech), false);
}

void svlOverlayStaticImage::SetPosition(vctInt2 pos)
{
    Pos = pos;
}

void svlOverlayStaticImage::SetPosition(int x, int y)
{
    Pos[0] = x;
    Pos[1] = y;
}

void svlOverlayStaticImage::SetAlpha(unsigned char alpha)
{
    Alpha = alpha;
}

vctInt2 svlOverlayStaticImage::GetPosition() const
{
    return Pos;
}

unsigned char svlOverlayStaticImage::GetAlpha() const
{
    return Alpha;
}

void svlOverlayStaticImage::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (!Buffer) return;

    svlImageRGB* ovrlimage = Buffer->Pull(false);
    if (!ovrlimage) return;

    int i, ws, hs, wo, ho, xs, ys, xo, yo, copylen, linecount;

    // Prepare for data copy
    ws = static_cast<int>(bgimage->GetWidth(VideoCh) * 3);
    hs = static_cast<int>(bgimage->GetHeight(VideoCh));
    wo = static_cast<int>(ovrlimage->cols());
    ho = static_cast<int>(ovrlimage->rows());

    copylen = wo;
    linecount = ho;
    xs = Pos.X() * 3;
    ys = Pos.Y();
    xo = yo = 0;

    // If overlay position reaches out of the background on the left
    if (xs < 0) {
        copylen += xs;
        xo -= xs;
        xs = 0;
    }
    // If overlay position reaches out of the background on the right
    if ((xs + copylen) > ws) {
        copylen += ws - (xs + copylen);
    }
    // If overlay is outside the background boundaries
    if (copylen <= 0) return;
    
    // If overlay position reaches out of the background on the top
    if (ys < 0) {
        linecount += ys;
        yo -= ys;
        ys = 0;
    }
    // If overlay position reaches out of the background on the bottom
    if ((ys + linecount) > hs) {
        linecount += hs - (ys + linecount);
    }
    // If overlay is outside the background boundaries
    if (linecount <= 0) return;

    unsigned char *bgdata = bgimage->GetUCharPointer(VideoCh) + (ys * ws) + xs;
    unsigned char *ovrldata = ovrlimage->Pointer() + (yo * wo) + xo;

    if (Alpha == 255) {
        for (i = 0; i < linecount; i ++) {
            memcpy(bgdata, ovrldata, copylen);
            bgdata += ws;
            ovrldata += wo;
        }
    }
    else {
        // TO DO
    }
}


/**********************************/
/*** svlOverlayStaticText class ***/
/**********************************/

svlOverlayStaticText::svlOverlayStaticText() :
    svlOverlay(),
    Rect(0, 0, 100, 10),
    FontSize(10.0 / SVL_OCV_FONT_SCALE),
    TxtColor(255, 255, 255),
    BGColor(0, 0, 0),
    Background(true),
    FontChanged(true),
    Baseline(0)
{
}

svlOverlayStaticText::svlOverlayStaticText(unsigned int videoch,
                                           bool visible,
                                           const std::string & text,
                                           svlRect rect,
                                           double fontsize,
                                           svlRGB txtcolor) :
    svlOverlay(videoch, visible),
    Text(text),
    Rect(rect),
    FontSize(fontsize / SVL_OCV_FONT_SCALE),
    TxtColor(txtcolor),
    BGColor(0, 0, 0),
    Background(false),
    FontChanged(true),
    Baseline(0)
{
}

svlOverlayStaticText::svlOverlayStaticText(unsigned int videoch,
                                           bool visible,
                                           const std::string & text,
                                           svlRect rect,
                                           double fontsize,
                                           svlRGB txtcolor,
                                           svlRGB bgcolor) :
    svlOverlay(videoch, visible),
    Text(text),
    Rect(rect),
    FontSize(fontsize / SVL_OCV_FONT_SCALE),
    TxtColor(txtcolor),
    BGColor(bgcolor),
    Background(true),
    FontChanged(true),
    Baseline(0)
{
}

svlOverlayStaticText::~svlOverlayStaticText()
{
}

void svlOverlayStaticText::SetText(const std::string & text)
{
    Text = text;
}

void svlOverlayStaticText::SetRect(svlRect rect)
{
    Rect = rect;
}

void svlOverlayStaticText::SetRect(int left, int top, int right, int bottom)
{
    Rect.Assign(left, top, right, bottom);
}

void svlOverlayStaticText::SetTextColor(svlRGB txtcolor)
{
    TxtColor = txtcolor;
}

void svlOverlayStaticText::SetFontSize(double size)
{
    FontSize = size / SVL_OCV_FONT_SCALE;
    FontChanged = true;
}

void svlOverlayStaticText::SetBackground(bool enable)
{
    Background = enable;
}

void svlOverlayStaticText::SetBackgroundColor(svlRGB bgcolor)
{
    BGColor = bgcolor;
}

const std::string & svlOverlayStaticText::GetText() const
{
    return Text;
}

svlRect svlOverlayStaticText::GetRect() const
{
    return Rect;
}

void svlOverlayStaticText::GetRect(int & left, int & top, int & right, int & bottom) const
{
    left   = Rect.left;
    top    = Rect.top;
    right  = Rect.right;
    bottom = Rect.bottom;
}

svlRGB svlOverlayStaticText::GetTextColor() const
{
    return TxtColor;
}

double svlOverlayStaticText::GetFontSize() const
{
    return FontSize * SVL_OCV_FONT_SCALE;
}

bool svlOverlayStaticText::GetBackground() const
{
    return Background;
}

svlRGB svlOverlayStaticText::GetBackgroundColor() const
{
    return BGColor;
}

#if CISST_SVL_HAS_OPENCV

svlRect svlOverlayStaticText::GetTextSize(const std::string & text)
{
    CvSize size;
    int ival;

    if (FontChanged) {
        CvSize size;
        cvInitFont(&Font, CV_FONT_HERSHEY_PLAIN, FontSize, FontSize, 0, 1, 4);
        cvGetTextSize("|", &Font, &size, &Baseline);
        Baseline ++;
        FontChanged = false;
    }

    cvGetTextSize(text.c_str(), &Font, &size, &ival);

    return svlRect(0, 0, size.width, size.height);
}

#else // CISST_SVL_HAS_OPENCV

svlRect svlOverlayStaticText::GetTextSize(const std::string & CMN_UNUSED(text))
{
    // To be implemented

    return svlRect(-1, -1, -1, -1);
}

#endif // CISST_SVL_HAS_OPENCV

#if CISST_SVL_HAS_OPENCV

void svlOverlayStaticText::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (FontChanged) {
        CvSize size;
        cvInitFont(&Font, CV_FONT_HERSHEY_PLAIN, FontSize, FontSize, 0, 1, 4);
        cvGetTextSize("|", &Font, &size, &Baseline);
        Baseline ++;
        FontChanged = false;
    }

    if (Background) svlDraw::Rectangle(bgimage, VideoCh, Rect, BGColor);

    CvRect cvrect;
    cvrect.x      = Rect.left;
    cvrect.width  = Rect.right - Rect.left;
    cvrect.y      = Rect.top + 1;
    cvrect.height = Rect.bottom - Rect.top + 1;

    IplImage* cvimg = bgimage->IplImageRef(VideoCh);
    cvSetImageROI(cvimg, cvrect);
    cvPutText(cvimg, Text.c_str(),
              cvPoint(0, cvrect.height - Baseline),
              &Font, cvScalar(TxtColor.r, TxtColor.g, TxtColor.b));
    cvResetImageROI(cvimg);
}

#else // CISST_SVL_HAS_OPENCV

void svlOverlayStaticText::DrawInternal(svlSampleImage* CMN_UNUSED(bgimage), svlSample* CMN_UNUSED(input))
{
    // To be implemented
}

#endif // CISST_SVL_HAS_OPENCV


/********************************/
/*** svlOverlayText class *******/
/********************************/

svlOverlayText::svlOverlayText() :
    svlOverlayStaticText(),
    svlOverlayInput()
{
}

svlOverlayText::svlOverlayText(unsigned int videoch,
                               bool visible,
                               const std::string & inputname,
                               svlRect rect,
                               double fontsize,
                               svlRGB txtcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor),
    svlOverlayInput(inputname)
{
}

svlOverlayText::svlOverlayText(unsigned int videoch,
                               bool visible,
                               const std::string & inputname,
                               svlRect rect,
                               double fontsize,
                               svlRGB txtcolor,
                               svlRGB bgcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor, bgcolor),
    svlOverlayInput(inputname)
{
}

svlOverlayText::~svlOverlayText()
{
}

bool svlOverlayText::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeText) return true;
    return false;
}

void svlOverlayText::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleText* ovrltext = dynamic_cast<svlSampleText*>(input);
    if (ovrltext) {
        SetText(ovrltext->GetStringRef());
        svlOverlayStaticText::DrawInternal(bgimage, 0);
    }
}


/**********************************/
/*** svlOverlayStaticRect class ***/
/**********************************/

svlOverlayStaticRect::svlOverlayStaticRect() :
    svlOverlay(),
    Rect(0, 0, 10, 10),
    Color(255, 255, 255),
    Fill(true)
{
}

svlOverlayStaticRect::svlOverlayStaticRect(unsigned int videoch,
                                           bool visible,
                                           svlRect rect,
                                           svlRGB color,
                                           bool fill) :
    svlOverlay(videoch, visible),
    Rect(rect),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticRect::~svlOverlayStaticRect()
{
}

void svlOverlayStaticRect::SetRect(svlRect rect)
{
    Rect = rect;
}

void svlOverlayStaticRect::SetRect(int left, int top, int right, int bottom)
{
    Rect.Assign(left, top, right, bottom);
}

void svlOverlayStaticRect::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayStaticRect::SetFill(bool fill)
{
    Fill = fill;
}

svlRect svlOverlayStaticRect::GetRect() const
{
    return Rect;
}

void svlOverlayStaticRect::GetRect(int & left, int & top, int & right, int & bottom) const
{
    left   = Rect.left;
    top    = Rect.top;
    right  = Rect.right;
    bottom = Rect.bottom;
}

svlRGB svlOverlayStaticRect::GetColor() const
{
    return Color;
}

bool svlOverlayStaticRect::GetFill() const
{
    return Fill;
}

void svlOverlayStaticRect::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    svlDraw::Rectangle(bgimage, VideoCh, Rect, Color, Fill);
}


/*************************************/
/*** svlOverlayStaticEllipse class ***/
/*************************************/

svlOverlayStaticEllipse::svlOverlayStaticEllipse() :
    svlOverlay(),
    Center(0, 0),
    RadiusHoriz(0),
    RadiusVert(0),
    Angle(0.0),
    Color(255, 255, 255),
    Fill(true)
{
}

svlOverlayStaticEllipse::svlOverlayStaticEllipse(unsigned int videoch,
                                                 bool visible,
                                                 const svlPoint2D center,
                                                 int radius_horiz,
                                                 int radius_vert,
                                                 double angle,
                                                 svlRGB color,
                                                 bool fill) :
    svlOverlay(videoch, visible),
    Center(center),
    RadiusHoriz(radius_horiz),
    RadiusVert(radius_vert),
    Angle(angle),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticEllipse::svlOverlayStaticEllipse(unsigned int videoch,
                                                 bool visible,
                                                 const svlPoint2D center,
                                                 int radius,
                                                 svlRGB color,
                                                 bool fill) :
    svlOverlay(videoch, visible),
    Center(center),
    RadiusHoriz(radius),
    RadiusVert(radius),
    Angle(0.0),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticEllipse::~svlOverlayStaticEllipse()
{
}

void svlOverlayStaticEllipse::SetCenter(const svlPoint2D center)
{
    Center = center;
}

void svlOverlayStaticEllipse::SetRadius(const int radius_horiz, const int radius_vert)
{
    RadiusHoriz = radius_horiz;
    RadiusVert  = radius_vert;
}

void svlOverlayStaticEllipse::SetRadius(const int radius)
{
    RadiusHoriz = RadiusVert = radius;
}

void svlOverlayStaticEllipse::SetAngle(const double angle)
{
    Angle = angle;
}

void svlOverlayStaticEllipse::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayStaticEllipse::SetFill(bool fill)
{
    Fill = fill;
}

svlPoint2D svlOverlayStaticEllipse::GetCenter() const
{
    return Center;
}

void svlOverlayStaticEllipse::GetRadius(int & radius_horiz, int & radius_vert) const
{
    radius_horiz = RadiusHoriz;
    radius_vert  = RadiusVert;
}

double svlOverlayStaticEllipse::GetAngle() const
{
    return Angle;
}

svlRGB svlOverlayStaticEllipse::GetColor() const
{
    return Color;
}

bool svlOverlayStaticEllipse::GetFill() const
{
    return Fill;
}

void svlOverlayStaticEllipse::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
#if CISST_SVL_HAS_OPENCV

    cvEllipse(bgimage->IplImageRef(VideoCh),
              cvPoint(Center.x, Center.y),
              cvSize(RadiusHoriz, RadiusVert),
              Angle, 0.0, 360.0,
              cvScalar(Color.r, Color.g, Color.b),
              (Fill ? -1 : 1));

#else // CISST_SVL_HAS_OPENCV

    // TO DO: to be implemented

#endif // CISST_SVL_HAS_OPENCV
}


/**************************************/
/*** svlOverlayStaticTriangle class ***/
/**************************************/

svlOverlayStaticTriangle::svlOverlayStaticTriangle() :
    svlOverlay(),
    Color(255, 255, 255),
    Fill(true)
{
    DrawInternals = new svlDraw::Internals;
}

svlOverlayStaticTriangle::svlOverlayStaticTriangle(unsigned int videoch,
                                                   bool visible,
                                                   const svlPoint2D corner1,
                                                   const svlPoint2D corner2,
                                                   const svlPoint2D corner3,
                                                   svlRGB color,
                                                   bool fill) :
    svlOverlay(videoch, visible),
    Corner1(corner1),
    Corner2(corner2),
    Corner3(corner3),
    Color(color),
    Fill(fill)
{
    DrawInternals = new svlDraw::Internals;
}

svlOverlayStaticTriangle::svlOverlayStaticTriangle(unsigned int videoch,
                                                   bool visible,
                                                   const int x1, const int y1,
                                                   const int x2, const int y2,
                                                   const int x3, const int y3,
                                                   svlRGB color,
                                                   bool fill) :
    svlOverlay(videoch, visible),
    Corner1(x1, y1),
    Corner2(x2, y2),
    Corner3(x3, y3),
    Color(color),
    Fill(fill)
{
    DrawInternals = new svlDraw::Internals;
}

svlOverlayStaticTriangle::~svlOverlayStaticTriangle()
{
    delete DrawInternals;
}

void svlOverlayStaticTriangle::SetCorners(const svlPoint2D corner1,
                                          const svlPoint2D corner2,
                                          const svlPoint2D corner3)
{
    Corner1 = corner1;
    Corner2 = corner2;
    Corner3 = corner3;
}

void svlOverlayStaticTriangle::SetCorners(const int x1, const int y1,
                                          const int x2, const int y2,
                                          const int x3, const int y3)
{
    Corner1.Assign(x1, y1);
    Corner2.Assign(x2, y2);
    Corner3.Assign(x3, y3);
}

void svlOverlayStaticTriangle::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayStaticTriangle::SetFill(bool fill)
{
    Fill = fill;
}

void svlOverlayStaticTriangle::GetCorners(svlPoint2D& corner1,
                                          svlPoint2D& corner2,
                                          svlPoint2D& corner3) const
{
    corner1 = Corner1;
    corner2 = Corner2;
    corner3 = Corner3;
}

void svlOverlayStaticTriangle::GetCorners(int& x1, int& y1,
                                          int& x2, int& y2,
                                          int& x3, int& y3) const
{
    x1 = Corner1.x; y1 = Corner1.y;
    x2 = Corner2.x; y2 = Corner2.y;
    x3 = Corner3.x; y3 = Corner3.y;
}

svlRGB svlOverlayStaticTriangle::GetColor() const
{
    return Color;
}

bool svlOverlayStaticTriangle::GetFill() const
{
    return Fill;
}

void svlOverlayStaticTriangle::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Fill) {
        svlDraw::Triangle(bgimage, VideoCh, Corner1, Corner2, Corner3, Color, DrawInternals[0]);
    }
    else {
        svlDraw::Line(bgimage, VideoCh, Corner1, Corner2, Color);
        svlDraw::Line(bgimage, VideoCh, Corner2, Corner3, Color);
        svlDraw::Line(bgimage, VideoCh, Corner3, Corner1, Color);
    }
}


/**********************************/
/*** svlOverlayStaticPoly class ***/
/**********************************/

svlOverlayStaticPoly::svlOverlayStaticPoly() :
    svlOverlay(),
    Color(255, 255, 255),
    Thickness(1),
    Start(0)
{
}

svlOverlayStaticPoly::svlOverlayStaticPoly(unsigned int videoch,
                                           bool visible,
                                           svlRGB color) :
    svlOverlay(videoch, visible),
    Color(color),
    Thickness(1),
    Start(0)
{
}

svlOverlayStaticPoly::svlOverlayStaticPoly(unsigned int videoch,
                                           bool visible,
                                           svlRGB color,
                                           unsigned int thickness) :
    svlOverlay(videoch, visible),
    Color(color),
    Thickness(thickness),
    Start(0)
{
}

svlOverlayStaticPoly::svlOverlayStaticPoly(unsigned int videoch,
                                           bool visible,
                                           const TypeRef poly,
                                           svlRGB color) :
    svlOverlay(videoch, visible),
    Poly(poly),
    Color(color),
    Thickness(1),
    Start(0)
{
}

svlOverlayStaticPoly::svlOverlayStaticPoly(unsigned int videoch,
                                           bool visible,
                                           const TypeRef poly,
                                           svlRGB color,
                                           unsigned int thickness,
                                           unsigned int start) :
    svlOverlay(videoch, visible),
    Poly(poly),
    Color(color),
    Thickness(thickness),
    Start(start)
{
}

svlOverlayStaticPoly::~svlOverlayStaticPoly()
{
}

void svlOverlayStaticPoly::SetPoints(const TypeRef points)
{
    CS.Enter();
        Poly = points;
    CS.Leave();
}

void svlOverlayStaticPoly::SetPoints(const TypeRef points, unsigned int start)
{
    CS.Enter();
        Poly = points;
        Start = start;
    CS.Leave();
}

void svlOverlayStaticPoly::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayStaticPoly::SetThickness(unsigned int thickness)
{
    Thickness = thickness;
}

void svlOverlayStaticPoly::SetStart(unsigned int start)
{
    Start = start;
}

svlOverlayStaticPoly::TypeRef svlOverlayStaticPoly::GetPoints()
{
    return Poly;
}

svlRGB svlOverlayStaticPoly::GetColor() const
{
    return Color;
}

unsigned int svlOverlayStaticPoly::GetThickness() const
{
    return Thickness;
}

unsigned int svlOverlayStaticPoly::GetStart() const
{
    return Start;
}

unsigned int svlOverlayStaticPoly::AddPoint(svlPoint2D point)
{
    CS.Enter();
        unsigned int size = Poly.size();
        Poly.resize(size + 1);
        Poly[size] = point;
    CS.Leave();
    return size;
}

unsigned int svlOverlayStaticPoly::AddPoint(int x, int y)
{
    CS.Enter();
        unsigned int size = Poly.size();
        Poly.resize(size + 1);
        Poly[size].x = x;
        Poly[size].y = y;
    CS.Leave();
    return size;
}

int svlOverlayStaticPoly::SetPoint(unsigned int idx, svlPoint2D point)
{
    if (idx >= Poly.size()) return SVL_FAIL;
    Poly[idx] = point;
    return SVL_OK;
}

int svlOverlayStaticPoly::SetPoint(unsigned int idx, vctInt2 point)
{
    if (idx >= Poly.size()) return SVL_FAIL;
    Poly[idx].x = point.X();
    Poly[idx].y = point.Y();
    return SVL_OK;
}

int svlOverlayStaticPoly::SetPoint(unsigned int idx, int x, int y)
{
    if (idx >= Poly.size()) return SVL_FAIL;
    Poly[idx].x = x;
    Poly[idx].y = y;
    return SVL_OK;
}

int svlOverlayStaticPoly::GetPoint(unsigned int idx, svlPoint2D & point) const
{
    if (idx >= Poly.size()) return SVL_FAIL;
    point = Poly[idx];
    return SVL_OK;
}

int svlOverlayStaticPoly::GetPoint(unsigned int idx, vctInt2 & point) const
{
    if (idx >= Poly.size()) return SVL_FAIL;
    point.X() = Poly[idx].x;
    point.Y() = Poly[idx].y;
    return SVL_OK;
}

int svlOverlayStaticPoly::GetPoint(unsigned int idx, int & x, int & y) const
{
    if (idx >= Poly.size()) return SVL_FAIL;
    x = Poly[idx].x;
    y = Poly[idx].y;
    return SVL_OK;
}

void svlOverlayStaticPoly::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Transformed) {
    CS.Enter();
            const unsigned int length = Poly.size();
            if (PolyXF.size() != length) PolyXF.SetSize(length);

            vct3 pos, pos_xf;
            for (unsigned int i = 0; i < length; i ++) {
                pos[0] = Poly[i].x;
                pos[1] = Poly[i].y;
                pos[2] = 1.0;
                pos_xf.ProductOf(Transform, pos);
                PolyXF[i].x = pos_xf[0];
                PolyXF[i].y = pos_xf[1];
            }
        CS.Leave();

        svlDraw::Poly(bgimage, VideoCh, PolyXF, Color, Thickness, Start);
    }
    else {
        CS.Enter();
        svlDraw::Poly(bgimage, VideoCh, Poly, Color, Thickness, Start);
    CS.Leave();
    }
}


/*********************************/
/*** svlOverlayStaticBar class ***/
/*********************************/

svlOverlayStaticBar::svlOverlayStaticBar() :
    svlOverlay(),
    Range(0.0, 100.0),
    Value(50.0),
    Vertical(true),
    Rect(0, 0, 10, 10),
    Color(255, 255, 255),
    BGColor(128, 128, 128),
    BorderWidth(1),
    BorderColor(0, 0, 0)
{
}

svlOverlayStaticBar::svlOverlayStaticBar(unsigned int videoch,
                                         bool visible,
                                         vct2 range,
                                         double value,
                                         bool vertical,
                                         svlRect rect,
                                         svlRGB color,
                                         svlRGB bgcolor,
                                         unsigned int borderwidth,
                                         svlRGB bordercolor) :
    svlOverlay(videoch, visible),
    Range(range),
    Value(value),
    Vertical(vertical),
    Rect(rect),
    Color(color),
    BGColor(bgcolor),
    BorderWidth(borderwidth),
    BorderColor(bordercolor)
{
    Rect.Normalize();
}

svlOverlayStaticBar::svlOverlayStaticBar(unsigned int videoch,
                                         bool visible,
                                         vct2 range,
                                         double value,
                                         bool vertical,
                                         svlRect rect,
                                         svlRGB color,
                                         svlRGB bgcolor) :
    svlOverlay(videoch, visible),
    Range(range),
    Value(value),
    Vertical(vertical),
    Rect(rect),
    Color(color),
    BGColor(bgcolor),
    BorderWidth(0),
    BorderColor(0, 0, 0)
{
    Rect.Normalize();
}

svlOverlayStaticBar::~svlOverlayStaticBar()
{
}

void svlOverlayStaticBar::SetRange(const vct2 range)
{
    Range = range;
}

void svlOverlayStaticBar::SetRange(const double from, const double to)
{
    Range[0] = from;
    Range[1] = to;
}

void svlOverlayStaticBar::SetValue(const double value)
{
    Value = value;
}

void svlOverlayStaticBar::SetDirection(const bool vertical)
{
    Vertical = vertical;
}

void svlOverlayStaticBar::SetRect(svlRect rect)
{
    Rect = rect;
    Rect.Normalize();
}

void svlOverlayStaticBar::SetRect(int left, int top, int right, int bottom)
{
    Rect.Assign(left, top, right, bottom);
}

void svlOverlayStaticBar::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayStaticBar::SetBackgroundColor(svlRGB bgcolor)
{
    BGColor = bgcolor;
}

void svlOverlayStaticBar::SetBorderWidth(const unsigned int pixels)
{
    BorderWidth = pixels;
}

void svlOverlayStaticBar::SetBorderColor(svlRGB bordercolor)
{
    BorderColor = bordercolor;
}

vct2 svlOverlayStaticBar::GetRange() const
{
    return Range;
}

void svlOverlayStaticBar::GetRange(double & from, double & to) const
{
    from = Range[0];
    to = Range[1];
}

double svlOverlayStaticBar::GetValue() const
{
    return Value;
}

bool svlOverlayStaticBar::GetDirection() const
{
    return Vertical;
}

svlRect svlOverlayStaticBar::GetRect() const
{
    return Rect;
}

void svlOverlayStaticBar::GetRect(int & left, int & top, int & right, int & bottom) const
{
    left   = Rect.left;
    top    = Rect.top;
    right  = Rect.right;
    bottom = Rect.bottom;
}

svlRGB svlOverlayStaticBar::GetColor() const
{
    return Color;
}

svlRGB svlOverlayStaticBar::GetBackgroundColor() const
{
    return BGColor;
}

unsigned int svlOverlayStaticBar::GetBorderWidth() const
{
    return BorderWidth;
}

svlRGB svlOverlayStaticBar::GetBorderColor() const
{
    return BorderColor;
}

int svlOverlayStaticBar::GetValueInImagePos(double value, int & imagepos) const
{
    int ret = SVL_OK;

    double range;
    if (Range[0] != Range[1]) range = Range[1] - Range[0];
    else range = 100.0;
    double position = (value - Range[0]) / range;
    if (position < 0.0 || position > 1.0) ret = SVL_FAIL;

    if (Vertical) {
        imagepos = Rect.bottom - static_cast<int>(static_cast<double>(Rect.bottom - Rect.top) * position);
    }
    else {
        imagepos = Rect.left + static_cast<int>(static_cast<double>(Rect.right - Rect.left) * position);
    }

    return ret;
}

int svlOverlayStaticBar::GetImagePosInValue(int imagepos, double & value) const
{
    int ret = SVL_OK;

    double range;
    if (Range[0] != Range[1]) range = Range[1] - Range[0];
    else range = 100.0;

    if (Vertical) {
        if (imagepos < static_cast<int>(Rect.top) || imagepos > static_cast<int>(Rect.bottom)) ret = SVL_FAIL;

        value = range * static_cast<double>(Rect.bottom - imagepos) / (Rect.bottom - Rect.top);
    }
    else {
        if (imagepos < static_cast<int>(Rect.left) || imagepos > static_cast<int>(Rect.right)) ret = SVL_FAIL;

        value = range * static_cast<double>(imagepos - Rect.left) / (Rect.right - Rect.left);
    }

    return ret;
}

void svlOverlayStaticBar::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    svlRect rect1, rect2;

    if (BorderWidth > 0) {
        rect1.left = Rect.left - BorderWidth;
        rect1.right = Rect.right + BorderWidth;
        rect1.top = Rect.top - BorderWidth;
        rect1.bottom = Rect.bottom + BorderWidth;

        if (rect1.left <= rect1.right &&
            rect1.top  <= rect1.bottom) svlDraw::Rectangle(bgimage, VideoCh, rect1, BorderColor, true);
    }

    double range;
    if (Range[0] != Range[1]) range = Range[1] - Range[0];
    else range = 100.0;
    double position = (Value - Range[0]) / range;
    if (position < 0.0) position = 0.0;
    else if (position > 1.0) position = 1.0;

    if (Vertical) {
        rect1.left = Rect.left;
        rect1.right = Rect.right;
        rect1.top =  Rect.bottom - static_cast<int>(static_cast<double>(Rect.bottom - Rect.top) * position);
        rect1.bottom = Rect.bottom;

        rect2.left = Rect.left;
        rect2.right = Rect.right;
        rect2.top = Rect.top;
        rect2.bottom = rect1.top;
    }
    else {
        rect1.left = Rect.left;
        rect1.right = Rect.left + static_cast<int>(static_cast<double>(Rect.right - Rect.left) * position);
        rect1.top = Rect.top;
        rect1.bottom = Rect.bottom;

        rect2.left = rect1.right;
        rect2.right = Rect.right;
        rect2.top = Rect.top;
        rect2.bottom = Rect.bottom;
    }

    if (rect1.left <= rect1.right &&
        rect1.top  <= rect1.bottom) svlDraw::Rectangle(bgimage, VideoCh, rect1, Color, true);
    if (rect2.left <= rect2.right &&
        rect2.top  <= rect2.bottom) svlDraw::Rectangle(bgimage, VideoCh, rect2, BGColor, true);
}


/*********************************/
/*** svlOverlayFramerate class ***/
/*********************************/

svlOverlayFramerate::svlOverlayFramerate() :
    svlOverlayStaticText(),
    Filter(0),
    PrevSampleCount(0),
    PrevSampleTime(-1.0),
    UpdateRate(0.5),
    ResetFlag(true)
{
    StopWatch.Start();
}

svlOverlayFramerate::svlOverlayFramerate(unsigned int videoch,
                                         bool visible,
                                         svlFilterBase* filter,
                                         svlRect rect,
                                         double fontsize,
                                         svlRGB txtcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor),
    Filter(filter),
    PrevSampleCount(0),
    PrevSampleTime(-1.0),
    UpdateRate(0.5),
    ResetFlag(true)
{
    StopWatch.Start();
}

svlOverlayFramerate::svlOverlayFramerate(unsigned int videoch,
                                         bool visible,
                                         svlFilterBase* filter,
                                         svlRect rect,
                                         double fontsize,
                                         svlRGB txtcolor,
                                         svlRGB bgcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor, bgcolor),
    Filter(filter),
    PrevSampleCount(0),
    PrevSampleTime(-1.0),
    UpdateRate(0.5),
    ResetFlag(true)
{
    StopWatch.Start();
}

svlOverlayFramerate::~svlOverlayFramerate()
{
}

void svlOverlayFramerate::SetUpdateRate(double seconds)
{
    UpdateRate = seconds;
}

double svlOverlayFramerate::GetUpdateRate() const
{
    return UpdateRate;
}

void svlOverlayFramerate::Reset()
{
    ResetFlag = true;
}

void svlOverlayFramerate::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Filter) {

        unsigned int samplecount = Filter->GetFrameCounter();
        double sampletime = StopWatch.GetElapsedTime();

        if (ResetFlag) {
            PrevSampleCount = samplecount;
            SetText("NA");
            ResetFlag = false;
        }

        if (PrevSampleCount != samplecount) {

            double elapsed = sampletime - PrevSampleTime;
            if (elapsed >= UpdateRate) {
                elapsed /= samplecount - PrevSampleCount;

                std::stringstream strstr;
                strstr << std::fixed << std::setprecision(2) << 1.0 / elapsed;
                SetText(strstr.str());

                PrevSampleTime = sampletime;
                PrevSampleCount = samplecount;
            }
        }
        else {
            PrevSampleTime = sampletime;
        }
    }

    svlOverlayStaticText::DrawInternal(bgimage, 0);
}


/*********************************/
/*** svlOverlayTimestamp class ***/
/*********************************/

svlOverlayTimestamp::svlOverlayTimestamp() :
    svlOverlayStaticText(),
    Filter(0)
{
}

svlOverlayTimestamp::svlOverlayTimestamp(unsigned int videoch,
                                         bool visible,
                                         svlFilterBase* filter,
                                         svlRect rect,
                                         double fontsize,
                                         svlRGB txtcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor),
    Filter(filter)
{
}

svlOverlayTimestamp::svlOverlayTimestamp(unsigned int videoch,
                                         bool visible,
                                         svlFilterBase* filter,
                                         svlRect rect,
                                         double fontsize,
                                         svlRGB txtcolor,
                                         svlRGB bgcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor, bgcolor),
    Filter(filter)
{
}

svlOverlayTimestamp::~svlOverlayTimestamp()
{
}

void svlOverlayTimestamp::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Filter) {

        double timestamp = -1.0;
        svlFilterInput* input = Filter->GetInput();
        if (input) timestamp = input->GetTimestamp();
        else {
            svlFilterOutput* output = Filter->GetOutput();
            if (output) timestamp = output->GetTimestamp();
        }

        std::stringstream strstr;
        strstr << std::fixed << std::setprecision(3) << timestamp;
        SetText(strstr.str());
    }

    svlOverlayStaticText::DrawInternal(bgimage, 0);
}


/*********************************************/
/*** svlOverlayAsyncOutputProperties class ***/
/*********************************************/

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties() :
    svlOverlayStaticText(),
    Output(0)
{
}

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties(unsigned int videoch,
                                                                 bool visible,
                                                                 svlFilterOutput* output,
                                                                 svlRect rect,
                                                                 double fontsize,
                                                                 svlRGB txtcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor),
    Output(output)
{
}

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties(unsigned int videoch,
                                                                 bool visible,
                                                                 svlFilterOutput* output,
                                                                 svlRect rect,
                                                                 double fontsize,
                                                                 svlRGB txtcolor,
                                                                 svlRGB bgcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor, bgcolor),
    Output(output)
{
}

svlOverlayAsyncOutputProperties::~svlOverlayAsyncOutputProperties()
{
}

void svlOverlayAsyncOutputProperties::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Output) {
        double usageratio = Output->GetBufferUsageRatio();
        int dropped = Output->GetDroppedSampleCount();

        std::stringstream strstr;
        strstr << "Buffer: " << std::fixed << std::setprecision(1) << usageratio * 100.0 << "%, Dropped: " << dropped;
        SetText(strstr.str());
    }

    svlOverlayStaticText::DrawInternal(bgimage, 0);
}

