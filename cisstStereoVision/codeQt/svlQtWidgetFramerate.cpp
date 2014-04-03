/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2011-06-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlQtWidgetFramerate.h>
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlQtDialog.h>
#include <ui_svlQtWidgetFramerate.h>


/**********************************/
/*** svlQtWidgetFramerate class ***/
/**********************************/

CMN_IMPLEMENT_SERVICES(svlQtWidgetFramerate)

svlQtWidgetFramerate::svlQtWidgetFramerate() :
    QObject(),
    cmnGenericObject(),
    Dialog(0),
    UIWidget(0),
    Framerate(30.0),
    retSuccess(false)
{
}

svlQtWidgetFramerate::svlQtWidgetFramerate(const svlQtWidgetFramerate& CMN_UNUSED(other)) :
    QObject(),
    cmnGenericObject(),
    Dialog(0),
    UIWidget(0),
    Framerate(30.0),
    retSuccess(false)
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlQtWidgetFramerate::~svlQtWidgetFramerate()
{
    QSlotDestroy();
}

svlQtWidgetFramerate* svlQtWidgetFramerate::New(bool create)
{
    svlQtWidgetFramerate* instance = dynamic_cast<svlQtWidgetFramerate*>(svlQtObjectFactory::Create("svlQtWidgetFramerate"));
    if (instance && create) instance->Create();
    return instance;
}

void svlQtWidgetFramerate::Delete()
{
    svlQtObjectFactory::Delete(this);
}

bool svlQtWidgetFramerate::WaitForClose()
{
    if (Dialog) {
        Dialog->EventClosed.Wait();
        return Dialog->Success;
    }
    return false;
}

bool svlQtWidgetFramerate::Create()
{
    if (Dialog) return false;
    QMetaObject::invokeMethod(this, "QSlotCreate", Qt::BlockingQueuedConnection);
    return retSuccess;
}

double svlQtWidgetFramerate::GetFramerate()
{
    QMetaObject::invokeMethod(this, "QSlotGetFramerate", Qt::BlockingQueuedConnection);
    return Framerate;
}

bool svlQtWidgetFramerate::Destroy()
{
    if (!Dialog) return false;
    QMetaObject::invokeMethod(this, "QSlotDestroy", Qt::BlockingQueuedConnection);
    return retSuccess;
}

void svlQtWidgetFramerate::QSlotCustomToggled(bool checked)
{
    if (checked) {
        UIWidget->CustomText->setFocus(Qt::OtherFocusReason);
    }
}

void svlQtWidgetFramerate::QSlotCustomTextEdited(const QString & text)
{
    bool ok;
    text.toDouble(&ok);
    if (ok || text.isEmpty()) CustomText = text;
    else UIWidget->CustomText->setText(CustomText);
}

void svlQtWidgetFramerate::QSlotCreate()
{
    Dialog = new svlQtDialog;
    CMN_ASSERT(Dialog);

    UIWidget = new Ui_WidgetFramerate;
    CMN_ASSERT(UIWidget);
    UIWidget->setupUi(Dialog);

    QObject::connect(UIWidget->RBCustom, SIGNAL(toggled(bool)), this, SLOT(QSlotCustomToggled(bool)));
    QObject::connect(UIWidget->CustomText, SIGNAL(textEdited(const QString&)), this, SLOT(QSlotCustomTextEdited(const QString&)));

    Dialog->setWindowTitle(tr("Video Framerate"));

    UIWidget->RB30->setChecked(true);
    UIWidget->RB30->setFocus(Qt::OtherFocusReason);

    Dialog->show();
    Dialog->raise();
    Dialog->activateWindow();

    retSuccess = true;
}

void svlQtWidgetFramerate::QSlotGetFramerate()
{
    if (UIWidget->RB120->isChecked()) {
        Framerate = 120.0;
        return;
    }
    if (UIWidget->RB60->isChecked()) {
        Framerate = 60.0;
        return;
    }
    if (UIWidget->RB5994->isChecked()) {
        Framerate = 59.94;
        return;
    }
    if (UIWidget->RB50->isChecked()) {
        Framerate = 50.0;
        return;
    }
    if (UIWidget->RB30->isChecked()) {
        Framerate = 30.0;
        return;
    }
    if (UIWidget->RB2997->isChecked()) {
        Framerate = 29.97;
        return;
    }
    if (UIWidget->RB25->isChecked()) {
        Framerate = 25.0;
        return;
    }
    if (UIWidget->RB24->isChecked()) {
        Framerate = 24.0;
        return;
    }
    if (UIWidget->RB23976->isChecked()) {
        Framerate = 23.976;
        return;
    }
    if (UIWidget->RB15->isChecked()) {
        Framerate = 15.0;
        return;
    }
    if (UIWidget->RB75->isChecked()) {
        Framerate = 7.5;
        return;
    }
    if (UIWidget->RB375->isChecked()) {
        Framerate = 3.75;
        return;
    }
    if (UIWidget->RB1875->isChecked()) {
        Framerate = 1.875;
        return;
    }
    if (UIWidget->RBCustom->isChecked()) {
        Framerate = CustomText.toDouble();
        if (Framerate == 0.0) Framerate = 30.0;
        return;
    }
}

void svlQtWidgetFramerate::QSlotDestroy()
{
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

