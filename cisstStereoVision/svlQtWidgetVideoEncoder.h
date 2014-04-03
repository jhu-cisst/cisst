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

#ifndef _svlQtWidgetVideoEncoder_h
#define _svlQtWidgetVideoEncoder_h

#include <cisstStereoVision/svlRequiredInterfaces.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstVector/vctDynamicVectorTypes.h>

#include <QObject>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

// Forward declarations
class svlQtDialog;
class QListWidgetItem;
class Ui_WidgetVideoEncoder;


class CISST_EXPORT svlQtWidgetVideoEncoder: public QObject, public mtsComponent
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlQtWidgetVideoEncoder();
    svlQtWidgetVideoEncoder(const svlQtWidgetVideoEncoder& other);
    ~svlQtWidgetVideoEncoder();

    static svlQtWidgetVideoEncoder* New();
    static svlQtWidgetVideoEncoder* New(const std::string & filename);
    void Delete();

    bool WaitForClose();

    bool Create(const std::string & filename);
    svlVideoIO::Compression* GetCodecParams();
    bool Destroy();

    bool Connect(svlVideoCodecBase *codec, const std::string & filename);
    bool Connect(const std::string & component_name, const std::string & filename);
    bool Connect(const std::string & process_name, const std::string & component_name, const std::string & filename);
    bool Disconnect();

    bool UpdateData();

protected:
    svlQtDialog *Dialog;
    Ui_WidgetVideoEncoder *UIWidget;

protected slots:
    void QSlotOnSliderMove(int value);
    void QSlotOnQualityBasedCBStateChanged(int value);

    void QSlotCreate();
    void QSlotGetCodecParams();
    void QSlotDestroy();
    void QSlotConnect();
    void QSlotConnect2();
    void QSlotConnect3();
    void QSlotDisconnect();
    void QSlotUpdateData();

public:
    IReqVideoEncoder VideoEncoder;

private:
    vctDynamicVector<int> EncoderIDs;
    vctDynamicVector<QListWidgetItem*> EncoderItems;
    svlVideoCodecBase* ExternalCodec;
    std::string ExternalCodecName;

    svlVideoCodecBase* argCodec;
    std::string        argFileName;
    std::string        argProcessName;
    std::string        argComponentName;

    bool                     retSuccess;
    svlVideoIO::Compression* retCodecParams;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlQtWidgetVideoEncoder)

#endif // _svlQtWidgetVideoEncoder_h

