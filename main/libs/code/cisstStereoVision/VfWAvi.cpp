/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: VfWAvi.cpp 644 2009-08-10 14:06:33Z bvagvol1 $
  
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

#include "VfWAvi.h"

#define AVI_CLOSED		0
#define AVI_READING		1
#define AVI_WRITING		2


/*************************************/
/*** CVfWAvi class *******************/
/*************************************/

CVfWAvi::CVfWAvi()
{
    InitConversionTables();

    WriteBuffer = 0;
    ResetStates();
    memset(&CompressionOptions, 0, sizeof(AVICOMPRESSOPTIONS));
}

CVfWAvi::~CVfWAvi()
{
    Close();
}

int CVfWAvi::InitPlaying(LPCTSTR path)
{
    if (FileIsOpened != AVI_CLOSED) return 0;
    if (path == 0) return 0;

    strcpy(PathName, path);

    AVIFileInit();
    FileIsOpened = AVI_READING;

    if (AVIFileOpen(&PAviFile, PathName, OF_READ, 0) == 0) {
        if (AVIFileInfo(PAviFile, &FileInfo, sizeof(AVIFILEINFO)) == 0) {
            if (AVIFileGetStream(PAviFile, &PAviStream, streamtypeVIDEO, 0) == 0) {
                if (AVIStreamInfo(PAviStream, &StreamInfo, sizeof(AVISTREAMINFO)) == 0) {
                    PGetFrame = AVIStreamGetFrameOpen(PAviStream, 0);
                    if (PGetFrame != 0) {
                        unsigned char* temp = reinterpret_cast<unsigned char*>(AVIStreamGetFrame(PGetFrame, 0));
                        if (temp != 0) {
                            // video parameters
                            FirstFrame = StreamInfo.dwStart;
                            CurrentFrame = FirstFrame;
                            LastFrame = StreamInfo.dwLength;
                            Framerate = static_cast<double>(StreamInfo.dwRate) / StreamInfo.dwScale;
                            // determining correct image size and orientation
                            Width = reinterpret_cast<BITMAPINFOHEADER*>(temp)->biWidth;
                            Height = abs(reinterpret_cast<BITMAPINFOHEADER*>(temp)->biHeight);
                            if ((reinterpret_cast<BITMAPINFOHEADER*>(temp)->biBitCount == 24 ||
                                 reinterpret_cast<BITMAPINFOHEADER*>(temp)->biBitCount == 32) &&
                                reinterpret_cast<BITMAPINFOHEADER*>(temp)->biHeight > 0) {
                                UpsideDown = 1;
                            }
                            else {
                                UpsideDown = 0;
                            }
                            return 1;
                        }
                    }
                }
            }
        }
    }

    Close();
    return 0;
}

int CVfWAvi::InitCreation(LPCTSTR path, int width, int height, double fps)
{
	if (FileIsOpened != AVI_CLOSED) return 0;
	if (path == NULL) return 0;

    Framerate = fps;
	Width = width;
	Height = height;
    strcpy(PathName, path);

	DeleteFile(PathName);

	// Setting AVI parameters
	int bmihsize = sizeof(BITMAPINFOHEADER);
	unsigned char* tptr = new unsigned char[bmihsize];
    memset(tptr, 0, bmihsize * sizeof(unsigned char));
	BITMAPINFOHEADER* pbmih = reinterpret_cast<BITMAPINFOHEADER*>(tptr);
	pbmih->biSize = bmihsize;
	pbmih->biWidth = Width;
	pbmih->biHeight = Height;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = 24;
	pbmih->biClrUsed = 0;

	AVICOMPRESSOPTIONS* tCompr[1] = {&CompressionOptions};

	AVIFileInit();
	FileIsOpened = AVI_WRITING;

	if (AVIFileOpen(&PAviFile, PathName, OF_WRITE|OF_CREATE, NULL) == 0) {
		StreamInfo.fccType = streamtypeVIDEO;
		StreamInfo.fccHandler = 0;
		StreamInfo.dwScale = static_cast<DWORD>(1000000 / Framerate);
		StreamInfo.dwRate = 1000000;
		StreamInfo.dwSuggestedBufferSize = 1000000;
		StreamInfo.rcFrame.left = 0;
		StreamInfo.rcFrame.top = 0;
		StreamInfo.rcFrame.right = Width;
		StreamInfo.rcFrame.bottom = Height;

		if (AVIFileCreateStream(PAviFile, &PAviStream, &StreamInfo) == 0) {
			if (AVIMakeCompressedStream(&PAviCompressedStream, PAviStream,
										&CompressionOptions, NULL) == AVIERR_OK) {
				if (AVIStreamSetFormat(PAviCompressedStream, 0, pbmih,
									   pbmih->biSize) == 0) {
					delete [] tptr;

					WriteBufferLen = Width * Height * 3;
	                WriteBuffer = new unsigned char[WriteBufferLen];
                    memset(WriteBuffer, 0, WriteBufferLen * sizeof(unsigned char));

					FirstFrame = 0;
					LastFrame = 0;
					CurrentFrame = 0;

					return 1;
				}
			}
		}
	}

	Close();
	if (tptr) delete [] tptr;
	return 0;
}

void CVfWAvi::SetKeyFrameInterval(unsigned int interval)
{
    if (interval > 0) KeyFrames = interval;
    else KeyFrames = 1;
}

int CVfWAvi::CopyAVIFrame(unsigned char* dest, int datasize, int framenum)
{
    // Dest must be preallocated with appropriate size
    if (dest == 0 || datasize < (Width * Height * 3)) return 0;

    if (FileIsOpened == AVI_READING) {
        unsigned char* temp = reinterpret_cast<unsigned char*>(AVIStreamGetFrame(PGetFrame, framenum));
        if (temp == 0) return 0;

        CurrentFrame = framenum;
        DWORD size = Width * Height;
        BOOL ret = ConvertToTrueColor(temp, dest, size);
        if (UpsideDown) TopDownImage(dest, Width, Height);

        CurrentFrame ++;
        if (CurrentFrame >= LastFrame) CurrentFrame = FirstFrame;

        return ret;
    }

    return 0;
}

int CVfWAvi::CopyNextAVIFrame(unsigned char* dest, int datasize)
{
    // Dest must be preallocated with appropriate size
    if (dest == 0 || datasize < (Width * Height * 3)) return 0;

    if (FileIsOpened == AVI_READING) {

        unsigned char* temp = reinterpret_cast<unsigned char*>(AVIStreamGetFrame(PGetFrame, CurrentFrame));
        if (temp == 0) return 0;

        DWORD size = Width * Height;
        BOOL ret = ConvertToTrueColor(temp, dest, size);
        if (UpsideDown) TopDownImage(dest, Width, Height);

        CurrentFrame ++;
        if (CurrentFrame >= LastFrame) CurrentFrame = FirstFrame;

        return ret;
    }

    return 0;
}

int CVfWAvi::AddFrameToAVI(unsigned char* buffer, int buffersize)
{
	if (buffer == NULL ||
		buffersize < WriteBufferLen) {
		return 0;
	}

	if (FileIsOpened == AVI_WRITING) {
		int i;
		int rowsize = Width * 3;
		DWORD c = 0;
        DWORD flag = 0;

		for (i = 0; i < Height; i ++) {
			memcpy(WriteBuffer + c, buffer + (Height - i - 1) * rowsize, rowsize);
			c += rowsize;
		}

        if ((CurrentFrame%KeyFrames) == 0) flag = AVIIF_KEYFRAME;
		if (AVIStreamWrite(PAviCompressedStream,
						   LastFrame,
						   1,
						   WriteBuffer,
						   WriteBufferLen,
						   flag,
						   NULL,
						   NULL) == NULL) {
			LastFrame ++;
			CurrentFrame ++;
			return 1;
		}
	}

	return 0;
}

void CVfWAvi::InitConversionTables()
{
    WORD RedMask16 = 0x7C00;
    WORD GreenMask16 = 0x03E0;
    WORD BlueMask16 = 0x001F;

    int i;
    WORD wi;

    for (i = 0; i < 65536; i ++) {
        wi = i;
        ConvLUT_16_24[i].R = (wi & RedMask16) >> 7;
        ConvLUT_16_24[i].G = (wi & GreenMask16) >> 2;
        ConvLUT_16_24[i].B = (wi & BlueMask16) << 3;
    }
}

int CVfWAvi::ConvertToTrueColor(unsigned char* source, unsigned char* dest, const DWORD size)
{
    BITMAPINFOHEADER* pBMIH = reinterpret_cast<BITMAPINFOHEADER*>(source);
    unsigned int dataoffset = pBMIH->biSize + pBMIH->biClrUsed * 4;

    DWORD i;

    // Source: 32 bits per pixel, single channel
    if (pBMIH->biBitCount == 32 && pBMIH->biPlanes == 1) {
        unsigned char uchval;
        DWORD dwval, *pdw = reinterpret_cast<DWORD*>(source + dataoffset);
        for (i = 0; i < size; i ++) {
            dwval = (*pdw) >> 16; pdw ++;
            if (dwval <= 255) uchval = static_cast<unsigned char>(dwval);
            else uchval = 255;
            *dest = uchval; dest ++;
            *dest = uchval; dest ++;
            *dest = uchval; dest ++;
        }
        return 1;
    }

    // Source: 24 bits per pixel, RGB
    if (pBMIH->biBitCount == 24) {
        memcpy(dest,
               source + dataoffset,
               size * 3);
        return 1;
    }

    if (pBMIH->biBitCount == 16) {
        // Source: 16 bits per pixel, YUV2
        if (pBMIH->biCompression == 844715353) {
            const DWORD sizehalf = size / 2;
            unsigned char *y1, *y2, *u, *v;
            unsigned char *r1, *g1, *b1, *r2, *g2, *b2;
            int ty1, ty2, tv1, tv2, tu1, tu2;
            int res;

            y1 = source + dataoffset;
            u = y1 + 1;
            y2 = u + 1;
            v = y2 + 1;

            b1 = dest;
            g1 = b1 + 1;
            r1 = g1 + 1;
            b2 = r1 + 1;
            g2 = b2 + 1;
            r2 = g2 + 1;

            for (i = 0; i < sizehalf; i ++) {
                tu1 = *u;
                tu1 -= 128;

                tv1 = *v;
                tv1 -= 128;

                ty1 = *y1;
                ty1 -= 16;
                ty1 *= 298; // 1.164 * 256

                ty2 = *y2;
                ty2 -= 16;
                ty2 *= 298; // 1.164 * 256

                tu2 = tu1 * 517; // 2.018 * 256

                res = (ty1 + tu2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *b1 = static_cast<char>(res);
                res = (ty2 + tu2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *b2 = static_cast<char>(res);

                tu2 = tu1 * 100; // 0.391 * 256
                tv2 = tv1 * 208; // 0.813 * 256
                tv2 -= tu2;

                res = (ty1 - tv2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *g1 = static_cast<char>(res);
                res = (ty2 - tv2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *g2 = static_cast<char>(res);

                tv2 = tv1 * 409; // 1.596 * 256

                res = (ty1 + tv2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *r1 = static_cast<char>(res);
                res = (ty2 + tv2) >> 8;
                if (res > 255) res = 255;
                if (res < 0) res = 0;
                *r2 = static_cast<char>(res);

                y1 += 4;
                u += 4;
                y2 += 4;
                v += 4;

                r1 += 6;
                g1 += 6;
                b1 += 6;
                r2 += 6;
                g2 += 6;
                b2 += 6;
            }
            return 1;
        }
        // Source: 16 bits per pixel, RGB
        else {
            AVI_RGBSTRUCT* rgbptr;
            WORD* wptr = reinterpret_cast<WORD*>(source + dataoffset);
            for (i = 0; i < size; i ++) {
                rgbptr = ConvLUT_16_24 + *wptr; wptr ++;
                *dest = rgbptr->R; dest ++;
                *dest = rgbptr->G; dest ++;
                *dest = rgbptr->B; dest ++;
            }
            return 1;
        }
    }

    // Source: 8 bits per pixel, single channel
    if (pBMIH->biBitCount == 8) {
        unsigned char uchval;
        source += dataoffset;
        for (i = 0; i < size; i ++) {
            uchval = *source; source ++;
            *dest = uchval; dest ++;
            *dest = uchval; dest ++;
            *dest = uchval; dest ++;
        }
        return 1;
    }

    return 0;
}

void CVfWAvi::TopDownImage(unsigned char* image, int width, int height)
{
    int i, x = width * 3, y = height - 1;
    unsigned char* tLine = new unsigned char[x];
    unsigned char *tptr1, *tptr2;

    for (i = y / 2; i >= 0; i --) {
        tptr1 = image + i * x;
        tptr2 = image + (y - i) * x;
        memcpy(tLine, tptr1, x);
        memcpy(tptr1, tptr2, x);
        memcpy(tptr2, tLine, x);
    }

    delete [] tLine;
}

int CVfWAvi::ShowCompressionDialog()
{
    if (FileIsOpened != 0) return 0;

    int ret = 0;
	AVICOMPRESSOPTIONS* tCompr[1] = {&CompressionOptions};

	AVIFileInit();

	char path[] = "__test.avi";
	if (AVIFileOpen(&PAviFile, path, OF_WRITE|OF_CREATE, NULL) == 0) {
		StreamInfo.fccType = streamtypeVIDEO;
		StreamInfo.fccHandler = 0;
		StreamInfo.dwScale = static_cast<DWORD>(1000000 / 30.0);
		StreamInfo.dwRate = 1000000;
		StreamInfo.dwSuggestedBufferSize = 1000000;
		StreamInfo.rcFrame.left = 0;
		StreamInfo.rcFrame.top = 0;
		StreamInfo.rcFrame.right = 320;
		StreamInfo.rcFrame.bottom = 240;

        if (AVIFileCreateStream(PAviFile, &PAviStream, &StreamInfo) == 0) {

            // Open codec selection dialog
			if (AVISaveOptions(0, ICMF_CHOOSE_DATARATE|ICMF_CHOOSE_KEYFRAME, 1, &PAviStream,
                               reinterpret_cast<LPAVICOMPRESSOPTIONS*>(&tCompr)) == TRUE) {

                // Create compressed stream
                HRESULT error = AVIMakeCompressedStream(&PAviCompressedStream, PAviStream, &CompressionOptions, NULL);
                if (error == AVIERR_OK) {

                    // Store compressor name
                    AVISTREAMINFO info;
                    if (AVIStreamInfo(PAviCompressedStream, &info, sizeof(AVISTREAMINFO)) == 0) {
                        CompressorName = info.szName;
                    }

                    ret = 1;
                }
                else if (error == AVIERR_NOCOMPRESSOR) {
                }
                else if (error == AVIERR_MEMORY) {
                }
                else if (error == AVIERR_UNSUPPORTED) {
                }
                else {
                }
            }
        }
	}

	Close();
	DeleteFile("__test.avi");

	return ret;
}

void CVfWAvi::Close()
{
    if (PGetFrame != 0) AVIStreamGetFrameClose(PGetFrame);
    if (PAviStream != 0) AVIStreamRelease(PAviStream);
    if (PAviCompressedStream != 0) AVIStreamRelease(PAviCompressedStream);
    if (PAviFile != 0) AVIFileRelease(PAviFile);
    if (FileIsOpened != AVI_CLOSED) AVIFileExit();
    ResetStates();
}

void CVfWAvi::ResetStates()
{
    memset(PathName, 0, 1024);

    PAviFile = 0;
    memset(&FileInfo, 0, sizeof(AVIFILEINFO));
    PAviStream = 0;
    memset(&StreamInfo, 0, sizeof(AVISTREAMINFO));
    PAviCompressedStream = 0;
    PGetFrame = 0;

    FirstFrame = -1;
    LastFrame = -1;
    CurrentFrame = -1;
    Width = -1;
    Height = -1;
    Framerate = -1;
    KeyFrames = 30;

	if (WriteBuffer) {
		delete [] WriteBuffer;
		WriteBuffer = 0;
	}
	WriteBufferLen = 0;

    FileIsOpened = AVI_CLOSED;
}

