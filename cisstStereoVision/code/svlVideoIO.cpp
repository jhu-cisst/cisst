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

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlTypes.h>

#if (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
    #include "commdlg.h"
#endif // CISST_WINDOWS


const static int CACHE_SIZE_STEP = 5;

/*******************************/
/*** svlVideoIO class **********/
/*******************************/

svlVideoIO::svlVideoIO()
{
    int handlers = 0;
    svlVideoCodecBase* ft;
    cmnGenericObject* go;

    Codecs.SetSize(256);
    Names.SetSize(256);
    Extensions.SetSize(256);

    // Go through all registered classes
    for (cmnClassRegister::const_iterator iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        if ((*iter).second && (handlers < 256) && (*iter).second->IsDerivedFrom<svlVideoCodecBase>()) {
            go = (*iter).second->Create();
            ft = dynamic_cast<svlVideoCodecBase*>(go);
            if (ft) {
                Codecs[handlers]     = (*iter).second;
                Names[handlers]      = ft->GetEncoderName();
                Extensions[handlers] = ft->GetExtensions();
                handlers ++;
            }
            (*iter).second->Delete(go);
        }
    }

    Codecs.resize(handlers);
    Names.resize(handlers);
    Extensions.resize(handlers);
    CodecCache.SetSize(handlers);
    CodecCacheUsed.SetSize(handlers);
}

svlVideoIO* svlVideoIO::GetInstance()
{
    static svlVideoIO Instance;
    return &Instance;
}

svlVideoIO::~svlVideoIO()
{
    const unsigned int size = static_cast<unsigned int>(CodecCache.size());
    unsigned int i, j, cachesize;
    for (i = 0; i < size; i ++) {
        cachesize = static_cast<unsigned int>(CodecCache[i].size());
        for (j = 0; j < cachesize; j ++) {
            delete CodecCache[i][j];
        }
    }
}

int svlVideoIO::DialogCodec(const std::string &filename, Compression **compression)
{
    if (!compression) return SVL_FAIL;

    std::string path(filename);

    if (path.empty()) {
        std::cout << std::endl << "Supported file types:" << std::endl;

        // Get format list
        std::string list;
        if (GetFormatList(list) != SVL_OK) {
            std::cout << " -!- No video codecs are available" << std::endl;
            return SVL_FAIL;
        }

        std::string format;
        size_t beg = 0, end;

        while (1) {
            // find format delimiter
            end = list.find('\n', beg);
            if (end == std::string::npos) break;
            format = list.substr(beg, end - beg);
            beg = end + 1;

            std::cout << " - " << format << std::endl;
        }

        std::cout << "Enter file extension: ";
        std::cin >> path;
        std::cin.ignore();
        if (path[0] != '.') path.insert(0, ".");
    }

    svlVideoCodecBase* codec = GetCodec(path);
    if (!codec) return SVL_FAIL;

    // Set previous value (if available) as default
    codec->SetCompression(compression[0]);
    ReleaseCompression(compression[0]);

    int ret = SVL_OK;

    if (codec->DialogCompression(filename) == SVL_OK) {
        compression[0] = codec->GetCompression();
        if (!(compression[0])) ret = SVL_FAIL;
    }

    ReleaseCodec(codec);

    return ret;
}

int svlVideoIO::DialogFilePath(bool save, const std::string &title, std::string &filename)
{
#if (CISST_OS == CISST_WINDOWS)

    // WINDOWS: Open a standard file open dialog box

    // Build filter
    std::string filter;
    if (GetWindowsExtensionFilter(filter) != SVL_OK) {
        // No video file codecs found
        return SVL_FAIL;
    }

    // Create dialog box
    BOOL ok;
    char path[2048];
    memset(path, 0, 2048);
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= path;
    ofn.nMaxFile = 2048;
    ofn.lpstrTitle = title.c_str();

    // Removing the keyboard focus from the parent window
    SetForegroundWindow(GetDesktopWindow());

    if (save) {
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
        ok = GetSaveFileName(&ofn);
    }
    else {
        ofn.Flags = OFN_ENABLESIZING | OFN_NOCHANGEDIR;
        ok = GetOpenFileName(&ofn);
    }

    if (ok) {
        filename.assign(path);
        return SVL_OK;
    }

    return SVL_FAIL;

#else // (CISST_OS == CISST_WINDOWS)

    // CONSOLE: list supported file types and ask for filename

    std::cout << std::endl << "Supported file types:" << std::endl;

    // Get format list
    std::string list;
    if (GetFormatList(list) != SVL_OK) {
        // 'save' is used only to prevent GCC from complaining
        if (save) std::cout << " -!- No video codecs are available" << std::endl;
        else std::cout << " -!- No video codecs are available" << std::endl;
        return SVL_FAIL;
    }

    std::string format;
    size_t beg = 0, end;

    while (1) {
        // find format delimiter
        end = list.find('\n', beg);
        if (end == std::string::npos) break;
        format = list.substr(beg, end - beg);
        beg = end + 1;

        std::cout << " - " << format << std::endl;
    }

    std::cout << title << ": ";
    std::cin >> filename;
    std::cin.ignore();

    return SVL_OK;

#endif // (CISST_OS == CISST_WINDOWS)
}

int svlVideoIO::GetFormatList(std::string &formatlist)
{
    svlVideoIO* instance = GetInstance();

    const unsigned int codeccount = static_cast<unsigned int>(instance->Names.size());
    if (codeccount < 1) return SVL_FAIL;

    char ext[128];
    std::istringstream in;
    std::ostringstream out;
    unsigned int i, j;

    for (i = 0; i < codeccount; i ++) {
        out << instance->Names[i] << " (";
        in.str(instance->Extensions[i]);

        j = 0;
        while (1) {
            memset(ext, 0, 128);
            if (in.get(ext, 127, ';').fail()) break;
            in.seekg(1, std::ios_base::cur);

            if (j ++ > 0) out << "; ";
            out << "*" << ext;
        }

        in.clear();
        out << ")\n";
    }

    formatlist.assign(out.str());

    return SVL_OK;
}

int svlVideoIO::GetWindowsExtensionFilter(std::string &filter)
{
    svlVideoIO* instance = GetInstance();

    const unsigned int codeccount = static_cast<unsigned int>(instance->Names.size());
    if (codeccount < 1) return SVL_FAIL;

    char ext[128], zero = 0;
    std::istringstream in;
    std::ostringstream out1, out2;
    unsigned int i, j;

    // Add 'All Video Files' field
    out1 << "All Video Files (";
    for (i = 0, j = 0; i < codeccount; i ++) {
        in.str(instance->Extensions[i]);
        while (1) {
            memset(ext, 0, 128);
            if (in.get(ext, 127, ';').fail()) break;
            in.seekg(1, std::ios_base::cur);

            if (j ++ > 0) {
                out1 << "; ";
                out2 << ";";
            }
            out1 << "*" << ext;
            out2 << "*" << ext;
        }
        in.clear();
    }
    out1 << ")";
    out1.write(&zero, 1);
    out1 << out2.str();
    out1.write(&zero, 1);

    // Add single file type fields
    for (i = 0; i < codeccount; i ++) {
        out2.str("");
        out1 << instance->Names[i] << " (";
        in.str(instance->Extensions[i]);

        j = 0;
        while (1) {
            memset(ext, 0, 128);
            if (in.get(ext, 127, ';').fail()) break;
            in.seekg(1, std::ios_base::cur);

            if (j ++ > 0) {
                out1 << "; ";
                out2 << ";";
            }
            out1 << "*" << ext;
            out2 << "*" << ext;
        }
        in.clear();
        out1 << ")";
        out1.write(&zero, 1);
        out1 << out2.str();
        out1.write(&zero, 1);
    }

    // Add all files field
    out1 << "All Files (*.*)";
    out1.write(&zero, 1);
    out1 << "*.*";
    out1.write(&zero, 1);

    filter.assign(out1.str());

    return SVL_OK;
}

int svlVideoIO::GetExtension(const std::string &filename, std::string &extension)
{
    size_t dotpos = filename.find_last_of('.');
    // If no '.' is found, then take the whole string as extension
    if (dotpos != std::string::npos) {
        extension = filename.substr(dotpos + 1);
    }
    else {
        extension = filename;
    }

    // Convert upper-case characters to lower-case
    char ch;
    const int offset = 'a' - 'A';
    const unsigned int len = static_cast<unsigned int>(extension.size());
    if (len == 0) return SVL_FAIL;

    for (unsigned int i = 0; i < len; i ++) {
        ch = extension[i];
        if (ch >= 'A' && ch <= 'Z') {
            extension[i] = ch + offset;
        }
        else {
            extension[i] = ch;
        }
    }

    return SVL_OK;
}

svlVideoCodecBase* svlVideoIO::GetCodec(const std::string &filename)
{
    // Extension list format:
    //      ('.' + [a..z0..9]* + ';')*

    std::string extension;
    if (GetExtension(filename, extension) != SVL_OK) return 0;
    extension.insert(0, ".");
    extension.append(";");

    svlVideoIO* instance = GetInstance();
    const unsigned int size = static_cast<unsigned int>(instance->Codecs.size());
    unsigned int i;
    int j, cachesize, cacheitem;

    for (i = 0; i < size; i ++) {
        if (instance->Extensions[i].find(extension) != std::string::npos) {

#if (CISST_OS == CISST_WINDOWS)
            // Exception for AVI files in Windows:
            //   Use Video for Windows instead of OpenCV
            if (extension == ".avi;" &&
                instance->Names[i].find("FFMPEG") != std::string::npos) continue;
#endif // (CISST_OS == CISST_WINDOWS)

            ///////////////////////////
            // Enter critical section
            instance->CS.Enter();

            // check if we have any unused video handlers in the cache
            cacheitem = 0;
            cachesize = static_cast<int>(instance->CodecCacheUsed[i].size());
            while (cacheitem < cachesize && instance->CodecCacheUsed[i][cacheitem]) cacheitem ++;

            // if there is no unused video handler in the
            // cache, then increase the size of cache
            if (cacheitem >= cachesize) {
                cacheitem = cachesize;
                cachesize += CACHE_SIZE_STEP;
                instance->CodecCache[i].resize(cachesize);
                instance->CodecCacheUsed[i].resize(cachesize);
                for (j = cacheitem; j < cachesize; j ++) {
                    instance->CodecCache[i][j] = dynamic_cast<svlVideoCodecBase*>(instance->Codecs[i]->Create());
                    instance->CodecCacheUsed[i][j] = false;
                }
            }

            instance->CodecCacheUsed[i][cacheitem] = true;

            instance->CS.Leave();
            // Leave critical section
            ///////////////////////////

            return instance->CodecCache[i][cacheitem];
        }
    }

    return 0;
}

void svlVideoIO::ReleaseCodec(svlVideoCodecBase* codec)
{
    if (!codec) return;

    svlVideoIO* instance = GetInstance();

    ///////////////////////////
    // Enter critical section
    instance->CS.Enter();

    const unsigned int size = static_cast<unsigned int>(instance->CodecCache.size());
    unsigned int i, j, cachesize;
    for (i = 0; i < size; i ++) {
        cachesize = static_cast<unsigned int>(instance->CodecCache[i].size());
        for (j = 0; j < cachesize; j ++) {
            if (codec == instance->CodecCache[i][j]) {
                codec->Close();
                instance->CodecCacheUsed[i][j] = false;

                instance->CS.Leave();
                // Leave critical section
                ///////////////////////////

                return;
            }
        }
    }

    instance->CS.Leave();
    // Leave critical section
    ///////////////////////////
}

int svlVideoIO::ReleaseCompression(svlVideoIO::Compression *compression)
{
    if (!compression) return SVL_FAIL;
    delete [] reinterpret_cast<unsigned char*>(compression);
    return SVL_OK;
}


/**********************************/
/*** svlVideoCodecBase class ******/
/**********************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecBase, mtsComponent)

svlVideoCodecBase::svlVideoCodecBase() :
    mtsComponent(),
    Codec(0),
    Multithreaded(false),
    VariableFramerate(false),
    ComponentAddedtoLCM(false)
{
    CreateInterfaces();
}

svlVideoCodecBase::~svlVideoCodecBase()
{
    svlVideoIO::ReleaseCompression(Codec);
}

const std::string& svlVideoCodecBase::GetEncoderName() const
{
    return EncoderName;
}

const std::string& svlVideoCodecBase::GetExtensions() const
{
    return ExtensionList;
}

bool svlVideoCodecBase::IsMultithreaded() const
{
    return Multithreaded;
}

bool svlVideoCodecBase::IsVariableFramerate() const
{
    return VariableFramerate;
}

int svlVideoCodecBase::SetPos(const int CMN_UNUSED(pos))
{
    return SVL_FAIL;
}

double svlVideoCodecBase::GetBegTime() const
{
    return -1.0;
}

double svlVideoCodecBase::GetEndTime() const
{
    return -1.0;
}

double svlVideoCodecBase::GetTimeAtPos(const int CMN_UNUSED(pos)) const
{
    return -1.0;
}

int svlVideoCodecBase::GetPosAtTime(const double CMN_UNUSED(time)) const
{
    return -1;
}

svlVideoIO::Compression* svlVideoCodecBase::GetCompression() const
{
    return 0;
}

int svlVideoCodecBase::SetCompression(const svlVideoIO::Compression * CMN_UNUSED(compression))
{
    return SVL_FAIL;
}

int svlVideoCodecBase::DialogCompression()
{
    return SVL_FAIL;
}

int svlVideoCodecBase::DialogCompression(const std::string &filename)
{
    if (!filename.empty()) {
        CMN_LOG_INIT_WARNING << "svlVideoCodecBase::DialogCompression - This codec (" << GetEncoderName()
                             << ") ignores filename argument (" << filename << ")" << std::endl;
    }
    return DialogCompression();
}

int svlVideoCodecBase::SetTimestamp(const double CMN_UNUSED(timestamp))
{
    return SVL_FAIL;
}

void svlVideoCodecBase::SetName(const std::string &name)
{
    EncoderName = name;
}

void svlVideoCodecBase::SetExtensionList(const std::string &list)
{
    ExtensionList = list;
}

void svlVideoCodecBase::SetMultithreaded(bool multithreaded)
{
    Multithreaded = multithreaded;
}

void svlVideoCodecBase::SetVariableFramerate(bool variableframerate)
{
    VariableFramerate = variableframerate;
}

void svlVideoCodecBase::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("ProvidesVideoEncoder", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlVideoCodecBase::SetExtension,              this, "SetExtension");
        provided->AddCommandWrite(&svlVideoCodecBase::SetEncoderID,              this, "SetEncoderID");
        provided->AddCommandWrite(&svlVideoCodecBase::SetCompressionLevel,       this, "SetCompressionLevel");
        provided->AddCommandWrite(&svlVideoCodecBase::SetQualityBased,           this, "SetQualityBased");
        provided->AddCommandWrite(&svlVideoCodecBase::SetTargetQuantizer,        this, "SetTargetQuantizer");
        provided->AddCommandWrite(&svlVideoCodecBase::SetDatarate,               this, "SetDatarate");
        provided->AddCommandWrite(&svlVideoCodecBase::SetKeyFrameEvery,          this, "SetKeyFrameEvery");
        provided->AddCommandRead (&svlVideoCodecBase::IsCompressionLevelEnabled, this, "IsCompressionLevelEnabled");
        provided->AddCommandRead (&svlVideoCodecBase::IsEncoderListEnabled,      this, "IsEncoderListEnabled");
        provided->AddCommandRead (&svlVideoCodecBase::IsTargetQuantizerEnabled,  this, "IsTargetQuantizerEnabled");
        provided->AddCommandRead (&svlVideoCodecBase::IsDatarateEnabled,         this, "IsDatarateEnabled");
        provided->AddCommandRead (&svlVideoCodecBase::IsKeyFrameEveryEnabled,    this, "IsKeyFrameEveryEnabled");
        provided->AddCommandRead (&svlVideoCodecBase::GetCompressionLevel,       this, "GetCompressionLevel");
        provided->AddCommandRead (&svlVideoCodecBase::GetEncoderList,            this, "GetEncoderList");
        provided->AddCommandRead (&svlVideoCodecBase::GetEncoderID,              this, "GetEncoderID");
        provided->AddCommandRead (&svlVideoCodecBase::GetQualityBased,           this, "GetQualityBased");
        provided->AddCommandRead (&svlVideoCodecBase::GetTargetQuantizer,        this, "GetTargetQuantizer");
        provided->AddCommandRead (&svlVideoCodecBase::GetDatarate,               this, "GetDatarate");
        provided->AddCommandRead (&svlVideoCodecBase::GetKeyFrameEvery,          this, "GetKeyFrameEvery");
    }
}

