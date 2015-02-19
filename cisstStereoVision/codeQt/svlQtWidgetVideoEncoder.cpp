/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-05-24

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlQtWidgetVideoEncoder.h>
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlQtDialog.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <ui_svlQtWidgetVideoEncoder.h>

#include <iomanip>


/*************************************/
/*** svlQtWidgetVideoEncoder class ***/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlQtWidgetVideoEncoder, mtsComponent)

svlQtWidgetVideoEncoder::svlQtWidgetVideoEncoder() :
    QObject(),
    mtsComponent(),
    Dialog(0),
    VideoEncoder("RequiresVideoEncoder", this),
    ExternalCodec(0)
{
}

svlQtWidgetVideoEncoder::svlQtWidgetVideoEncoder(const svlQtWidgetVideoEncoder& CMN_UNUSED(other)) :
    QObject(),
    mtsComponent(),
    VideoEncoder("RequiresVideoEncoder", this)
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlQtWidgetVideoEncoder::~svlQtWidgetVideoEncoder()
{
    QSlotDestroy();

    this->RemoveInterfaceRequired("RequiresVideoEncoder");
}

svlQtWidgetVideoEncoder* svlQtWidgetVideoEncoder::New()
{
    return dynamic_cast<svlQtWidgetVideoEncoder*>(svlQtObjectFactory::Create("svlQtWidgetVideoEncoder"));
}

svlQtWidgetVideoEncoder* svlQtWidgetVideoEncoder::New(const std::string & filename)
{
    svlQtWidgetVideoEncoder* instance = dynamic_cast<svlQtWidgetVideoEncoder*>(svlQtObjectFactory::Create("svlQtWidgetVideoEncoder"));
    if (instance) instance->Create(filename);
    return instance;
}

void svlQtWidgetVideoEncoder::Delete()
{
    svlQtObjectFactory::Delete(this);
}

bool svlQtWidgetVideoEncoder::WaitForClose()
{
    if (Dialog) {
        Dialog->EventClosed.Wait();
        return Dialog->Success;
    }
    return false;
}

bool svlQtWidgetVideoEncoder::Create(const std::string & filename)
{
    argFileName = filename;
    QMetaObject::invokeMethod(this, "QSlotCreate", Qt::BlockingQueuedConnection);
    return retSuccess;
}

svlVideoIO::Compression* svlQtWidgetVideoEncoder::GetCodecParams()
{
    QMetaObject::invokeMethod(this, "QSlotGetCodecParams", Qt::BlockingQueuedConnection);
    return retCodecParams;
}

bool svlQtWidgetVideoEncoder::Destroy()
{
    QMetaObject::invokeMethod(this, "QSlotDestroy", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVideoEncoder::Connect(svlVideoCodecBase *codec, const std::string & filename)
{
    argCodec    = codec;
    argFileName = filename;
    QMetaObject::invokeMethod(this, "QSlotConnect", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVideoEncoder::Connect(const std::string & component_name, const std::string & filename)
{
    argComponentName = component_name;
    argFileName      = filename;
    QMetaObject::invokeMethod(this, "QSlotConnect2", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVideoEncoder::Connect(const std::string & process_name, const std::string & component_name, const std::string & filename)
{
    argProcessName   = process_name;
    argComponentName = component_name;
    argFileName      = filename;
    QMetaObject::invokeMethod(this, "QSlotConnect3", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVideoEncoder::Disconnect()
{
    QMetaObject::invokeMethod(this, "QSlotDisconnect", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVideoEncoder::UpdateData()
{
    QMetaObject::invokeMethod(this, "QSlotUpdateData", Qt::BlockingQueuedConnection);
    return retSuccess;
}

void svlQtWidgetVideoEncoder::QSlotOnSliderMove(int value)
{
    std::stringstream strstr;
    strstr << std::fixed << std::setprecision(2) << (0.1 * value);
    UIWidget->TQText->setText(strstr.str().c_str());
}

void svlQtWidgetVideoEncoder::QSlotOnQualityBasedCBStateChanged(int value)
{
    if (value == Qt::Checked) {
        UIWidget->TQText->setDisabled(false);
        UIWidget->TQSlider->setDisabled(false);
        UIWidget->TQLabel->setDisabled(false);
        UIWidget->DatarateText->setDisabled(true);
        UIWidget->DatarateLabel->setDisabled(true);
        UIWidget->DatarateSuffix->setDisabled(true);
        UIWidget->KeyframeText->setDisabled(true);
        UIWidget->KeyframeLabel->setDisabled(true);
    }
    else {
        UIWidget->TQText->setDisabled(true);
        UIWidget->TQSlider->setDisabled(true);
        UIWidget->TQLabel->setDisabled(true);
        UIWidget->DatarateText->setDisabled(false);
        UIWidget->DatarateLabel->setDisabled(false);
        UIWidget->DatarateSuffix->setDisabled(false);
        UIWidget->KeyframeText->setDisabled(false);
        UIWidget->KeyframeLabel->setDisabled(false);
    }
}

void svlQtWidgetVideoEncoder::QSlotCreate()
{
    retSuccess = false;

    if (ExternalCodec) return;

    argCodec = svlVideoIO::GetCodec(argFileName);
    if (!argCodec) return;

    QSlotConnect();
    if (!retSuccess) {
        svlVideoIO::ReleaseCodec(argCodec);
        argCodec = 0;
    }
}

void svlQtWidgetVideoEncoder::QSlotGetCodecParams()
{
    if (ExternalCodec) {
        retCodecParams = ExternalCodec->GetCompression();
    }
    else {
        retCodecParams = 0;
    }
}

void svlQtWidgetVideoEncoder::QSlotDestroy()
{
    retSuccess = false;

    if (!ExternalCodec) return;

    svlVideoCodecBase* codec = ExternalCodec;

    QSlotDisconnect();

    svlVideoIO::ReleaseCodec(codec);

    retSuccess = true;
}

void svlQtWidgetVideoEncoder::QSlotConnect()
{
    retSuccess = false;

    if (ExternalCodec || !ExternalCodecName.empty() || !argCodec) return;

    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();

    if (LCM->FindComponent(this->GetName()) == false) {
        CMN_LOG_CLASS_INIT_DEBUG << "Connect - adding dialog component (\"" << this->GetName() << "\") to LCM" << std::endl;

        if (LCM->AddComponent(this) == false) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect - failed to add dialog component (\"" << this->GetName() << "\") to LCM" << std::endl;
            return;
        }

        osaSleep(0.25); // BV TEMP (remove when AddComponent is a blocking command)
    }

    if (LCM->FindComponent(argCodec->GetName()) == false) {
        CMN_LOG_CLASS_INIT_DEBUG << "Connect - adding codec component (\"" << argCodec->GetName() << "\") to LCM" << std::endl;

        if (LCM->AddComponent(argCodec) == false) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect - failed to add codec component (\"" << argCodec->GetName() << "\") to LCM" << std::endl;
            return;
        }

        osaSleep(0.25); // BV TEMP (remove when AddComponent is a blocking command)
    }

    if (LCM->Connect(this->GetName(), "RequiresVideoEncoder", argCodec->GetName(), "ProvidesVideoEncoder") == false) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect - failed to connect dialog component (\"" << this->GetName()
                                 << "\") to codec component (\"" << argCodec->GetName() << "\")" << std::endl;
        return;
    }

    osaSleep(1.0); // BV TEMP (remove when Connect is a blocking command)

    std::string extension;
    svlVideoIO::GetExtension(argFileName, extension);
    VideoEncoder.SetExtension(extension);
    osaSleep(0.25); // BV TEMP (remove when command blocks)

    ExternalCodec = argCodec;

    ///////////////////
    // Create widget

    Dialog = new svlQtDialog;
    CMN_ASSERT(Dialog);

    UIWidget = new Ui_WidgetVideoEncoder;
    CMN_ASSERT(UIWidget);
    UIWidget->setupUi(Dialog);

    QObject::connect(UIWidget->TQSlider, SIGNAL(valueChanged(int)), this, SLOT(QSlotOnSliderMove(int)));
    QObject::connect(UIWidget->QualityBasedCB, SIGNAL(stateChanged(int)), this, SLOT(QSlotOnQualityBasedCBStateChanged(int)));

    std::stringstream strstr;
    strstr << std::fixed << std::setprecision(2) << (0.1 * UIWidget->TQSlider->value());
    UIWidget->TQText->setText(strstr.str().c_str());

    Dialog->setWindowTitle(tr("Video Encoder Settings"));

    QSlotUpdateData();
    if (retSuccess) {
        Dialog->show();
        Dialog->raise();
        Dialog->activateWindow();
    }
}

void svlQtWidgetVideoEncoder::QSlotConnect2()
{
    retSuccess = false;

    if (ExternalCodec || !ExternalCodecName.empty()) return;

    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();

    if (LCM->FindComponent(this->GetName()) == false) {
        CMN_LOG_CLASS_INIT_DEBUG << "Connect - adding dialog component (\"" << this->GetName() << "\") to LCM" << std::endl;

        if (LCM->AddComponent(this) == false) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect - failed to add dialog component (\"" << this->GetName() << "\") to LCM" << std::endl;
            return;
        }

        osaSleep(0.25); // BV TEMP (remove when AddComponent is a blocking command)
    }

    if (LCM->Connect(this->GetName(), "RequiresVideoEncoder", argComponentName, "ProvidesVideoEncoder") == false) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect - failed to connect dialog component (\"" << this->GetName()
                                 << "\") to codec component (\"" << argComponentName << "\")" << std::endl;
        return;
    }

    osaSleep(1.0); // BV TEMP (remove when Connect is a blocking command)

    std::string extension;
    svlVideoIO::GetExtension(argFileName, extension);
    VideoEncoder.SetExtension(extension);
    osaSleep(0.25); // BV TEMP (remove when command blocks)

    ExternalCodecName = argComponentName;

    ///////////////////
    // Create widget

    Dialog = new svlQtDialog;
    CMN_ASSERT(Dialog);

    UIWidget = new Ui_WidgetVideoEncoder;
    CMN_ASSERT(UIWidget);
    UIWidget->setupUi(Dialog);

    QObject::connect(UIWidget->TQSlider, SIGNAL(sliderMoved(int)), this, SLOT(QSlotOnSliderMove(int)));
    QObject::connect(UIWidget->QualityBasedCB, SIGNAL(stateChanged(int)), this, SLOT(QSlotOnQualityBasedCBStateChanged(int)));

    std::stringstream strstr;
    strstr << std::fixed << std::setprecision(2) << (0.1 * UIWidget->TQSlider->value());
    UIWidget->TQText->setText(strstr.str().c_str());

    Dialog->setWindowTitle(tr("Video Encoder Settings"));

    QSlotUpdateData();
    if (retSuccess) {
        Dialog->show();
        Dialog->raise();
        Dialog->activateWindow();
    }
}

void svlQtWidgetVideoEncoder::QSlotConnect3()
{
    retSuccess = false;
    CMN_LOG_CLASS_INIT_ERROR << "Connect(process_name, ...) - not yet implemented" << std::endl;
    return;
}

void svlQtWidgetVideoEncoder::QSlotDisconnect()
{
    retSuccess = false;

    if (!ExternalCodec &&  ExternalCodecName.empty()) return;

    std::string codec_name;
    if (ExternalCodecName.empty()) codec_name = ExternalCodec->GetName();
    else codec_name = ExternalCodecName;

    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();

    if (!LCM->Disconnect(this->GetName(), "RequiresVideoEncoder", codec_name, "ProvidesVideoEncoder")) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect - failed to disconnect component (\"" << codec_name << "\")" << std::endl;
        return;
    }

    ExternalCodec = 0;
    ExternalCodecName.clear();

    ////////////////////
    // Destroy widget

    Dialog->close();
    delete Dialog;
    Dialog = 0;
    delete UIWidget;
    UIWidget = 0;

    retSuccess = true;
}

void svlQtWidgetVideoEncoder::QSlotUpdateData()
{
    retSuccess = false;

    // TO DO: check if connected

    bool compression_level_enabled = false;
    bool encoder_list_enabled      = false;
    bool target_quantizer_enabled  = false;
    bool datarate_enabled          = false;
    bool keyframe_every_enabled    = false;
    int compression_level          = -1;
    std::string encoder_list;
    int encoder_id                 = -1;
    bool quality_based             = false;
    double target_quantizer        = -1.0;
    int datarate                   = -1;
    int keyframe_every             = -1;

    // Get features supported by the encoder
    VideoEncoder.IsCompressionLevelEnabled(compression_level_enabled);
    //std::cerr << "Compression level enabled=" << compression_level_enabled << std::endl;
    VideoEncoder.IsEncoderListEnabled(encoder_list_enabled);
    //std::cerr << "Encoder list enabled=" << encoder_list_enabled << std::endl;
    VideoEncoder.IsTargetQuantizerEnabled(target_quantizer_enabled);
    //std::cerr << "Target quantizer enabled=" << target_quantizer_enabled << std::endl;
    VideoEncoder.IsDatarateEnabled(datarate_enabled);
    //std::cerr << "Datarate enabled=" << datarate_enabled << std::endl;
    VideoEncoder.IsKeyFrameEveryEnabled(keyframe_every_enabled);
    //std::cerr << "Keyframe every enabled=" << keyframe_every_enabled << std::endl;

    // Get current encoder settings
    if (compression_level_enabled) {
        VideoEncoder.GetCompressionLevel(compression_level);
        //std::cerr << "Compression level: " << compression_level << std::endl;
    }
    if (encoder_list_enabled) {
        VideoEncoder.GetEncoderList(encoder_list);
        //std::cerr << "Encoder list: " << encoder_list << std::endl;
        VideoEncoder.GetEncoderID(encoder_id);
        //std::cerr << "Encoder ID: " << encoder_id << std::endl;
    }
    VideoEncoder.GetQualityBased(quality_based);
    //std::cerr << "Quality based: " << quality_based << std::endl;
    VideoEncoder.GetTargetQuantizer(target_quantizer);
    //std::cerr << "Target quantizer: " << target_quantizer << std::endl;
    VideoEncoder.GetDatarate(datarate);
    //std::cerr << "Datarate: " << datarate << std::endl;
    VideoEncoder.GetKeyFrameEvery(keyframe_every);
    //std::cerr << "Key frame every: " << keyframe_every << std::endl;

    EncoderIDs.SetSize(0);
    EncoderItems.SetSize(0);

    int count = 0, encoder_list_id;
    std::string encoder_info, encoder_name;
    size_t begpos = 0, pos = encoder_list.find("\n"), pos2;
    while (pos != std::string::npos) {

        while (1) {
            if (begpos >= pos) break;

            // Extract encoder info string
            encoder_info = encoder_list.substr(begpos, pos - begpos);

            // Extract encoder id from info string
            pos2 = encoder_info.find(':');
            if (pos == std::string::npos || pos2 <= 0) break;

            encoder_list_id = atoi(encoder_info.substr(0, pos2).c_str());
            if (encoder_list_id < 0 || encoder_list_id > 1000) break;

            begpos += pos2 + 1;
            if (begpos >= pos) break;

            // Extract encoder name from info string
            encoder_name = encoder_info.substr(pos2 + 1, pos - begpos);

            EncoderIDs.resize(count + 1);
            EncoderItems.resize(count + 1);

            EncoderItems[count] = new QListWidgetItem(encoder_name.c_str());
            UIWidget->EncoderList->addItem(EncoderItems[count]);

            EncoderIDs[count] = encoder_list_id;

            count ++;

            break;
        }

        begpos = pos + 1;
        pos = encoder_list.find("\n", begpos);
    }
    if (EncoderIDs.size() > 0) {
        UIWidget->EncoderList->setDisabled(false);

        // Find encoder_id in the list
        int found_id = EncoderIDs.size() - 1;
        while (found_id >= 0 && EncoderIDs[found_id] != encoder_id) found_id --;
        if (found_id < 0) found_id = 0;

        UIWidget->EncoderList->setCurrentItem(EncoderItems[found_id]);
    }
    else {
        // No encoder in the list
        UIWidget->EncoderList->setDisabled(true);
    }

    // TO DO: finish this up
    UIWidget->AllEncodersCB->setDisabled(true);

    std::stringstream strstr;
    strstr << std::fixed << std::setprecision(2) << target_quantizer;
    UIWidget->TQText->setText(strstr.str().c_str());
    UIWidget->TQSlider->setValue(static_cast<int>(target_quantizer * 10.0));

    UIWidget->DatarateText->setText(QString::number(datarate));

    UIWidget->KeyframeText->setText(QString::number(keyframe_every));

    if (quality_based) {
        UIWidget->QualityBasedCB->setCheckState(Qt::Checked);
        UIWidget->TQText->setDisabled(false);
        UIWidget->TQSlider->setDisabled(false);
        UIWidget->TQLabel->setDisabled(false);
        UIWidget->DatarateText->setDisabled(true);
        UIWidget->DatarateLabel->setDisabled(true);
        UIWidget->DatarateSuffix->setDisabled(true);
        UIWidget->KeyframeText->setDisabled(true);
        UIWidget->KeyframeLabel->setDisabled(true);
    }
    else {
        UIWidget->QualityBasedCB->setCheckState(Qt::Unchecked);
        UIWidget->TQText->setDisabled(true);
        UIWidget->TQSlider->setDisabled(true);
        UIWidget->TQLabel->setDisabled(true);
        UIWidget->DatarateText->setDisabled(false);
        UIWidget->DatarateLabel->setDisabled(false);
        UIWidget->DatarateSuffix->setDisabled(false);
        UIWidget->KeyframeText->setDisabled(false);
        UIWidget->KeyframeLabel->setDisabled(false);
    }

    if (!target_quantizer_enabled ||
        (!datarate_enabled && !keyframe_every_enabled)) {
        UIWidget->QualityBasedCB->setDisabled(true);
    }
    else {
        UIWidget->QualityBasedCB->setDisabled(false);
    }

    retSuccess = true;
}

