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
    TransformTimestamp(-1.0),
    TransformID(-1),
    Transformed(false),
    TransformSynchronized(false),
    Next(0),
    Prev(0),
    Used(false),
    MarkedForRemoval(_DoNotRemove)
{
}

svlOverlay::svlOverlay(unsigned int videoch,
                       bool visible) :
    VideoCh(videoch),
    Visible(visible),
    Transform(vct3x3::Eye()),
    TransformTimestamp(-1.0),
    TransformID(-1),
    Transformed(false),
    TransformSynchronized(false),
    Next(0),
    Prev(0),
    Used(false),
    MarkedForRemoval(_DoNotRemove)
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

void svlOverlay::SetTransform(const vct3x3 & transform, const double timestamp)
{
    Transform.Assign(transform);
    TransformTimestamp = timestamp;
    if (Transform != vct3x3::Eye()) Transformed = true;
    else Transformed = false;
}

double svlOverlay::GetTransformTimestamp() const
{
    return TransformTimestamp;
}

void svlOverlay::SetTransformID(int ID)
{
    TransformID = ID;
}

int svlOverlay::GetTransformID() const
{
    return TransformID;
}

bool svlOverlay::IsTransformed() const
{
    return Transformed;
}

void svlOverlay::SetTransformSynchronized(bool transform_synchronized)
{
    TransformSynchronized = transform_synchronized;
}

bool svlOverlay::GetTransformSynchronized() const
{
    return TransformSynchronized;
}

void svlOverlay::Draw(svlSampleImage* bgimage, svlSample* input)
{
    if (!bgimage || !Visible) return;

    if (VideoCh != SVL_ALL_CHANNELS) {
        if (VideoCh < bgimage->GetVideoChannels()) {
            DrawInternal(bgimage, input);
        }
    }
    else {
        // Iterate through all video channels
        for (unsigned int vch = 0; vch < bgimage->GetVideoChannels(); vch ++) {
            VideoCh = vch;
            DrawInternal(bgimage, input);
        }
        VideoCh = SVL_ALL_CHANNELS;
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

void svlOverlayInput::SetInputSynchronized(bool input_synchronized)
{
    InputSynchronized = input_synchronized;
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
    Alpha(255),
    QuadMappingEnabled(false),
    QuadMappingSet(false)
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
    Alpha(alpha),
    QuadMappingEnabled(false),
    QuadMappingSet(false)
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

void svlOverlayImage::SetEnableQuadMapping(bool enable)
{
    QuadMappingEnabled = enable;
}

void svlOverlayImage::SetQuadMapping(vctInt2 ul, vctInt2 ur, vctInt2 ll, vctInt2 lr)
{
    QuadUL = ul;
    QuadUR = ur;
    QuadLL = ll;
    QuadLR = lr;
    QuadMappingSet = true;
}

void svlOverlayImage::SetQuadMapping(int xul, int yul, int xur, int yur, int xll, int yll, int xlr, int ylr)
{
    QuadUL.Assign(xul, yul);
    QuadUR.Assign(xur, yur);
    QuadLL.Assign(xll, yll);
    QuadLR.Assign(xlr, ylr);
    QuadMappingSet = true;
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

bool svlOverlayImage::GetEnableQuadMapping() const
{
    return QuadMappingEnabled;
}

bool svlOverlayImage::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeImageRGB || inputtype == svlTypeImageRGBStereo) return true;
    return false;
}

void svlOverlayImage::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Skip drawing if fully transparent
    if (Alpha == 0) return;

    // Get sample from input
    svlSampleImage* ovrlimage = dynamic_cast<svlSampleImage*>(input);

    // Check video inputs
    if (!ovrlimage ||
        InputCh >= ovrlimage->GetVideoChannels() ||
        bgimage->GetBPP() != ovrlimage->GetBPP()) return;

    if (QuadMappingEnabled && QuadMappingSet) {
        const int ulx = 0, uly = 0, llx = 0, ury = 0;
        const int urx = ovrlimage->GetWidth(InputCh)  - 1, lrx = ovrlimage->GetWidth(InputCh)  - 1;
        const int lly = ovrlimage->GetHeight(InputCh) - 1, lry = ovrlimage->GetHeight(InputCh) - 1;
        svlQuad quad_in, quad_out;

        quad_in.Assign(ulx, uly, urx, ury, lrx, lry, llx, lly);
        quad_out.Assign(QuadUL[0], QuadUL[1], QuadUR[0], QuadUR[1], QuadLR[0], QuadLR[1], QuadLL[0], QuadLL[1]);
        svlDraw::WarpQuad(ovrlimage, InputCh, quad_in, bgimage, VideoCh, quad_out, WarpInternals, Alpha);

        return;
    }

    if (Transformed) {
        const int iulx = 0, iuly = 0, illx = 0, iury = 0;
        const int iurx = ovrlimage->GetWidth(InputCh)  - 1, ilrx = ovrlimage->GetWidth(InputCh)  - 1;
        const int illy = ovrlimage->GetHeight(InputCh) - 1, ilry = ovrlimage->GetHeight(InputCh) - 1;
        const int halfwidth  = ovrlimage->GetWidth(InputCh)  / 2;
        const int halfheight = ovrlimage->GetHeight(InputCh) / 2;
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        double x, y;

        x = iulx - halfwidth; y = iuly - halfheight;
        const int oulx = static_cast<int>(x * m00 + y * m01 + m02);
        const int ouly = static_cast<int>(x * m10 + y * m11 + m12);

        x = iurx - halfwidth; y = iury - halfheight;
        const int ourx = static_cast<int>(x * m00 + y * m01 + m02);
        const int oury = static_cast<int>(x * m10 + y * m11 + m12);

        x = illx - halfwidth; y = illy - halfheight;
        const int ollx = static_cast<int>(x * m00 + y * m01 + m02);
        const int olly = static_cast<int>(x * m10 + y * m11 + m12);

        x = ilrx - halfwidth; y = ilry - halfheight;
        const int olrx = static_cast<int>(x * m00 + y * m01 + m02);
        const int olry = static_cast<int>(x * m10 + y * m11 + m12);

        svlQuad quad_in, quad_out;

        quad_in.Assign(iulx, iuly, iurx, iury, ilrx, ilry, illx, illy);
        quad_out.Assign(oulx, ouly, ourx, oury, olrx, olry, ollx, olly);
        svlDraw::WarpQuad(ovrlimage, InputCh, quad_in, bgimage, VideoCh, quad_out, WarpInternals, Alpha);

        return;
    }

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
        const unsigned int w1 = Alpha + 1;
        const unsigned int w0 = 256 - w1;
        unsigned char *pbg, *pov;

        for (int j = 0; j < linecount; j ++) {
            pbg = bgdata;   bgdata   += ws;
            pov = ovrldata; ovrldata += wo;

            for (i = 0; i < copylen; i ++) {
                *pbg = (w0 * (*pbg) + w1 * (*pov)) >> 8;
                pov ++; pbg ++;
            }
        }
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
                x = static_cast<int>(pos_xf[0]);
                y = static_cast<int>(pos_xf[1]);
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

    if (Transformed) {
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        int x1, y1, x2, y2, x3, y3, x4, y4;
        double x, y;

        for (unsigned int j = 0; j < maxblobs; j ++) {
            if (blob->used) {
                x = blob->left - 1; y = blob->top - 1;
                x1 = static_cast<int>(x * m00 + y * m01 + m02);
                y1 = static_cast<int>(x * m10 + y * m11 + m12);

                x = blob->right + 1; y = blob->top - 1;
                x2 = static_cast<int>(x * m00 + y * m01 + m02);
                y2 = static_cast<int>(x * m10 + y * m11 + m12);

                x = blob->right + 1; y = blob->bottom + 1;
                x3 = static_cast<int>(x * m00 + y * m01 + m02);
                y3 = static_cast<int>(x * m10 + y * m11 + m12);

                x = blob->left - 1; y = blob->bottom + 1;
                x4 = static_cast<int>(x * m00 + y * m01 + m02);
                y4 = static_cast<int>(x * m10 + y * m11 + m12);

                svlDraw::Line(bgimage, VideoCh, x1, y1, x2, y2, 20, 128, 20);
                svlDraw::Line(bgimage, VideoCh, x2, y2, x3, y3, 20, 128, 20);
                svlDraw::Line(bgimage, VideoCh, x3, y3, x4, y4, 20, 128, 20);
                svlDraw::Line(bgimage, VideoCh, x4, y4, x1, y1, 20, 128, 20);
            }
            blob ++;
        }
    }
    else {
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
    const unsigned int tool_count = static_cast<unsigned int>(matrix.rows() / 3);
    const unsigned int vch_count  = static_cast<unsigned int>(matrix.cols() / 3);
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
                              static_cast<int>(vertices.Element(0, j - 1)),
                              static_cast<int>(vertices.Element(1, j - 1)),
                              static_cast<int>(vertices.Element(0, j)),
                              static_cast<int>(vertices.Element(1, j)),
                              Color.r, Color.g, Color.b);
            }
        }
        else {
            // Draw poly line
            for (j = 1; j < vertices.cols(); j ++) {
                svlDraw::Line(bgimage, VideoCh,
                              static_cast<int>(xf_vertices.Element(0, j - 1)),
                              static_cast<int>(xf_vertices.Element(1, j - 1)),
                              static_cast<int>(xf_vertices.Element(0, j)),
                              static_cast<int>(xf_vertices.Element(1, j)),
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
    Alpha(255),
    QuadMappingEnabled(false),
    QuadMappingSet(false)
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
    Alpha(alpha),
    QuadMappingEnabled(false),
    QuadMappingSet(false)
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

void svlOverlayStaticImage::SetEnableQuadMapping(bool enable)
{
    QuadMappingEnabled = enable;
}

void svlOverlayStaticImage::SetQuadMapping(vctInt2 ul, vctInt2 ur, vctInt2 ll, vctInt2 lr)
{
    QuadUL = ul;
    QuadUR = ur;
    QuadLL = ll;
    QuadLR = lr;
    QuadMappingSet = true;
}

void svlOverlayStaticImage::SetQuadMapping(int xul, int yul, int xur, int yur, int xll, int yll, int xlr, int ylr)
{
    QuadUL.Assign(xul, yul);
    QuadUR.Assign(xur, yur);
    QuadLL.Assign(xll, yll);
    QuadLR.Assign(xlr, ylr);
    QuadMappingSet = true;
}

vctInt2 svlOverlayStaticImage::GetPosition() const
{
    return Pos;
}

unsigned char svlOverlayStaticImage::GetAlpha() const
{
    return Alpha;
}

bool svlOverlayStaticImage::GetEnableQuadMapping() const
{
    return QuadMappingEnabled;
}

void svlOverlayStaticImage::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (!Buffer) return;

    // Skip drawing if fully transparent
    if (Alpha == 0) return;

    svlImageRGB* ovrlimage = Buffer->Pull(false);
    if (!ovrlimage) return;

    if (QuadMappingEnabled && QuadMappingSet) {
        svlSampleImageRGB _ovrlimage(false);
        _ovrlimage.SetMatrix(*ovrlimage);

        const int ulx = 0, uly = 0, llx = 0, ury = 0;
        const int urx = _ovrlimage.GetWidth(0)  - 1, lrx = _ovrlimage.GetWidth(0)  - 1;
        const int lly = _ovrlimage.GetHeight(0) - 1, lry = _ovrlimage.GetHeight(0) - 1;
        svlQuad quad_in, quad_out;

        quad_in.Assign(ulx, uly, urx, ury, lrx, lry, llx, lly);
        quad_out.Assign(QuadUL[0], QuadUL[1], QuadUR[0], QuadUR[1], QuadLR[0], QuadLR[1], QuadLL[0], QuadLL[1]);
        svlDraw::WarpQuad(&_ovrlimage, 0, quad_in, bgimage, VideoCh, quad_out, WarpInternals, Alpha);

        return;
    }

    if (Transformed) {
        svlSampleImageRGB _ovrlimage(false);
        _ovrlimage.SetMatrix(*ovrlimage);

        const int iulx = 0, iuly = 0, illx = 0, iury = 0;
        const int iurx = _ovrlimage.GetWidth(0)  - 1, ilrx = _ovrlimage.GetWidth(0)  - 1;
        const int illy = _ovrlimage.GetHeight(0) - 1, ilry = _ovrlimage.GetHeight(0) - 1;
        const int halfwidth  = _ovrlimage.GetWidth(0)  / 2;
        const int halfheight = _ovrlimage.GetHeight(0) / 2;
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        double x, y;

        x = iulx - halfwidth; y = iuly - halfheight;
        const int oulx = static_cast<int>(x * m00 + y * m01 + m02);
        const int ouly = static_cast<int>(x * m10 + y * m11 + m12);

        x = iurx - halfwidth; y = iury - halfheight;
        const int ourx = static_cast<int>(x * m00 + y * m01 + m02);
        const int oury = static_cast<int>(x * m10 + y * m11 + m12);

        x = illx - halfwidth; y = illy - halfheight;
        const int ollx = static_cast<int>(x * m00 + y * m01 + m02);
        const int olly = static_cast<int>(x * m10 + y * m11 + m12);

        x = ilrx - halfwidth; y = ilry - halfheight;
        const int olrx = static_cast<int>(x * m00 + y * m01 + m02);
        const int olry = static_cast<int>(x * m10 + y * m11 + m12);

        svlQuad quad_in, quad_out;

        quad_in.Assign(iulx, iuly, iurx, iury, ilrx, ilry, illx, illy);
        quad_out.Assign(oulx, ouly, ourx, oury, olrx, olry, ollx, olly);
        svlDraw::WarpQuad(&_ovrlimage, 0, quad_in, bgimage, VideoCh, quad_out, WarpInternals, Alpha);

        return;
    }

    int i, ws, hs, wo, ho, xs, ys, xo, yo, copylen, linecount;

    // Prepare for data copy
    ws = static_cast<int>(bgimage->GetWidth(VideoCh) * bgimage->GetBPP());
    hs = static_cast<int>(bgimage->GetHeight(VideoCh));
    wo = static_cast<int>(ovrlimage->cols());
    ho = static_cast<int>(ovrlimage->rows());

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
    unsigned char *ovrldata = ovrlimage->Pointer() + (yo * wo) + xo;

    if (Alpha == 255) {
        for (i = 0; i < linecount; i ++) {
            memcpy(bgdata, ovrldata, copylen);
            bgdata += ws;
            ovrldata += wo;
        }
    }
    else {
        const unsigned int w1 = Alpha + 1;
        const unsigned int w0 = 256 - w1;
        unsigned char *pbg, *pov;

        for (int j = 0; j < linecount; j ++) {
            pbg = bgdata;   bgdata   += ws;
            pov = ovrldata; ovrldata += wo;

            for (i = 0; i < copylen; i ++) {
                *pbg = (w0 * (*pbg) + w1 * (*pov)) >> 8;
                pov ++; pbg ++;
            }
        }
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
    svlRect _rect;

    if (Transformed) {
        _rect.left   = static_cast<int>(Transform.Element(0, 0) * Rect.left +
                                        Transform.Element(0, 1) * Rect.top +
                                        Transform.Element(0, 2));
        _rect.top  = static_cast<int>(Transform.Element(1, 0) * Rect.left +
                                      Transform.Element(1, 1) * Rect.top +
                                      Transform.Element(1, 2));
        _rect.right  = _rect.left + (Rect.right - Rect.left);
        _rect.bottom = _rect.top  + (Rect.bottom - Rect.top);
    }
    else {
        _rect.Assign(Rect);
    }

    if (FontChanged) {
        CvSize size;
        cvInitFont(&Font, CV_FONT_HERSHEY_PLAIN, FontSize, FontSize, 0, 1, 4);
        cvGetTextSize("|", &Font, &size, &Baseline);
        Baseline ++;
        FontChanged = false;
    }

    if (Background) svlDraw::Rectangle(bgimage, VideoCh, _rect, BGColor);

    CvRect cvrect;
    cvrect.x      = _rect.left;
    cvrect.width  = _rect.right - _rect.left;
    cvrect.y      = _rect.top + 1;
    cvrect.height = _rect.bottom - _rect.top + 1;

    const int w = static_cast<int>(bgimage->GetWidth(VideoCh));
    const int h = static_cast<int>(bgimage->GetHeight(VideoCh));
    if (cvrect.x     <  0 || cvrect.y      < 0  ||
        cvrect.width <  0 || cvrect.height < 0  ||
        (cvrect.x + cvrect.width)  >= w ||
        (cvrect.y + cvrect.height) >= h) return;

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
    Fill(true),
    DrawInternals(0)
{
}

svlOverlayStaticRect::svlOverlayStaticRect(unsigned int videoch,
                                           bool visible,
                                           const svlRect & rect,
                                           const svlRGB & color,
                                           bool fill) :
    svlOverlay(videoch, visible),
    Rect(rect),
    Color(color),
    Fill(fill),
    DrawInternals(0)
{
}

svlOverlayStaticRect::~svlOverlayStaticRect()
{
    if (DrawInternals) delete DrawInternals;
}

void svlOverlayStaticRect::SetRect(const svlRect & rect)
{
    Rect = rect;
}

void svlOverlayStaticRect::SetRect(int left, int top, int right, int bottom)
{
    Rect.Assign(left, top, right, bottom);
}

void svlOverlayStaticRect::SetColor(const svlRGB & color)
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
    if (Transformed) {
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        int x1, y1, x2, y2, x3, y3, x4, y4;
        double x, y;

        x = Rect.left; y = Rect.top;
        x1 = static_cast<int>(x * m00 + y * m01 + m02);
        y1 = static_cast<int>(x * m10 + y * m11 + m12);

        x = Rect.right; y = Rect.top;
        x2 = static_cast<int>(x * m00 + y * m01 + m02);
        y2 = static_cast<int>(x * m10 + y * m11 + m12);

        x = Rect.right; y = Rect.bottom;
        x3 = static_cast<int>(x * m00 + y * m01 + m02);
        y3 = static_cast<int>(x * m10 + y * m11 + m12);

        x = Rect.left; y = Rect.bottom;
        x4 = static_cast<int>(x * m00 + y * m01 + m02);
        y4 = static_cast<int>(x * m10 + y * m11 + m12);

        if (!Fill) {
            svlDraw::Line(bgimage, VideoCh, x1, y1, x2, y2, Color.r, Color.g, Color.b);
            svlDraw::Line(bgimage, VideoCh, x2, y2, x3, y3, Color.r, Color.g, Color.b);
            svlDraw::Line(bgimage, VideoCh, x3, y3, x4, y4, Color.r, Color.g, Color.b);
            svlDraw::Line(bgimage, VideoCh, x4, y4, x1, y1, Color.r, Color.g, Color.b);
        }
        else {
            if (!DrawInternals) DrawInternals = new svlDraw::Internals;
            svlDraw::Quad(bgimage, VideoCh, x1, y1, x2, y2, x3, y3, x4, y4, Color, DrawInternals[0]);
        }
    }
    else {
        svlDraw::Rectangle(bgimage, VideoCh, Rect, Color, Fill);
    }
}


/*************************************/
/*** svlOverlayStaticEllipse class ***/
/*************************************/

svlOverlayStaticEllipse::svlOverlayStaticEllipse() :
    svlOverlay(),
    Ellipse(0, 0, 0, 0, 0.0),
    Thickness(1),
    Color(255, 255, 255),
    Fill(true)
{
}

svlOverlayStaticEllipse::svlOverlayStaticEllipse(unsigned int videoch,
                                                 bool visible,
                                                 const svlEllipse & ellipse,
                                                 const svlRGB & color,
                                                 bool fill) :
    svlOverlay(videoch, visible),
    Ellipse(ellipse),
    Thickness(1),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticEllipse::svlOverlayStaticEllipse(unsigned int videoch,
                                                 bool visible,
                                                 const svlPoint2D & center,
                                                 int radius_horiz,
                                                 int radius_vert,
                                                 double angle,
                                                 const svlRGB & color,
                                                 bool fill) :
    svlOverlay(videoch, visible),
    Ellipse(center.x, center.y, radius_horiz, radius_vert, angle),
    Thickness(1),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticEllipse::svlOverlayStaticEllipse(unsigned int videoch,
                                                 bool visible,
                                                 const svlPoint2D & center,
                                                 int radius,
                                                 const svlRGB & color,
                                                 bool fill) :
    svlOverlay(videoch, visible),
    Ellipse(center.x, center.y, radius, radius, 0.0),
    Thickness(1),
    Color(color),
    Fill(fill)
{
}

svlOverlayStaticEllipse::~svlOverlayStaticEllipse()
{
}

void svlOverlayStaticEllipse::SetEllipse(const svlEllipse & ellipse)
{
    Ellipse = ellipse;
}

void svlOverlayStaticEllipse::SetCenter(const svlPoint2D & center)
{
    Ellipse.cx = center.x;
    Ellipse.cy = center.y;
}

void svlOverlayStaticEllipse::SetRadius(const int radius_horiz, const int radius_vert)
{
    Ellipse.rx = radius_horiz;
    Ellipse.ry = radius_vert;
}

void svlOverlayStaticEllipse::SetRadius(const int radius)
{
    Ellipse.rx = Ellipse.ry = radius;
}

void svlOverlayStaticEllipse::SetAngle(const double angle)
{
    Ellipse.angle = angle;
}

void svlOverlayStaticEllipse::SetThickness(unsigned int thickness) 
{
    Thickness = thickness;
}

void svlOverlayStaticEllipse::SetColor(const svlRGB & color)
{
    Color = color;
}

void svlOverlayStaticEllipse::SetFill(bool fill)
{
    Fill = fill;
}

svlEllipse svlOverlayStaticEllipse::GetEllipse() const
{
    return Ellipse;
}

svlPoint2D svlOverlayStaticEllipse::GetCenter() const
{
    return svlPoint2D(Ellipse.cx, Ellipse.cy);
}

void svlOverlayStaticEllipse::GetRadius(int & radius_horiz, int & radius_vert) const
{
    radius_horiz = Ellipse.rx;
    radius_vert  = Ellipse.ry;
}

double svlOverlayStaticEllipse::GetAngle() const
{
    return Ellipse.angle;
}

unsigned int svlOverlayStaticEllipse::GetThickness()
{
    return Thickness;
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
    int cx, cy, rx, ry;
    double angle;

    if (Transformed) {
        // Calculate translation
        cx = static_cast<int>(Transform.Element(0, 0) * Ellipse.cx +
                              Transform.Element(0, 1) * Ellipse.cy +
                              Transform.Element(0, 2));
        cy = static_cast<int>(Transform.Element(1, 0) * Ellipse.cx +
                              Transform.Element(1, 1) * Ellipse.cy +
                              Transform.Element(1, 2));
        // Calculate scale from rotation matrix norm
        vctFixedSizeMatrixRef<double, 2, 2, 3, 1> rot(Transform.Pointer());
        double norm = rot.Norm();
        rx = static_cast<int>(norm * Ellipse.rx);
        ry = static_cast<int>(norm * Ellipse.ry);
        // Calculate angle
        angle = Ellipse.angle + acos(Transform.Element(0, 0));
    }
    else {
        cx = Ellipse.cx; cy = Ellipse.cy;
        rx = Ellipse.rx; ry = Ellipse.ry;
        angle = Ellipse.angle;
    }

    svlDraw::Ellipse(bgimage, VideoCh,
                     cx, cy, rx, ry,
                     Color,
                     0.0, 360.0,
                     angle,
                     (Fill ? -1 : Thickness));
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
    int x1, y1, x2, y2, x3, y3;

    if (Transformed) {
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);

        x1 = static_cast<int>(m00 * Corner1.x + m01 * Corner1.y + m02);
        y1 = static_cast<int>(m10 * Corner1.x + m11 * Corner1.y + m12);
        x2 = static_cast<int>(m00 * Corner2.x + m01 * Corner2.y + m02);
        y2 = static_cast<int>(m10 * Corner2.x + m11 * Corner2.y + m12);
        x3 = static_cast<int>(m00 * Corner3.x + m01 * Corner3.y + m02);
        y3 = static_cast<int>(m10 * Corner3.x + m11 * Corner3.y + m12);
    }
    else {
        x1 = Corner1.x; y1 = Corner1.y;
        x2 = Corner2.x; y2 = Corner2.y;
        x3 = Corner3.x; y3 = Corner3.y;
    }

    if (Fill) {
        svlDraw::Triangle(bgimage, VideoCh, x1, y1, x2, y2, x3, y3, Color, DrawInternals[0]);
    }
    else {
        svlDraw::Line(bgimage, VideoCh, x1, y1, x2, y2, Color.r, Color.g, Color.b);
        svlDraw::Line(bgimage, VideoCh, x2, y2, x3, y3, Color.r, Color.g, Color.b);
        svlDraw::Line(bgimage, VideoCh, x3, y3, x1, y1, Color.r, Color.g, Color.b);
    }
}


/****************************/
/*** svlOverlayPoly class ***/
/****************************/

svlOverlayPoly::svlOverlayPoly() :
    svlOverlay(),
    svlOverlayInput(),
    Color(255, 255, 255),
    Thickness(1)
{
}

svlOverlayPoly::svlOverlayPoly(unsigned int videoch,
                               bool visible,
                               const std::string & inputname,
                               svlRGB color) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    Color(color),
    Thickness(1)
{
}

svlOverlayPoly::svlOverlayPoly(unsigned int videoch,
                               bool visible,
                               const std::string & inputname,
                               svlRGB color,
                               unsigned int thickness) :
    svlOverlay(videoch, visible),
    svlOverlayInput(inputname),
    Color(color),
    Thickness(thickness)
{
}

svlOverlayPoly::~svlOverlayPoly()
{
}

void svlOverlayPoly::SetColor(svlRGB color)
{
    Color = color;
}

void svlOverlayPoly::SetThickness(unsigned int thickness)
{
    Thickness = thickness;
}

svlRGB svlOverlayPoly::GetColor() const
{
    return Color;
}

unsigned int svlOverlayPoly::GetThickness() const
{
    return Thickness;
}

bool svlOverlayPoly::IsInputTypeValid(svlStreamType inputtype)
{
    if (inputtype == svlTypeMatrixInt32) return true;
    return false;
}

void svlOverlayPoly::DrawInternal(svlSampleImage* bgimage, svlSample* input)
{
    // Get sample from input
    svlSampleMatrixInt32* matrix_smpl = dynamic_cast<svlSampleMatrixInt32*>(input);
    if (!matrix_smpl) return;

    vctDynamicMatrix<int>& matrix = matrix_smpl->GetDynamicMatrixRef();
    const int length = static_cast<int>(matrix.cols()) - 1;
    if (length < 1) return;
    int start = matrix.Element(0, length);
    if (start < 0 || start >= length) start = 0;
    if (Poly.size() != static_cast<unsigned int>(length)) Poly.SetSize(length);

    if (Transformed) {
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        double x, y;

        for (int i = 0; i < length; i ++) {
            x = matrix.Element(0, i);
            y = matrix.Element(1, i);
            Poly[i].x = static_cast<int>(x * m00 + y * m01 + m02);
            Poly[i].y = static_cast<int>(x * m10 + y * m11 + m12);
        }
    }
    else {
        for (int i = 0; i < length; i ++) {
            Poly[i].x = matrix.Element(0, i);
            Poly[i].y = matrix.Element(1, i);
        }
    }

    svlDraw::Poly(bgimage, VideoCh, Poly, Color, Thickness, start);
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

void svlOverlayStaticPoly::SetPoints()
{
    CS.Enter();
        Poly.SetSize(0);
    CS.Leave();
}

void svlOverlayStaticPoly::SetPoints(const TypeRef points)
{
    CS.Enter();
        Poly.ForceAssign(points);
    CS.Leave();
}

void svlOverlayStaticPoly::SetPoints(const TypeRef points, unsigned int start)
{
    CS.Enter();
        Poly.ForceAssign(points);
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
        unsigned int size = static_cast<unsigned int>(Poly.size());
        Poly.resize(size + 1);
        Poly[size] = point;
    CS.Leave();
    return size;
}

unsigned int svlOverlayStaticPoly::AddPoint(int x, int y)
{
    CS.Enter();
        unsigned int size = static_cast<unsigned int>(Poly.size());
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
            const unsigned int length = static_cast<unsigned int>(Poly.size());
            if (PolyXF.size() != length) PolyXF.SetSize(length);

            const double m00 = Transform.Element(0, 0);
            const double m01 = Transform.Element(0, 1);
            const double m02 = Transform.Element(0, 2);
            const double m10 = Transform.Element(1, 0);
            const double m11 = Transform.Element(1, 1);
            const double m12 = Transform.Element(1, 2);
            double x, y;

            for (unsigned int i = 0; i < length; i ++) {
                x = Poly[i].x;
                y = Poly[i].y;
                PolyXF[i].x = static_cast<int>(x * m00 + y * m01 + m02);
                PolyXF[i].y = static_cast<int>(x * m10 + y * m11 + m12);
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
    BorderColor(0, 0, 0),
    DrawInternals(0)
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
    BorderWidth(static_cast<int>(borderwidth)),
    BorderColor(bordercolor),
    DrawInternals(0)
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
    BorderColor(0, 0, 0),
    DrawInternals(0)
{
    Rect.Normalize();
}

svlOverlayStaticBar::~svlOverlayStaticBar()
{
    if (DrawInternals) delete DrawInternals;
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
    BorderWidth = static_cast<int>(pixels);
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
    return static_cast<unsigned int>(BorderWidth);
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
    if (Transformed) {
        const double m00 = Transform.Element(0, 0);
        const double m01 = Transform.Element(0, 1);
        const double m02 = Transform.Element(0, 2);
        const double m10 = Transform.Element(1, 0);
        const double m11 = Transform.Element(1, 1);
        const double m12 = Transform.Element(1, 2);
        int x11, y11, x12, y12, x13, y13, x14, y14;
        int x21, y21, x22, y22, x23, y23, x24, y24;
        double x, y;

        if (BorderWidth > 0) {
            x = Rect.left - BorderWidth; y = Rect.top - BorderWidth;
            x11 = static_cast<int>(x * m00 + y * m01 + m02);
            y11 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right + BorderWidth; y = Rect.top - BorderWidth;
            x12 = static_cast<int>(x * m00 + y * m01 + m02);
            y12 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right + BorderWidth; y = Rect.bottom + BorderWidth;
            x13 = static_cast<int>(x * m00 + y * m01 + m02);
            y13 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.left - BorderWidth; y = Rect.bottom + BorderWidth;
            x14 = static_cast<int>(x * m00 + y * m01 + m02);
            y14 = static_cast<int>(x * m10 + y * m11 + m12);

            if (!DrawInternals) DrawInternals = new svlDraw::Internals;
            svlDraw::Quad(bgimage, VideoCh, x11, y11, x12, y12, x13, y13, x14, y14, BorderColor, DrawInternals[0]);
        }

        double range;
        if (Range[0] != Range[1]) range = Range[1] - Range[0];
        else range = 100.0;
        double position = (Value - Range[0]) / range;
        if (position < 0.0) position = 0.0;
        else if (position > 1.0) position = 1.0;

        if (Vertical) {
            x = Rect.left; y = Rect.bottom - static_cast<int>(static_cast<double>(Rect.bottom - Rect.top) * position);
            x11 = static_cast<int>(x * m00 + y * m01 + m02);
            y11 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right; // y is unchanged
            x12 = static_cast<int>(x * m00 + y * m01 + m02);
            y12 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right; y = Rect.bottom;
            x13 = static_cast<int>(x * m00 + y * m01 + m02);
            y13 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.left; y = Rect.bottom;
            x14 = static_cast<int>(x * m00 + y * m01 + m02);
            y14 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.left; y = Rect.top;
            x21 = static_cast<int>(x * m00 + y * m01 + m02);
            y21 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right; y = Rect.top;
            x22 = static_cast<int>(x * m00 + y * m01 + m02);
            y22 = static_cast<int>(x * m10 + y * m11 + m12);

            x23 = x12; y23 = y12;
            x24 = x11; y24 = y11;
        }
        else {
            x = Rect.left; y = Rect.top;
            x11 = static_cast<int>(x * m00 + y * m01 + m02);
            y11 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.left + static_cast<int>(static_cast<double>(Rect.right - Rect.left) * position); y = Rect.top;
            x12 = static_cast<int>(x * m00 + y * m01 + m02);
            y12 = static_cast<int>(x * m10 + y * m11 + m12);

            y = Rect.bottom; // x is unchanged
            x13 = static_cast<int>(x * m00 + y * m01 + m02);
            y13 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.left; y = Rect.bottom;
            x14 = static_cast<int>(x * m00 + y * m01 + m02);
            y14 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right; y = Rect.top;
            x22 = static_cast<int>(x * m00 + y * m01 + m02);
            y22 = static_cast<int>(x * m10 + y * m11 + m12);

            x = Rect.right; y = Rect.bottom;
            x23 = static_cast<int>(x * m00 + y * m01 + m02);
            y23 = static_cast<int>(x * m10 + y * m11 + m12);

            x21 = x12; y21 = y12;
            x24 = x13; y24 = y13;
        }

        if (!DrawInternals) DrawInternals = new svlDraw::Internals;
        svlDraw::Quad(bgimage, VideoCh, x11, y11, x12, y12, x13, y13, x14, y14, Color,   DrawInternals[0]);
        svlDraw::Quad(bgimage, VideoCh, x21, y21, x22, y22, x23, y23, x24, y24, BGColor, DrawInternals[0]);
    }
    else {
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

