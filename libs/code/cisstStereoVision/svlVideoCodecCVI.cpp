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

//#define READ_CORRUPT_V11_FILE


/*************************************/
/*** svlVideoCodecCVI class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlVideoCodecCVI)

svlVideoCodecCVI::svlVideoCodecCVI() :
    svlVideoCodecBase(),
    cmnGenericObject(),
    CodecName("CISST Video"),
    FileStartMarker("CisstSVLVideo\r\n",  // all version strings shall be of equal length
                    "CisstVid_1.10\r\n",
                    "CisstVid_1.20\r\n"),
    FrameStartMarker("\r\nFrame\r\n"),
    Version(-1),
    File(0),
    FooterOffset(0),
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
        if (File->read(strbuffer, FileStartMarker[0].length()).fail()) break;
        strbuffer[FileStartMarker[0].length()] = 0;
        Version = static_cast<int>(FileStartMarker.size()) - 1;
        while (Version >= 0) {
            if (FileStartMarker[Version].compare(strbuffer) == 0) break;
            Version --;
        }
        if (Version < 0) break;

        if (Version > 0) {
            // Read "footer offset"
            if (File->read(reinterpret_cast<char*>(&FooterOffset), sizeof(long long int)).fail() || FooterOffset <= 0) break;

#ifdef READ_CORRUPT_V110_FILE
            Version = 0;
#else
            std::streampos pos = File->tellg(); // Store file position

            // Seek to footer offset
            if (File->seekg(FooterOffset).fail()) break;

            // Read the frame ID of the last frame
            if (File->read(reinterpret_cast<char*>(&EndPos), sizeof(int)).fail() || EndPos < 0) break;

            // Create frame offsets
            FrameOffsets.SetSize(EndPos + 1);
            FrameOffsets.SetAll(0);

            // Read frame offsets
            if (File->read(reinterpret_cast<char*>(FrameOffsets.Pointer()), FrameOffsets.size() * sizeof(long long int)).fail()) break;

            if (Version > 1) {
                // Create frame timestamps
                FrameTimestamps.SetSize(EndPos + 1);
                FrameTimestamps.SetAll(0.0);

                // Read frame timestamps
                if (File->read(reinterpret_cast<char*>(FrameTimestamps.Pointer()), FrameTimestamps.size() * sizeof(double)).fail()) break;
            }

            File->seekg(pos); // Restore file position
#endif
        }
        else {
            EndPos = 0;
        }

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

        Pos = BegPos = 0;
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
	if (Opened || width < 1 || width > 4096 || height < 1 || height > 4096) return SVL_FAIL;

    if (!Codec) {
        // Set default compression level to 4
        Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);
        std::string name("Multiblock ZLib Compression (YUV422)");
        memset(&(Codec->extension[0]), 0, 16);
        memcpy(&(Codec->extension[0]), ".cvi", 4);
        memset(&(Codec->name[0]), 0, 64);
        memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
        Codec->size = sizeof(svlVideoIO::Compression);
        Codec->supports_timestamps = true;
        Codec->datasize = 1;
        Codec->data[0] = 4;
    }

    unsigned int size;

    while (1) {

        File = new std::fstream;
        if (!File) return SVL_FAIL;
        // Open file
        File->open(filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (!File->is_open()) break;

        // Write "file start marker" (always writes the latest version)
        Version = FileStartMarker.size() - 1;
        if (File->write(FileStartMarker[Version].c_str(), FileStartMarker[Version].length()).fail()) break;

        // Write "footer offset" placeholder (will be filled later)
        FooterOffset = 0;
        if (File->write(reinterpret_cast<const char*>(&FooterOffset), sizeof(long long int)).fail()) break;

        // Pre-allocate large frame offsets & timestamps tables
        FrameOffsets.SetSize(100000);
        FrameTimestamps.SetSize(100000);
        
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
    int ret = SVL_OK;

    if (Opened && Writing) {

        // Stop data saving thread
        KillSaveThread = true;
        if (SaveInitialized) {
            NewFrameEvent.Raise();
            SaveThread.Wait();
        }

        if (File) {
            while (1) {
                // Store current file position
                FooterOffset = static_cast<long long int>(File->tellg());

                // Write the offset of the last frame
                EndPos --;
                if (File->write(reinterpret_cast<const char*>(&EndPos), sizeof(int)).fail()) {
                    ret = SVL_FAIL;
                    break;
                }

                // Write frame offsets
                if (File->write(reinterpret_cast<const char*>(FrameOffsets.Pointer()), (EndPos + 1) * sizeof(long long int)).fail()) {
                    ret = SVL_FAIL;
                    break;
                }

                // Write frame timestamps
                if (File->write(reinterpret_cast<const char*>(FrameTimestamps.Pointer()), (EndPos + 1) * sizeof(double)).fail()) {
                    ret = SVL_FAIL;
                    break;
                }

                // Seek back to "frame offsets pointer"
                if (File->seekg(FileStartMarker[Version].length()).fail()) {
                    ret = SVL_FAIL;
                    break;
                }

                // Replace the "footer offset" placeholder with the real data
                if (File->write(reinterpret_cast<const char*>(&FooterOffset), sizeof(long long int)).fail()) {
                    ret = SVL_FAIL;
                    break;
                }
                
                break;
            }
        }
    }

    if (File) {
        if (File->is_open()) File->close();
        delete File;
        File = 0;
    }
    else ret = SVL_FAIL;

    Version = -1;
    FooterOffset = 0;
    FrameOffsets.SetSize(0);
    FrameTimestamps.SetSize(0);
    Width = 0;
    Height = 0;
    BegPos = -1;
    EndPos = -1;
    Pos = -1;
    Writing = false;
    Opened = false;

    return ret;
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

int svlVideoCodecCVI::SetPos(const int pos)
{
    if (Version == 0) return SVL_FAIL;
    if (pos < 0 || pos > EndPos) return SVL_FAIL;
    Pos = pos;
    return SVL_OK;
}

double svlVideoCodecCVI::GetBegTime() const
{
    if (Opened && !Writing && Version > 1) {
        return FrameTimestamps[0];
    }
    return -1.0;
}

double svlVideoCodecCVI::GetEndTime() const
{
    if (Opened && !Writing && Version > 1) {
        return FrameTimestamps[FrameTimestamps.size() - 1];
    }
    return -1.0;
}

double svlVideoCodecCVI::GetTimeAtPos(const int pos) const
{
    if (Opened && !Writing && Version > 1) {

        if (pos < 0) {
            return FrameTimestamps[0];
        }
        else if (pos >= static_cast<int>(FrameTimestamps.size())) {
            return FrameTimestamps[FrameTimestamps.size() - 1];
        }
        else {
            return FrameTimestamps[pos];
        }
    }
    return -1.0;
}

int svlVideoCodecCVI::GetPosAtTime(const double time) const
{
    if (Opened && !Writing && Version > 1) {

        int begpos     = 0;
        int endpos     = FrameTimestamps.size() - 1;
        double begtime = FrameTimestamps[begpos];
        double endtime = FrameTimestamps[endpos];
        int halfpos;
        double halftime;

        while (1) {
            if (time <= begtime) {
                return begpos;
            }
            else if (time >= endtime) {
                return endpos;
            }
            else {
                if ((endpos - begpos) <= 1) return begpos;

                halfpos = (begpos + endpos) / 2;
                halftime = FrameTimestamps[halfpos];
                if (time <= halftime) {
                    endpos  = halfpos;
                    endtime = halftime;
                }
                else {
                    begpos  = halfpos;
                    begtime = halftime;
                }
            }
        }
    }
    return -1;
}

svlVideoIO::Compression* svlVideoCodecCVI::GetCompression() const
{
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    unsigned int size = sizeof(svlVideoIO::Compression);
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(compression->extension[0]), 0, 16);
    memcpy(&(compression->extension[0]), ".cvi", 4);
    memset(&(compression->name[0]), 0, 64);
    memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    compression->size = size;
    compression->supports_timestamps = true;
    compression->datasize = 1;
    if (Codec) compression->data[0] = Codec->data[0];
    else compression->data[0] = 4; // Set default compression level to 4

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
    int level = 0;
    while (level < '0' || level > '9') level = cmnGetChar();
    level -= '0';
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

int svlVideoCodecCVI::Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize)
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

    if (Version > 0) {
        if (Pos > EndPos) {
            Pos = 0;
            return SVL_VID_END_REACHED;
        }

        // Look up the position in the frame offsets table and move the file pointer
        if (File->seekg(FrameOffsets[Pos]).fail()) return SVL_FAIL;
    }
    else {
        if (Pos == 0) {
            // Go to the beginning of the data, just after the header
#ifdef READ_CORRUPT_V11_FILE
            if (File->seekg(35).fail()) return SVL_FAIL;
#else
            if (File->seekg(27).fail()) return SVL_FAIL;
#endif
        }
    }

    while (1) {

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

    if (Version > 0) {
        if (ret != SVL_OK) {
            // Video data ended earlier than expected: error
            if (Pos > 0) {

                // Clear stream error flags
                File->clear();

                // Set pointer back to the first frame
                Pos = 0;

                return SVL_VID_END_REACHED;
            }
            else {
                // If it was the first frame, then file is invalid, let it fail
            }
        }
    }
    else {
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
                // If it was the first frame, then file is invalid, let it fail
            }
        }
    }

    return ret;
}

int svlVideoCodecCVI::Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || !Writing) return SVL_FAIL;
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) return SVL_FAIL;

    // Check for video saving errors
    if (SaveThreadError) return SVL_FAIL;

    bool err = false;

    if (Pos == 0) {

        _OnSingleThread(procInfo)
        {
            // Initialize multithreaded processing
            ComprPartOffset.SetSize(procInfo->count);
            ComprPartSize.SetSize(procInfo->count);

            // Write "part count"
            if (File->write(reinterpret_cast<char*>(&procInfo->count), sizeof(unsigned int)).fail()) err = true;
        }

        // Synchronize threads
        _SynchronizeThreads(procInfo);

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
    _SynchronizeThreads(procInfo);

    if (err) return SVL_FAIL;

    // Single threaded data serialization phase
    _OnSingleThread(procInfo)
    {
        const unsigned int savebufferid = SaveBufferUsedID ? 0 : 1;
        const double timestamp = image.GetTimestamp();

        // Wait until the previous write operation is done
        WriteDoneEvent.Wait();

        // Store current file position in frame offsets table (increase table size if needed)
        if (FrameOffsets.size() <= static_cast<unsigned int>(EndPos)) FrameOffsets.resize(FrameOffsets.size() + 100000);
        FrameOffsets[EndPos] = static_cast<long long int>(File->tellg());

        // Store current timestamp in frame timestamps table (increase table size if needed)
        if (FrameTimestamps.size() <= static_cast<unsigned int>(EndPos)) FrameTimestamps.resize(FrameTimestamps.size() + 100000);
        FrameTimestamps[EndPos] = timestamp;

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

