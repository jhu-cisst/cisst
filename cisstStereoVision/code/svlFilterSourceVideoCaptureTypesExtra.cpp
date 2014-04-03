/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterSourceVideoCaptureTypes.h>
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>

std::string svlFilterSourceVideoCaptureTypes::DeviceInfo::HumanReadable(void) const
{
    std::stringstream stream;
    stream << this->name;
    switch (this->platform) {
        case svlFilterSourceVideoCaptureTypes::WinDirectShow:
            stream << " [DirectShow device]";
        break;

        case svlFilterSourceVideoCaptureTypes::WinSVS:
            stream << " [Videre Design SVS device]";
        break;

        case svlFilterSourceVideoCaptureTypes::LinVideo4Linux2:
            stream << " [Video4Linux2 device]";
        break;

        case svlFilterSourceVideoCaptureTypes::LinLibDC1394:
            stream << " [DC1394 IIDC/DCAM device]";
        break;

        case svlFilterSourceVideoCaptureTypes::OpenCV:
            stream << " [Detected by OpenCV]";
        break;

        case svlFilterSourceVideoCaptureTypes::MatroxImaging:
            stream << " [Matrox Imaging device]";
        break;

        case svlFilterSourceVideoCaptureTypes::BlackMagicDeckLink:
            stream << " [BlackMagic DeckLink device]";
        break;

        case svlFilterSourceVideoCaptureTypes::NumberOfPlatformTypes:
            stream << " [Unknown device type]";
        default:
        break;
    }

    if (this->testok) stream << " [Tested]";

    stream << std::endl;

    for (int i = 0; i < this->inputcount; i ++) {

        if (i == 0) stream << "   Inputs:"
                           << std::endl;
        stream << "    "
               << i
               << ") ";

        if (i != this->activeinput) stream << " ";
        else stream << "*";

        stream << this->inputnames[i]
               << std::endl;
    }

    return stream.str();
}

std::string svlFilterSourceVideoCaptureTypes::ImageProperties::HumanReadable(void) const
{
    std::stringstream stream;
    stream << " shutter:       "
           << this->shutter;
    if (this->manual & svlFilterSourceVideoCapture::propShutter) stream << " auto";
    stream << ", ";

    stream << " gain:          "
           << this->gain;
    if (this->manual & svlFilterSourceVideoCapture::propGain) stream << " auto";
    stream << ", ";

    stream << " white balance: "
           << this->wb_u_b
           << ", "
           << this->wb_v_r;
    if (this->manual & svlFilterSourceVideoCapture::propWhiteBalance) stream << " auto";
    stream << ", ";

    stream << " brightness:    "
           << this->brightness;
    if (this->manual & svlFilterSourceVideoCapture::propBrightness) stream << " auto";
    stream << ", ";

    stream << " gamma:         "
           << this->gamma;
    if (this->manual & svlFilterSourceVideoCapture::propGamma) stream << " auto";
    stream << ", ";

    stream << " saturation:    "
           << this->saturation;
    if (this->manual & svlFilterSourceVideoCapture::propSaturation) stream << " auto";

    return stream.str();
}

std::string svlFilterSourceVideoCaptureTypes::Config::HumanReadable(void) const
{
    std::stringstream stream;
    for (int i = 0; i < this->Channels; i ++) {
        if (i > 0) {
            stream << ", (";
        } else {
            stream << "(";
        }
        stream << this->Device[i] << ", "
               << this->Input[i]  << ")";
    }
    return stream.str();
}

void svlFilterSourceVideoCaptureTypes::Config::SetChannels(const int channels)
{
    if (channels < 0) return;

    Device.resize(channels);
    Input.resize(channels);
    Format.resize(channels);
    Properties.resize(channels);
    Trigger.resize(channels);

    Channels = channels;
    Device.assign(channels, -1);
    Input.assign(channels, -1);
    for (int i = 0; i < channels; i ++) {
        memset(&(Format[i]),     0, sizeof(svlFilterSourceVideoCapture::ImageFormat));
        memset(&(Properties[i]), 0, sizeof(svlFilterSourceVideoCapture::ImageProperties));
        memset(&(Trigger[i]),    0, sizeof(svlFilterSourceVideoCapture::ExternalTrigger));
    }
}

std::string svlFilterSourceVideoCaptureTypes::ImageFormat::HumanReadable(void) const
{
    std::stringstream stream;
    stream << " "
           << this->width
           << "x"
           << this->height
           << " ";

    stream << svlFilterSourceVideoCapture::GetPixelTypeName(this->colorspace);

    if (this->framerate > 0.0) {

        stream << " (<="
               << this->framerate
               << "fps)"
               << std::endl;
    }
    else {

        stream << " (unknown framerate)"
               << std::endl;
    }

    if (this->custom_mode >= 0) {

        stream << "  [CUSTOM mode="
               << this->custom_mode
               << std::endl;

        stream << "          maxsize=("
               << this->custom_maxwidth
               << ", "
               << this->custom_maxheight
               << "); ";

        stream << "unit=(" << this->custom_unitwidth
        << ", "
        << this->custom_unitheight
        << ")"
        << std::endl;

        stream << "          roipos=("
               << this->custom_roileft
               << ", "
               << this->custom_roitop
               << "); ";

        stream << "unit=("
               << this->custom_unitleft
               << ", "
               << this->custom_unittop
               << ")"
               << std::endl;

        stream << "          colorspaces=(";

        for (unsigned int j = 0;
             j < svlFilterSourceVideoCaptureTypes::PixelTypeCount &&
             this->custom_colorspaces[j] != svlFilterSourceVideoCaptureTypes::PixelUnknown;
             j ++) {

            if (j > 0) std::cout << ", ";

            stream << svlFilterSourceVideoCapture::GetPixelTypeName(this->custom_colorspaces[j]);
        }

        stream << ")"
               << std::endl;

        stream << "          pattern="
               << svlFilterSourceVideoCapture::GetPatternTypeName(this->custom_pattern)
               << "]"
               << std::endl;
    }
    return stream.str();
}
