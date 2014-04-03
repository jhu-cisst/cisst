/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#define __STDC_CONSTANT_MACROS

#include "svlVideoCodecFFMPEG.h"
#include <cisstCommon/cmnGetChar.h>

//#define __FFMPEG_VERBOSE__

#ifdef __FFMPEG_VERBOSE__
    #define __AV_LOG_LEVEL__ 32
#else // __FFMPEG_VERBOSE__
    #define __AV_LOG_LEVEL__ 16
#endif // __FFMPEG_VERBOSE__

#if LIBAVCODEC_VERSION_INT < ((52 << 16) + (64 << 8))
    #define AVMEDIA_TYPE_VIDEO  CODEC_TYPE_VIDEO
#endif

#if LIBAVCODEC_VERSION_INT >= ((53 << 16) + (35 << 8))
    extern "C" {
        #include <libavutil/mathematics.h>
    }
#endif

//#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (2 << 8))
//    #define av_open_input_file  avformat_open_input
//#endif

#ifndef AVSEEK_FLAG_FRAME
    #define AVSEEK_FLAG_FRAME   8
#endif

#ifndef AV_PKT_FLAG_KEY
    #define AV_PKT_FLAG_KEY     0x0001
#endif


/*********************************/
/*** svlVideoCodecFFMPEG class ***/
/***************av_open_input_file******************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecFFMPEG, svlVideoCodecBase)

svlVideoCodecFFMPEG::svlVideoCodecFFMPEG() :
    svlVideoCodecBase(),
    Opened(false),
    Writing(false),
    Width(0),
    Height(0),
    Framerate(-1.0),
    Framestep(1),
    Length(0),
    Position(-1),
    pFormatCtx(0),
    pDecoderCtx(0),
    pEncoderCtx(0),
    pStream(0),
    pFrame(0),
    pFrameRGB(0),
    pConvertCtx(0),
    VideoStreamID(-1),
    RepeatFrame(false)
{
    SetName("FFMPEG Codec");
    SetExtensionList(".avi;.mpg;.mpeg;.dv;.wmv;.mov;.m4v;.mp4;.flv;.ogg;.asv;.rv;.roq;");
    SetMultithreaded(false);
    SetVariableFramerate(false);

    // Initialize libAVCodec library
    av_register_all();
    av_log_set_level(__AV_LOG_LEVEL__);

    // Compile list of available encoders
    BuildEncoderList();

    // Set default compression settings
    Config.EncoderID       = 0;
    Config.QualityBased    = false;
    Config.TargetQuantizer = 3.0;
    Config.Bitrate         = 4000;
    Config.GoP             = 30;

    ProcInfoSingleThread.count = 1;
    ProcInfoSingleThread.ID    = 0;
}

svlVideoCodecFFMPEG::~svlVideoCodecFFMPEG()
{
    Close();
}

int svlVideoCodecFFMPEG::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "Open - codec is already open" << std::endl;
        return SVL_FAIL;
    }

    Opened  = true;
    Writing = false;

    while (1) {
        AVCodec *av_codec = 0;
        unsigned int i;
        int ret;

        // Open video file
#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (2 << 8))
        ret = av_open_input_file(&pFormatCtx, filename.c_str(), 0, 0, 0);
#else
        ret = avformat_open_input(&pFormatCtx, filename.c_str(), 0, 0);
#endif
        if (ret != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to open file: " << filename << std::endl;
            break;
        }
        // Retrieve stream information
#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (6 << 8))
        ret = av_find_stream_info(pFormatCtx);
#else
        ret = avformat_find_stream_info(pFormatCtx, 0);
#endif
        if ((pFormatCtx) < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to find stream info" << std::endl;
            break;
        }

#ifdef __FFMPEG_VERBOSE__
        std::cerr << "svlVideoCodecFFMPEG::Open - calling `dump_format`:" << std::endl;
        // Dump information about file onto standard error
        dump_format(pFormatCtx, 0, filename.c_str(), 0);
#endif // __FFMPEG_VERBOSE__

        // Find the first video stream
        for (i = 0; i < pFormatCtx->nb_streams; i ++) {
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                VideoStreamID = i;
                break;
            }
        }
        if (VideoStreamID < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to find a video stream" << std::endl;
            break;
        }

        // Get a pointer to the codec context for the video stream
        pDecoderCtx = pFormatCtx->streams[VideoStreamID]->codec;

        // Find the decoder for the video stream
        av_codec = avcodec_find_decoder(pDecoderCtx->codec_id);
        if(av_codec == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - unsupported video codec" << std::endl;
            break;
        }

        // Open codec
#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (8 << 8))
        ret = avcodec_open(pDecoderCtx, av_codec);
#else
        ret = avcodec_open2(pDecoderCtx, av_codec, 0);
#endif
        if (ret < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to open FFMPEG decoder" << std::endl;
            break;
        }

        Width  = pDecoderCtx->width;
        Height = pDecoderCtx->height;

        // Allocate video frame
        pFrame = avcodec_alloc_frame();
        if (!pFrame) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to allocate AV frame (raw)" << std::endl;
            break;
        }
        // Allocate an RGB frame
        pFrameRGB = avcodec_alloc_frame();
        if (!pFrameRGB) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to allocate AV frame (RGB)" << std::endl;
            break;
        }
        // Allocate image conversion context
        pConvertCtx = sws_getContext(Width,
                                     Height, 
                                     pDecoderCtx->pix_fmt, 
                                     Width,
                                     Height,
                                     PIX_FMT_BGR24,
                                     SWS_BICUBIC,
                                     0, 0, 0);
        if (!pConvertCtx) {
            CMN_LOG_CLASS_INIT_ERROR << "Open - failed to initialize conversion context" << std::endl;
            break;
        }

        double fps_format = static_cast<double>(pFormatCtx->streams[VideoStreamID]->time_base.den) /
                            pFormatCtx->streams[VideoStreamID]->time_base.num;
        double fps_codec  = static_cast<double>(pDecoderCtx->time_base.den) /
                            pDecoderCtx->time_base.num;
        Framerate = (fps_format < 1000.0) ? fps_format : fps_codec;

        // Initialize seeking states
        LastExtractedFrame = -1;
        RepeatFrame = false;
        UseIndex = false;

        // Decompress the first 2 frames in order to find out the frame step size in the video
        svlSampleImageRGB image;
        Position = -1;
        Read(0, image, 0, false);
        Framestep = LastDTS;
        Position = -1;
        Read(0, image, 0, false);
        Framestep = LastDTS - Framestep;
        if (Framestep < 1) Framestep = 1;
        if (Framestep > 1000) Framestep = 1; // Bug with certian codecs
        // Seek back to the beginning
        LastExtractedFrame = 1;
        Position = 2;
        Length = 2;
        SetPos(0);

        if (Framestep == 1) Length = pFormatCtx->streams[VideoStreamID]->nb_frames;
        else Length = (pFormatCtx->streams[VideoStreamID]->nb_frames + 1) / Framestep;

        Position = 0;

#ifdef __FFMPEG_VERBOSE__
        std::cerr << "svlVideoCodecFFMPEG::Open - File opened (" << Width
                  << "x" << Height
                  << ", " << std::fixed << Framerate
                  << "fps, " << Length << " frames)" << std::endl;
#endif // __FFMPEG_VERBOSE__

        // - Check if the codec uses predicted frames
        // - If so, then build key-frame index
        // - Finally, set `UseIndex` flag
        if (Length > 1) BuildIndex();

        width     = Width;
        height    = Height;
        framerate = Framerate;

        return SVL_OK;
    }

    Close();
    return SVL_FAIL;
}

int svlVideoCodecFFMPEG::Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate)
{
	if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "Create - codec is already open" << std::endl;
        return SVL_FAIL;
    }
    if (width < 1 || width > MAX_DIMENSION || height < 1 || height > MAX_DIMENSION) {
        CMN_LOG_CLASS_INIT_ERROR << "Create - invalid image dimensions" << std::endl;
        return SVL_FAIL;
    }
    if ((width % 2) != 0 || (height % 2) != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Create - width and height must be multiples of 2" << std::endl;
        return SVL_FAIL;
    }
    double _framerate = framerate;
    if (_framerate < 0.1) {
        CMN_LOG_CLASS_INIT_WARNING << "Create - invalid framerate; using default setting (30.0) instead" << std::endl;
        _framerate = 30.0;
    }
    if (Config.EncoderID >= EncoderIDs.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "Create - invalid encoder" << std::endl;
        return SVL_FAIL;
    }

    Opened  = true;
    Writing = true;

    AVCodec *av_codec = 0;
    bool error = false;
    int ret;

    while (1) {

#if LIBAVFORMAT_VERSION_INT < ((52 << 16) + (45 << 8))
        AVOutputFormat *output_format = guess_stream_format("avi", NULL, NULL);
#else
// libAVFormat version >= 52.45.0
        AVOutputFormat *output_format = av_guess_format("avi", NULL, NULL);
#endif
        if (!output_format) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to allocate output format" << std::endl;
            break;
        }
        if (output_format->flags & AVFMT_NOFILE) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - format doesn't require file to be created" << std::endl;
            break;
        }

        pFormatCtx = avformat_alloc_context();
        if (!pFormatCtx) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to allocate format context" << std::endl;
            break;
        }
        pFormatCtx->oformat = output_format;

#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (10 << 8))
        pStream = av_new_stream(pFormatCtx, 0);
#else
        pStream = avformat_new_stream(pFormatCtx, 0);
#endif
        if (!pStream || !pStream->codec) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to allocate video stream" << std::endl;
            break;
        }
        pEncoderCtx = pStream->codec;

        // Set generic encoder parameters
        pEncoderCtx->codec_id      = static_cast<CV_CODEC_ID>(EncoderIDs[Config.EncoderID]);
        pEncoderCtx->codec_type    = AVMEDIA_TYPE_VIDEO;
        pEncoderCtx->bit_rate      = Config.Bitrate * 1024; // bits per second
        pEncoderCtx->width         = width;                 // must be multiple of 2
        pEncoderCtx->height        = height;                // must be multiple of 2
        pEncoderCtx->time_base.den = static_cast<int>(framerate * 1000000);
        pEncoderCtx->time_base.num = 1000000;
        if (pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            pEncoderCtx->flags    |= CODEC_FLAG_GLOBAL_HEADER;
        }

        // Set encoder-specific settings
        ConfigureEncoder();

        av_codec = avcodec_find_encoder(pEncoderCtx->codec_id);
        if (!av_codec) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to find video encoder (" << EncoderNames[Config.EncoderID] << ")" << std::endl;
            break;
        }

#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (8 << 8))
        ret = avcodec_open(pEncoderCtx, av_codec);
#else
        ret = avcodec_open2(pEncoderCtx, av_codec, 0);
#endif
        if (ret < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to open encoder" << std::endl;
            break;
        }

#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (2 << 8))
        if (av_set_parameters(pFormatCtx, 0) < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - invalid output format parameters" << std::endl;
            break;
        }
#endif

#ifdef __FFMPEG_VERBOSE__
        std::cerr << "svlVideoCodecFFMPEG::Create - calling `dump_format`:" << std::endl;
        // Dump information about file onto standard error
        dump_format(pFormatCtx, 0, filename.c_str(), 1);
#endif // __FFMPEG_VERBOSE__

#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (2 << 8))
        ret = url_fopen(&pFormatCtx->pb, filename.c_str(), URL_WRONLY);
#else
        ret = avio_open(&pFormatCtx->pb, filename.c_str(), AVIO_FLAG_WRITE);
#endif
        if (ret < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to create file" << std::endl;
            break;
        }

#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (2 << 8))
        ret = av_write_header(pFormatCtx);
#else
        ret = avformat_write_header(pFormatCtx, 0);
#endif
        if (ret != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to write file header" << std::endl;
            break;
        }

        // Allocate an RGB frame
        pFrameRGB = avcodec_alloc_frame();
        if (!pFrameRGB) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to allocate AV frame (RGB)" << std::endl;
            break;
        }
        // Allocate a frame
        pFrame = avcodec_alloc_frame();
        if (!pFrame) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to allocate AV frame" << std::endl;
            break;
        }

#ifdef __FFMPEG_VERBOSE__
        std::cerr << "svlVideoCodecFFMPEG::Create - File created (" << width
                  << "x" << height
                  << ", " << std::fixed << framerate
                  << "fps, " << pEncoderCtx->bit_rate
                  << "kbps, gop=" << pEncoderCtx->gop_size << ")" << std::endl;
#endif // __FFMPEG_VERBOSE__

        // Allocate image conversion context
        pConvertCtx = sws_getContext(width,
                                     height, 
                                     PIX_FMT_BGR24, 
                                     width,
                                     height,
                                     pEncoderCtx->pix_fmt,
                                     SWS_BICUBIC,
                                     0, 0, 0);
        if (!pConvertCtx) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to initialize conversion context" << std::endl;
            break;
        }

        // Allocate image buffers large enough for any data format
        const unsigned int datasize = width * height * 3;
        if (ConversionBuffer.size() < datasize) ConversionBuffer.SetSize(datasize);
        if (OutputBuffer.size() < datasize) OutputBuffer.SetSize(datasize);

        // Assign appropriate parts of the conversion buffer to image planes in pFrame
        //   Note: pFrame is an AVFrame, but AVFrame is a superset of AVPicture
        avpicture_fill(reinterpret_cast<AVPicture*>(pFrame),
                       ConversionBuffer.Pointer(),
                       pEncoderCtx->pix_fmt,
                       width,
                       height);

        Width     = width;
        Height    = height;
        Framerate = framerate;

        return SVL_OK;
    }

    if (error) {
        Close();
        return SVL_FAIL;
    }

	return SVL_OK;
}

int svlVideoCodecFFMPEG::Close()
{
    if (Opened && Writing) {
        // Finish up writing video file
        if (av_write_trailer(pFormatCtx) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Create - failed to write file footer" << std::endl;
        }
    }

    if (pDecoderCtx) {
        avcodec_close(pDecoderCtx);
        pDecoderCtx = 0;
    }
    if (pEncoderCtx) {
        avcodec_close(pEncoderCtx);
        av_free(pEncoderCtx);
        pStream->codec = 0;
        pEncoderCtx = 0;
    }
    if (pStream) {
        av_free(pStream);
        pStream = 0;
    }
    if (pFormatCtx) {
        if (Writing) {
#if LIBAVCODEC_VERSION_INT < ((52 << 16) + (105 << 8))
            url_fclose(pFormatCtx->pb);
#else
            avio_close(pFormatCtx->pb);
#endif
            av_free(pFormatCtx);
        }
        else {
#if LIBAVCODEC_VERSION_INT < ((53 << 16) + (17 << 8))
            av_close_input_file(pFormatCtx);
#else
            avformat_close_input(&pFormatCtx);
#endif
        }
        pFormatCtx = 0;
    }
    if (pFrame) {
        av_free(pFrame);
        pFrame = 0;
    }
    if (pFrameRGB) {
        av_free(pFrameRGB);
        pFrameRGB = 0;
    }
    if (pConvertCtx) {
        sws_freeContext(pConvertCtx);
        pConvertCtx = 0;
    }

    VideoStreamID = -1;
    Width         = 0;
    Height        = 0;
    Framerate     = -1.0;
    Length        = 0;
    Position      = -1;

    Opened  = false;
    Writing = false;

    return SVL_OK;
}

int svlVideoCodecFFMPEG::GetBegPos() const
{
    return 0;
}

int svlVideoCodecFFMPEG::GetEndPos() const
{
    return Length - 1;
}

int svlVideoCodecFFMPEG::GetPos() const
{
    return Position;
}

int svlVideoCodecFFMPEG::SetPos(const int pos)
{
    if (!Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPos - file has not been opened yet" << std::endl;
        return SVL_FAIL;
    }
    if (Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPos - file is open for writing" << std::endl;
        return SVL_FAIL;
    }
    if (Length < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPos - seeking not supported" << std::endl;
        return SVL_FAIL;
    }

    CS.Enter();

#ifdef __FFMPEG_VERBOSE__
    std::cerr << "svlVideoCodecFFMPEG::SetPos - pos=" << pos
              << ", Position=" << Position
              << ", LastExtractedFrame=" << LastExtractedFrame
              << ", RepeatFrame=" << RepeatFrame << std::endl;
#endif // __FFMPEG_VERBOSE__

    int _pos = pos;

    // Do nothing if `pos` is pointing to the next frame
    if (_pos == Position) {
        CS.Leave();
        return SVL_OK;
    }

    // Repeat previous frame if `pos` points to the last extracted frame
    if (_pos == LastExtractedFrame) {
        RepeatFrame = true;
        if (Position == LastExtractedFrame + 1) {
            // Return if read position is in the right place
            CS.Leave();
            return SVL_OK;
        }
        // Seek to the frame right after `pos` since
        // that is going to be extracted next
        _pos ++;
    }

    if (!UseIndex || KeyFrameIndex[_pos] == _pos) {
        if (av_seek_frame(pFormatCtx, VideoStreamID, _pos * Framestep,
                          AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME) < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "SetPos - failed while seeking to position: " << _pos << std::endl;
            CS.Leave();
            return SVL_FAIL;
        }

        // Flush AV buffers
        avcodec_flush_buffers(pDecoderCtx);
    }
    else {
        // Seek back to the closest key-frame
        if (av_seek_frame(pFormatCtx, VideoStreamID, KeyFrameIndex[_pos],
                          AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME) < 0) {
            CMN_LOG_CLASS_INIT_ERROR << "SetPos - failed while seeking to key-frame position: " << KeyFrameIndex[_pos] << std::endl;
            CS.Leave();
            return SVL_FAIL;
        }

        // Gradually build the `pos` frame starting from the key-frame
        AVPacket packet;
        int frameFinished, ret, frames_to_pos = _pos - KeyFrameIndex[_pos];
        bool error = false;

        for (int i = 0; i < frames_to_pos; i ++) {

            while (1) {

                // Read out a frame
                while (1) {
                    ret = av_read_frame(pFormatCtx, &packet);
                    if (ret < 0 || packet.stream_index == VideoStreamID) break;
                    av_free_packet(&packet);
                }
                if (ret < 0) {
                    error = true;
                    break;
                }
                // Decode video frame
#if LIBAVCODEC_VERSION_INT < ((52 << 16) + (25 << 8))
                if (avcodec_decode_video(pDecoderCtx, pFrame, &frameFinished, packet.data, packet.size) < 0) {
#else
// libAVCodec version >= 52.25.0
                if (avcodec_decode_video2(pDecoderCtx, pFrame, &frameFinished, &packet) < 0) {
#endif
                    av_free_packet(&packet);
                    error = true;
                    break;
                }
                if (!frameFinished) {
                    av_free_packet(&packet);
                    continue;
                }
                av_free_packet(&packet);

                break;
            }

            if (error) {
                CMN_LOG_CLASS_INIT_ERROR << "SetPos - failed while reconstructing non-key-frame: " << _pos << std::endl;
                CS.Leave();
                return SVL_FAIL;
            }
        }
    }

    Position = _pos;

    CS.Leave();

    return SVL_OK;
}

svlVideoIO::Compression* svlVideoCodecFFMPEG::GetCompression() const
{
    svlVideoIO::Compression* compression = 0;

    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    if (Codec) {
        compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[Codec->size]);
        memcpy(compression, Codec, Codec->size);
    }
    else {
        unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
        compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

        // Output settings
        CompressionData* output_data = reinterpret_cast<CompressionData*>(&(compression->data[0]));

        // Generic settings
        std::string name("FFMPEG Codec");
        memset(&(compression->extension[0]), 0, 16);
        memcpy(&(compression->extension[0]), ".mpg", 4);
        memset(&(compression->name[0]), 0, 64);
        memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
        compression->size = size;
        compression->datasize = sizeof(CompressionData);

        // CVI specific settings
        output_data->EncoderID       = Config.EncoderID;
        output_data->QualityBased    = Config.QualityBased;
        output_data->TargetQuantizer = Config.TargetQuantizer;
        output_data->Bitrate         = Config.Bitrate;
        output_data->GoP             = Config.GoP;
    }

    return compression;
}

int svlVideoCodecFFMPEG::SetCompression(const svlVideoIO::Compression *compression)
{
    if (Opened || !compression) return SVL_FAIL;

    unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
    if (compression->size < size) return SVL_FAIL;

    // Create a safe copy of the string `extension`
    char _ext[16];
    memcpy(_ext, compression->extension, 15);
    _ext[15] = 0;

    std::string extensionlist(GetExtensions());
    std::string extension(_ext);
    extension += ";";
    if (extensionlist.find(extension) == std::string::npos) {
        CMN_LOG_CLASS_INIT_ERROR << "SetCompression - codec parameters do not match this codec" << std::endl;
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(Codec);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    // Local settings
    CompressionData* local_data = reinterpret_cast<CompressionData*>(&(Codec->data[0]));
    // Input settings
    const CompressionData* input_data = reinterpret_cast<const CompressionData*>(&(compression->data[0]));

    // Generic settings
    std::string name("FFMPEG Codec");
    memcpy(&(Codec->extension[0]), _ext, 16);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = size;
    Codec->datasize = sizeof(CompressionData);

    // FFMPEG specific settings
    if (input_data->EncoderID < EncoderIDs.size()) {
        Config.EncoderID = local_data->EncoderID = input_data->EncoderID;
    }
    else {
        local_data->EncoderID = Config.EncoderID;
    }

    Config.QualityBased = local_data->QualityBased = input_data->QualityBased;

    if (input_data->TargetQuantizer >= 1.0 && input_data->TargetQuantizer <= 10.0) {
        Config.TargetQuantizer = local_data->TargetQuantizer = input_data->TargetQuantizer;
    }
    else {
        local_data->TargetQuantizer = Config.TargetQuantizer;
    }

    if (input_data->Bitrate >= 100 && input_data->Bitrate <= 100000) {
        Config.Bitrate = local_data->Bitrate = input_data->Bitrate;
    }
    else {
        local_data->Bitrate = Config.Bitrate;
    }

    if (input_data->GoP >= 1 && input_data->GoP <= 300) {
        Config.GoP = local_data->GoP = input_data->GoP;
    }
    else {
        local_data->GoP = Config.GoP;
    }

    return SVL_OK;
}

int svlVideoCodecFFMPEG::DialogCompression()
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogCompression - codec is already open" << std::endl;
        return SVL_FAIL;
    }

    unsigned int i, encoder_count = EncoderIDs.size(), encoder_id = 0;

    // Print video encoder list
    std::cout << " List of available encoders:" << std::endl;
    if  (encoder_count < 1) {
        std::cout << " No video encoders are available" << std::endl;
        return SVL_FAIL;
    }
    for (i = 0; i < encoder_count; i ++) {
        std::cout << "  " << i << ") " << EncoderNames[i] << std::endl;
    }

    std::cout << " # Select video encoder: ";
    std::cin >> encoder_id;
    std::cin.ignore();
    if (encoder_id >= encoder_count) {
        std::cout << " Invalid encoder selected; default encoder will be used" << std::endl;
        encoder_id = 0;
    }
    std::cout << " Encoder selected: " << EncoderNames[encoder_id] << std::endl;

    std::string extension;
    extension = GetExtensionFromEncoderID(encoder_id);
    extension.insert(0, ".");

    return DialogCompression(extension, encoder_id);
}

int svlVideoCodecFFMPEG::DialogCompression(const std::string &filename)
{
    if (filename.empty()) return DialogCompression();

    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogCompression - codec is already open" << std::endl;
        return SVL_FAIL;
    }

    std::string extension;
    svlVideoIO::GetExtension(filename, extension);

    unsigned int encoder_id, encoder_count = 0;
    vctDynamicVector<unsigned int> encoder_list(EncoderIDs.size());

    if (extension == "mpg" || extension == "mpeg") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MPEG1VIDEO);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MPEG2VIDEO);
    }
    else if (extension == "avi") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MPEG4);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V2);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V3);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MJPEG);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_HUFFYUV);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_FFV1);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_H261);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_H263);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_LJPEG);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_JPEGLS);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_RAWVIDEO);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V1);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_H263P);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_H264);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_ZLIB);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_SNOW);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PNG);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PPM);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PBM);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PGM);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PGMYUV);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_PAM);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_FFVHUFF);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_BMP);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_ZMBV);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_TARGA);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_TIFF);
    }
    else if (extension == "ogg") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_THEORA);
    }
    else if (extension == "dv") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO);
    }
    else if (extension == "wmv") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_WMV1);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_WMV2);
    }
    else if (extension == "flv") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_FLV1);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_SVQ1);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_FLASHSV);
    }
    else if (extension == "rv") {
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_RV10);
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_RV20);
    }
    else if (extension == "asv") {
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_ASV1);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_ASV2);
    }
    else if (extension == "roq") {
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_ROQ);
    }
    else if (extension == "mov") {
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_QTRLE);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MPEG4);
        encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_MJPEG);
    }
    else if (extension == "m4v" || extension == "mp4") {
//      encoder_list[encoder_count ++] = GetFFMPEGEncoderID(CODEC_ID_H264);
    }

    if (encoder_count < 1) {
        std::stringstream strstr;
        strstr << "failed to find matching encoder for the file extension (" << extension << ")";
        CMN_LOG_CLASS_INIT_WARNING << "DialogCompression: " << strstr.str() << std::endl;
        std::cerr << " ! Warning: " << strstr.str() << std::endl;

        return DialogCompression();
    }

    std::cout << " List of available encoders:" << std::endl;
    for (unsigned int i = 0; i < encoder_count; i ++) {
        std::cout << "  " << i << ") " << EncoderNames[encoder_list[i]] << std::endl;
    }

    std::cout << " # Select video encoder: ";
    std::cin >> encoder_id;
    std::cin.ignore();
    if (encoder_id >= encoder_count) {
        std::cout << " Invalid encoder selected; default encoder will be used" << std::endl;
        encoder_id = 0;
    }

    extension.insert(0, ".");

    return DialogCompression(extension, encoder_list[encoder_id]);
}

int svlVideoCodecFFMPEG::DialogCompression(const std::string &extension, unsigned int encoder_id)
{
    if (extension.empty() || encoder_id >= EncoderIDs.size()) return SVL_FAIL;

    std::cout << " Encoder selected: " << EncoderNames[encoder_id] << std::endl;

    char input[256];
    int min, max, defaultval, ival;
    bool   quality_based = false;
    double target_q      = Config.TargetQuantizer;
    int    bitrate       = Config.Bitrate;
    int    gop           = Config.GoP;

    std::cout << " # Enable quality based compression ('y' or other): ";
    ival = cmnGetChar();
    if (ival == 'y' || ival == 'Y') {
        quality_based = true;
        std::cout << "YES" << std::endl;
    }
    else {
        quality_based = false;
        std::cout << "NO" << std::endl;
    }

    if (quality_based) {
        min = 1; max = 10; defaultval = 3;
        std::cout << " # Enter target quantizer (min=" << min << "; max=" << max << "; default=" << defaultval << "): ";
        std::cin.getline(input, 256);
        if (std::cin.gcount() > 1) {
            ival = atoi(input);
            if (ival < min) ival = min;
            if (ival > max) ival = max;
        }
        else ival = defaultval;
        std::cout << "    Target quantizer = " << ival << std::endl;
        target_q = static_cast<double>(ival);
    }
    else {
        min = 100; max = 100000; defaultval = 4000;
        std::cout << " # Enter bitrate [kbps] (min=" << min << "; max=" << max << "; default=" << defaultval << "): ";
        std::cin.getline(input, 256);
        if (std::cin.gcount() > 1) {
            bitrate = atoi(input);
            if (bitrate < min) bitrate = min;
            if (bitrate > max) bitrate = max;
        }
        else bitrate = defaultval;
        std::cout << "    Bitrate = " << bitrate << std::endl;

        min = 1; max = 300; defaultval = 30;
        std::cout << " # Enter GoP size [every Nth frame is keyframe] (min=" << min << "; max=" << max << "; default=" << defaultval << "): ";
        std::cin.getline(input, 256);
        if (std::cin.gcount() > 1) {
            gop = atoi(input);
            if (gop < min) gop = 0;
            if (gop > max) gop = max;
        }
        else gop = defaultval;
        std::cout << "    GoP size = " << std::max(1, gop) << std::endl;
    }

    svlVideoIO::ReleaseCompression(Codec);
    unsigned int size = sizeof(svlVideoIO::Compression) - sizeof(unsigned char) + sizeof(CompressionData);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    // Local settings
    CompressionData* local_data = reinterpret_cast<CompressionData*>(&(Codec->data[0]));

    // Generic settings
    std::string name("FFMPEG Codec");
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), extension.c_str(), std::min(static_cast<int>(extension.size()), 15));
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    Codec->size = size;
    Codec->datasize = sizeof(CompressionData);

    // FFMPEG specific settings
    Config.EncoderID       = local_data->EncoderID       = encoder_id;
    Config.QualityBased    = local_data->QualityBased    = quality_based;
    Config.TargetQuantizer = local_data->TargetQuantizer = target_q;
    Config.Bitrate         = local_data->Bitrate         = bitrate;
    Config.GoP             = local_data->GoP             = gop;

    return SVL_OK;
}

double svlVideoCodecFFMPEG::GetTimestamp() const
{
    if (!Opened || Writing) return -1.0;
    return -1.0;
}

int svlVideoCodecFFMPEG::Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize)
{
    if (!procInfo) procInfo = &ProcInfoSingleThread;

    if (videoch >= image.GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "Read - (thread=" << procInfo->ID << ") video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!Opened || Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "Read - (thread=" << procInfo->ID << ") file needs to be opened for reading" << std::endl;
        return SVL_FAIL;
    }

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    // Allocate image buffer if not done yet
    if (Width  != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        if (noresize) {
            CMN_LOG_CLASS_INIT_ERROR << "Read - unexpected change in image dimensions" << std::endl;
            return SVL_FAIL;
        }
        image.SetSize(videoch, Width, Height);
    }

    AVPacket packet;
    int frameFinished, ret;

    while (1) {
        if (Length > 0 && Position >= Length) {
            SetPos(0);
            return SVL_VID_END_REACHED;
        }

        CS.Enter();

        // Assign appropriate parts of buffer to image planes in pFrameRGB
        //   Note: pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
        avpicture_fill(reinterpret_cast<AVPicture*>(pFrameRGB),
                       image.GetUCharPointer(videoch),
                       PIX_FMT_RGB24,
                       Width,
                       Height);

        if (!RepeatFrame) {
            while (1) {
                ret = av_read_frame(pFormatCtx, &packet);
                if (ret < 0 || packet.stream_index == VideoStreamID) break;
                av_free_packet(&packet);
            }
            if (ret < 0) {
                if (Position > 0) {
                    // Update file length to the correct value
                    Length = Position;
                    CS.Leave();
                    SetPos(0);
                    CMN_LOG_CLASS_INIT_WARNING << "Read - unexpected end-of-file" << std::endl;
                    return SVL_VID_END_REACHED;
                }
                else {
                    CMN_LOG_CLASS_INIT_ERROR << "Read - failed to extract frame" << std::endl;
                }
                break;
            }

            // Decode video frame
#if LIBAVCODEC_VERSION_INT < ((52 << 16) + (25 << 8))
            if (avcodec_decode_video(pDecoderCtx, pFrame, &frameFinished, packet.data, packet.size) < 0) {
#else
            if (avcodec_decode_video2(pDecoderCtx, pFrame, &frameFinished, &packet) < 0) {
#endif
                CMN_LOG_CLASS_INIT_ERROR << "Read - failed to decode video frame" << std::endl;
                av_free_packet(&packet);
                break;
            }
            if (!frameFinished) {
                // Failed to get new video frame
                av_free_packet(&packet);
                CS.Leave();
                continue;
            }
        }

        // Convert the image from its native format to RGB
        sws_scale(pConvertCtx,
                  pFrame->data,
                  pFrame->linesize,
                  0,
                  Height,
                  pFrameRGB->data,
                  pFrameRGB->linesize);

        LastDTS = packet.dts;
        av_free_packet(&packet);

        if (!RepeatFrame) {
            LastExtractedFrame = Position;
            Position ++;
        }
        RepeatFrame = false;

        //std::cerr << Position << " ";

        CS.Leave();

        return SVL_OK;
    }

    CS.Leave();

    return SVL_FAIL;
}

int svlVideoCodecFFMPEG::Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch)
{
    if (!procInfo) procInfo = &ProcInfoSingleThread;

    if (videoch >= image.GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "Write - (thread=" << procInfo->ID << ") video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!Opened || !Writing) {
        CMN_LOG_CLASS_INIT_ERROR << "Write - (thread=" << procInfo->ID << ") file needs to be opened for writing" << std::endl;
        return SVL_FAIL;
    }
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        CMN_LOG_CLASS_INIT_ERROR << "Write - (thread=" << procInfo->ID << ") unexpected change in image dimensions" << std::endl;
        return SVL_FAIL;
    }

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    //   Note: pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
    avpicture_fill(reinterpret_cast<AVPicture*>(pFrameRGB),
                   const_cast<unsigned char*>(image.GetUCharPointer(videoch)),
                   PIX_FMT_BGR24,
                   Width,
                   Height);

    // Convert the image from RGB to the format the encoder requires
    sws_scale(pConvertCtx,
              pFrameRGB->data,
              pFrameRGB->linesize,
              0,
              Height,
              pFrame->data,
              pFrame->linesize);

    while (1) {
        AVFrame *frame = pFrame;
        int ret;

        AVPacket packet;
        av_init_packet(&packet);
        if (pEncoderCtx->coded_frame->pts != static_cast<int64_t>(AV_NOPTS_VALUE)) {
            packet.pts= av_rescale_q(pEncoderCtx->coded_frame->pts, pEncoderCtx->time_base, pStream->time_base);
        }
        if(pEncoderCtx->coded_frame->key_frame) {
            packet.flags |= AV_PKT_FLAG_KEY;
        }
        packet.stream_index= pStream->index;
        packet.data = OutputBuffer.Pointer();
        packet.size = OutputBuffer.size();

        // Encode the image
#if LIBAVCODEC_VERSION_INT < ((54 << 16) + (1 << 8))
        int out_size = avcodec_encode_video(pEncoderCtx, OutputBuffer.Pointer(), OutputBuffer.size(), frame);
        if (out_size >= 0) { // Success
            packet.size = out_size;
            ret = 0;
        }
        else { // Error
            ret = out_size;
        }
#else
        int nonempty;
        ret = avcodec_encode_video2(pEncoderCtx, &packet, frame, &nonempty);
#endif
        if (ret >= 0) {
            if (packet.size == 0) {
#ifdef __FFMPEG_VERBOSE__
                std::cerr << "svlVideoCodecFFMPEG::Write - `avcodec_encode_video` output packet is empty; need to call again" << std::endl;
#endif // __FFMPEG_VERBOSE__
                frame = 0;
                continue;
            }
            else {
#ifdef __FFMPEG_VERBOSE__
                std::cerr << "svlVideoCodecFFMPEG::Write - `avcodec_encode_video` compressed frame to " << packet.size
                          << " bytes (" << std::fixed << (100.0 * packet.size / image.GetDataSize(videoch)) << "%)" << std::endl;
#endif // __FFMPEG_VERBOSE__

                if (av_interleaved_write_frame(pFormatCtx, &packet) != 0) {
                    CMN_LOG_CLASS_INIT_ERROR << "Write - failed to write encoded data to disk" << std::endl;
                    break;
                }

                return SVL_OK;
            }
        }
        else {
            CMN_LOG_CLASS_INIT_ERROR << "Write - failed to encode video frame (`avcodec_encode_video` returned " << packet.size << ")" << std::endl;
            break;
        }
    }

    return SVL_FAIL;
}

void svlVideoCodecFFMPEG::SetExtension(const std::string & extension)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetExtension - codec is already open" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetExtension - called (\"" << extension << "\")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();

    memset(&(compr->extension[0]), 0, 16);
    memcpy(&(compr->extension[0]), extension.c_str(), std::min(static_cast<int>(extension.size()), 15));

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::SetEncoderID(const int & encoder_id)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetEncoderID - codec is already open" << std::endl;
        return;
    }
    if (encoder_id < 0 || encoder_id >= static_cast<int>(EncoderIDs.size())) {
        CMN_LOG_CLASS_INIT_ERROR << "SetEncoderID - argument out of range [0, " << EncoderIDs.size() - 1 << "]" << std::endl;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetEncoderID - called (" << encoder_id << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->EncoderID = encoder_id;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::SetCompressionLevel(const int & CMN_UNUSED(compr_level))
{
    CMN_LOG_CLASS_INIT_ERROR << "SetCompressionLevel - feature is not supported by the FFMPEG codec" << std::endl;
}

void svlVideoCodecFFMPEG::SetQualityBased(const bool & enabled)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetQualityBased - codec is already open" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetQualityBased - called (" << enabled << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->QualityBased = enabled;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::SetTargetQuantizer(const double & target_quant)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTargetQuantizer - codec is already open" << std::endl;
        return;
    }
    if (target_quant < 1.0 || target_quant > 10.0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTargetQuantizer - argument out of range [1.0, 10.0]" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetTargetQuantizer - called (" << std::fixed << target_quant << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->TargetQuantizer = target_quant;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::SetDatarate(const int & datarate)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDatarate - codec is already open" << std::endl;
        return;
    }
    if (datarate < 100 || datarate > 100000) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDatarate - argument out of range [100, 100000]" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetDatarate - called (" << datarate << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->Bitrate = datarate;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::SetKeyFrameEvery(const int & key_every)
{
    if (Opened) {
        CMN_LOG_CLASS_INIT_ERROR << "SetKeyFrameEvery - codec is already open" << std::endl;
        return;
    }
    if (key_every < 1 || key_every > 300) {
        CMN_LOG_CLASS_INIT_ERROR << "SetKeyFrameEvery - argument out of range [1, 300]" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "SetKeyFrameEvery - called (" << key_every << ")" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    data->GoP = key_every;

    SetCompression(compr);
    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::IsCompressionLevelEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsCompressionLevelEnabled - called" << std::endl;
    enabled = false;
}

void svlVideoCodecFFMPEG::IsEncoderListEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsEncoderListEnabled - called" << std::endl;
    enabled = true;
}

void svlVideoCodecFFMPEG::IsTargetQuantizerEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsTargetQuantizerEnabled - called" << std::endl;
    enabled = true;
}

void svlVideoCodecFFMPEG::IsDatarateEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsDatarateEnabled - called" << std::endl;
    enabled = true;
}

void svlVideoCodecFFMPEG::IsKeyFrameEveryEnabled(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "IsFramesEveryEnabled - called" << std::endl;
    enabled = true;
}

void svlVideoCodecFFMPEG::GetCompressionLevel(int & compr_level) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetCompressionLevel - feature is not supported by the FFMPEG codec" << std::endl;
    compr_level = -1;
}

void svlVideoCodecFFMPEG::GetEncoderList(std::string & encoder_list) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetEncoderList - called" << std::endl;

    // Extract current extension field
    svlVideoIO::Compression* compr = GetCompression();
    std::string extension = compr->extension;
    svlVideoIO::ReleaseCompression(compr);

    int encid;
    std::stringstream strstr;

    if (extension.empty()) {

        const unsigned int encoder_count = EncoderIDs.size();
        if (encoder_count > 0) {
            for (unsigned int i = 0; i < encoder_count; i ++) {
                strstr << i << ":" << EncoderNames[i] << "\n";
            }
        }
        else {
            strstr << ":\n";
        }

    }
    else {

        if (extension == "mpg" || extension == "mpeg") {
            encid = GetFFMPEGEncoderID(CODEC_ID_MPEG1VIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MPEG2VIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "avi") {
            encid = GetFFMPEGEncoderID(CODEC_ID_MPEG4); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V2); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V3); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MJPEG); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_HUFFYUV); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_FFV1); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_H261); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_H263); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_LJPEG); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_JPEGLS); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_RAWVIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_MSMPEG4V1); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_H263P); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_H264); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_ZLIB); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_SNOW); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PNG); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PPM); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PBM); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PGM); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PGMYUV); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PAM); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_FFVHUFF); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_PGMYUV); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_BMP); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_ZMBV); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_TARGA); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_TIFF); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "ogg") {
            encid = GetFFMPEGEncoderID(CODEC_ID_THEORA); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "dv") {
            encid = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "wmv") {
            encid = GetFFMPEGEncoderID(CODEC_ID_WMV1); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_WMV2); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "flv") {
            encid = GetFFMPEGEncoderID(CODEC_ID_FLV1); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_SVQ1); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_FLASHSV); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "rv") {
//          encid = GetFFMPEGEncoderID(CODEC_ID_RV10); strstr << encid << ":" << EncoderNames[encid] << "\n";
//          encid = GetFFMPEGEncoderID(CODEC_ID_RV20); strstr << encid << ":" << EncoderNames[encid] << "\n";
            strstr << ":\n";
        }
        else if (extension == "asv") {
            encid = GetFFMPEGEncoderID(CODEC_ID_ASV1); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_ASV2); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "roq") {
//          encid = GetFFMPEGEncoderID(CODEC_ID_ROQ); strstr << encid << ":" << EncoderNames[encid] << "\n";
            strstr << ":\n";
        }
        else if (extension == "mov") {
//          encid = GetFFMPEGEncoderID(CODEC_ID_QTRLE); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_DVVIDEO); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MPEG4); strstr << encid << ":" << EncoderNames[encid] << "\n";
            encid = GetFFMPEGEncoderID(CODEC_ID_MJPEG); strstr << encid << ":" << EncoderNames[encid] << "\n";
        }
        else if (extension == "m4v" || extension == "mp4") {
//          encid = GetFFMPEGEncoderID(CODEC_ID_H264); strstr << encid << ":" << EncoderNames[encid] << "\n";
            strstr << ":\n";
        }

    }

    encoder_list = strstr.str();
}

void svlVideoCodecFFMPEG::GetEncoderID(int & encoder_id) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetEncoderID - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    encoder_id = data->EncoderID;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::GetQualityBased(bool & enabled) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetQualityBased - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    enabled = data->QualityBased;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::GetTargetQuantizer(double & target_quant) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetTargetQuantizer - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    target_quant = data->TargetQuantizer;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::GetDatarate(int & datarate) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetDatarate - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    datarate = data->Bitrate;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::GetKeyFrameEvery(int & key_every) const
{
    CMN_LOG_CLASS_INIT_VERBOSE << "GetKeyFrameEvery - called" << std::endl;

    svlVideoIO::Compression* compr = GetCompression();
    CompressionData* data = reinterpret_cast<CompressionData*>(&(compr->data[0]));

    key_every = data->GoP;

    svlVideoIO::ReleaseCompression(compr);
}

void svlVideoCodecFFMPEG::BuildIndex()
{
    // If the first `intra_only_threshold` frames are all key-frames,
    // then we will assume that the whole file is intra-only
    const int intra_only_threshold = 20;

    AVPacket packet;
    vctDynamicVector<int> keyframes(Length);
    int frameFinished, ret, i = 0, j, keyframe_count = 0, frame_id;
    int64_t timebase = 1;
    bool useindex = false;

    while (1) {

        // Read out a frame
        while (1) {
            ret = av_read_frame(pFormatCtx, &packet);
            if (ret < 0 || packet.stream_index == VideoStreamID) break;
            av_free_packet(&packet);
        }
        if (ret < 0) {
            break;
        }

        // Decode video frame
#if LIBAVCODEC_VERSION_INT < ((52 << 16) + (25 << 8))
        if (avcodec_decode_video(pDecoderCtx, pFrame, &frameFinished, packet.data, packet.size) < 0) {
#else
// libAVCodec version >= 52.25.0
        if (avcodec_decode_video2(pDecoderCtx, pFrame, &frameFinished, &packet) < 0) {
#endif
            av_free_packet(&packet);
            break;
        }
        if (!frameFinished) {
            av_free_packet(&packet);
            continue;
        }

        if (i == 1) {
            timebase = pFrame->pts;
            if (timebase < 1 || timebase > 1000000) {
                // Something is wrong. Stop indexing.
                break;
            }
            pDecoderCtx->skip_frame = AVDISCARD_NONKEY;
        }

        frame_id = pFrame->pts / timebase;
        if (pFrame->key_frame) {
            keyframes[keyframe_count] = frame_id;

            if (!useindex && keyframe_count != frame_id) useindex = true;
            if (!useindex && keyframe_count == intra_only_threshold) {
                // All the frames were key-frames so far thus we can assume that
                // all the video frames are intra-coded frames
                break;
            }

            keyframe_count ++;
        }

        av_free_packet(&packet);

        i ++;
    }

    pDecoderCtx->skip_frame = AVDISCARD_DEFAULT;

    if (useindex) {
        KeyFrameIndex.SetSize(Length);
        KeyFrameIndex.SetAll(0);

        for (i = 0, j = 0; i < Length; i ++) {
            if (j < keyframe_count && keyframes[j] == i) {
                KeyFrameIndex[i] = keyframes[j];
                j ++;
            }
            else {
                if (j > 0) KeyFrameIndex[i] = keyframes[j - 1];
                else KeyFrameIndex[i] = 0;
            }
        }
    }

    // Seek back to beginning of the video
    LastExtractedFrame = -1;
    Position = -1;
    SetPos(0);


    // Set `UseIndex` flag
    UseIndex = useindex;
}

void svlVideoCodecFFMPEG::ConfigureEncoder()
{
    CV_CODEC_ID codec_id = static_cast<CV_CODEC_ID>(EncoderIDs[Config.EncoderID]);

    pEncoderCtx->max_b_frames = 0;
    pEncoderCtx->gop_size     = Config.GoP;
    pEncoderCtx->pix_fmt      = PIX_FMT_YUV420P;

    if (codec_id == CODEC_ID_MPEG2VIDEO ||
        codec_id == CODEC_ID_HUFFYUV    ||
        codec_id == CODEC_ID_DVVIDEO    ||
        codec_id == CODEC_ID_ZLIB) {
        pEncoderCtx->pix_fmt = PIX_FMT_YUV422P;
    }
    else if (codec_id == CODEC_ID_MJPEG ||
             codec_id == CODEC_ID_LJPEG) {
        pEncoderCtx->pix_fmt = PIX_FMT_YUVJ420P;
    }
    else if (codec_id == CODEC_ID_JPEGLS) {
        pEncoderCtx->pix_fmt = PIX_FMT_RGB24;
    }
}

void svlVideoCodecFFMPEG::BuildEncoderList()
{
    const unsigned int first_num_of_codecs = 100;

    EncoderNames.SetSize(first_num_of_codecs);
    EncoderIDs.SetSize(first_num_of_codecs);

    unsigned int i, encoder_count = 0;
    AVCodec *av_codec = 0;

    // Enumerate video encoders
    for (i = 0; i < first_num_of_codecs; i ++) {
        av_codec = avcodec_find_encoder(static_cast<CV_CODEC_ID>(i));
        if (av_codec && av_codec->type == AVMEDIA_TYPE_VIDEO) {
            EncoderNames[encoder_count] = av_codec->long_name;
            EncoderIDs[encoder_count] = i;
            encoder_count ++;
        }
    }

    // Consolidate name list
    EncoderNames.resize(encoder_count);
    EncoderIDs.resize(encoder_count);
}

int svlVideoCodecFFMPEG::GetFFMPEGEncoderID(CV_CODEC_ID codec_id) const
{
    for (unsigned int i = 0; i < EncoderIDs.size(); i ++) {
        if (EncoderIDs[i] == static_cast<unsigned int>(codec_id)) return i;
    }
    return -1;
}

std::string svlVideoCodecFFMPEG::GetExtensionFromEncoderID(unsigned int encoder_id)
{
    std::string extension;

    switch (EncoderIDs[encoder_id]) {
        case CODEC_ID_MPEG1VIDEO:   // MPEG-1 Video
        case CODEC_ID_MPEG2VIDEO:   // MPEG-2 Video
            extension = "mpg";
        break;

        case CODEC_ID_H261:         // H.261
        case CODEC_ID_H263:         // H.263 / H.263-1996
        case CODEC_ID_MJPEG:        // Motion JPEG
        case CODEC_ID_LJPEG:        // Lossless JPEG
        case CODEC_ID_JPEGLS:       // JPEG-LS
        case CODEC_ID_MPEG4:        // MPEG-4 part 2
        case CODEC_ID_RAWVIDEO:     // Raw video
        case CODEC_ID_MSMPEG4V1:    // MPEG-4 part 2 Microsoft variant version 1
        case CODEC_ID_MSMPEG4V2:    // MPEG-4 part 2 Microsoft variant version 2
        case CODEC_ID_MSMPEG4V3:    // MPEG-4 part 2 Microsoft variant version 3
        case CODEC_ID_H263P:        // H.263+ / H.263-1998 / H.263 version 2
        case CODEC_ID_HUFFYUV:      // HuffYUV
        case CODEC_ID_H264:         // libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10
        case CODEC_ID_FFV1:         // FFmpeg video codec #1
        case CODEC_ID_ZLIB:         // LCL (LossLess Codec Library) ZLIB
        case CODEC_ID_SNOW:         // Snow
        case CODEC_ID_PNG:          // PNG image
        case CODEC_ID_PPM:          // PPM (Portable PixelMap) image
        case CODEC_ID_PBM:          // PBM (Portable BitMap) image
        case CODEC_ID_PGM:          // PGM (Portable GrayMap) image
        case CODEC_ID_PGMYUV:       // PBM (Portable GrayMap YUV) image
        case CODEC_ID_PAM:          // PAM (Portable AnyMap) image
        case CODEC_ID_FFVHUFF:      // Huffyuv FFmpeg variant
        case CODEC_ID_BMP:          // BMP image
        case CODEC_ID_ZMBV:         // Zip Motion Blocks Video
        case CODEC_ID_TARGA:        // Truevision Targa image
        case CODEC_ID_TIFF:         // TIFF image
            extension = "avi";
        break;

        case CODEC_ID_THEORA:       // libtheora Theora
            extension = "ogg";
        break;

        case CODEC_ID_DVVIDEO:      // Digital Video
            extension = "dv";
        break;

        case CODEC_ID_WMV1:         // Windows Media Video 7
        case CODEC_ID_WMV2:         // Windows Media Video 8
            extension = "wmv";
        break;

        case CODEC_ID_FLV1:         // Flash Video (FLV) / Sorenson Spark / Sorenson H.263
        case CODEC_ID_SVQ1:         // Sorenson Vector Quantizer 1
        case CODEC_ID_FLASHSV:      // Flash Screen Video
            extension = "flv";
        break;

        case CODEC_ID_RV10:         // Real Video 1
        case CODEC_ID_RV20:         // Real Video 2
            extension = "rv";
        break;

        case CODEC_ID_ASV1:         // ASUS V1
        case CODEC_ID_ASV2:         // ASUS V2
            extension = "asv";
        break;

        case CODEC_ID_ROQ:         // id RoQ Video
            extension = "roq";
        break;

        case CODEC_ID_QTRLE:       // QuickTime Animation (RLE) video
            extension = "mov";
        break;

        default:
        break;
    }

    return extension;
}

