/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi & Min Yang Jung
  Created on: 2010

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
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

#include "zlib.h"

#if (CISST_OS == CISST_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define WINSOCKVERSION MAKEWORD(2,2)
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <errno.h>
#endif

#define _NET_VERBOSE_

#define PACKET_ID_SIZE      4u
#define PACKET_SIZE         1400u
#define PACKET_DATA_SIZE    1396u

#define BROKEN_FRAME        1
#define MAX_UDP_BURST       8192
#define UDP_BURST_WAIT      0.0001


/*************************************/
/*** svlVideoCodecUDPStream class ****/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlVideoCodecUDPStream)

svlVideoCodecUDPStream::svlVideoCodecUDPStream() :
    svlVideoCodecBase(),
    cmnGenericObject(),
    CodecName("CISST Video Stream over UDP"),
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
    PacketData(0),
    streamingBuffer(0),
    streamingBufferSize(0)
{
    SetName("CISST Video Stream over UDP");
    SetExtensionList(".ncvi;");
    SetMultithreaded(true);

    SockAddr = new char[sizeof(sockaddr_in)];
    PacketData = new char[PACKET_SIZE];
}

svlVideoCodecUDPStream::~svlVideoCodecUDPStream()
{
    Close();
    if (yuvBuffer) delete [] yuvBuffer;
    if (comprBuffer) delete [] comprBuffer;
    if (streamingBuffer) delete streamingBuffer;
    if (SockAddr) delete [] SockAddr;
    if (PacketData) delete [] PacketData;
}

int svlVideoCodecUDPStream::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened || ParseFilename(filename) != SVL_OK) return SVL_FAIL;

    unsigned char* buffer;
    unsigned int size;

    while (1) {

        Opened = true;
        Writing = false;

        // Releasing existing buffers
        delete [] yuvBuffer;
        yuvBuffer = 0;
        yuvBufferSize = 0;
        delete streamingBuffer;
        streamingBuffer = 0;
        streamingBufferSize = 0;
        ReceivedWidth = ReceivedHeight = 0;

        // Start data receiving thread
        StreamingInitialized = false;
        KillStreamingThread = false;
        StreamingThread.Create<svlVideoCodecUDPStream, int>(this, &svlVideoCodecUDPStream::ReceiveProc, 0);
        StreamingInitEvent.Wait();
        if (StreamingInitialized == false) break;

        // Wait until header is received
        while (ReceivedWidth == 0 || ReceivedHeight == 0) osaSleep(0.1);
        Width = ReceivedWidth;
        Height = ReceivedHeight;

        // Allocate buffers for the specific image size
        size = Width * Height * 2;
        buffer = new unsigned char[size];
        yuvBufferSize = size;
        yuvBuffer = buffer;
        size = yuvBufferSize + yuvBufferSize / 100 + 4096 + PACKET_ID_SIZE;
        streamingBuffer = new svlBufferMemory(size);
        streamingBufferSize = size;

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
        width < 1 || width > 4096 || height < 1 || height > 4096) return SVL_FAIL;

    if (!Codec) {
        // Set default compression level to 4
        Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[sizeof(svlVideoIO::Compression)]);
        std::string name("CISST Video Stream (CVI)");
        memset(&(Codec->extension[0]), 0, 16);
        memcpy(&(Codec->extension[0]), ".ncvi", 5);
        memset(&(Codec->name[0]), 0, 64);
        memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
        Codec->size = sizeof(svlVideoIO::Compression);
        Codec->supports_timestamps = true;
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
        size += PACKET_ID_SIZE;
        if (!streamingBuffer) {
            streamingBuffer = new svlBufferMemory(size);
            streamingBufferSize = size;
        }
        else if (streamingBuffer && streamingBufferSize < size) {
            delete streamingBuffer;
            streamingBuffer = new svlBufferMemory(size);
            streamingBufferSize = size;
        }

        StreamingBufferUsedSize = 0;
        StreamingBufferUsedID = 0;

        // Start data saving thread
        StreamingInitialized = false;
        KillStreamingThread = false;
        StreamingThread.Create<svlVideoCodecUDPStream, int>(this, &svlVideoCodecUDPStream::SendProc, 0);
        StreamingInitEvent.Wait();
        if (StreamingInitialized == false) break;

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
    if (Opened && Writing) {

        // Stop data saving thread
        KillStreamingThread = true;
        if (StreamingInitialized) {
            StreamingThread.Wait();
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
    
    std::string name("CISST Video Stream (CVI)");
    memset(&(compression->extension[0]), 0, 16);
    memcpy(&(compression->extension[0]), ".ncvi", 4);
    memset(&(compression->name[0]), 0, 64);
    memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    compression->size = size;
    compression->supports_timestamps = true;
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

    std::string name("CISST Video Stream (CVI)");
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

    std::string name("CISST Video Stream (CVI)");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".ncvi", 5);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = sizeof(svlVideoIO::Compression);
    Codec->supports_timestamps = true;
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
    if (procInfo && procInfo->id != 0) return SVL_OK;

    unsigned int i, width, height, partcount, compressedpartsize, offset = 0, strmoffset;
    unsigned long longsize;
    unsigned char *img, *strmbuf = 0;
    int ret = SVL_FAIL;

    // Wait until new frame is received
    while (!strmbuf) {
        strmbuf = streamingBuffer->Pull(true, 0.1);
    }
    strmbuf += PACKET_ID_SIZE; // Skip the work room left before the data for the packet ID

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

    // Check for video saving errors
    if (StreamingThreadError) return SVL_FAIL;

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

    const unsigned int procid = procInfo->id;
    const unsigned int proccount = procInfo->count;
    unsigned int i, start, end, size, offset;
    unsigned char* strmbuf = streamingBuffer->GetPushBuffer();
    unsigned long comprsize;
    int compr = Codec->data[0];

    strmbuf += PACKET_ID_SIZE; // Skip the work room left before the data for the packet ID

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

        // Add "frame start marker"
        memcpy(strmbuf, FrameStartMarker.c_str(), FrameStartMarker.length());
        StreamingBufferUsedSize = FrameStartMarker.length();

        // Add "data size after frame start marker"
        size = sizeof(unsigned int) * (4 + proccount) + sizeof(double);
        for (i = 0; i < proccount; i ++) size += ComprPartSize[i];
        memcpy(strmbuf + StreamingBufferUsedSize, &size, sizeof(unsigned int));
        StreamingBufferUsedSize += sizeof(unsigned int);

        // Add "width"
        memcpy(strmbuf + StreamingBufferUsedSize, &Width, sizeof(unsigned int));
        StreamingBufferUsedSize += sizeof(unsigned int);

        // Add "height"
        memcpy(strmbuf + StreamingBufferUsedSize, &Height, sizeof(unsigned int));
        StreamingBufferUsedSize += sizeof(unsigned int);

        // Add "timestamp"
        memcpy(strmbuf + StreamingBufferUsedSize, &timestamp, sizeof(double));
        StreamingBufferUsedSize += sizeof(double);

        // Add "partcount"
        memcpy(strmbuf + StreamingBufferUsedSize, &proccount, sizeof(unsigned int));
        StreamingBufferUsedSize += sizeof(unsigned int);

        for (i = 0; i < proccount; i ++) {
            // Add "compressed part size"
            memcpy(strmbuf + StreamingBufferUsedSize, &(ComprPartSize[i]), sizeof(unsigned int));
            StreamingBufferUsedSize += sizeof(unsigned int);

            // Add compressed frame
            memcpy(strmbuf + StreamingBufferUsedSize, comprBuffer + ComprPartOffset[i], ComprPartSize[i]);
            StreamingBufferUsedSize += ComprPartSize[i];
        }

        // Signal data saving thread to start writing
        streamingBuffer->Push();

		EndPos ++; Pos ++;
    }

	return SVL_OK;
}

int svlVideoCodecUDPStream::CreateServer()
{
#if (CISST_OS == CISST_WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(WINSOCKVERSION, &wsaData) != 0) {
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecCVINet::CreateServer - WSAStartup failed" << std::endl;
#endif
        return SVL_FAIL;
    }
#endif

    Socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socket < 0) return SVL_FAIL;

    hostent hostEnt;
    long hostNum;
    hostent* pHostEnt = gethostbyname(SocketAddress.c_str());
    if (!pHostEnt) return SVL_FAIL;
    memcpy(&hostEnt, pHostEnt, sizeof(hostent));
    memcpy(&hostNum, hostEnt.h_addr_list[0], sizeof(long));

    sockaddr_in* address = reinterpret_cast<sockaddr_in*>(SockAddr);
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = hostNum;
    address->sin_port = htons(SocketPort);

    return SVL_OK;
}

int svlVideoCodecUDPStream::CreateClient()
{
#if (CISST_OS == CISST_WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(WINSOCKVERSION, &wsaData) != 0) {
#ifdef _NET_VERBOSE_
        std::cerr << "svlVideoCodecCVINet::CreateServer - WSAStartup failed" << std::endl;
#endif
        return SVL_FAIL;
    }
#endif

    Socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socket < 0) return SVL_FAIL;
    
    sockaddr_in address;
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(SocketPort);
    
    if (bind(Socket, (sockaddr*)&address, sizeof(sockaddr_in)) < 0) return SVL_FAIL;
    
    return SVL_OK;
}

void svlVideoCodecUDPStream::CloseSocket()
{
    if (Socket) {
#if (CISST_OS == CISST_WINDOWS)
        closesocket(Socket);
#else
        close(Socket);
#endif
        Socket = 0;
    }
    
#if (CISST_OS == CISST_WINDOWS)
    WSACleanup();
#endif
}

int svlVideoCodecUDPStream::Send(unsigned char* buffer, unsigned int size, unsigned int& packet_id)
{
    // Incoming picture data starts at 'buffer + PACKET_ID_SIZE'.
    // The first 'PACKET_ID_SIZE' bytes are there to leave room for the first packet ID.

    unsigned int send_size;
    int ret, udp_load = 0;

#ifdef _NET_VERBOSE_
    std::cerr << "svlVideoCodecUDPStream::Send - sending frame (" << size << " bytes; packets: " << packet_id << "-";
#endif

    while (size > 0) {
        // Add packet ID to the beginning of the packet [first 'PACKET_ID_SIZE' bytes]
        reinterpret_cast<unsigned int*>(buffer)[0] = packet_id ++;

        // Calculate data size to send (including packet ID)
        send_size = PACKET_ID_SIZE + std::min(size, PACKET_DATA_SIZE);

        ret = sendto(Socket,
                     reinterpret_cast<const char*>(buffer),
                     send_size,
                     0,
                     reinterpret_cast<sockaddr*>(SockAddr),
                     sizeof(sockaddr));

        if (ret == static_cast<int>(send_size)) {
            send_size -= PACKET_ID_SIZE;

            // Advance buffer pointer
            buffer += send_size;
            size -= send_size;

            // UDP load balancing
            udp_load += ret;
            if (udp_load > MAX_UDP_BURST) {
                udp_load -= MAX_UDP_BURST;
                osaSleep(UDP_BURST_WAIT);
            }
        }
        else {
#ifdef _NET_VERBOSE_
            std::cerr << std::endl << "svlVideoCodecUDPStream::Send - sendto() error" << std::endl;
#endif
            return SVL_FAIL;
        }
    }

#ifdef _NET_VERBOSE_
    std::cerr << packet_id - 1 << ")" << std::endl;
#endif
    return SVL_OK;
}

int svlVideoCodecUDPStream::Receive(unsigned char* buffer, unsigned int size)
{
    const unsigned char* fsm = reinterpret_cast<const unsigned char*>(FrameStartMarker.c_str());
    const unsigned int fsm_len = FrameStartMarker.length();
    bool framestart, started = false, onlyheader = buffer ? false : true;
    unsigned int framesize = 0;
    socklen_t from_len = sizeof(sockaddr_in);
    sockaddr_in from_addr;
    fd_set mask;
    int ret;

    if (onlyheader) {
        // The caller requests only the image dimensions
        // Use temporary packet buffer to receive header
        size = PACKET_SIZE;
        buffer = reinterpret_cast<unsigned char*>(PacketData);
    }

    while (1) {

        // Wait for new packet
        FD_ZERO(&mask);
        FD_SET(Socket, &mask);
        ret = select(FD_SETSIZE, &mask, 0, 0, 0);
        if (ret < 0) {
#ifdef _NET_VERBOSE_
            std::cerr << "svlVideoCodecUDPStream::Receive - select() error" << std::endl;
#endif
            break;
        }

        ret = -1;
        // Receive packet
        if (FD_ISSET(Socket, &mask)) {
            ret = recvfrom(Socket,
                           reinterpret_cast<char*>(buffer),
                           std::min(size, PACKET_SIZE),
                           0,
                           (sockaddr*)&from_addr,
                           &from_len);
        }

        if (ret >= 0) {
            // Check if it starts with a "frame start marker"
            framestart = CompareData(buffer, fsm, fsm_len);

            if (!framestart) {
                if (!started) {
                    // Looking for "frame start marker"
                    continue;
                }
            }
            else {
                if (started) {
#ifdef _NET_VERBOSE_
                    std::cerr << "svlVideoCodecUDPStream::Receive - broken frame" << std::endl;
#endif
                    return BROKEN_FRAME;
                }
                else {
                    if (onlyheader) {
                        // Extract dimensions from header
                        int offset = fsm_len + sizeof(unsigned int);
                        ReceivedWidth = reinterpret_cast<unsigned int*>(buffer + offset)[0];
                        offset += sizeof(unsigned int);
                        ReceivedHeight = reinterpret_cast<unsigned int*>(buffer + offset)[0];
                        // Release temporary packet buffer and return without receiving more data
                        return BROKEN_FRAME;
                    }

                    // Get "data size after frame start marker"
                    framesize = reinterpret_cast<unsigned int*>(buffer + fsm_len)[0] + fsm_len;
                    // Don't request the rest of the data if buffer size is too small
                    if (framesize > size) {
#ifdef _NET_VERBOSE_
                        std::cerr << "svlVideoCodecUDPStream::Receive - received data larger than buffer" << std::endl;
#endif
                        break;
                    }

                    // Signal "frame start"
                    size = 0;
                    started = true;
                }
            }

            buffer += ret;
            size += ret;

            if (framesize == size) {
#ifdef _NET_VERBOSE_
                std::cerr << "svlVideoCodecUDPStream::Receive - frame received (" << framesize << ")" << std::endl;
#endif
                return SVL_OK;
            }
        }
#ifdef _NET_VERBOSE_
        else {
            std::cerr << "svlVideoCodecUDPStream::Receive - recvfrom() error" << std::endl;
        }
#endif
    }

    return SVL_FAIL;
}

void* svlVideoCodecUDPStream::SendProc(int CMN_UNUSED(param))
{
    if (CreateServer() == SVL_OK) {
        StreamingThreadError = false;
        StreamingInitialized = true;
    }
    StreamingInitEvent.Raise();

    unsigned char* strmbuf;
    unsigned int packet_id = 0;

    while (!StreamingThreadError && !KillStreamingThread) {

        // Wait until new frame is acquired
        strmbuf = 0;
        while (!strmbuf && !KillStreamingThread) {
            strmbuf = streamingBuffer->Pull(true, 0.1);
        }

        // Send frame
        if (Send(strmbuf, StreamingBufferUsedSize, packet_id) != SVL_OK) {
            StreamingThreadError = true;
            break;
        }
    }
    CloseSocket();

    return this;
}

void* svlVideoCodecUDPStream::ReceiveProc(int CMN_UNUSED(param))
{
    if (CreateClient() == SVL_OK) {
        StreamingThreadError = false;
        StreamingInitialized = true;
    }
    StreamingInitEvent.Raise();

    unsigned char* strmbuf;
    int framesize, ret;

    while (!StreamingThreadError && !KillStreamingThread) {

        // Receive frame
        if (streamingBuffer) strmbuf = streamingBuffer->GetPushBuffer();
        else strmbuf = 0;
        framesize = streamingBufferSize;
        ret = Receive(strmbuf, framesize);

        switch(ret) {
            case SVL_OK:
                StreamingBufferUsedSize = framesize;
                streamingBuffer->Push();
            break;

            case BROKEN_FRAME:
                // NOP
            break;

            default:
                StreamingThreadError = true;
            break;
        }
    }

    CloseSocket();

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
        // Extract address string (substring before ':')
        SocketAddress = filename.substr(0, pos);

        // Extract port (numerical string after ':')
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

