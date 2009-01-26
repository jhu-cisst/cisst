/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ftImageBMP.cpp,v 1.4 2008/10/22 14:51:58 vagvoba Exp $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "ftImageBMP.h"

/*************************************/
/*** ftImageBMP class ******************/
/*************************************/

ftImageBMP::ftImageBMP() : svlImageFile()
{
    FileHandle = 0;
    BufferPadded = false;
    BufferSize = 0;
    LineSize = 0;
    Padding = 0;
    Height = 0;
    UpsideDown = false;
}

ftImageBMP::~ftImageBMP()
{
    Close();
}

svlImageFile* ftImageBMP::GetInstance()
{
    ftImageBMP* instance = new ftImageBMP();
    return instance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImageBMP::ExtractDimensions
// Description:
//             - all color spaces supported
//             - opens file, extracts dimensions from bitmap info header, and closes file
int ftImageBMP::ExtractDimensions(const char* filepath, int & width, int & height)
{
    FILE* hfile;
    svlBMPFileHeader fileheader;
    svlDIBHeader dibheader;

    hfile = fopen(filepath, "rb");
    if (!hfile ||
        // reading file header
        fread(&fileheader, sizeof(svlBMPFileHeader), 1, hfile) < 1 ||
        // checking for BMP signature
        fileheader.bfType != 0x4D42 ||
        // reading bitmap info header
        fread(&dibheader, sizeof(svlDIBHeader), 1, hfile) < 1 ||
        // checking header integrity
        dibheader.biSize != (sizeof(svlDIBHeader))) return SVL_FAIL;
    fclose(hfile);

    width = dibheader.biWidth;
    height = dibheader.biHeight;
    if (height < 0) height = -height;

    return SVL_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImageBMP::Open
// Limitations:
//             - only 24bpp bitmaps are supported
//             - maximal image size: IMAGEBMP_MAX_DIMENISION x IMAGEBMP_MAX_DIMENISION (could be anything)
int ftImageBMP::Open(const char* filepath, svlImageProperties& properties)
{
    Close();

    int width, height;

    FileHandle = fopen(filepath, "rb");
    if (!FileHandle) goto labError;

    if (fread(&FileHeader, sizeof(svlBMPFileHeader), 1, FileHandle) < 1) goto labError;
    if (FileHeader.bfType != 0x4D42) goto labError;

    if (fread(&DIBHeader, sizeof(svlDIBHeader), 1, FileHandle) < 1) goto labError;
    if (DIBHeader.biSize != (sizeof(svlDIBHeader)) ||
        DIBHeader.biBitCount != 24 ||
        DIBHeader.biCompression != 0 ||
        DIBHeader.biClrUsed != 0)  goto labError;
    width = DIBHeader.biWidth;
    height = DIBHeader.biHeight;
    if (height < 0) {
        height = -height;
        UpsideDown = false;
    }
    else {
        UpsideDown = true;
    }
    if (width < 1 ||
        width > IMAGEBMP_MAX_DIMENISION ||
        height < 1 ||
        height > IMAGEBMP_MAX_DIMENISION) goto labError;

    LineSize = width * 3;
    Padding = (4 - (LineSize % 4)) % 4;
    LineSize += Padding;

    properties.DataType = svlTypeImageRGB;
    BufferPadded = properties.Padding;
    if (BufferPadded) {
        BufferSize = properties.DataSize = LineSize * height;
    }
    else {
        BufferSize = properties.DataSize = width * height * 3;
    }
    properties.Width = width;
    Height = properties.Height = height;

    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

int ftImageBMP::ReadAndClose(unsigned char* buffer, unsigned int size)
{
    if (!FileHandle || !buffer || size < BufferSize) goto labError;

    unsigned int i, lsize;
    unsigned char *chptr;

    // reading data
    if (BufferPadded || Padding == 0) {
        if (UpsideDown) {
            chptr = buffer + (Height - 1) * LineSize;
            for (i = 0; i < Height; i ++) {
                if (fread(chptr, LineSize, 1, FileHandle) < 1) goto labError;
                chptr -= LineSize;
            }
        }
        else {
            if (fread(buffer, BufferSize, 1, FileHandle) < 1) goto labError;
        }
    }
    else {
        lsize = LineSize - Padding;
        if (UpsideDown) {
            chptr = buffer + (Height - 1) * lsize;
            for (i = 0; i < Height; i ++) {
                if (fread(chptr, lsize, 1, FileHandle) < 1) goto labError;
                if (fseek(FileHandle, Padding, SEEK_CUR) != 0) goto labError;
                chptr -= lsize;
            }
        }
        else {
            for (i = 0; i < Height; i ++) {
                if (fread(buffer, lsize, 1, FileHandle) < 1) goto labError;
                if (fseek(FileHandle, Padding, SEEK_CUR) != 0) goto labError;
                buffer += lsize;
            }
        }
    }

    Close();
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ftImageBMP::CreateBMP
// Limitations:
//             - only 24bpp bitmaps are supported
//             - maximal image size: IMAGEBMP_MAX_DIMENISION x IMAGEBMP_MAX_DIMENISION (could be anything)
int ftImageBMP::Create(const char* filepath, svlImageProperties* properties, unsigned char* buffer)
{
    int width, height, linesize, padding;
    unsigned int pad = 0;
    FILE* fh = 0;

    if (!filepath || !properties || !buffer) goto labError;

    width = properties->Width;
    height = properties->Height;

    if (properties->DataType != svlTypeImageRGB ||
        width < 1 ||
        width > IMAGEBMP_MAX_DIMENISION ||
        height < 1 ||
        height > IMAGEBMP_MAX_DIMENISION) goto labError;

    linesize = width * 3;
    padding = (4 - (linesize % 4)) % 4;
    linesize += padding;

    if (properties->Padding) {
        if (properties->DataSize != static_cast<unsigned int>(linesize * height)) goto labError;
    }
    else {
        if (properties->DataSize != static_cast<unsigned int>(width * height * 3)) goto labError;
    }

    // creating headers
	DIBHeader.biSize = sizeof(svlDIBHeader);
    DIBHeader.biWidth = width;
    DIBHeader.biHeight = -1 * height;
	DIBHeader.biPlanes = 1;
	DIBHeader.biBitCount = 24;
	DIBHeader.biCompression = 0;
	DIBHeader.biSizeImage = 0;
	DIBHeader.biXPelsPerMeter = 0;
	DIBHeader.biYPelsPerMeter = 0;
	DIBHeader.biClrUsed = 0;
	DIBHeader.biClrImportant = 0;

    FileHeader.bfType = 0x4D42;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    FileHeader.bfOffBits = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
    FileHeader.bfSize = FileHeader.bfOffBits + linesize * height;

    // opening file
    fh = fopen(filepath, "wb");
    if (!fh) goto labError;

    // writing headers
    if (fwrite(&FileHeader, sizeof(svlBMPFileHeader), 1, fh) < 1) goto labError;
    if (fwrite(&DIBHeader, sizeof(svlDIBHeader), 1, fh) < 1) goto labError;

    // writing data
    if (properties->Padding || padding == 0) {
        if (fwrite(buffer, properties->DataSize, 1, fh) < 1) goto labError;
    }
    else {
        width *= 3;
        for (int i = 0; i < height; i ++) {
            if (fwrite(buffer, width, 1, fh) < 1) goto labError;
            buffer += width;
            if (fwrite(&pad, padding, 1, fh) < 1) goto labError;
        }
    }

    fclose(fh);
    return SVL_OK;

labError:
    if (fh) fclose(fh);
    return SVL_FAIL;
}

void ftImageBMP::Close()
{
    if (FileHandle) fclose(FileHandle);
    FileHandle = 0;
    BufferPadded = false;
    BufferSize = 0;
    LineSize = 0;
    Padding = 0;
    Height = 0;
}

