/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVideoCodecCVI.h"
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlConverters.h>
#include <cisstStereoVision/svlSyncPoint.h>

#include "zlib.h"


/*************************************/
/*** svlVideoCodecCVI class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlVideoCodecCVI)

svlVideoCodecCVI::svlVideoCodecCVI() :
    svlVideoCodecBase(),
    cmnGenericObject(),
    CodecName("CISST Video"),
    FileStartMarker("CisstSVLVideo\r\n"),
    FrameStartMarker("\r\nFrame\r\n"),
    File(0),
    PartCount(0),
    Width(0),
    Height(0),
    BegPos(-1),
    EndPos(-1),
    Pos(-1),
    Opened(false),
    Writing(false),
    Timestamp(-1.0),
    yuvBuffer(0),
    yuvBufferSize(0),
    comprBuffer(0),
    comprBufferSize(0),
    saveBufferSize(0)
{
    SetName("CISST Video Files");
    SetExtensionList(".cvi;");
    SetMultithreaded(true);

    saveBuffer.SetAll(0);
}

svlVideoCodecCVI::~svlVideoCodecCVI()
{
    Close();
    if (yuvBuffer) delete [] yuvBuffer;
    if (comprBuffer) delete [] comprBuffer;
    if (saveBuffer[0]) delete [] saveBuffer[0];
    if (saveBuffer[1]) delete [] saveBuffer[1];
}

int svlVideoCodecCVI::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened) return SVL_FAIL;

    unsigned int size;
    char strbuffer[32];

    while (1) {

        File = new std::fstream;
        if (!File) return SVL_FAIL;
        // Open file
        File->open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!File->is_open()) break;
        // Read "file start marker"
        if (File->read(strbuffer, FileStartMarker.length()).fail()) break;
        strbuffer[FileStartMarker.length()] = 0;
        if (FileStartMarker.compare(strbuffer) != 0) break;
        // Read "width"
        if (File->read(reinterpret_cast<char*>(&Width), sizeof(unsigned int)).fail() || Width < 1 || Width > 4096) break;
        // Read "height"
        if (File->read(reinterpret_cast<char*>(&Height), sizeof(unsigned int)).fail() || Height < 1 || Height > 4096) break;
        // Read "part count"
        if (File->read(reinterpret_cast<char*>(&PartCount), sizeof(unsigned int)).fail() || PartCount < 1 || PartCount > 256) break;

        // Allocate YUV buffer if not done yet
        size = Width * Height * 2;
        if (!yuvBuffer) {
            yuvBuffer = new unsigned char[size];
            yuvBufferSize = size;
        }
        else if (yuvBuffer && yuvBufferSize < size) {
            delete [] yuvBuffer;
            yuvBuffer = new unsigned char[size];
            yuvBufferSize = size;
        }

        // Allocate compression buffer if not done yet
        size = yuvBufferSize + yuvBufferSize / 100 + 4096;
        if (!comprBuffer) {
            comprBuffer = new unsigned char[size];
            comprBufferSize = size;
        }
        else if (comprBuffer && comprBufferSize < size) {
            delete [] comprBuffer;
            comprBuffer = new unsigned char[size];
            comprBufferSize = size;
        }

        Pos = BegPos = EndPos = 0;
        width = Width;
        height = Height;
        framerate = -1.0;
        Opened = true;
        Writing = false;

        return SVL_OK;
    }

    Close();
    return SVL_FAIL;
}

int svlVideoCodecCVI::Create(const std::string &filename, const unsigned int width, const unsigned int height, const double CMN_UNUSED(framerate))
{
	if (Opened || !Codec || width < 1 || width > 4096 || height < 1 || height > 4096) return SVL_FAIL;

    unsigned int size;

    while (1) {

        File = new std::fstream;
        if (!File) return SVL_FAIL;
        // Open file
        File->open(filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (!File->is_open()) break;
        // Write "file start marker"
        if (File->write(FileStartMarker.c_str(), FileStartMarker.length()).fail()) break;
        // Write "width"
        if (File->write(reinterpret_cast<const char*>(&width), sizeof(unsigned int)).fail()) break;
        // Write "height"
        if (File->write(reinterpret_cast<const char*>(&height), sizeof(unsigned int)).fail()) break;

        // Allocate YUV buffer if not done yet
        size = width * height * 2;
        if (!yuvBuffer) {
            yuvBuffer = new unsigned char[size];
            yuvBufferSize = size;
        }
        else if (yuvBuffer && yuvBufferSize < size) {
            delete [] yuvBuffer;
            yuvBuffer = new unsigned char[size];
            yuvBufferSize = size;
        }

        // Allocate compression buffer if not done yet
        size = width * height * 3;
        size += size / 100 + 4096;
        if (!comprBuffer) {
            comprBuffer = new unsigned char[size];
            comprBufferSize = size;
        }
        else if (comprBuffer && comprBufferSize < size) {
            delete [] comprBuffer;
            comprBuffer = new unsigned char[size];
            comprBufferSize = size;
        }

        // Allocate save buffers if not done yet
        if (saveBufferSize == 0) {
            saveBuffer[0] = new unsigned char[size];
            saveBuffer[1] = new unsigned char[size];
            saveBufferSize = size;
        }
        else if (saveBufferSize < size) {
            delete [] saveBuffer[0];
            delete [] saveBuffer[1];
            saveBuffer[0] = new unsigned char[size];
            saveBuffer[1] = new unsigned char[size];
            saveBufferSize = size;
        }

        SaveBufferUsedSize = 0;
        SaveBufferUsedID = 0;

        // Start data saving thread
        SaveInitialized = false;
        KillSaveThread = false;
        SaveThread.Create<svlVideoCodecCVI, int>(this, &svlVideoCodecCVI::SaveProc, 0);
        SaveInitEvent.Wait();
        if (SaveInitialized == false) break;

        BegPos = EndPos = Pos = 0;
        Width = width;
        Height = height;
        Opened = true;
	    Writing = true;

        return SVL_OK;
    }

	Close();
	return SVL_FAIL;
}

int svlVideoCodecCVI::Close()
{
    if (Opened && Writing) {

        // Stop data saving thread
        KillSaveThread = true;
        if (SaveInitialized) {
            NewFrameEvent.Raise();
            SaveThread.Wait();
        }
    }

    if (File) {
        if (File->is_open()) File->close();
        delete File;
        File = 0;
    }

    Width = 0;
    Height = 0;
    BegPos = -1;
    EndPos = -1;
    Pos = -1;
    Writing = false;
    Opened = false;

    return SVL_OK;
}

int svlVideoCodecCVI::GetBegPos() const
{
    return BegPos;
}

int svlVideoCodecCVI::GetEndPos() const
{
    return EndPos;
}

int svlVideoCodecCVI::GetPos() const
{
    return Pos;
}

svlVideoIO::Compression* svlVideoCodecCVI::GetCompression() const
{
    if (!Codec) return 0;
    // Make a copy and return the pointer to it
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[Codec->size]);
    memcpy(compression, Codec, Codec->size);
    return compression;
}

int svlVideoCodecCVI::SetCompression(const svlVideoIO::Compression *compression)
{
    if (Opened || !compression || compression->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    std::string extensionlist(GetExtensions());
    std::string extension(compression->extension);
    extension += ";";
    if (extensionlist.find(extension) == std::string::npos) {
        // Codec parameters do not match this codec
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(Codec);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);

    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(Codec->extension[0]), 0, 16);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = sizeof(svlVideoIO::Compression);
    Codec->supports_timestamps = true;
    Codec->datasize = 1;
    if (compression->data[0] <= 9) Codec->data[0] = compression->data[0];
    else Codec->data[0] = 4;

    return SVL_OK;
}

int svlVideoCodecCVI::DialogCompression()
{
    if (Opened) return SVL_FAIL;

    std::cout << std::endl << " # Enter compression level [0-9]: ";
    int level = cmnGetChar() - '0';
    if (level < 0) level = 0;
    else if (level > 9) level = 9;
    std::cout << level << std::endl;

    svlVideoIO::ReleaseCompression(Codec);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);

    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".cvi", 4);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = sizeof(svlVideoIO::Compression);
    Codec->supports_timestamps = true;
    Codec->datasize = 1;
    Codec->data[0] = static_cast<unsigned char>(level);

	return SVL_OK;
}

double svlVideoCodecCVI::GetTimestamp() const
{
    if (!Opened || Writing) return -1.0;
    return Timestamp;
}

int svlVideoCodecCVI::SetTimestamp(const double timestamp)
{
    if (!Opened || !Writing) return SVL_FAIL;
    Timestamp = timestamp;
    return SVL_OK;
}

int svlVideoCodecCVI::Read(svlProcInfo* procInfo, svlSampleImageBase &image, const unsigned int videoch, const bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || Writing) return SVL_FAIL;

    // Uses only a single thread
    if (procInfo && procInfo->id != 0) return SVL_OK;

    // Allocate image buffer if not done yet
    if (Width  != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, Width, Height);
    }

    unsigned char* img = image.GetUCharPointer(videoch);
    unsigned int i, compressedpartsize, offset;
    unsigned long longsize;
    char strbuffer[32];
    int ret = SVL_FAIL;

    while (1) {

        if (Pos == 0) {
            // Go to the beginning of the data, just after the header
            if (File->seekg(27).fail()) return SVL_FAIL;
        }

        // Read "frame start marker"
        if (File->read(strbuffer, FrameStartMarker.length()).fail()) break;
        strbuffer[FrameStartMarker.length()] = 0;
        if (FrameStartMarker.compare(strbuffer) != 0) return SVL_FAIL;

        // Read "timestamp"
        if (File->read(reinterpret_cast<char*>(&Timestamp), sizeof(double)).fail()) break;
        if (Timestamp < 0.0) return SVL_FAIL;

        offset = 0;
        for (i = 0; i < PartCount; i ++) {

            // Read "compressed part size"
            if (File->read(reinterpret_cast<char*>(&compressedpartsize), sizeof(unsigned int)).fail()) break;
            if (compressedpartsize == 0 || compressedpartsize > comprBufferSize) return SVL_FAIL;

            // Read compressed frame part
            if (File->read(reinterpret_cast<char*>(comprBuffer), compressedpartsize).fail()) break;

            // Decompress frame part
            longsize = yuvBufferSize - offset;
            if (uncompress(yuvBuffer + offset, &longsize, comprBuffer, compressedpartsize) != Z_OK) return SVL_FAIL;

            // Convert YUV422 planar to RGB format
            svlConverter::YUV422PtoRGB24(yuvBuffer + offset, img + offset * 3 / 2, longsize >> 1);

            offset += longsize;
        }
        if (i < PartCount) break;

        Pos ++;
        ret = SVL_OK;

        break;
    }

    if (ret != SVL_OK) {
        // End of file reached
        if (Pos > 0) {

            // Clear stream error flags
            File->clear();

            // Set pointer back to the first frame
            EndPos = Pos;
            Pos = 0;

            return SVL_VID_END_REACHED;
        }
        else {
            // If it was the first frame, then file is invalid
        }
    }
    else {
        // Other error, let it fail
    }

    return ret;
}

int svlVideoCodecCVI::Write(svlProcInfo* procInfo, const svlSampleImageBase &image, const unsigned int videoch)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || !Writing) return SVL_FAIL;
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) return SVL_FAIL;

    // Check for video saving errors
    if (SaveThreadError) return SVL_FAIL;

    bool err = false;

    if (Pos == 0) {

        if (procInfo->id == 0) {
            // Initialize multithreaded processing
            ComprPartOffset.SetSize(procInfo->count);
            ComprPartSize.SetSize(procInfo->count);

            // Write "part count"
            if (File->write(reinterpret_cast<char*>(&procInfo->count), sizeof(unsigned int)).fail()) err = true;
        }

        // Synchronize threads
        if (procInfo->count > 1) {
            if (procInfo->sync->Sync(procInfo->id) != SVL_SYNC_OK) {
                return SVL_FAIL;
            }
        }

        if (err) return SVL_FAIL;
    }

    const unsigned int procid = procInfo->id;
    const unsigned int proccount = procInfo->count;
    unsigned int start, end, size, offset;
    unsigned long comprsize;
    int compr = Codec->data[0];

    // Multithreaded compression phase
    while (1) {

        // Compute part size and offset
        size = Height / proccount + 1;
        comprsize = comprBufferSize / proccount;
        start = procid * size;
        if (start >= Height) break;
        end = start + size;
        if (end > Height) end = Height;
        offset = start * Width;
        size = Width * (end - start);
        ComprPartOffset[procid] = procid * comprsize;

        // Convert RGB to YUV422 planar format
        svlConverter::RGB24toYUV422P(const_cast<unsigned char*>(image.GetUCharPointer(videoch)) + offset * 3, yuvBuffer + offset * 2, size);

        // Compress part
        if (compress2(comprBuffer + ComprPartOffset[procid], &comprsize, yuvBuffer + offset * 2, size * 2, compr) != Z_OK) {
            err = true;
            break;
        }
        ComprPartSize[procid] = comprsize;

        break;
    }

    // Synchronize threads
    if (procInfo->count > 1) {
        if (procInfo->sync->Sync(procInfo->id) != SVL_SYNC_OK) {
            return SVL_FAIL;
        }
    }

    if (err) return SVL_FAIL;

    // Single threaded data serialization phase
    if (procInfo->id == 0) {

        const unsigned int savebufferid = SaveBufferUsedID ? 0 : 1;
        const double timestamp = image.GetTimestamp();

        // Wait until the previous write operation is done
        WriteDoneEvent.Wait();

        // Add "frame start marker"
        memcpy(saveBuffer[savebufferid], FrameStartMarker.c_str(), FrameStartMarker.length());
        SaveBufferUsedSize = FrameStartMarker.length();

        // Add "timestamp"
        memcpy(saveBuffer[savebufferid] + SaveBufferUsedSize, &timestamp, sizeof(double));
        SaveBufferUsedSize += sizeof(double);

        for (unsigned int i = 0; i < proccount; i ++) {
            // Add "compressed part size"
            memcpy(saveBuffer[savebufferid] + SaveBufferUsedSize, &(ComprPartSize[i]), sizeof(unsigned int));
            SaveBufferUsedSize += sizeof(unsigned int);

            // Add compressed frame
            memcpy(saveBuffer[savebufferid] + SaveBufferUsedSize, comprBuffer + ComprPartOffset[i], ComprPartSize[i]);
            SaveBufferUsedSize += ComprPartSize[i];
        }

        // Signal data saving thread to start writing
        SaveBufferUsedID = savebufferid;
        NewFrameEvent.Raise();

		EndPos ++; Pos ++;
    }

	return SVL_OK;
}

void* svlVideoCodecCVI::SaveProc(int CMN_UNUSED(param))
{
    SaveThreadError = false;
    SaveInitialized = true;
    SaveInitEvent.Raise();
    WriteDoneEvent.Raise();

    while (1) {

        // Wait for new frame to arrive
        NewFrameEvent.Wait();
        if (KillSaveThread || SaveThreadError) break;

        // Write frame
        if (File->write(reinterpret_cast<char*>(saveBuffer[SaveBufferUsedID]), SaveBufferUsedSize).fail()) {
            SaveThreadError = true;
            return this;
        }

        // Signal that write is done
        WriteDoneEvent.Raise();
    }

    return this;
}

