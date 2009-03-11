/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Balazs Vagvolgyi
  Created on:	2009-03-02

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3ImagePlane.h>
#include <vtkAssembly.h>

#define USE_BGR


CMN_IMPLEMENT_SERVICES(ui3ImagePlane);

ui3ImagePlane::ui3ImagePlane(ui3Manager * manager):
    ui3VisibleObject(manager),
    Texture(0),
    ImageData(0),
    PlaneSrc(0),
    Mapper(0),
    Actor(0),
    BitmapWidth(0),
    BitmapHeight(0),
    TextureWidth(0),
    TextureHeight(0),
    TextureBuffer(0),
    PhysicalWidth(1.0),
    PhysicalHeight(1.0),
    PhysicalPositionRelativeToPivot(0.0, 0.0, 0.0)
{
}


ui3ImagePlane::~ui3ImagePlane()
{
    if (this->Texture)   this->Texture->Delete();
    if (this->ImageData) this->ImageData->Delete();
    if (this->PlaneSrc)  this->PlaneSrc->Delete();
    if (this->Mapper)    this->Mapper->Delete();
    if (this->Actor)     this->Actor->Delete();
}


bool ui3ImagePlane::CreateVTKObjects()
{
    if (BitmapWidth  < 1 || TextureWidth  < BitmapWidth  ||
        BitmapHeight < 1 || TextureHeight < BitmapHeight) {
        // Image size should be initialized before creating VTK objects
        return false;
    }

    // Create VTK objects
    this->Texture = vtkTexture::New();
    CMN_ASSERT(this->Texture);

    this->ImageData = vtkImageData::New();
    CMN_ASSERT(this->ImageData);

    this->PlaneSrc = vtkPlaneSource::New();
    CMN_ASSERT(this->PlaneSrc);

    this->Mapper = vtkPolyDataMapper::New();
    CMN_ASSERT(this->Mapper);

    this->Actor = vtkActor::New();
    CMN_ASSERT(this->Actor);


    // Create image object
    this->ImageData->SetDimensions(this->TextureWidth, this->TextureHeight, 1);
    this->ImageData->SetScalarType(VTK_UNSIGNED_CHAR);
    this->ImageData->SetOrigin(0.0, 0.0, 0.0);
    this->ImageData->SetSpacing(1.0, 1.0, 1.0);
    this->ImageData->SetNumberOfScalarComponents(4);
    this->ImageData->AllocateScalars();
    this->TextureBuffer = (unsigned char*)this->ImageData->GetScalarPointer();

    // Make whole image opaque by default
    this->SetAlpha(128);

    // Create texture object
    this->Texture->SetInput(this->ImageData);
    this->Texture->InterpolateOn();

    // Create image plane
    this->PlaneSrc->SetOrigin(this->PhysicalPositionRelativeToPivot.X(),
                                this->PhysicalPositionRelativeToPivot.Y(),
                                  this->PhysicalPositionRelativeToPivot.Z());
    this->PlaneSrc->SetPoint1(this->PhysicalPositionRelativeToPivot.X() + this->PhysicalWidth * ((double)this->TextureWidth / this->BitmapWidth),
                                this->PhysicalPositionRelativeToPivot.Y(),
                                  this->PhysicalPositionRelativeToPivot.Z());
    this->PlaneSrc->SetPoint2(this->PhysicalPositionRelativeToPivot.X(),
                                this->PhysicalPositionRelativeToPivot.Y() - this->PhysicalHeight * ((double)this->TextureHeight / this->BitmapHeight),
                                  this->PhysicalPositionRelativeToPivot.Z());
    this->PlaneSrc->SetXResolution(1);
    this->PlaneSrc->SetYResolution(1);

    // Create mapper
    this->Mapper->SetInput(this->PlaneSrc->GetOutput());

    // Create actor
    this->Actor->SetMapper(this->Mapper);

    // Map texture onto the plane
    this->Actor->SetTexture(this->Texture);
    this->Actor->GetProperty()->SetOpacity(1.0);

    this->Assembly->AddPart(this->Actor);

    return true;
}


bool ui3ImagePlane::SetBitmapSize(unsigned int width, unsigned int height)
{
    if (width < 1 || height < 1) return false;

    this->BitmapWidth = width;
    this->BitmapHeight = height;

    unsigned int i, c;

    // Compute texture width
    c = 0;
    i = width;
    while (i > 0) {
        i >>= 1;
        c ++;
    }
    i = 1 << (c - 1);
    if (width == i) this->TextureWidth = width;
    else this->TextureWidth = i << 1;

    // Compute texture height
    c = 0;
    i = height;
    while (i > 0) {
        i >>= 1;
        c ++;
    }
    i = 1 << (c - 1);
    if (height == i) this->TextureHeight = height;
    else this->TextureHeight = i << 1;

    return true;
}


bool ui3ImagePlane::SetPhysicalSize(double width, double height)
{
    if (width <= 0.0 || height <= 0.0) return false;
    this->PhysicalWidth = width;
    this->PhysicalHeight = height;
    return true;
}


void ui3ImagePlane::SetPhysicalPositionRelativeToPivot(vct3 position)
{
    this->PhysicalPositionRelativeToPivot.Assign(position);
}


void ui3ImagePlane::SetAlpha(const unsigned char alpha)
{
    const unsigned int  bmpwidth = this->BitmapWidth;
    const unsigned int  bmpheight = this->BitmapHeight;
    const unsigned int  texwidth = this->TextureWidth;
    const unsigned int  rightborder = texwidth - bmpwidth;
    const unsigned int  bottomborder = this->TextureHeight - bmpheight;

    unsigned char       *texture = this->TextureBuffer + 3; // alpha channel
    unsigned int        i, j;

    for (j = 0; j < bmpheight; j ++) {

        // Set alpha of bitmap area
        for (i = 0; i < bmpwidth; i ++) {
            *texture = alpha;
            texture += 4;
        }

        // Set texture area right of the bitmap fully transparent
        for (i = 0; i < rightborder; i ++) {
            *texture = 0;
            texture += 4;
        }
    }

    // Set texture area under the bitmap fully transparent
    for (j = 0; j < bottomborder; j ++) {
        for (i = 0; i < texwidth; i ++) {
            *texture = 0;
            texture += 4;
        }
    }
}


void ui3ImagePlane::SetImage(svlSampleImageBase* image, unsigned int channel)
{
    if (this->TextureBuffer &&
        image &&
        image->GetVideoChannels() > channel &&
        image->GetWidth(channel) == this->BitmapWidth &&
        image->GetHeight(channel) == this->BitmapHeight) {

        // Copy RGB image to RGBA texture buffer
        const unsigned int  bmpwidth = this->BitmapWidth;
        const unsigned int  bmpheight = this->BitmapHeight;
        const unsigned int  rightborder = (this->TextureWidth - bmpwidth) << 2;

        unsigned char *imagebuf = reinterpret_cast<unsigned char*>(image->GetPointer(channel));
        unsigned char *texture = this->TextureBuffer;
        unsigned int i, j;
#ifdef USE_BGR
        unsigned char r, g, b;
#endif

        for (j = 0; j < bmpheight; j ++) {
            for (i = 0; i < bmpwidth; i ++) {
#ifdef USE_BGR
                r = *imagebuf; imagebuf ++;
                g = *imagebuf; imagebuf ++;
                b = *imagebuf; imagebuf ++;
                *texture = b; texture ++;
                *texture = g; texture ++;
                *texture = r; texture += 2;
#else
                *texture = *imagebuf; imagebuf ++; texture ++;
                *texture = *imagebuf; imagebuf ++; texture ++;
                *texture = *imagebuf; imagebuf ++; texture += 2;
#endif
            }
            texture += rightborder;
        }

        // Signal VTK that the texture has been modified
        this->ImageData->Modified();
    }
}

