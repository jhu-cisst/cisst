    /* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Daniel Obenshain & Tom Tantillo
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVideoCodecUDPStream.h"
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlConverters.h>
#include <cisstStereoVision/svlSyncPoint.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>

#include "zlib.h"

typedef struct {
	unsigned int applicationID;
	unsigned int packet;
	unsigned int frame;
	unsigned int indexInFrame;
	unsigned short totalInFrame;
} header;

typedef struct {
//put timestamps here
} timesync;

#if (CISST_OS == CISST_WINDOWS)
#define WINSOCKVERSION MAKEWORD(2,2)

    static int inet_pton4(const char *src, unsigned char *dst)
    {
        static const char digits[] = "0123456789";
        int saw_digit, octets, ch;
        unsigned char tmp[4], *tp;

        saw_digit = 0;
        octets = 0;
        tp = tmp;
        *tp = 0;
        while ((ch = *src++) != '\0') {
            const char *pch;

            if ((pch = strchr(digits, ch)) != NULL) {
                unsigned int val = *tp * 10 + (unsigned int) (pch - digits);

                if (val > 255) return (0);
                *tp = (unsigned char) val;
                if (!saw_digit) {
                    if (++octets > 4) return (0);
                    saw_digit = 1;
                }
            } else if (ch == '.' && saw_digit) {
                if (octets == 4) return (0);
                *++tp = 0;
                saw_digit = 0;
            } else return (0);
        }
        if (octets < 4) return (0);
        memcpy(dst, tmp, 4);
        return (1);
    }

#endif

#if (CISST_OS == CISST_WINDOWS)
    #define __errno         WSAGetLastError()
    #define __ECONNABORTED  WSAECONNABORTED
    #define __EAGAIN        WSATRY_AGAIN
#else
    #define __errno         errno
    #define __ECONNABORTED  ECONNABORTED
    #define __EAGAIN        EAGAIN
#endif

//#define _NET_VERBOSE_

#define MAX_CLIENTS                     5
#define PACKET_SIZE			1316u
#define	DATA_SIZE			(PACKET_SIZE - sizeof(header))
#define BROKEN_FRAME                    1

#define MAX_RATE			120 //in Mbps
#define BURST				3 //this implies that we send 2^BURST packets at once.
#define	WAIT_TIME			((PACKET_SIZE*(1<<BURST)*8*1.0)/(MAX_RATE*1000000)) //in seconds
#define WRAP_AROUND			50000  // corresponds to 13 hours before time origin is reset


/************************************/
/*** svlVideoCodecUDPStream class ***/
/************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecUDPStream, svlVideoCodecBase)

svlVideoCodecUDPStream::svlVideoCodecUDPStream() :
    svlVideoCodecBase(),
    CodecName("CISST Video Stream over UDP"),
    FrameStartMarker("\r\nFrame\r\n"),
    pktcount(0),
    start_t(0),
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
    PacketData(0),
    yuvBuffer(0),
    yuvBufferSize(0),
    comprBuffer(0),
    comprBufferSize(0),
    ServerSocket(-1),
    ServerThread(0),
    ServerInitEvent(0),
    ServerInitialized(false),
    ReceiveBuffer(0),
    SendThreadSync(0),
    ReceiveSocket(-1),
    ReceiveThread(0),
    ReceiveInitEvent(0),
    ReceiveInitialized(false),
    ReceiveSub(0),
    ReceiveSubSync(0)
{
    SetName("CISST Video Stream over UDP");
    SetExtensionList(".ucvi;");
    SetMultithreaded(true);
    SetVariableFramerate(true);

    SendBuffer.SetSize(MAX_CLIENTS);
    SendBuffer.SetAll(0);

    SockAddr = new char[sizeof(sockaddr_in)];
    PacketData = new char[DATA_SIZE];

    ots = new osaTimeServer();
    //ots->SetTimeOrigin();
}

svlVideoCodecUDPStream::~svlVideoCodecUDPStream()
{
    Close();

    if (ots) delete ots;

    delete ReceiveBuffer;
    for (unsigned int i = 0; i < MAX_CLIENTS; i ++) delete SendBuffer[i];
    
    if (yuvBuffer) delete [] yuvBuffer;
    if (comprBuffer) delete [] comprBuffer;
    if (SockAddr) delete [] SockAddr;
    if (PacketData) delete [] PacketData;
}

int svlVideoCodecUDPStream::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened || ParseFilename(filename) != SVL_OK) return SVL_FAIL;

    while (1) {

        Opened = true;
        Writing = false;
        Width = Height = 0;

        // Releasing existing buffers
        delete [] yuvBuffer;
        yuvBuffer = 0;
        yuvBufferSize = 0;

        // Start data receiving thread
        ReceiveInitialized = false;
        KillReceiveThread = false;
        ReceiveThread = new osaThread;
        //ReceiveThreadSync = new osaThread;
        ReceiveInitEvent = new osaThreadSignal;

        ReceiveThread->Create<svlVideoCodecUDPStream, int>(this, &svlVideoCodecUDPStream::ReceiveProc, 0);
        //ReceiveThreadSync->Create<svlVideoCodecUDPStream, int>(this, &svlVideoCodecUDPStream::ReceiveSync, 0);

        ReceiveInitEvent->Wait();
        if (ReceiveInitialized == false) break;

        // Wait until header is received
        while (Width == 0 || Height == 0) osaSleep(0.1);

        Pos = BegPos = EndPos = 0;
        width = Width;
        height = Height;
        framerate = -1.0;

        return SVL_OK;
    }

    Close();
    return SVL_FAIL;
}

int svlVideoCodecUDPStream::Create(const std::string &filename, const unsigned int width, const unsigned int height, const double CMN_UNUSED(framerate))
{
	if (Opened || ParseFilename(filename) != SVL_OK ||
        width < 1 || width > MAX_DIMENSION || height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (!Codec) {
        // Set default compression level to 4
        Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);
        std::string name("CISST Video Stream over UDP");
        memset(&(Codec->extension[0]), 0, 16);
        memcpy(&(Codec->extension[0]), ".ucvi", 3);
        memset(&(Codec->name[0]), 0, 64);
        memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
        Codec->size = sizeof(svlVideoIO::Compression);
        Codec->datasize = 1;
        Codec->data[0] = 4;
    }

    unsigned int size;

    while (1) {

        Opened = true;
	    Writing = true;

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

        // Allocate streaming buffers if not done yet
        for (unsigned int i = 0; i < MAX_CLIENTS; i ++) {
            if (!SendBuffer[i]) {
                SendBuffer[i] = new svlBufferMemory(size);
            }
            else if (SendBuffer[i] && SendBuffer[i]->GetMaxSize() < size) {
                delete SendBuffer[i];
                SendBuffer[i] = new svlBufferMemory(size);
            }
        }

        // Start data saving thread
        ServerInitialized = false;
        KillServerThread = false;
        ServerThread = new osaThread;
        //ServerThreadSync = new osaThread;
        ServerInitEvent = new osaThreadSignal;
        ServerThread->Create<svlVideoCodecUDPStream, unsigned short>(this, &svlVideoCodecUDPStream::ServerProc, SocketPort);
        ServerInitEvent->Wait();
        if (ServerInitialized == false) break;

        BegPos = EndPos = Pos = 0;
        Width = width;
        Height = height;

        return SVL_OK;
    }

	Close();
	return SVL_FAIL;
}

int svlVideoCodecUDPStream::Close()
{
    if (Opened) {
        if (Writing) {
            // Shut down server
            if (ServerThread && ServerInitialized) {
                KillServerThread = true;
                ServerThread->Wait();
                delete ServerThread;
                ServerThread = 0;
                delete ServerInitEvent;
                ServerInitEvent = 0;
            }
        }
        else {
            // Shut down receiver
            if (ReceiveThread && ReceiveInitialized) {
                KillReceiveThread = true;
                ReceiveThread->Wait();
                delete ReceiveThread;
                std::cout << "destroyed ReceiveThread..." << std::endl;
                ReceiveThread = 0;
                delete ReceiveInitEvent;
                ReceiveInitEvent = 0;
            }
        }
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

int svlVideoCodecUDPStream::GetBegPos() const
{
    return BegPos;
}

int svlVideoCodecUDPStream::GetEndPos() const
{
    return EndPos;
}

int svlVideoCodecUDPStream::GetPos() const
{
    return Pos;
}

svlVideoIO::Compression* svlVideoCodecUDPStream::GetCompression() const
{
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    unsigned int size = sizeof(svlVideoIO::Compression);
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    std::string name("CISST Video Stream over UDP");
    memset(&(compression->extension[0]), 0, 16);
    memcpy(&(compression->extension[0]), ".ucvi", 3);
    memset(&(compression->name[0]), 0, 64);
    memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    compression->size = size;
    compression->datasize = 1;
    if (Codec) compression->data[0] = Codec->data[0];
    else compression->data[0] = 4; // Set default compression level to 4

    return compression;
}

int svlVideoCodecUDPStream::SetCompression(const svlVideoIO::Compression *compression)
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

    std::string name("CISST Video Stream over UDP");
    memset(&(Codec->extension[0]), 0, 16);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = sizeof(svlVideoIO::Compression);
    Codec->datasize = 1;
    if (compression->data[0] <= 9) Codec->data[0] = compression->data[0];
    else Codec->data[0] = 4;

    return SVL_OK;
}

int svlVideoCodecUDPStream::DialogCompression()
{
    if (Opened) return SVL_FAIL;

    std::cout << std::endl << " # Enter compression level [0-9]: ";
    int level = 0;
    while (level < '0' || level > '9') level = cmnGetChar();
    level -= '0';
    std::cout << level << std::endl;

    svlVideoIO::ReleaseCompression(Codec);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);

    std::string name("CISST Video Stream over UDP");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".ucvi", 3);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = sizeof(svlVideoIO::Compression);
    Codec->datasize = 1;
    Codec->data[0] = static_cast<unsigned char>(level);

	return SVL_OK;
}

double svlVideoCodecUDPStream::GetTimestamp() const
{
    if (!Opened || Writing) return -1.0;
    return Timestamp;
}

int svlVideoCodecUDPStream::SetTimestamp(const double timestamp)
{
    if (!Opened || !Writing) return SVL_FAIL;
    Timestamp = timestamp;
    return SVL_OK;
}

int svlVideoCodecUDPStream::Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || Writing) return SVL_FAIL;

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    unsigned int i, used, width, height, partcount, compressedpartsize, offset = 0, strmoffset;
    unsigned long longsize;
    unsigned char *img, *strmbuf = 0;
    int ret = SVL_FAIL;

    // Wait until new frame is received
    while (!strmbuf) {
        strmbuf = ReceiveBuffer->Pull(used, 0.1);
    }
    if (!strmbuf || !used) return SVL_FAIL;

    while (1) {
        // file start marker
        strmoffset = FrameStartMarker.length();

        // frame data size after file start marker
        strmoffset += sizeof(unsigned int);

        // image width
        width = reinterpret_cast<unsigned int*>(strmbuf + strmoffset)[0];
        strmoffset += sizeof(unsigned int);

        // image height
        height = reinterpret_cast<unsigned int*>(strmbuf + strmoffset)[0];
        strmoffset += sizeof(unsigned int);

        // timestamp
        Timestamp = reinterpret_cast<double*>(strmbuf + strmoffset)[0];
        strmoffset += sizeof(double);
        // Do not return timestamp for now...
        Timestamp = -1;

        // part count
        partcount = reinterpret_cast<unsigned int*>(strmbuf + strmoffset)[0];
        strmoffset += sizeof(unsigned int);

        // Allocate image buffer if not done yet
        if (width != image.GetWidth(videoch) || height != image.GetHeight(videoch)) {
            if (noresize) break;
            image.SetSize(videoch, width, height);
        }

        img = image.GetUCharPointer(videoch);

        for (i = 0; i < partcount; i ++) {
            // compressed part size
            compressedpartsize = reinterpret_cast<unsigned int*>(strmbuf + strmoffset)[0];
            strmoffset += sizeof(unsigned int);

            // Decompress frame part
            longsize = yuvBufferSize - offset;
            if (uncompress(yuvBuffer + offset, &longsize, strmbuf + strmoffset, compressedpartsize) != Z_OK) break;

            // Convert YUV422 planar to RGB format
            svlConverter::YUV422PtoRGB24(yuvBuffer + offset, img + offset * 3 / 2, longsize >> 1);

            strmoffset += compressedpartsize;
            offset += longsize;
        }
        if (i < PartCount) break;

        ret = SVL_OK;

        break;
    }

    return ret;
}

int svlVideoCodecUDPStream::Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || !Writing) return SVL_FAIL;
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) return SVL_FAIL;

    bool err = false;

    if (Pos == 0) {

        _OnSingleThread(procInfo)
        {
            // Initialize multithreaded processing
            ComprPartOffset.SetSize(procInfo->count);
            ComprPartSize.SetSize(procInfo->count);
        }

        // Synchronize threads
        _SynchronizeThreads(procInfo);

        if (err) return SVL_FAIL;
    }

    const unsigned int procid = procInfo->ID;
    const unsigned int proccount = procInfo->count;
    unsigned int i, start, end, size, offset;
    unsigned char* strmbuf = SendBuffer[0]->GetPushBuffer();
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
        const double timestamp = image.GetTimestamp();
        unsigned int used;

        // Add "frame start marker"
        memcpy(strmbuf, FrameStartMarker.c_str(), FrameStartMarker.length());
        used = FrameStartMarker.length();

        // Add "data size after frame start marker"
        size = sizeof(unsigned int) * (4 + proccount) + sizeof(double);
        for (i = 0; i < proccount; i ++) size += ComprPartSize[i];
        memcpy(strmbuf + used, &size, sizeof(unsigned int));
        used += sizeof(unsigned int);

        // Add "width"
        memcpy(strmbuf + used, &Width, sizeof(unsigned int));
        used += sizeof(unsigned int);

        // Add "height"
        memcpy(strmbuf + used, &Height, sizeof(unsigned int));
        used += sizeof(unsigned int);

        // Add "timestamp"
        memcpy(strmbuf + used, &timestamp, sizeof(double));
        used += sizeof(double);

        // Add "partcount"
        memcpy(strmbuf + used, &proccount, sizeof(unsigned int));
        used += sizeof(unsigned int);

        for (i = 0; i < proccount; i ++) {
            // Add "compressed part size"
            memcpy(strmbuf + used, &(ComprPartSize[i]), sizeof(unsigned int));
            used += sizeof(unsigned int);

            // Add compressed frame
            memcpy(strmbuf + used, comprBuffer + ComprPartOffset[i], ComprPartSize[i]);
            used += ComprPartSize[i];
        }

        // Signal data sending threads
        for (i = 1; i < MAX_CLIENTS; i ++) {
            if (SendConnection[i] >= 0) {
                memcpy(SendBuffer[i]->GetPushBuffer(), strmbuf, used);
                SendBuffer[i]->Push(used);
            }
        }
        SendBuffer[0]->Push(used);

		EndPos ++; Pos ++;
    }

	return SVL_OK;
}

void svlVideoCodecUDPStream::SetExtension(const std::string & extension)
{
}

void svlVideoCodecUDPStream::SetEncoderID(const int & encoder_id)
{
}

void svlVideoCodecUDPStream::SetCompressionLevel(const int & compr_level)
{
}

void svlVideoCodecUDPStream::SetQualityBased(const bool & enabled)
{
}

void svlVideoCodecUDPStream::SetTargetQuantizer(const double & target_quant)
{
}

void svlVideoCodecUDPStream::SetDatarate(const int & datarate)
{
}

void svlVideoCodecUDPStream::SetKeyFrameEvery(const int & key_every)
{
}

void svlVideoCodecUDPStream::IsCompressionLevelEnabled(bool & enabled) const
{
}

void svlVideoCodecUDPStream::IsEncoderListEnabled(bool & enabled) const
{
}

void svlVideoCodecUDPStream::IsTargetQuantizerEnabled(bool & enabled) const
{
}

void svlVideoCodecUDPStream::IsDatarateEnabled(bool & enabled) const
{
}

void svlVideoCodecUDPStream::IsKeyFrameEveryEnabled(bool & enabled) const
{
}

void svlVideoCodecUDPStream::GetCompressionLevel(int & compr_level) const
{
}

void svlVideoCodecUDPStream::GetEncoderList(std::string & encoder_list) const
{
}

void svlVideoCodecUDPStream::GetEncoderID(int & encoder_id) const
{
}

void svlVideoCodecUDPStream::GetQualityBased(bool & enabled) const
{
}

void svlVideoCodecUDPStream::GetTargetQuantizer(double & target_quant) const
{
}

void svlVideoCodecUDPStream::GetDatarate(int & datarate) const
{
}

void svlVideoCodecUDPStream::GetKeyFrameEvery(int & key_every) const
{
}

void* svlVideoCodecUDPStream::ServerProc(unsigned short port)
{
    unsigned int clientid;
	int ret;

#if (CISST_OS == CISST_WINDOWS)
    bool wsa_running = false;
#endif
    bool socket_open = false;

#if (CISST_OS == CISST_WINDOWS)
        WSADATA wsaData;
        if (WSAStartup(WINSOCKVERSION, &wsaData) != 0) {
#ifdef _NET_VERBOSE_
            std::cerr << "svlVideoCodecUDPStream::ServerProc - WSAStartup failed" << std::endl;
#endif
        }
        wsa_running = true;
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ServerProc - WSAStartup success" << std::endl;
#endif
#endif

        ServerSocket = socket(PF_INET, SOCK_DGRAM, 0);
        if (ServerSocket < 0) {
#ifdef _NET_VERBOSE_
            std::cerr << "svlVideoCodecUDPStream::ServerProc - cannot create socket" << std::endl;
#endif
        }
        socket_open = true;
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ServerProc - socket created" << std::endl;
#endif

        memset(&s_address, 0, sizeof(sockaddr_in));

        s_address.sin_family = AF_INET;
        s_address.sin_port = htons(port);

#if (CISST_OS == CISST_WINDOWS)
        ret = inet_pton4(SocketAddress.c_str(), (unsigned char*)(&s_address.sin_addr));
#else
        ret = inet_pton(AF_INET, SocketAddress.c_str(), &s_address.sin_addr);
#endif

#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ServerProc - bind success (port=" << port << ")" << std::endl;
#endif

        ServerInitialized = true;
        ServerInitEvent->Raise();

        // Create client pool
        SendThread.SetSize(MAX_CLIENTS);
        SendConnection.SetSize(MAX_CLIENTS);
        KillSendThread.SetSize(MAX_CLIENTS);
        for (clientid = 0; clientid < MAX_CLIENTS; clientid ++) {
            SendThread[clientid]      = new osaThread;
            SendConnection[clientid]  = -1;
            KillSendThread[clientid]  = false;
        }



            clientid = 0;

            KillSendThread[clientid] = false;
            SendConnection[clientid] = ServerSocket;
            SendThread[clientid]->Create<svlVideoCodecUDPStream, unsigned int>(this, &svlVideoCodecUDPStream::SendProc, clientid);

        while(!KillServerThread) {
                osaSleep(0.1);
	}

    if (ServerInitialized) {
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ServerProc - shutting down all connections" << std::endl;
#endif
        // Destroy client pool
        for (clientid = 0; clientid < SendThread.size(); clientid ++) {
            if (SendConnection[clientid] >= 0) {
                KillSendThread[clientid] = true;
                SendThread[clientid]->Wait();
            }
            delete SendThread[clientid];
            SendThread[clientid] = 0;
        }
        ServerInitialized = false;
    }
    else {
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ServerProc - server failed to initialize" << std::endl;
#endif
        ServerInitEvent->Raise();
    }

#if (CISST_OS == CISST_WINDOWS)
    if (wsa_running) WSACleanup();
#endif

    ServerSocket = -1;

    return this;
}

void* svlVideoCodecUDPStream::SendProc(unsigned int clientid)
{
    unsigned char* strmbuf;
	unsigned char localbuf[PACKET_SIZE];
    unsigned int used = 0;
    int ret;
        double then;
	sizepkt = 0;

	header hdr;
	hdr.frame = 1;
	hdr.packet = 1;
	hdr.applicationID = 25;
	hdr.indexInFrame = 0;
	hdr.totalInFrame = 0;

    while (!KillSendThread[clientid]) {


        // Wait until new frame is acquired
        strmbuf = 0;
        while (!strmbuf && !KillSendThread[clientid]) {
            strmbuf = SendBuffer[clientid]->Pull(used, 0.1);
        }

        if (KillSendThread[clientid]) { break; }

#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::SendProc - sending frame (" << used << " bytes)" << std::endl;
#endif

			unsigned int i;
			unsigned int ssize = DATA_SIZE;
			hdr.indexInFrame = 0;
			hdr.frame++;
			// this is the ceiling function to determine number of packets total to be sent in this frame
			hdr.totalInFrame = (used+(DATA_SIZE-1))/DATA_SIZE;
			for (i = 0; i < used; i += DATA_SIZE) {
				memset(localbuf,0,PACKET_SIZE);
				memcpy(localbuf,&hdr,sizeof(header));

				if (used - i >= DATA_SIZE) {
					memcpy(localbuf+sizeof(header),strmbuf + i,DATA_SIZE);
				}
				else {
					memcpy(localbuf+sizeof(header),strmbuf + i,used - i);
					ssize = used - i;
				}
				hdr.indexInFrame++;
				hdr.packet++;
				if ( ((hdr.indexInFrame >> BURST) << BURST) == hdr.indexInFrame) { 
					//figure out ''then''
					then = ots->GetRelativeTime() + WAIT_TIME;
					//while not then;
					while(ots->GetRelativeTime() < then) osaSleep(0.001);
					//std::cout << ots->GetRelativeTime() << ", " << WAIT_TIME << std::endl;

				
				} //flow control
				ret = sendto(SendConnection[clientid], reinterpret_cast<const char*>(localbuf), ssize + sizeof(header), 
								0, (sockaddr*)&s_address, sizeof(s_address));

				if (ret > 0) {
					if (pktcount == 0)
						start_t = osaGetTime();
					pktcount++;
					sizepkt += ret;
					if ( ((pktcount >> 14) << 14) == pktcount) {
#ifdef _NET_VERBOSE_
                                                std::cout << "sent packet #" << pktcount << " and BW = " << (sizepkt*8) / (1000000*(osaGetTime()-start_t)) << " Mbps. Most recent frame was "<< (used/DATA_SIZE +1) << " packets" << std::endl;
#endif
						sizepkt = 0;
						start_t = osaGetTime();
					}
				}

				if (ret < 0) {
#ifdef _NET_VERBOSE_
                                        std::cerr << "svlVideoCodecUDPStream::SendProc - send failed to " << SendConnection[clientid] << "(" << __errno << ")" << std::endl;
#endif
					KillSendThread[clientid] = true;
					break;
				}

			}
			if (ots->GetRelativeTime() > WRAP_AROUND) {
				ots->SetTimeOrigin();
			}
    }

#if (CISST_OS == CISST_WINDOWS)
    closesocket(SendConnection[clientid]);
#else
    close(SendConnection[clientid]);
#endif

    SendConnection[clientid] = -1;

#ifdef _NET_VERBOSE_
    std::cerr << "svlVideoCodecUDPStream::SendProc - client (" << clientid << ") shut down" << std::endl;
#endif

    return this;
}

void* svlVideoCodecUDPStream::ReceiveProc(int CMN_UNUSED(param))
{
    int ret;
#if (CISST_OS == CISST_WINDOWS)
    bool wsa_running      = false;
#endif
    bool socket_open      = false;
    bool socket_connected = false;

#if (CISST_OS == CISST_WINDOWS)
        WSADATA wsaData;
        if (WSAStartup(WINSOCKVERSION, &wsaData) != 0) {
#ifdef _NET_VERBOSE_
            std::cerr << "svlVideoCodecUDPStream::ReceiveProc - WSAStartup failed" << std::endl;
#endif
            exit(1);
        }
        wsa_running = true;
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ReceiveProc - WSAStartup success" << std::endl;
#endif
#endif

        ReceiveSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (ReceiveSocket < 0) {
            std::cout << "svlVideoCodecUDPStream::ReceiveProc - cannot create socket" << std::endl;
            exit(1);
        }
        socket_open = true;
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ReceiveProc - socket created" << std::endl;
#endif

        memset(&r_address, 0, sizeof(sockaddr_in));
        r_address.sin_family = AF_INET;
        r_address.sin_port = htons(SocketPort);
        s_address.sin_addr.s_addr = INADDR_ANY;

        ret = bind(ReceiveSocket, (sockaddr*)(&r_address), sizeof(sockaddr_in));
        if (ret < 0) {
                std::cout << "svlVideoCodecUDPStream::ReceiveProc - bind failed (port=" << SocketPort << ")" << std::endl;
#if (CISST_OS == CISST_WINDOWS)
                std::cerr << "Windows reports error " << WSAGetLastError() << std::endl;
#endif
//#endif
            exit(1);
        }
        
        ReceiveSub      = new osaThread;
        KillReceiveSub  = false;
        socket_connected = true;
        ReceiveInitialized = true;
        ReceiveInitEvent->Raise();
        ReceiveSub->Create<svlVideoCodecUDPStream,unsigned int>(this, &svlVideoCodecUDPStream::Receive,0);

        while (!KillReceiveThread) {
            osaSleep(0.1);
        }

        if (ReceiveInitialized) {
            // Destroy client pool
            KillReceiveSub = true;
            ReceiveSub->Wait();
            delete ReceiveSub;
            ReceiveSub = 0;
            std::cout << "destroyed ReceiveSub..." << std::endl;
            ReceiveInitialized = false;
        }
        else {
            ReceiveInitEvent->Raise();
        }

    if (!socket_open) {
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecUDPStream::ReceiveProc - client failed to initialize" << std::endl;
#endif
        ReceiveInitEvent->Raise();
    }

if (socket_open) {
#if (CISST_OS == CISST_WINDOWS)
        closesocket(ReceiveSocket);
#else
        close(ReceiveSocket);
#endif
}
#if (CISST_OS == CISST_WINDOWS)
    if (wsa_running) WSACleanup();
#endif

    ReceiveSocket = -1;
    return this;
}

void* svlVideoCodecUDPStream::Receive(unsigned int dummy)
{
    const unsigned char* fsm = reinterpret_cast<const unsigned char*>(FrameStartMarker.c_str());
    const unsigned int fsm_len = FrameStartMarker.length();
    unsigned char* framebuf;
    unsigned char localbuf[PACKET_SIZE];
    unsigned int size = PACKET_SIZE;
    unsigned int framesize = 0;
    bool framestart = false;
    bool started = false;
    fd_set mask;
    int ret;
    header hdr;
    sizepkt = 0;
    struct timeval select_to;

    while (!KillReceiveSub) {
        // Wait for new packet
        FD_ZERO(&mask);
        FD_SET(ReceiveSocket, &mask);
        select_to.tv_sec = 1;
        select_to.tv_usec = 0;
        ret = select(FD_SETSIZE, &mask, 0, 0, &select_to);
        if (ret < 0) {
            std::cerr << "svlVideoCodecUDPStream::Receive - select failed (" << __errno << ")" << std::endl;
            KillReceiveSub = true;
            break;
        }

        // Receive packet
        if (FD_ISSET(ReceiveSocket, &mask) == 0) {
#ifdef _NET_VERBOSE_
            std::cout << "svlVideoCodecUDPStream::Receive - select timed out" << std::endl;
#endif
            continue;
        }

        ret = recv(ReceiveSocket,
                   reinterpret_cast<char*>(localbuf),
                   PACKET_SIZE,
                   0);
        if (ret == 0) {
            std::cerr << "svlVideoCodecUDPStream::Receive - connection terminated" << std::endl;
            KillReceiveSub = true;
            break;
        }
        else if (ret > 0) {
                if (pktcount == 0)
                        start_t = osaGetTime();
                pktcount++;
                sizepkt += ret;
                if ( ((pktcount >> 13) << 13) == pktcount) {
                        //std::cerr << "received packet #" << pktcount << " and BW = " << (sizepkt*8) / (1000000*(osaGetTime()-start_t)) << " Mbps" << std::endl;
                        sizepkt = 0;
                        start_t = osaGetTime();
                }

            // Check if it starts with a "frame start marker"
            framestart = CompareData(localbuf+sizeof(header), fsm, fsm_len);

            if (!framestart) {
                if (!started) {
                    // Looking for "frame start marker"
					// If we miss one of the framestart packets, we currently
					//		skip all of the remaining packets for that frame,
					//		essentially dropping that frame, and wait for the next
					//		one.
                    continue;
                }
            }
            else {
                if (started) {
                    std::cout << "svlVideoCodecUDPStream::Receive - broken frame" << std::endl;
                    size = PACKET_SIZE;
                    framesize = 0;
                    framestart = false;
                    started = false;
                    continue;
                }
                else {
                    // Extract dimensions from header
                    int offset = fsm_len + sizeof(unsigned int) + sizeof(header);
                    const unsigned int w = reinterpret_cast<unsigned int*>(localbuf + offset)[0];
                    offset += sizeof(unsigned int);
                    const unsigned int h = reinterpret_cast<unsigned int*>(localbuf + offset)[0];

                    size = w * h * 2;
                    if (yuvBuffer && yuvBufferSize < size) {
                        delete [] yuvBuffer;
                        yuvBuffer = 0;
                        yuvBufferSize = 0;
                    }
                    if (yuvBuffer == 0 || yuvBufferSize == 0) {
                        yuvBuffer = new unsigned char[size];
                        yuvBufferSize = size;
                    }
                    size += size / 100 + 4096;
                    if (!ReceiveBuffer) {
                        ReceiveBuffer = new svlBufferMemory(size);
                    }
                    else if (ReceiveBuffer && ReceiveBuffer->GetMaxSize() < size) {
                        delete ReceiveBuffer;
                        ReceiveBuffer = new svlBufferMemory(size);
                    }

                    framebuf = ReceiveBuffer->GetPushBuffer();

                    Width = w;
                    Height = h;

                    //Possible problem: we think this hasn't been set yet and won't be until the memcopy below
                    // Get "data size after frame start marker"
                    framesize = reinterpret_cast<unsigned int*>(localbuf + sizeof(header) + fsm_len)[0] + fsm_len;
                    // Don't request the rest of the data if buffer size is too small
                    if (framesize > size) {
                        std::cout << "svlVideoCodecUDPStream::Receive - received data larger than buffer" << std::endl;
                        break;
                    }

                    // Signal "frame start"
                    size = 0;
                    started = true;
                }
            }
            memcpy(&hdr, localbuf, sizeof(header));
            memcpy(framebuf + (hdr.indexInFrame*DATA_SIZE), localbuf+sizeof(header), ret-sizeof(header));
            size += ret-sizeof(header);

            if (framesize == size) {
                //std::cout << "svlVideoCodecUDPStream::Receive - frame received (" << framesize << ")" << std::endl;
                ReceiveBuffer->Push(framesize);
                size = PACKET_SIZE;
                framesize = 0;
                framestart = false;
                started = false;
                continue;
            }
        }
        else {
            int err = __errno;
            if (err == __EAGAIN) {
                std::cout << "svlVideoCodecUDPStream::Receive - recv failed, retrying..." << std::endl;
            }
            else {
                if (err == __ECONNABORTED) {
                    std::cout << "svlVideoCodecUDPStream::Receive - recv failed, connection aborted" << std::endl;
                }
                else {
                    std::cout << "svlVideoCodecUDPStream::Receive - recv failed (" << err << ")" << std::endl;
                }
                KillReceiveSub = true;
                break;
            }
        }
    }
    return this;
}

bool svlVideoCodecUDPStream::CompareData(const unsigned char* data1, const unsigned char* data2, unsigned int size)
{
    while (size) {
        if (data1[size - 1] != data2[size - 1]) break;
        size --;
    }
    return size ? false : true;
}

int svlVideoCodecUDPStream::ParseFilename(const std::string & filename)
{
    // Port is "CISST" on numerical keypad by default
    SocketAddress.clear();
    SocketPort = 24778;

    size_t pos = filename.find('@');
    if (pos != std::string::npos) {
        // Extract address string (substring before '@')
        SocketAddress = filename.substr(0, pos);

        // From DanO and Tom (02-22-2011)
        if (SocketAddress.find_last_of("/\\")) {
            SocketAddress = SocketAddress.substr(SocketAddress.find_last_of("/\\") + 1);
        }

        // Extract port (numerical string after '@')
        pos += 1;
        unsigned int ui_port = 0;
        while (ui_port < 65536 && filename[pos] >= '0' && filename[pos] <= '9') {
            ui_port *= 10;
            ui_port += filename[pos] - '0';
            pos ++;
        }
        if (ui_port > 0 && ui_port < 65536) SocketPort = static_cast<unsigned short>(ui_port);
    }
    else {
        // If no port is specified, then extract
        // the whole string before the extension
        pos = filename.rfind('.');
        if (pos != std::string::npos) SocketAddress = filename.substr(0, pos);
    }

    return SVL_OK;
}

