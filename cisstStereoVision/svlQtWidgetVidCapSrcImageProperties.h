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

#ifndef _svlQtWidgetVidCapSrcImageProperties_h
#define _svlQtWidgetVidCapSrcImageProperties_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlRequiredInterfaces.h>

#include <QObject>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

// Forward declarations
class osaThread;
class QTimer;
class svlQtDialog;
class Ui_WidgetVidCapSrcImageProperties;


class CISST_EXPORT svlQtWidgetVidCapSrcImageProperties: public QObject, public mtsComponent
{
friend void* svlQtWidgetVidCapSrcImagePropertiesThreadProc(svlQtWidgetVidCapSrcImageProperties* obj);

    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlQtWidgetVidCapSrcImageProperties();
    svlQtWidgetVidCapSrcImageProperties(const svlQtWidgetVidCapSrcImageProperties& other);
    ~svlQtWidgetVidCapSrcImageProperties();

    static svlQtWidgetVidCapSrcImageProperties* New();
    static svlQtWidgetVidCapSrcImageProperties* New(svlFilterSourceVideoCapture *filter, unsigned int videoch = SVL_LEFT);
    void Delete();

    bool WaitForClose();

    bool Create(svlFilterSourceVideoCapture *filter, unsigned int videoch = SVL_LEFT);
    bool Destroy();

    bool Connect(const std::string & component_name, unsigned int videoch = SVL_LEFT);
    bool Connect(const std::string & process_name, const std::string & component_name, unsigned int videoch = SVL_LEFT);
    bool Disconnect();

protected:
    bool StopThread;
    osaThread *Thread;

    svlQtDialog *Dialog;
    Ui_WidgetVidCapSrcImageProperties *UIWidget;

protected slots:
    void QSlotOnShutterSliderMove(int value);
    void QSlotOnGainSliderMove(int value);
    void QSlotOnColorUSliderMove(int value);
    void QSlotOnColorVSliderMove(int value);
    void QSlotOnBrightnessSliderMove(int value);
    void QSlotOnGammaSliderMove(int value);
    void QSlotOnSaturationSliderMove(int value);

    void QSlotOnShutterAutoCBStateChanged(int value);
    void QSlotOnGainAutoCBStateChanged(int value);
    void QSlotOnColorAutoCBStateChanged(int value);
    void QSlotOnBrightnessAutoCBStateChanged(int value);
    void QSlotOnGammaAutoCBStateChanged(int value);
    void QSlotOnSaturationAutoCBStateChanged(int value);

    void QSlotCreate();
    void QSlotDestroy();
    void QSlotConnect();
    void QSlotConnect2();
    void QSlotDisconnect();

public:
    IReqFilterSourceVideoCapture VideoCaptureSettings;

private:
    svlFilterSourceVideoCapture* argFilter;
    unsigned int                 argVideoCh;
    std::string                  argProcessName;
    std::string                  argComponentName;

    bool                         retSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlQtWidgetVidCapSrcImageProperties)

#endif // _svlQtWidgetVidCapSrcImageProperties_h

