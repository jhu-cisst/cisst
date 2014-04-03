/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-06-03

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlQtWidgetVidCapSrcImageProperties.h>
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlQtDialog.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <ui_svlQtWidgetVidCapSrcImageProperties.h>

#include <QString>

// Forward declarations
void* svlQtWidgetVidCapSrcImagePropertiesThreadProc(svlQtWidgetVidCapSrcImageProperties* obj);


/*************************************************/
/*** svlQtWidgetVidCapSrcImageProperties class ***/
/*************************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlQtWidgetVidCapSrcImageProperties, mtsComponent)

svlQtWidgetVidCapSrcImageProperties::svlQtWidgetVidCapSrcImageProperties() :
    QObject(),
    mtsComponent(),
    Thread(0),
    Dialog(0),
    UIWidget(0),
    VideoCaptureSettings("RequiresVideoCaptureSettings", this),
    argFilter(0),
    retSuccess(false)
{
}

svlQtWidgetVidCapSrcImageProperties::svlQtWidgetVidCapSrcImageProperties(const svlQtWidgetVidCapSrcImageProperties& CMN_UNUSED(other)) :
    QObject(),
    mtsComponent(),
    Thread(0),
    Dialog(0),
    UIWidget(0),
    VideoCaptureSettings("RequiresVideoCaptureSettings", this),
    argFilter(0),
    retSuccess(false)
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlQtWidgetVidCapSrcImageProperties::~svlQtWidgetVidCapSrcImageProperties()
{
    QSlotDestroy();

    this->RemoveInterfaceRequired("RequiresVideoCaptureSettings");
}

svlQtWidgetVidCapSrcImageProperties* svlQtWidgetVidCapSrcImageProperties::New()
{
    svlQtWidgetVidCapSrcImageProperties* instance = dynamic_cast<svlQtWidgetVidCapSrcImageProperties*>(svlQtObjectFactory::Create("svlQtWidgetVidCapSrcImageProperties"));
    return instance;
}

svlQtWidgetVidCapSrcImageProperties* svlQtWidgetVidCapSrcImageProperties::New(svlFilterSourceVideoCapture *filter, unsigned int videoch)
{
    svlQtWidgetVidCapSrcImageProperties* instance = dynamic_cast<svlQtWidgetVidCapSrcImageProperties*>(svlQtObjectFactory::Create("svlQtWidgetVidCapSrcImageProperties"));
    if (instance) instance->Create(filter, videoch);
    return instance;
}

void svlQtWidgetVidCapSrcImageProperties::Delete()
{
    svlQtObjectFactory::Delete(this);
}

bool svlQtWidgetVidCapSrcImageProperties::WaitForClose()
{
    if (Dialog) {
        Dialog->EventClosed.Wait();
        return Dialog->Success;
    }
    return false;
}

bool svlQtWidgetVidCapSrcImageProperties::Create(svlFilterSourceVideoCapture *filter, unsigned int videoch)
{
    argFilter  = filter;
    argVideoCh = videoch;
    QMetaObject::invokeMethod(this, "QSlotCreate", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVidCapSrcImageProperties::Destroy()
{
    if (!Dialog) return false;
    QMetaObject::invokeMethod(this, "QSlotDestroy", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVidCapSrcImageProperties::Connect(const std::string & component_name, unsigned int videoch)
{
    argComponentName = component_name;
    argVideoCh       = videoch;
    QMetaObject::invokeMethod(this, "QSlotConnect", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVidCapSrcImageProperties::Connect(const std::string & process_name, const std::string & component_name, unsigned int videoch)
{
    argProcessName   = process_name;
    argComponentName = component_name;
    argVideoCh       = videoch;
    QMetaObject::invokeMethod(this, "QSlotConnect2", Qt::BlockingQueuedConnection);
    return retSuccess;
}

bool svlQtWidgetVidCapSrcImageProperties::Disconnect()
{
    QMetaObject::invokeMethod(this, "QSlotDisconnect", Qt::BlockingQueuedConnection);
    return retSuccess;
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnShutterSliderMove(int value)
{
    UIWidget->ShutterText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnGainSliderMove(int value)
{
    UIWidget->GainText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnColorUSliderMove(int value)
{
    UIWidget->ColorUText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnColorVSliderMove(int value)
{
    UIWidget->ColorVText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnBrightnessSliderMove(int value)
{
    UIWidget->BrightnessText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnGammaSliderMove(int value)
{
    UIWidget->GammaText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnSaturationSliderMove(int value)
{
    UIWidget->SaturationText->setText(QString::number(value));
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnShutterAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->ShutterSlider->setDisabled(false);
        UIWidget->ShutterSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->ShutterSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnGainAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->GainSlider->setDisabled(false);
        UIWidget->GainSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->GainSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnColorAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->ColorUSlider->setDisabled(false);
        UIWidget->ColorVSlider->setDisabled(false);
        UIWidget->ColorUSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->ColorUSlider->setDisabled(true);
        UIWidget->ColorVSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnBrightnessAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->BrightnessSlider->setDisabled(false);
        UIWidget->BrightnessSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->BrightnessSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnGammaAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->GammaSlider->setDisabled(false);
        UIWidget->GammaSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->GammaSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotOnSaturationAutoCBStateChanged(int value)
{
    if (value != Qt::Checked) {
        UIWidget->SaturationSlider->setDisabled(false);
        UIWidget->SaturationSlider->setFocus(Qt::OtherFocusReason);
    }
    else {
        UIWidget->SaturationSlider->setDisabled(true);
    }
}

void svlQtWidgetVidCapSrcImageProperties::QSlotCreate()
{
    Dialog = new svlQtDialog;
    CMN_ASSERT(Dialog);

    UIWidget = new Ui_WidgetVidCapSrcImageProperties;
    CMN_ASSERT(UIWidget);
    UIWidget->setupUi(Dialog);

    QObject::connect(UIWidget->ShutterSlider,    SIGNAL(valueChanged(int)), this, SLOT(QSlotOnShutterSliderMove(int)));
    QObject::connect(UIWidget->GainSlider,       SIGNAL(valueChanged(int)), this, SLOT(QSlotOnGainSliderMove(int)));
    QObject::connect(UIWidget->ColorUSlider,     SIGNAL(valueChanged(int)), this, SLOT(QSlotOnColorUSliderMove(int)));
    QObject::connect(UIWidget->ColorVSlider,     SIGNAL(valueChanged(int)), this, SLOT(QSlotOnColorVSliderMove(int)));
    QObject::connect(UIWidget->BrightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(QSlotOnBrightnessSliderMove(int)));
    QObject::connect(UIWidget->GammaSlider,      SIGNAL(valueChanged(int)), this, SLOT(QSlotOnGammaSliderMove(int)));
    QObject::connect(UIWidget->SaturationSlider, SIGNAL(valueChanged(int)), this, SLOT(QSlotOnSaturationSliderMove(int)));
    QObject::connect(UIWidget->ShutterAutoCB,    SIGNAL(stateChanged(int)), this, SLOT(QSlotOnShutterAutoCBStateChanged(int)));
    QObject::connect(UIWidget->GainAutoCB,       SIGNAL(stateChanged(int)), this, SLOT(QSlotOnGainAutoCBStateChanged(int)));
    QObject::connect(UIWidget->ColorAutoCB,      SIGNAL(stateChanged(int)), this, SLOT(QSlotOnColorAutoCBStateChanged(int)));
    QObject::connect(UIWidget->BrightnessAutoCB, SIGNAL(stateChanged(int)), this, SLOT(QSlotOnBrightnessAutoCBStateChanged(int)));
    QObject::connect(UIWidget->GammaAutoCB,      SIGNAL(stateChanged(int)), this, SLOT(QSlotOnGammaAutoCBStateChanged(int)));
    QObject::connect(UIWidget->SaturationAutoCB, SIGNAL(stateChanged(int)), this, SLOT(QSlotOnSaturationAutoCBStateChanged(int)));

    Dialog->setWindowTitle(tr("Video Capture Image Properties"));

    Thread = new osaThread;
    StopThread = false;
    Thread->Create(svlQtWidgetVidCapSrcImagePropertiesThreadProc, this);

    Dialog->show();
    Dialog->raise();
    Dialog->activateWindow();

    retSuccess = true;
}

void svlQtWidgetVidCapSrcImageProperties::QSlotDestroy()
{
    if (Thread) {
        StopThread = true;
        Thread->Wait();
        delete Thread;
        Thread = 0;
    }
    if (Dialog) {
        Dialog->close();
        delete Dialog;
        Dialog = 0;
    }
    if (UIWidget) {
        delete UIWidget;
        UIWidget = 0;
    }

    retSuccess = true;
}

void svlQtWidgetVidCapSrcImageProperties::QSlotConnect()
{
/*
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
*/
}

void svlQtWidgetVidCapSrcImageProperties::QSlotConnect2()
{
/*
    retSuccess = false;
    CMN_LOG_CLASS_INIT_ERROR << "Connect(process_name, ...) - not yet implemented" << std::endl;
*/
}

void svlQtWidgetVidCapSrcImageProperties::QSlotDisconnect()
{
/*
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
*/
}

void* svlQtWidgetVidCapSrcImagePropertiesThreadProc(svlQtWidgetVidCapSrcImageProperties* obj)
{
    if (!obj || !obj->UIWidget || !obj->argFilter) return 0;

    svlFilterSourceVideoCapture::ImageProperties gui_properties, filt_properties;
    bool update_gui;

    memset(&gui_properties, 0, sizeof(svlFilterSourceVideoCapture::ImageProperties));

    while (!obj->StopThread) {

        // Check for changes on Capture Filter
        update_gui = false;
        if (obj->argFilter->GetImageProperties(filt_properties, obj->argVideoCh) == SVL_OK) {
            if (gui_properties.manual     != filt_properties.manual     ||
                gui_properties.shutter    != filt_properties.shutter    ||
                gui_properties.gain       != filt_properties.gain       ||
                gui_properties.wb_u_b     != filt_properties.wb_u_b     ||
                gui_properties.wb_v_r     != filt_properties.wb_v_r     ||
                gui_properties.brightness != filt_properties.brightness ||
                gui_properties.gamma      != filt_properties.gamma      ||
                gui_properties.saturation != filt_properties.saturation) {
                update_gui = true;
            }
        }

        // Update GUI state
        if (update_gui) {
            // Shutter
            QMetaObject::invokeMethod(obj->UIWidget->ShutterAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propShutter) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->ShutterSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.shutter));
            QMetaObject::invokeMethod(obj->UIWidget->ShutterText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.shutter)));

            // Gain
            QMetaObject::invokeMethod(obj->UIWidget->GainAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propGain) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->GainSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.gain));
            QMetaObject::invokeMethod(obj->UIWidget->GainText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.gain)));

            // ColorBalance
            QMetaObject::invokeMethod(obj->UIWidget->ColorAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propWhiteBalance) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->ColorUSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.wb_u_b));
            QMetaObject::invokeMethod(obj->UIWidget->ColorVSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.wb_v_r));
            QMetaObject::invokeMethod(obj->UIWidget->ColorUText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.wb_u_b)));
            QMetaObject::invokeMethod(obj->UIWidget->ColorVText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.wb_v_r)));

            // Brightness
            QMetaObject::invokeMethod(obj->UIWidget->BrightnessAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propBrightness) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->BrightnessSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.brightness));
            QMetaObject::invokeMethod(obj->UIWidget->BrightnessText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.brightness)));

            // Gamma
            QMetaObject::invokeMethod(obj->UIWidget->GammaAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propGamma) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->GammaSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.gamma));
            QMetaObject::invokeMethod(obj->UIWidget->GammaText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.gamma)));

            // Saturation
            QMetaObject::invokeMethod(obj->UIWidget->SaturationAutoCB,
                                      "setChecked",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(bool, (filt_properties.manual & svlFilterSourceVideoCapture::propSaturation) ? false : true));
            QMetaObject::invokeMethod(obj->UIWidget->SaturationSlider,
                                      "setValue",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(int, filt_properties.saturation));
            QMetaObject::invokeMethod(obj->UIWidget->SaturationText,
                                      "setText",
                                      Qt::BlockingQueuedConnection,
                                      Q_ARG(QString, QString::number(filt_properties.saturation)));
        }

        osaSleep(0.2);

        // Check for changes in GUI state
        memset(&gui_properties, 0, sizeof(svlFilterSourceVideoCapture::ImageProperties));
        gui_properties.mask = -1; // Enable all settings
        gui_properties.manual += obj->UIWidget->ShutterAutoCB->isChecked()    ? 0 : svlFilterSourceVideoCapture::propShutter;
        gui_properties.manual += obj->UIWidget->GainAutoCB->isChecked()       ? 0 : svlFilterSourceVideoCapture::propGain;
        gui_properties.manual += obj->UIWidget->ColorAutoCB->isChecked()      ? 0 : svlFilterSourceVideoCapture::propWhiteBalance;
        gui_properties.manual += obj->UIWidget->BrightnessAutoCB->isChecked() ? 0 : svlFilterSourceVideoCapture::propBrightness;
        gui_properties.manual += obj->UIWidget->GammaAutoCB->isChecked()      ? 0 : svlFilterSourceVideoCapture::propGamma;
        gui_properties.manual += obj->UIWidget->SaturationAutoCB->isChecked() ? 0 : svlFilterSourceVideoCapture::propSaturation;
        gui_properties.shutter    = obj->UIWidget->ShutterSlider->value();
        gui_properties.gain       = obj->UIWidget->GainSlider->value();
        gui_properties.wb_u_b     = obj->UIWidget->ColorUSlider->value();
        gui_properties.wb_v_r     = obj->UIWidget->ColorVSlider->value();
        gui_properties.brightness = obj->UIWidget->BrightnessSlider->value();
        gui_properties.gamma      = obj->UIWidget->GammaSlider->value();
        gui_properties.saturation = obj->UIWidget->SaturationSlider->value();

        if (gui_properties.manual     != filt_properties.manual     ||
            gui_properties.shutter    != filt_properties.shutter    ||
            gui_properties.gain       != filt_properties.gain       ||
            gui_properties.wb_u_b     != filt_properties.wb_u_b     ||
            gui_properties.wb_v_r     != filt_properties.wb_v_r     ||
            gui_properties.brightness != filt_properties.brightness ||
            gui_properties.gamma      != filt_properties.gamma      ||
            gui_properties.saturation != filt_properties.saturation) {

            obj->argFilter->SetImageProperties(gui_properties, obj->argVideoCh);
        }
    }

    return 0;
}

