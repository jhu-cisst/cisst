/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecCVI, svlVideoCodecBase)

svlVideoCodecCVI::svlVideoCodecCVI() :
    svlVideoCodecBase(),
    CodecName("CISST Video"),
    FileStartMarker("CisstSVLVideo\r\n",  // all version strings shall be of equal length
                    "CisstVid_1.10\r\n",
                    "CisstVid_1.20\r\n",
                    "CisstVid_1.30\r\n"),
    FrameStartMarker("\r\nFrame\r\n"),
    Version(-1),
    FooterOffset(0),
    DataOffset(0),
    PartCount(0),
    Width(0),
    Height(0),
    BegPos(-1),
    EndPos(-1),
    Pos(-1),
    Opened(false),
    Writing(false),
    Timestamp(-1.0),
    prevYuvBuffer(0),
    prevYuvBufferSize(0),
    yuvBuffer(0),
    yuvBufferSize(0),
    comprBuffer(0),
    comprBufferSize(0),
    saveBufferSize(0),
    SaveThread(0),
    SaveInitEvent(0),
    NewFrameEvent(0),
    WriteDoneEvent(0)
{
    SetName("CISST Video Files");
    SetExtensionList(".cvi;");
    SetMultithreaded(true);
    SetVariableFramerate(true);

    saveBuffer.SetAll(0);

    Config.Level        = 4;
    Config.Differential = 0;

    ProcInfoSingleThread.count = 1;
    ProcInfoSingleThread.ID    = 0;
}

svlVideoCodecCVI::~svlVideoCodecCVI()
{
    Close();
    if (prevYuvBuffer) delete [] prevYuvBuffer;
    if (yuvBuffer)     delete [] yuvBuffer;
    if (comprBuffer)   delete [] comprBuffer;
    if (saveBuffer[0]) delete [] saveBuffer[0];
    if (saveBuffer[1]) delete [] saveBuffer[1];
}

int svlVideoCodecCVI::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "Open: codec is already open" << std::endl;
        return SVL_FAIL;
    }

    unsigned int size;
    long long int len, pos;
    char strbuffer[32];

    while (1) {

        // Open file
        if (File.Open(filename, svlFile::R) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to open file for reading" << std::endl;
            break;
        }

        // Read "file start marker"
        len = FileStartMarker[0].length();
        if (File.Read(strbuffer, len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `file start marker`" << std::endl;
            break;
        }
        strbuffer[len] = 0;
        Version = static_cast<int>(FileStartMarker.size()) - 1;
        while (Version >= 0) {
            if (FileStartMarker[Version].compare(strbuffer) == 0) break;
            Version --;
        }
        if (Version < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: invalid CVI version" << std::endl;
            break;
        }

        if (Version > 0) {

            if (Version > 2) {
                // Read "differential flag"
                len = sizeof(unsigned char);
                if (File.Read(reinterpret_cast<char*>(&(Config.Differential)), len) != len) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `differential flag`" << std::endl;
                    break;
                }
            }

            // Read "footer offset"
            len = sizeof(long long int);
            if (File.Read(reinterpret_cast<char*>(&FooterOffset), len) != len) {
                CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `footer offset`" << std::endl;
                break;
            }

            if (FooterOffset > 0) {
                // Store file position
                pos = File.GetPos();

                // Seek to footer offset
                if (File.Seek(FooterOffset) != SVL_OK) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: failed to seek to file footer" << std::endl;
                    break;
                }

                // Read the frame ID of the last frame
                len = sizeof(int);
                if (File.Read(reinterpret_cast<char*>(&EndPos), len) != len) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `end position`" << std::endl;
                    break;
                }
                if (EndPos < 0) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: invalid `end position`" << std::endl;
                    break;
                }

                // Create frame offsets
                FrameOffsets.SetSize(EndPos + 1);
                FrameOffsets.SetAll(0);

                // Read frame offsets
                len = FrameOffsets.size() * sizeof(long long int);
                if (File.Read(reinterpret_cast<char*>(FrameOffsets.Pointer()), len) != len) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `frame offsets`" << std::endl;
                    break;
                }

                if (Version > 1) {
                    // Create frame timestamps
                    FrameTimestamps.SetSize(EndPos + 1);
                    FrameTimestamps.SetAll(0.0);

                    // Read frame timestamps
                    len = FrameTimestamps.size() * sizeof(double);
                    if (File.Read(reinterpret_cast<char*>(FrameTimestamps.Pointer()), len) != len) {
                        CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `frame timestamps`" << std::endl;
                        break;
                    }
                }

                // Restore file position
                File.Seek(pos);
            }
            else {
                CMN_LOG_CLASS_INIT_WARNING << "Open: invalid `footer offset`; opening in recovery mode; seeking not supported" << std::endl;
                Version = 0;
            }
        }
        else {
            EndPos = 0;
        }

        // Read "width"
        len = sizeof(unsigned int);
        if (File.Read(reinterpret_cast<char*>(&Width), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `width`" << std::endl;
            break;
        }
        if (Width < 1 || Width > MAX_DIMENSION) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: invalid `width`" << std::endl;
            break;
        }
        // Read "height"
        len = sizeof(unsigned int);
        if (File.Read(reinterpret_cast<char*>(&Height), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `height`" << std::endl;
            break;
        }
        if (Height < 1 || Height > MAX_DIMENSION) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: invalid `height`" << std::endl;
            break;
        }
        // Read "part count"
        len = sizeof(unsigned int);
        if (File.Read(reinterpret_cast<char*>(&PartCount), len)!= len) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to read `part count`" << std::endl;
            break;
        }
        if (PartCount < 1 || PartCount > 256) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: invalid `part count`" << std::endl;
            break;
        }

        DataOffset = File.GetPos();

        if (Config.Differential) {
            // Allocate previous YUV buffer if not done yet
            size = Width * Height * 2;
            if (!prevYuvBuffer) {
                prevYuvBuffer = new unsigned char[size];
                prevYuvBufferSize = size;
            }
            else if (prevYuvBuffer && prevYuvBufferSize < size) {
                delete [] prevYuvBuffer;
                prevYuvBuffer = new unsigned char[size];
                prevYuvBufferSize = size;
            }
        }

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
	if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "Create: codec is already open" << std::endl;
        return SVL_FAIL;
    }
    if (width < 1 || width > MAX_DIMENSION || height < 1 || height > MAX_DIMENSION) {
        CMN_LOG_CLASS_INIT_ERROR << "Create: invalid image dimensions" << std::endl;
        return SVL_FAIL;
    }

    unsigned int size;
    long long int len;

    while (1) {

        // Open file
        if (File.Open(filename, svlFile::W) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to open file for writing" << std::endl;
            break;
        }

        // Write "file start marker" (always writes the latest version)
        Version = static_cast<int>(FileStartMarker.size()) - 1;
        len = FileStartMarker[Version].length();
        if (File.Write(FileStartMarker[Version].c_str(), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to write `file start marker`" << std::endl;
            break;
        }

        // Write "differential flag"
        len = sizeof(unsigned char);
        if (File.Write(reinterpret_cast<const char*>(&(Config.Differential)), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to write `differential flag`" << std::endl;
            break;
        }

        // Write "footer offset" placeholder (will be filled later)
        FooterOffset = 0;
        len = sizeof(long long int);
        if (File.Write(reinterpret_cast<const char*>(&FooterOffset), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to write `footer offset` placeholder" << std::endl;
            break;
        }

        // Pre-allocate large frame offsets & timestamps tables
        FrameOffsets.SetSize(100000);
        FrameTimestamps.SetSize(100000);
    
        // Write "width"
        len = sizeof(unsigned int);
        if (File.Write(reinterpret_cast<const char*>(&width), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to write `width`" << std::endl;
            break;
        }
        // Write "height"
        len = sizeof(unsigned int);
        if (File.Write(reinterpret_cast<const char*>(&height), len) != len) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to write `height`" << std::endl;
            break;
        }

        if (Config.Differential) {
            // Allocate previous YUV buffer if not done yet
            size = width * height * 2;
            if (!prevYuvBuffer) {
                prevYuvBuffer = new unsigned char[size];
                prevYuvBufferSize = size;
            }
            else if (prevYuvBuffer && prevYuvBufferSize < size) {
                delete [] prevYuvBuffer;
                prevYuvBuffer = new unsigned char[size];
                prevYuvBufferSize = size;
            }
            // Initialize previous YUV buffer to all zeros
            memset(prevYuvBuffer, 0, yuvBufferSize);
        }

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
        KillSaveThread  = false;
        SaveThread      = new osaThread;
        SaveInitEvent   = new osaThreadSignal;
        NewFrameEvent   = new osaThreadSignal;
        WriteDoneEvent  = new osaThreadSignal;
        SaveThread->Create<svlVideoCodecCVI, int>(this, &svlVideoCodecCVI::SaveProc, 0);
        SaveInitEvent->Wait();
        if (SaveInitialized == false) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: failed to create writer thread" << std::endl;
            break;
        }

        BegPos  = EndPos = Pos = 0;
        Width   = width;
        Height  = height;
        Opened  = true;
	    Writing = true;

        return SVL_OK;
    }

	Close();
	return SVL_FAIL;
}

int svlVideoCodecCVI::Close()
{
    int ret = SVL_OK;
    long long int len;

    if (Opened && Writing) {

        // Stop data saving thread
        KillSaveThread = true;
        if (SaveInitialized) {
            NewFrameEvent->Raise();
            SaveThread->Wait();
            delete SaveThread;
            SaveThread = 0;
        }

        if (File.IsOpen()) {
            while (1) {
                // Store current file position
                FooterOffset = File.GetPos();

                // Write the offset of the last frame
                EndPos --;
                len = sizeof(int);
                if (File.Write(reinterpret_cast<const char*>(&EndPos), len) != len) {
                    ret = SVL_FAIL;
                    CMN_LOG_CLASS_INIT_ERROR << "Close: failed to write `end position`" << std::endl;
                    break;
                }

                // Write frame offsets
                len = (EndPos + 1) * sizeof(long long int);
                if (File.Write(reinterpret_cast<const char*>(FrameOffsets.Pointer()), len) != len) {
                    ret = SVL_FAIL;
                    CMN_LOG_CLASS_INIT_ERROR << "Close: failed to write `frame offsets`" << std::endl;
                    break;
                }

                // Write frame timestamps
                len = (EndPos + 1) * sizeof(double);
                if (File.Write(reinterpret_cast<const char*>(FrameTimestamps.Pointer()), len) != len) {
                    ret = SVL_FAIL;
                    CMN_LOG_CLASS_INIT_ERROR << "Close: failed to write `frame timestamps`" << std::endl;
                    break;
                }

                // Seek back to "frame offsets pointer"
                long long int frameoffsetspointer = FileStartMarker[Version].length() +  // File start marker
                                                    sizeof(unsigned char);               // Differential flag
                if (File.Seek(frameoffsetspointer) != SVL_OK) {
                    ret = SVL_FAIL;
                    CMN_LOG_CLASS_INIT_ERROR << "Close: failed to seek to `frame offsets` placeholder" << std::endl;
                    break;
                }

                // Replace the "footer offset" placeholder with the real data
                len = sizeof(long long int);
                if (File.Write(reinterpret_cast<const char*>(&FooterOffset), len) != len) {
                    ret = SVL_FAIL;
                    CMN_LOG_CLASS_INIT_ERROR << "Close: failed to write `footer offset`" << std::endl;
                    break;
                }

                break;
            }
        }
    }

    File.Close();

    delete SaveInitEvent;
    delete NewFrameEvent;
    delete WriteDoneEvent;
    SaveInitEvent  = 0;
    NewFrameEvent  = 0;
    WriteDoneEvent = 0;

    Version      = -1;
    FooterOffset = 0;
    DataOffset   = 0;
    PartCount    = 0;
    Width        = 0;
    Height       = 0;
    BegPos       = -1;
    EndPos       = -1;
    Pos          = -1;
    Opened       = false;
    Writing      = false;
    Timestamp    = -1.0;

    FrameOffsets.SetSize(0);
    FrameTimestamps.SetSize(0);

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
    if (Version == 0 || Config.Differential) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPos: seeking is not supported in this CVI version" << std::endl;
        return SVL_FAIL;
    }
    if (pos < 0 || pos > EndPos) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPos: position=" << pos << " is out of valid range=[0, " << EndPos << "]" << std::endl;
        return SVL_FAIL;
    }
    Pos = pos;
    return SVL_OK;
}

double svlVideoCodecCVI::GetBegTime() const
{
    if (Opened && !Writing && Version > 1) {
        return FrameTimestamps[0];
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetBegTime: failed to get first timestamp" << std::endl;
    return -1.0;
}

double svlVideoCodecCVI::GetEndTime() const
{
    if (Opened && !Writing && Version > 1) {
        return FrameTimestamps[FrameTimestamps.size() - 1];
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetEndTime: failed to get last timestamp" << std::endl;
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
    CMN_LOG_CLASS_INIT_ERROR << "GetTimeAtPos: failed to get timestamp at position=" << pos << std::endl;
    return -1.0;
}

int svlVideoCodecCVI::GetPosAtTime(const double time) const
{
    if (Opened && !Writing && Version > 1) {

        int begpos     = 0;
        int endpos     = static_cast<int>(FrameTimestamps.size()) - 1;
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
    CMN_LOG_CLASS_INIT_ERROR << "GetPosAtTime: failed to get position at time=" << std::fixed << time << std::endl;
    return -1;
}

svlVideoIO::Compression* svlVideoCodecCVI::GetCompression() const
{
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    // Output settings
    CompressionData* output_data = reinterpret_cast<CompressionData*>(&(compression->data[0]));

    // Generic settings
    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(compression->extension[0]), 0, 16);
    memcpy(&(compression->extension[0]), ".cvi", 4);
    memset(&(compression->name[0]), 0, 64);
    memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    compression->size = size;
    compression->datasize = sizeof(CompressionData);

    // CVI specific settings
    output_data->Level        = Config.Level;
    output_data->Differential = Config.Differential;

    return compression;
}

int svlVideoCodecCVI::SetCompression(const svlVideoIO::Compression *compression)
{
    if (Opened || !compression || compression->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    // Create a safe copy of the string `extension`
    char _ext[16];
    memcpy(_ext, compression->extension, 15);
    _ext[15] = 0;

    std::string extensionlist(GetExtensions());
    std::string extension(_ext);
    extension += ";";
    if (extensionlist.find(extension) == std::string::npos) {
        CMN_LOG_CLASS_INIT_ERROR << "SetCompression: codec parameters do not match this codec" << std::endl;
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(Codec);
    unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    // Local settings
    CompressionData* local_data = reinterpret_cast<CompressionData*>(&(Codec->data[0]));
    // Input settings
    const CompressionData* input_data = reinterpret_cast<const CompressionData*>(&(compression->data[0]));

    // Generic settings
    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".cvi", 4);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = size;
    Codec->datasize = sizeof(CompressionData);

    // CVI specific settings
    if (input_data->Level <= 9) {
        Config.Level = local_data->Level = input_data->Level;
    }
    else {
        local_data->Level = Config.Level;
    }
    if (compression->datasize >= sizeof(CompressionData)) {
        Config.Differential = local_data->Differential = input_data->Differential;
    }
    else {
        // Maintaining compatibility with older versions of the structure
        local_data->Differential = Config.Differential;
    }

    return SVL_OK;
}

int svlVideoCodecCVI::DialogCompression()
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogCompression: codec is already open" << std::endl;
        return SVL_FAIL;
    }

    std::cout << " # Enter compression level [0-9]: ";
    int level = 0;
    while (level < '0' || level > '9') level = cmnGetChar();
    level -= '0';
    std::cout << level << std::endl;

    std::cout << " # Enable differential encoding (seeking not supported) ['y' or other]: ";
    int differential = cmnGetChar();
    if (differential == 'y' || differential == 'Y') {
        differential = 1;
        std::cout << "YES" << std::endl;
    }
    else {
        differential = 0;
        std::cout << "NO" << std::endl;
    }

    svlVideoIO::ReleaseCompression(Codec);
    unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    // Local settings
    CompressionData* local_data = reinterpret_cast<CompressionData*>(&(Codec->data[0]));

    // Generic settings
    std::string name("Multiblock ZLib Compression (YUV422)");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".cvi", 4);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = size;
    Codec->datasize = sizeof(CompressionData);

    // CVI specific settings
    Config.Level        = local_data->Level        = static_cast<unsigned char>(level);
    Config.Differential = local_data->Differential = static_cast<unsigned char>(differential);

	return SVL_OK;
}

double svlVideoCodecCVI::GetTimestamp() const
{
    if (!Opened || Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTimestamp: failed to get timestamp" << std::endl;
        return -1.0;
    }
    return Timestamp;
}

int svlVideoCodecCVI::SetTimestamp(const double timestamp)
{
    if (!Opened || !Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTimestamp: failed to set timestamp" << std::endl;
        return SVL_FAIL;
    }
    Timestamp = timestamp;
    return SVL_OK;
}

int svlVideoCodecCVI::Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize)
{
    if (!procInfo) procInfo = &ProcInfoSingleThread;

    if (videoch >= image.GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!Opened || Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") file needs to be opened for reading" << std::endl;
        return SVL_FAIL;
    }

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    // Allocate image buffer if not done yet
    if (Width  != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        if (noresize) {
            CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") unexpected change in image dimensions" << std::endl;
            return SVL_FAIL;
        }
        image.SetSize(videoch, Width, Height);
    }

    unsigned char* img = image.GetUCharPointer(videoch);
    unsigned int i, compressedpartsize, offset;
    unsigned long longsize;
    long long int len;
    char strbuffer[32];
    int ret = SVL_FAIL;

    if (Version > 0) {
        if (Pos > EndPos) {
            Pos = 0;
            return SVL_VID_END_REACHED;
        }

        // Look up the position in the frame offsets table and move the file pointer
        if (File.Seek(FrameOffsets[Pos]) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to seek to frame=" << Pos << std::endl;
            return SVL_FAIL;
        }

        if (Pos == 0) {
            if (Config.Differential) {
                // Reset previous YUV buffer to all zeros
                memset(prevYuvBuffer, 0, prevYuvBufferSize);
            }
        }
    }
    else {
        if (Pos == 0) {
            // Go to the beginning of the data, just after the header
            if (File.Seek(DataOffset) != SVL_OK) {
                CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to seek to position=" << DataOffset << std::endl;
                return SVL_FAIL;
            }
        }
    }

    while (1) {

        // Read "frame start marker"
        len = FrameStartMarker.length();
        if (File.Read(strbuffer, len) != len) break;
        strbuffer[FrameStartMarker.length()] = 0;
        if (FrameStartMarker.compare(strbuffer) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to read `frame start marker`" << std::endl;
            return SVL_FAIL;
        }

        // Read "timestamp"
        len = sizeof(double);
        if (File.Read(reinterpret_cast<char*>(&Timestamp), len) != len) break;
        if (Timestamp < 0.0) {
            CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to read `frame timestamp`" << std::endl;
            return SVL_FAIL;
        }

        offset = 0;
        for (i = 0; i < PartCount; i ++) {

            // Read "compressed part size"
            len = sizeof(unsigned int);
            if (File.Read(reinterpret_cast<char*>(&compressedpartsize), len) != len) break;
            if (compressedpartsize == 0 || compressedpartsize > comprBufferSize) {
                CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to read `compressed part size`" << std::endl;
                return SVL_FAIL;
            }

            // Read compressed frame part
            len = compressedpartsize;
            if (File.Read(reinterpret_cast<char*>(comprBuffer), len) != len) break;

            // Decompress frame part
            longsize = yuvBufferSize - offset;
            if (uncompress(yuvBuffer + offset, &longsize, comprBuffer, compressedpartsize) != Z_OK) {
                CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to uncompress data" << std::endl;
                return SVL_FAIL;
            }

            if (Config.Differential) {
                // Decode differential encoded data
                DiffDecode(yuvBuffer + offset, prevYuvBuffer + offset, yuvBuffer + offset, longsize);
            }

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
                // Set pointer back to the first frame
                Pos = 0;

                return SVL_VID_END_REACHED;
            }
            else {
                // If it was the first frame, then file is invalid, let it fail
                CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to read first frame" << std::endl;
            }
        }
    }
    else {
        if (ret != SVL_OK) {
            // End of file reached
            if (Pos > 0) {
                // Set pointer back to the first frame
                EndPos = Pos;
                Pos = 0;
                
                return SVL_VID_END_REACHED;
            }
            else {
                // If it was the first frame, then file is invalid, let it fail
                CMN_LOG_CLASS_INIT_ERROR << "Read: (thread=" << procInfo->ID << ") failed to read first frame" << std::endl;
            }
        }
    }

    return ret;
}

int svlVideoCodecCVI::Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch)
{
    if (!procInfo) procInfo = &ProcInfoSingleThread;

    if (videoch >= image.GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!Opened || !Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") file needs to be opened for writing" << std::endl;
        return SVL_FAIL;
    }
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") unexpected change in image dimensions" << std::endl;
        return SVL_FAIL;
    }

    // Check for video saving errors
    if (SaveThreadError) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") error detected on saving thread" << std::endl;
        return SVL_FAIL;
    }

    bool err = false;
    long long int len;

    if (Pos == 0) {

        _OnSingleThread(procInfo)
        {
            // Initialize multithreaded processing
            ComprPartOffset.SetSize(procInfo->count);
            ComprPartSize.SetSize(procInfo->count);

            // Write "part count"
            len = sizeof(unsigned int);
            if (File.Write(reinterpret_cast<char*>(&procInfo->count), len) != len) {
                err = true;
                CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") failed to write `part count`" << std::endl;
            }
        }

        // Synchronize threads
        _SynchronizeThreads(procInfo);

        if (err) return SVL_FAIL;
    }

    const unsigned int procid = procInfo->ID;
    const unsigned int proccount = procInfo->count;
    unsigned int start, end, size, offset;
    unsigned long comprsize;
    int compr = Config.Level;

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

        offset <<= 1; size <<= 1;

        if (Config.Differential) {
            // Encode data using differential coding
            DiffEncode(yuvBuffer + offset, prevYuvBuffer + offset, yuvBuffer + offset, size);
        }

        // Compress part
        if (compress2(comprBuffer + ComprPartOffset[procid], &comprsize, yuvBuffer + offset, size, compr) != Z_OK) {
            err = true;
            CMN_LOG_CLASS_INIT_ERROR << "Write: (thread=" << procInfo->ID << ") failed to compress data" << std::endl;
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
        WriteDoneEvent->Wait();

        // Store current file position in frame offsets table (increase table size if needed)
        if (FrameOffsets.size() <= static_cast<unsigned int>(EndPos)) FrameOffsets.resize(FrameOffsets.size() + 100000);
        FrameOffsets[EndPos] = File.GetPos();

        // Store current timestamp in frame timestamps table (increase table size if needed)
        if (FrameTimestamps.size() <= static_cast<unsigned int>(EndPos)) FrameTimestamps.resize(FrameTimestamps.size() + 100000);
        FrameTimestamps[EndPos] = timestamp;

        // Add "frame start marker"
        memcpy(saveBuffer[savebufferid], FrameStartMarker.c_str(), FrameStartMarker.length());
        SaveBufferUsedSize = static_cast<unsigned int>(FrameStartMarker.length());

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
        NewFrameEvent->Raise();

		EndPos ++; Pos ++;
    }

	return SVL_OK;
}

void svlVideoCodecCVI::SetExtension(const std::string & CMN_UNUSED(extension))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetExtension - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::SetEncoderID(const int & CMN_UNUSED(encoder_id))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetEncoderID - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::SetCompressionLevel(const int & compr_level)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetCompressionLevel - codec is already open" << std::endl;
        return;
    }
    if (compr_level < 0 || compr_level > 9) {
        CMN_LOG_CLASS_INIT_ERROR << "SetCompressionLevel - argument out of range [0, 9]" << std::endl;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetCompressionLevel - called (" << compr_level << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->Level = compr_level;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecCVI::SetQualityBased(const bool & CMN_UNUSED(enabled))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetQualityBased - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::SetTargetQuantizer(const double & CMN_UNUSED(target_quant))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetTargetQuantizer - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::SetDatarate(const int & CMN_UNUSED(datarate))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetDatarate - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::SetKeyFrameEvery(const int & CMN_UNUSED(key_every))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetKeyFrameEvery - feature is not supported by the CVI codec" << std::endl;
}

void svlVideoCodecCVI::IsCompressionLevelEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsCompressionLevelEnabled - called" << std::endl;
    enabled = true;
}

void svlVideoCodecCVI::IsEncoderListEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsEncoderListEnabled - called" << std::endl;
    enabled = false;
}

void svlVideoCodecCVI::IsTargetQuantizerEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsTargetQuantizerEnabled - called" << std::endl;
    enabled = false;
}

void svlVideoCodecCVI::IsDatarateEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsDatarateEnabled - called" << std::endl;
    enabled = false;
}

void svlVideoCodecCVI::IsKeyFrameEveryEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsFramesEveryEnabled - called" << std::endl;
    enabled = false;
}

void svlVideoCodecCVI::GetCompressionLevel(int & compr_level) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetCompressionLevel - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    compr_level = data->Level;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecCVI::GetEncoderList(std::string & encoder_list) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetEncoderList - feature is not supported by the CVI codec" << std::endl;
    encoder_list = "";
}

void svlVideoCodecCVI::GetEncoderID(int & encoder_id) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetEncoderID - feature is not supported by the CVI codec" << std::endl;
    encoder_id = -1;
}

void svlVideoCodecCVI::GetQualityBased(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetQualityBased - feature is not supported by the CVI codec" << std::endl;
    enabled = false;
}

void svlVideoCodecCVI::GetTargetQuantizer(double & target_quant) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetTargetQuantizer - feature is not supported by the CVI codec" << std::endl;
    target_quant = -1.0;
}

void svlVideoCodecCVI::GetDatarate(int & datarate) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetDatarate - feature is not supported by the CVI codec" << std::endl;
    datarate = -1;
}

void svlVideoCodecCVI::GetKeyFrameEvery(int & key_every) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetKeyFrameEvery - feature is not supported by the CVI codec" << std::endl;
    key_every = -1;
}

void svlVideoCodecCVI::DiffEncode(unsigned char* input, unsigned char* previous, unsigned char* output, const unsigned int size)
{
    if (!input || !previous || !output || !size) return;

    unsigned char* previous_temp = previous;
    char* output_c = reinterpret_cast<char*>(output);
    unsigned int pos = size;
    int in_i, prev_i, diff_i;

    while (pos) {
        pos --;

        prev_i = *previous;
        in_i = *input;
        diff_i = in_i - prev_i;

        if (diff_i >= 0) {
            if (diff_i < 64) {
                *output_c = static_cast<char>(diff_i);
            }
            else {
                *output_c = static_cast<char>(64 + (diff_i - 64) / 3);
            }
        }
        else {
            if (diff_i > -64) {
                *output_c = static_cast<char>(diff_i);
            }
            else {
                // Needs to be this convoluted because truncation
                // may happen toward negative infinity in C
                *output_c = static_cast<char>(-64 - ((-64 - diff_i) / 3));
            }
        }

        input    ++;
        previous ++;
        output_c ++;
    }

    // Decode encoded data in order to propagate possible codeing errors
    DiffDecode(output, previous_temp, previous_temp, size);
}

void svlVideoCodecCVI::DiffDecode(unsigned char* input, unsigned char* previous, unsigned char* output, const unsigned int size)
{
    if (!input || !previous || !output || !size) return;

    char* input_c = reinterpret_cast<char*>(input);
    unsigned int pos = size;
    int in_i, prev_i, res;

    while (pos) {
        pos --;

        in_i = *input_c;
        prev_i = *previous;

        if (in_i >= 0) {
            if (in_i < 64) {
                res = prev_i + in_i;
                if (res < 0) res = 0;
                else if (res > 255) res = 255;
                *output = *previous = static_cast<unsigned char>(res);
            }
            else {
                res = prev_i + (in_i - 64) * 3 + 64;
                if (res < 0) res = 0;
                else if (res > 255) res = 255;
                *output = *previous = static_cast<unsigned char>(res);
            }
        }
        else {
            if (in_i > -64) {
                res = prev_i + in_i;
                if (res < 0) res = 0;
                else if (res > 255) res = 255;
                *output = *previous = static_cast<unsigned char>(res);
            }
            else {
                res = prev_i + (in_i + 64) * 3 - 64;
                if (res < 0) res = 0;
                else if (res > 255) res = 255;
                *output = *previous = static_cast<unsigned char>(res);
            }
        }

        input_c  ++;
        previous ++;
        output   ++;
    }
}

void* svlVideoCodecCVI::SaveProc(int CMN_UNUSED(param))
{
    SaveThreadError = false;
    SaveInitialized = true;
    SaveInitEvent->Raise();
    WriteDoneEvent->Raise();

    long long int len;

    while (1) {

        // Wait for new frame to arrive
        NewFrameEvent->Wait();
        if (KillSaveThread || SaveThreadError) break;

        // Write frame
        len = SaveBufferUsedSize;
        if (File.Write(reinterpret_cast<char*>(saveBuffer[SaveBufferUsedID]), len) != len) {
            SaveThreadError = true;
            CMN_LOG_CLASS_INIT_ERROR << "SaveProc: failed to write compressed data" << std::endl;
            return this;
        }

        // Signal that write is done
        WriteDoneEvent->Raise();
    }

    return this;
}

