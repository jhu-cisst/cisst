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

#include <cisstStereoVision/svlQtObjectFactory.h>

#include <cisstCommon/cmnGenericObject.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

#include "winQt4OpenGL.h"
#include <cisstStereoVision/svlQtWidgetFileOpen.h>
#include <cisstStereoVision/svlQtWidgetFramerate.h>
#include <cisstStereoVision/svlQtWidgetVideoEncoder.h>
#include <cisstStereoVision/svlQtWidgetVidCapSrcImageProperties.h>


/********************************/
/*** svlQtObjectFactory class ***/
/********************************/

void svlQtObjectFactory::Init()
{
    GetInstance();
}

cmnGenericObject* svlQtObjectFactory::Create(const std::string & classname)
{
    svlQtObjectFactory* instance = GetInstance();
    cmnGenericObject* obj = 0;

    instance->CS->Enter();
        instance->ClassName = classname;
        QMetaObject::invokeMethod(instance, "QSlotCreate", Qt::BlockingQueuedConnection);
        obj = instance->Object;
        instance->Object = 0;
    instance->CS->Leave();

    return obj;
}

void svlQtObjectFactory::Delete(cmnGenericObject* obj)
{
    svlQtObjectFactory* instance = GetInstance();

    instance->CS->Enter();
        instance->Object = obj;
        QMetaObject::invokeMethod(instance, "QSlotDelete", Qt::BlockingQueuedConnection);
    instance->CS->Leave();
}

void svlQtObjectFactory::QSlotCreate()
{
    Object = cmnClassRegister::Create(ClassName);
    CMN_LOG_INIT_DEBUG << "svlQtObjectFactory::Create(\"" << ClassName << "\") executed" << std::endl;
}

void svlQtObjectFactory::QSlotDelete()
{
    delete Object;
    Object = 0;
    CMN_LOG_INIT_DEBUG << "svlQtObjectFactory::Delete() executed" << std::endl;
}

svlQtObjectFactory* svlQtObjectFactory::GetInstance()
{
    static svlQtObjectFactory instance;
    return &instance;
}

svlQtObjectFactory::svlQtObjectFactory() :
    QObject(),
    Object(0)
{
    CS = new osaCriticalSection;

    // Create prototypes of supported classes
    svlWindowManagerQt4OpenGL::ClassServices();
    svlQtWidgetFileOpen::ClassServices();
    svlQtWidgetFramerate::ClassServices();
    svlQtWidgetVideoEncoder::ClassServices();
    svlQtWidgetVidCapSrcImageProperties::ClassServices();
}

svlQtObjectFactory::~svlQtObjectFactory()
{
    delete CS;
}

