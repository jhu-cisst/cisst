/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtWidgetVectorTypes_h
#define _mtsQtWidgetVectorTypes_h

#include <cisstMultiTask/mtsQtWidgetGenericObject.h>
#include <cisstVector/vctQtWidgetDynamicVector.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

// -- mtsDoubleVec
class CISST_EXPORT mtsQtWidgetDoubleVecRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    vctQtWidgetDynamicVectorDoubleRead * VectorWidget;
 public:
    mtsQtWidgetDoubleVecRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetDoubleVecRead);

class CISST_EXPORT mtsQtWidgetDoubleVecWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    vctQtWidgetDynamicVectorDoubleWrite * VectorWidget;
 public:
    mtsQtWidgetDoubleVecWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetDoubleVecWrite);


// -- mtsBoolVec
class CISST_EXPORT mtsQtWidgetBoolVecRead: public mtsQtWidgetGenericObjectRead
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    vctQtWidgetDynamicVectorBoolRead * VectorWidget;
 public:
    mtsQtWidgetBoolVecRead(void);
    virtual bool SetValue(const mtsGenericObject & value);
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetBoolVecRead);

class CISST_EXPORT mtsQtWidgetBoolVecWrite: public mtsQtWidgetGenericObjectWrite
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 private:
    vctQtWidgetDynamicVectorBoolWrite * VectorWidget;
 public:
    mtsQtWidgetBoolVecWrite(void);
    virtual bool SetValue(const mtsGenericObject & value);
    virtual bool GetValue(mtsGenericObject & placeHolder) const;
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetBoolVecWrite);

#endif // _mtsQtWidgetVectorTypes_h
