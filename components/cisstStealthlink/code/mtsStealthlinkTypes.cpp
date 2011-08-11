/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2007

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStealthlink/mtsStealthlinkTypes.h>

// Stealthlink definitions
#include <GRI_Protocol/GRI.h>

CMN_IMPLEMENT_SERVICES(mtsStealthTool);
CMN_IMPLEMENT_SERVICES(mtsStealthFrame);
CMN_IMPLEMENT_SERVICES(mtsStealthRegistration);
CMN_IMPLEMENT_SERVICES(mtsStealthProbeCal);

void mtsStealthTool::Assign(const mtsStealthTool &that) {
    this->XForm = that.XForm;
    this->GeometryError = that.GeometryError;
    for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
    this->Valid() = that.Valid();
}

void mtsStealthTool::Assign (const struct tool &griTool) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            XForm.Rotation().at(i, j) =  griTool.xform[i][j];
        }
        XForm.Translation().at(i) = griTool.xform[i][3];
    }
    GeometryError = griTool.geometry_error;
    for (int k = 0; k < NAME_LENGTH; k++) Name[k] = griTool.name[k];
    this->SetValid(griTool.valid);
}

void mtsStealthTool::Assign (const prmPositionCartesianGet &that) {
    this->XForm = that.Position();
    this->GeometryError = 0;
    for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = 'n';
    this->Name[NAME_LENGTH-1] = '\0';
    this->Valid() = that.Valid();
}

std::string mtsStealthTool::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

void mtsStealthTool::ToStream(std::ostream &out) const {
    out << Name << ", " << XForm << ", " << GeometryError << std::endl;
}

void mtsStealthTool::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                 bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-Name";
        outputStream << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-GeomError";
    }
    else {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        outputStream << Name << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        outputStream << GeometryError;
   }
}

void mtsStealthTool::SerializeRaw(std::ostream & outputStream) const {
    mtsGenericObject::SerializeRaw(outputStream);
    //cmnSerializeRaw(outputStream, this->Name);
    this->XForm.SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->GeometryError);
}

void mtsStealthTool::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    //cmnDeSerializeRaw(outputStream, this->Name);
    this->XForm.DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->GeometryError);
}

void mtsStealthFrame::Assign(const mtsStealthFrame &that) {
    this->XForm = that.XForm;
    this->GeometryError = that.GeometryError;
    for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
    this->Valid() = that.Valid();
}

void mtsStealthFrame::Assign (const struct frame &griFrame) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            XForm.Rotation().at(i, j) =  griFrame.xform[i][j];
        }
        XForm.Translation().at(i) = griFrame.xform[i][3];
    }
    GeometryError = griFrame.geometry_error;
    for (int k = 0; k < NAME_LENGTH; k++) Name[k] = griFrame.name[k];
    this->SetValid(griFrame.valid);
}

void mtsStealthFrame::Assign(const prmPositionCartesianGet &that) {
    this->XForm = that.Position();
    this->GeometryError = 0;
    for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = 'n';
    this->Name[NAME_LENGTH-1] = '\0';
    this->Valid() = that.Valid();
}

std::string mtsStealthFrame::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}
void mtsStealthFrame::ToStream(std::ostream &out) const {
    out << Name << ", " << XForm << ", " << GeometryError << std::endl;
}
void mtsStealthFrame::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                  bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-Name";
        outputStream << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-GeomError";
    }
    else {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        outputStream << Name << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter << GeometryError;
    }
}

void mtsStealthRegistration::Assign(const mtsStealthRegistration &that) {
    this->XForm = that.XForm;
    this->predictedAccuracy = that.predictedAccuracy;
    this->Valid() = that.Valid();
}

void mtsStealthRegistration::Assign (const struct registration &griRegistration) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            XForm.Rotation().at(i, j) =  griRegistration.xform[i][j];
        }
        XForm.Translation().at(i) = griRegistration.xform[i][3];
    }
    predictedAccuracy = griRegistration.predicted_accuracy;
    this->SetValid(griRegistration.valid);
}

void mtsStealthRegistration::Assign(const vctFrm3 &tmpFrm, const double &tmpAccuracy,
                                    const bool &tmpValid){
    XForm = tmpFrm;
    predictedAccuracy = tmpAccuracy;
    this->SetValid(tmpValid);
}

std::string mtsStealthRegistration::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

void mtsStealthRegistration::ToStream(std::ostream &out) const {
    out << XForm << ", " << predictedAccuracy << std::endl;
}
void mtsStealthRegistration::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                             bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-PredAcc";
    }
    else {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        XForm.ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter << predictedAccuracy;
     }
}

void mtsStealthProbeCal::Assign(const mtsStealthProbeCal &that) {
    for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
    this->Valid() = that.Valid();
    this->Tip = that.Tip;
    this->Hind = that.Hind;
}

void mtsStealthProbeCal::Assign (const struct probe_calibration &griProbeCal) {
    for (int k = 0; k < NAME_LENGTH; k++) Name[k] = griProbeCal.probe_name[k];
    this->SetValid(griProbeCal.valid);
    Tip = vct3(griProbeCal.tip[0],griProbeCal.tip[1],griProbeCal.tip[2]);
    Hind = vct3(griProbeCal.hind[0],griProbeCal.hind[1],griProbeCal.hind[2]);
}

std::string mtsStealthProbeCal::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

void mtsStealthProbeCal::ToStream(std::ostream &out) const {
    out << Name << ", " << Valid() << ", " << Tip << ", " << Hind << std::endl;
}
void mtsStealthProbeCal::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                         bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter << headerPrefix << "-Name";
        Tip.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "Tip");
        outputStream << delimiter;
        Hind.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "Hind");
    }
    else {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        outputStream << Name << delimiter;
        Tip.ToStreamRaw(outputStream, delimiter);
        outputStream << delimiter;
        Hind.ToStreamRaw(outputStream, delimiter);
    }
}

