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
// Add support for prmPositionCartesianGet
#include <cisstParameterTypes/prmPositionCartesianGet.h>

#ifndef NAME_LENGTH
#define NAME_LENGTH 64  // from GRI.h
#endif

// Forward declarations of Stealthlink types
struct tool;
struct frame;
struct registration;
struct probe_calibration;

class mtsStealthTool : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
private:
    vctFrm3 XForm;
    double GeometryError;
    char Name[NAME_LENGTH];
public:
    mtsStealthTool():XForm(), GeometryError(0) {}
    ~mtsStealthTool() {}
    virtual const vctFrm3& GetFrame() { return XForm; }
    virtual const char *GetName() const { return Name; }
    virtual double GetGeometryError() { return GeometryError; }

    void Assign(const mtsStealthTool &that);
    mtsStealthTool & operator= (const mtsStealthTool &that) { this->Assign(that); return *this; }
    void Assign (const struct tool &griTool);
    mtsStealthTool & operator= (const tool & griTool) { this->Assign(griTool); return *this; }

    void Assign (const prmPositionCartesianGet &that);
    mtsStealthTool & operator= (const prmPositionCartesianGet &that) {this->Assign(that); return *this;}

    std::string ToString(void) const;
    virtual void ToStream(std::ostream &out) const;
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;
    virtual void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
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
    virtual const char *GetName() const { return Name; }
    void Assign(const mtsStealthFrame &that);
    mtsStealthFrame & operator= (const mtsStealthFrame &that) { this->Assign(that); return *this; }
    void Assign (const struct frame &griFrame);
    mtsStealthFrame & operator= (const frame &griFrame) { this->Assign(griFrame); return *this; }

    void Assign (const prmPositionCartesianGet &that);
    mtsStealthFrame & operator= (const prmPositionCartesianGet &that) {this->Assign(that); return *this;}

    std::string ToString(void) const;
    virtual void ToStream(std::ostream &out) const;
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;
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
    void Assign(const mtsStealthRegistration &that);
    mtsStealthRegistration & operator= (const mtsStealthRegistration &that) { this->Assign(that); return *this; }
    void Assign (const struct registration &griRegistration);
    mtsStealthRegistration & operator= (const registration &griRegistration) { this->Assign(griRegistration); return *this; }

    void Assign(const vctFrm3 &tmpFrm, const double &tmpAccuracy, const bool &tmpValid);

    std::string ToString(void) const;
    virtual void ToStream(std::ostream &out) const;
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;
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
    void Assign(const mtsStealthProbeCal &that);
    mtsStealthProbeCal & operator= (const mtsStealthProbeCal &that) { this->Assign(that); return *this; }
    void Assign (const struct probe_calibration &griProbeCal);
    mtsStealthProbeCal & operator= (const struct probe_calibration &griProbeCal) { this->Assign(griProbeCal); return *this; }
    std::string ToString(void) const;
    virtual void ToStream(std::ostream &out) const;
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthProbeCal);

#endif  // _mtsStealthlinkTypes_h
