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

#include <cisstStereoVision/svlQtWidgetFileOpen.h>
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstCommon/cmnLogger.h>

#include <QFileDialog>


/*********************************/
/*** svlQtWidgetFileOpen class ***/
/*********************************/

CMN_IMPLEMENT_SERVICES(svlQtWidgetFileOpen)

svlQtWidgetFileOpen::svlQtWidgetFileOpen() :
    QObject(),
    cmnGenericObject(),
    Dialog(0),
    retSuccess(false)
{
}

svlQtWidgetFileOpen::svlQtWidgetFileOpen(const svlQtWidgetFileOpen& CMN_UNUSED(other)) :
    QObject(),
    cmnGenericObject()
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlQtWidgetFileOpen::~svlQtWidgetFileOpen()
{
    QSlotDestroy();
}

svlQtWidgetFileOpen* svlQtWidgetFileOpen::New()
{
    return dynamic_cast<svlQtWidgetFileOpen*>(svlQtObjectFactory::Create("svlQtWidgetFileOpen"));
}

svlQtWidgetFileOpen* svlQtWidgetFileOpen::New(const std::string & kind, const std::string & extensions, bool save)
{
    svlQtWidgetFileOpen* instance = dynamic_cast<svlQtWidgetFileOpen*>(svlQtObjectFactory::Create("svlQtWidgetFileOpen"));
    if (instance) instance->Create(kind, extensions, save);
    return instance;
}

void svlQtWidgetFileOpen::Delete()
{
    svlQtObjectFactory::Delete(this);
}

bool svlQtWidgetFileOpen::WaitForClose()
{
    ClosedEvent.Wait();
    return retSuccess;
}

bool svlQtWidgetFileOpen::Create(const std::string & kind, const std::string & extensions, bool save)
{
    if (Dialog) return false;

    argKind       = kind;
    argExtensions = extensions;
    argSave       = save;
    QMetaObject::invokeMethod(this, "QSlotCreate", Qt::QueuedConnection);
    return retSuccess;
}

std::string svlQtWidgetFileOpen::GetFilePath()
{
    return FilePath;
}

bool svlQtWidgetFileOpen::Destroy()
{
    QMetaObject::invokeMethod(this, "QSlotDestroy", Qt::BlockingQueuedConnection);
    return retSuccess;
}

void svlQtWidgetFileOpen::QSlotCreate()
{
    retSuccess = false;

    // Convert extension list to Qt friendly string
    std::string qt_list(argExtensions);
    size_t pos;
    while (1) {
        pos = qt_list.find(';');
        if (pos == std::string::npos) break;
        qt_list.replace(pos, 1, " *.");
    }
    qt_list.erase(qt_list.length() - 3, 3);
    qt_list.insert(0, argKind + " (*.");
    qt_list.append(");;All files (*.*)");

    Dialog = new QFileDialog(0);
    Dialog->setFileMode(QFileDialog::AnyFile);
    Dialog->setNameFilter(QString::fromStdString(qt_list));
    Dialog->setDirectory(QDir::currentPath());
    Dialog->setOption(QFileDialog::DontUseNativeDialog);
    if (argSave) Dialog->setWindowTitle(tr("Save File"));
    else         Dialog->setWindowTitle(tr("Open File"));
    Dialog->show();
    Dialog->raise();
    Dialog->activateWindow();
    Dialog->exec();

    QStringList qnames = Dialog->selectedFiles();
    if (qnames.size() < 1 || qnames[0] == QDir::currentPath()) {
        delete Dialog;
        Dialog = 0;
        ClosedEvent.Raise();
        retSuccess = false;
        return;
    }
    FilePath = qnames[0].toStdString();

    delete Dialog;
    Dialog = 0;

    std::string extension;
    svlVideoIO::GetExtension(FilePath, extension);
    if (extension == "ncvi" ||
        extension == "njpg") {
        // These are special filenames that don't need to include the full path
        pos = FilePath.find_last_of("/\\");
        if (pos != std::string::npos) FilePath.erase(0, pos + 1);
    }

    retSuccess = true;
    ClosedEvent.Raise();
}

void svlQtWidgetFileOpen::QSlotDestroy()
{
    retSuccess = false;
    if (Dialog) {
        Dialog->reject();
        retSuccess = false;
    }
}

