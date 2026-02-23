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

#include <cisstMultiTask/mtsQtWidgetVectorTypes.h>
#include <cisstMultiTask/mtsVector.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>


// -- mtsDoubleVec
mtsQtWidgetDoubleVecRead::mtsQtWidgetDoubleVecRead(void):
    mtsQtWidgetGenericObjectRead()
{
    VectorWidget = new vctQtWidgetDynamicVectorDoubleRead();
    Layout->addWidget(VectorWidget);
    Layout->addStretch();
}

bool mtsQtWidgetDoubleVecRead::SetValue(const mtsGenericObject & value)
{
    const mtsDoubleVec* doubleVecData = dynamic_cast<const mtsDoubleVec*>(&value);
    if (!doubleVecData) {
        return false;
    }

    const Eigen::Map<const Eigen::VectorXd> vector(doubleVecData->Pointer(), doubleVecData->size());
    return VectorWidget->SetValue(vector);
}

mtsQtWidgetDoubleVecWrite::mtsQtWidgetDoubleVecWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    VectorWidget = new vctQtWidgetDynamicVectorDoubleWrite();
    Layout->addWidget(VectorWidget);
    Layout->addStretch();
}

bool mtsQtWidgetDoubleVecWrite::SetValue(const mtsGenericObject & value)
{
   const mtsDoubleVec* doubleVecData = dynamic_cast<const mtsDoubleVec*>(&value);
    if (!doubleVecData) {
        return false;
    }

    const Eigen::Map<const Eigen::VectorXd> vector(doubleVecData->Pointer(), doubleVecData->size());
    return VectorWidget->SetValue(vector);
}

bool mtsQtWidgetDoubleVecWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsDoubleVec* doubleVecData = dynamic_cast<mtsDoubleVec*>(&placeHolder);
    if (!doubleVecData) {
        return false;
    }

    Eigen::VectorXd output(doubleVecData->size());
    bool ok = VectorWidget->GetValue(output);
    if (!ok) {
        return false;
    }

    Eigen::Map<Eigen::VectorXd> mapped(doubleVecData->Pointer(), doubleVecData->size());
    mapped = output;
    return true;
}


// -- mtsBoolVec
mtsQtWidgetBoolVecRead::mtsQtWidgetBoolVecRead(void):
    mtsQtWidgetGenericObjectRead()
{
    VectorWidget = new vctQtWidgetDynamicVectorBoolRead();
    Layout->addWidget(VectorWidget);
    Layout->addStretch();
}

bool mtsQtWidgetBoolVecRead::SetValue(const mtsGenericObject & value)
{
    const mtsBoolVec* boolVecData = dynamic_cast<const mtsBoolVec*>(&value);
    if (!boolVecData) {
        return false;
    }

    const Eigen::Map<const Eigen::VectorX<bool>> vector(boolVecData->Pointer(), boolVecData->size());
    return VectorWidget->SetValue(vector);
}

mtsQtWidgetBoolVecWrite::mtsQtWidgetBoolVecWrite(void):
    mtsQtWidgetGenericObjectWrite()
{
    VectorWidget = new vctQtWidgetDynamicVectorBoolWrite();
    Layout->addWidget(VectorWidget);
    Layout->addStretch();
}

bool mtsQtWidgetBoolVecWrite::SetValue(const mtsGenericObject & value)
{
    const mtsBoolVec* boolVecData = dynamic_cast<const mtsBoolVec*>(&value);
    if (!boolVecData) {
        return false;
    }

    const Eigen::Map<const Eigen::VectorX<bool>> vector(boolVecData->Pointer(), boolVecData->size());
    return VectorWidget->SetValue(vector);
}

bool mtsQtWidgetBoolVecWrite::GetValue(mtsGenericObject & placeHolder) const
{
    mtsBoolVec* boolVecData = dynamic_cast<mtsBoolVec*>(&placeHolder);
    if (!boolVecData) {
        return false;
    }

    Eigen::VectorX<bool> output(boolVecData->size());
    bool ok = VectorWidget->GetValue(output);
    if (!ok) {
        return false;
    }

    Eigen::Map<Eigen::VectorX<bool>> mapped(boolVecData->Pointer(), boolVecData->size());
    mapped = output;
    return true;
}
