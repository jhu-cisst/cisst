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

#ifndef _mtsStealthlinkTypes_h
#define _mtsStealthlinkTypes_h

#include <cisstMultiTask/mtsGenericObject.h>

// Stealthlink definitions
#include <GRI_Protocol/GRI.h> // would be good to not expose this header file -- todo

// Add support for prmPositionCartesianGet
#include <cisstParameterTypes/prmPositionCartesianGet.h>

class mtsStealthTool : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
private:
    vctFrm3 XForm;
    double GeometryError;
    char Name[NAME_LENGTH];
public:
    mtsStealthTool():XForm(), GeometryError(0) {};
    ~mtsStealthTool() {};
    virtual const vctFrm3& GetFrame() { return XForm; }
    virtual const char *GetName() const { return Name; }
    virtual double GetGeometryError() { return GeometryError; }

    void Assign(const mtsStealthTool &that) {
        this->XForm = that.XForm;
        this->GeometryError = that.GeometryError;
        for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
        this->Valid() = that.Valid();
    }
    mtsStealthTool & operator= (const mtsStealthTool &that) { this->Assign(that); return *this; }
    void Assign (const struct tool &griTool) {
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
    mtsStealthTool & operator= (const tool & griTool) { this->Assign(griTool); return *this; }

    void Assign (const prmPositionCartesianGet &that){
		this->XForm = that.Position();
		this->GeometryError = 0;
        for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = 'n';
        this->Name[NAME_LENGTH-1] = '\0';
		this->Valid() = that.Valid();
    }
    mtsStealthTool & operator= (const prmPositionCartesianGet &that) {this->Assign(that); return *this;}

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    virtual void ToStream(std::ostream &out) const {
        out << Name << ", " << XForm << ", " << GeometryError << std::endl;
    }
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
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
    virtual void SerializeRaw(std::ostream & outputStream) const {
        mtsGenericObject::SerializeRaw(outputStream);
        //cmnSerializeRaw(outputStream, this->Name);
        this->XForm.SerializeRaw(outputStream);
        cmnSerializeRaw(outputStream, this->GeometryError);
    }
    void DeSerializeRaw(std::istream & inputStream)
    {
        mtsGenericObject::DeSerializeRaw(inputStream);
        //cmnDeSerializeRaw(outputStream, this->Name);
        this->XForm.DeSerializeRaw(inputStream);
        cmnDeSerializeRaw(inputStream, this->GeometryError);
    }
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthTool);

class mtsStealthFrame : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
private:
    vctFrm3 XForm;
    double GeometryError;
    char Name[NAME_LENGTH];
public:
    mtsStealthFrame():XForm(), GeometryError(0) {};
    ~mtsStealthFrame() {};
    virtual const vctFrm3& GetFrame() { return XForm; }
    virtual double GetGeometryError() { return GeometryError; }
    void Assign(const mtsStealthFrame &that) {
        this->XForm = that.XForm;
        this->GeometryError = that.GeometryError;
        for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
        this->Valid() = that.Valid();
    }
    mtsStealthFrame & operator= (const mtsStealthFrame &that) { this->Assign(that); return *this; }
    void Assign (const struct frame &griFrame) {
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
    mtsStealthFrame & operator= (const frame &griFrame) { this->Assign(griFrame); return *this; }

    void Assign (const prmPositionCartesianGet &that){
		this->XForm = that.Position();
		this->GeometryError = 0;
		for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = 'n';
        this->Name[NAME_LENGTH-1] = '\0';
		this->Valid() = that.Valid();
    }
    mtsStealthFrame & operator= (const prmPositionCartesianGet &that) {this->Assign(that); return *this;}


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    virtual void ToStream(std::ostream &out) const {
        out << Name << ", " << XForm << ", " << GeometryError << std::endl;
    }
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
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
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthFrame);

class mtsStealthRegistration : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
private:
    vctFrm3 XForm;
    double predictedAccuracy;
public:
    mtsStealthRegistration():XForm(), predictedAccuracy(0) {};
    ~mtsStealthRegistration() {};
    virtual const vctFrm3& GetFrame() { return XForm; }
    void Assign(const mtsStealthRegistration &that) {
        this->XForm = that.XForm;
        this->predictedAccuracy = that.predictedAccuracy;
        this->Valid() = that.Valid();
    }
    mtsStealthRegistration & operator= (const mtsStealthRegistration &that) { this->Assign(that); return *this; }
    void Assign (const struct registration &griRegistration) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                XForm.Rotation().at(i, j) =  griRegistration.xform[i][j];
            }
            XForm.Translation().at(i) = griRegistration.xform[i][3];
        }
        predictedAccuracy = griRegistration.predicted_accuracy;
        this->SetValid(griRegistration.valid);
    }
    mtsStealthRegistration & operator= (const registration &griRegistration) { this->Assign(griRegistration); return *this; }

    void Assign(const vctFrm3 &tmpFrm, const double &tmpAccuracy, const bool &tmpValid){
        XForm = tmpFrm;
        predictedAccuracy = tmpAccuracy;
        this->SetValid(tmpValid);
    }

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    virtual void ToStream(std::ostream &out) const {
        out << XForm << ", " << predictedAccuracy << std::endl;
    }
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
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
};
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthRegistration);

class mtsStealthProbeCal : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
private:
    char Name[NAME_LENGTH];
    vct3 Tip;
    vct3 Hind;
public:

    mtsStealthProbeCal() { Name[0] = 0; };
    ~mtsStealthProbeCal() {};
    virtual const char *GetName() const { return Name; }
    virtual const vct3& GetTip() { return Tip; }
    virtual const vct3& GetHind() { return Hind; }
    void Assign(const mtsStealthProbeCal &that) {
        for (int k = 0; k < NAME_LENGTH; k++) this->Name[k] = that.Name[k];
        this->Valid() = that.Valid();
        this->Tip = that.Tip;
        this->Hind = that.Hind;
    }
    mtsStealthProbeCal & operator= (const mtsStealthProbeCal &that) { this->Assign(that); return *this; }
    void Assign (const struct probe_calibration &griProbeCal) {
        for (int k = 0; k < NAME_LENGTH; k++) Name[k] = griProbeCal.probe_name[k];
        this->SetValid(griProbeCal.valid);
        Tip = vct3(griProbeCal.tip[0],griProbeCal.tip[1],griProbeCal.tip[2]);
        Hind = vct3(griProbeCal.hind[0],griProbeCal.hind[1],griProbeCal.hind[2]);
    }
    mtsStealthProbeCal & operator= (const struct probe_calibration &griProbeCal) { this->Assign(griProbeCal); return *this; }
    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    virtual void ToStream(std::ostream &out) const {
        out << Name << ", " << Valid() << ", " << Tip << ", " << Hind << std::endl;
    }
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
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
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthProbeCal);

#endif  // _mtsStealthlinkTypes_h
