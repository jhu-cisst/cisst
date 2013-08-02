// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmFixtureGainCartesianSet.ccc

#ifndef _cisstParameterTypes_prmFixtureGainCartesianSet_h
#define _cisstParameterTypes_prmFixtureGainCartesianSet_h

/* source line: 1 */
/* source line: 4 */

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDataFunctions.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/* source line: 14 */
class prmFixtureGainCartesianSet;
/* default functions */
void CISST_EXPORT cmnSerializeRaw(std::ostream & outputStream, const prmFixtureGainCartesianSet & object);
void CISST_EXPORT cmnDeSerializeRaw(std::istream & inputStream, prmFixtureGainCartesianSet & placeHolder);
/* data functions */
void CISST_EXPORT cmnDataCopy(prmFixtureGainCartesianSet & destination, const prmFixtureGainCartesianSet & source);
void CISST_EXPORT cmnDataSerializeBinary(std::ostream & outputStream, const prmFixtureGainCartesianSet & data) throw (std::runtime_error);
void CISST_EXPORT cmnDataDeSerializeBinary(std::istream & inputStream, prmFixtureGainCartesianSet & data, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);
void CISST_EXPORT cmnDataSerializeText(std::ostream & outputStream, const prmFixtureGainCartesianSet & data, const char delimiter) throw (std::runtime_error);
std::string CISST_EXPORT cmnDataSerializeTextDescription(const prmFixtureGainCartesianSet & data, const char delimiter, const std::string & userDescription = "");
void CISST_EXPORT cmnDataDeSerializeText(std::istream & inputStream, prmFixtureGainCartesianSet & data, const char delimiter) throw (std::runtime_error);
bool CISST_EXPORT cmnDataScalarNumberIsFixed(const prmFixtureGainCartesianSet & data);
size_t CISST_EXPORT cmnDataScalarNumber(const prmFixtureGainCartesianSet & data);
std::string CISST_EXPORT cmnDataScalarDescription(const prmFixtureGainCartesianSet & data, const size_t index, const std::string & userDescription = "") throw (std::out_of_range);
double CISST_EXPORT cmnDataScalar(const prmFixtureGainCartesianSet & data, const size_t index) throw (std::out_of_range);
#if CISST_HAS_JSON
#include <json/json.h>
void CISST_EXPORT cmnDataToJSON(const prmFixtureGainCartesianSet & data, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(prmFixtureGainCartesianSet & data, const Json::Value & jsonValue) throw (std::runtime_error);
#endif // CISST_HAS_JSON
class CISST_EXPORT prmFixtureGainCartesianSet: public mtsGenericObject
{
 friend void CISST_EXPORT cmnDataCopy(prmFixtureGainCartesianSet & destination, const prmFixtureGainCartesianSet & source);
 /* default constructors and destructors. */
 public:
    prmFixtureGainCartesianSet(void);
    prmFixtureGainCartesianSet(const prmFixtureGainCartesianSet & other);
    ~prmFixtureGainCartesianSet();

/* source line: 23 */
 protected:
    vctMatRot3 ForceOrientationMember; // Force orientation matrix
 public:
    /* accessors is set to: all */
    void GetForceOrientation(vctMatRot3 & placeHolder) const;
    void SetForceOrientation(const vctMatRot3 & newValue);
    /* accessors is set to: all */
    const vctMatRot3 & ForceOrientation(void) const;
    vctMatRot3 & ForceOrientation(void);
/* source line: 28 */
 protected:
    vct3 ForcePositionMember; // Force position vector
 public:
    /* accessors is set to: all */
    void GetForcePosition(vct3 & placeHolder) const;
    void SetForcePosition(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & ForcePosition(void) const;
    vct3 & ForcePosition(void);
/* source line: 34 */
 protected:
    vctMatRot3 TorqueOrientationMember; // Torque orientation matrix.
 public:
    /* accessors is set to: all */
    void GetTorqueOrientation(vctMatRot3 & placeHolder) const;
    void SetTorqueOrientation(const vctMatRot3 & newValue);
    /* accessors is set to: all */
    const vctMatRot3 & TorqueOrientation(void) const;
    vctMatRot3 & TorqueOrientation(void);
/* source line: 40 */
 protected:
    vct3 PositionStiffnessPosMember; // Positive position stiffness vector.
 public:
    /* accessors is set to: all */
    void GetPositionStiffnessPos(vct3 & placeHolder) const;
    void SetPositionStiffnessPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & PositionStiffnessPos(void) const;
    vct3 & PositionStiffnessPos(void);
/* source line: 46 */
 protected:
    vct3 PositionStiffnessNegMember; // Negative position stiffness vector.
 public:
    /* accessors is set to: all */
    void GetPositionStiffnessNeg(vct3 & placeHolder) const;
    void SetPositionStiffnessNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & PositionStiffnessNeg(void) const;
    vct3 & PositionStiffnessNeg(void);
/* source line: 52 */
 protected:
    vct3 PositionDampingPosMember; // Positive position damping vector.
 public:
    /* accessors is set to: all */
    void GetPositionDampingPos(vct3 & placeHolder) const;
    void SetPositionDampingPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & PositionDampingPos(void) const;
    vct3 & PositionDampingPos(void);
/* source line: 58 */
 protected:
    vct3 PositionDampingNegMember; // Negative position damping vector.
 public:
    /* accessors is set to: all */
    void GetPositionDampingNeg(vct3 & placeHolder) const;
    void SetPositionDampingNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & PositionDampingNeg(void) const;
    vct3 & PositionDampingNeg(void);
/* source line: 64 */
 protected:
    vct3 ForceBiasPosMember; // Positive force bias vector.
 public:
    /* accessors is set to: all */
    void GetForceBiasPos(vct3 & placeHolder) const;
    void SetForceBiasPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & ForceBiasPos(void) const;
    vct3 & ForceBiasPos(void);
/* source line: 70 */
 protected:
    vct3 ForceBiasNegMember; // Negative force bias vector.
 public:
    /* accessors is set to: all */
    void GetForceBiasNeg(vct3 & placeHolder) const;
    void SetForceBiasNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & ForceBiasNeg(void) const;
    vct3 & ForceBiasNeg(void);
/* source line: 76 */
 protected:
    vct3 OrientationStiffnessPosMember; // Positive orientation stiffness vector.
 public:
    /* accessors is set to: all */
    void GetOrientationStiffnessPos(vct3 & placeHolder) const;
    void SetOrientationStiffnessPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & OrientationStiffnessPos(void) const;
    vct3 & OrientationStiffnessPos(void);
/* source line: 82 */
 protected:
    vct3 OrientationStiffnessNegMember; // Negative orientation stiffness vector.
 public:
    /* accessors is set to: all */
    void GetOrientationStiffnessNeg(vct3 & placeHolder) const;
    void SetOrientationStiffnessNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & OrientationStiffnessNeg(void) const;
    vct3 & OrientationStiffnessNeg(void);
/* source line: 88 */
 protected:
    vct3 OrientationDampingPosMember; // Positive orientation damping vector.
 public:
    /* accessors is set to: all */
    void GetOrientationDampingPos(vct3 & placeHolder) const;
    void SetOrientationDampingPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & OrientationDampingPos(void) const;
    vct3 & OrientationDampingPos(void);
/* source line: 94 */
 protected:
    vct3 OrientationDampingNegMember; // Positive orientation damping vector.
 public:
    /* accessors is set to: all */
    void GetOrientationDampingNeg(vct3 & placeHolder) const;
    void SetOrientationDampingNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & OrientationDampingNeg(void) const;
    vct3 & OrientationDampingNeg(void);
/* source line: 100 */
 protected:
    vct3 TorqueBiasPosMember; // Positive torque bias vector.
 public:
    /* accessors is set to: all */
    void GetTorqueBiasPos(vct3 & placeHolder) const;
    void SetTorqueBiasPos(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & TorqueBiasPos(void) const;
    vct3 & TorqueBiasPos(void);
/* source line: 106 */
 protected:
    vct3 TorqueBiasNegMember; // Negative torque bias vector.
 public:
    /* accessors is set to: all */
    void GetTorqueBiasNeg(vct3 & placeHolder) const;
    void SetTorqueBiasNeg(const vct3 & newValue);
    /* accessors is set to: all */
    const vct3 & TorqueBiasNeg(void) const;
    vct3 & TorqueBiasNeg(void);
/* source line: 112 */

    private:
        CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
        /* default methods */
 public:
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
    void ToStream(std::ostream & outputStream) const;
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;
    /* default data methods */
 public:
    void SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error);
    void DeSerializeBinary(std::istream & inputStream, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);
    void SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error);
    std::string SerializeTextDescription(const char delimiter, const std::string & userDescription = "") const;
    void DeSerializeText(std::istream & inputStream, const char delimiter) throw (std::runtime_error);
    bool ScalarNumberIsFixed(void) const;
    size_t ScalarNumber(void) const;
    double Scalar(const size_t index) const throw (std::out_of_range);
    std::string ScalarDescription(const size_t index, const std::string & userDescription = "") const throw (std::out_of_range);
#if CISST_HAS_JSON
    void ToJSON(Json::Value & jsonValue) const;
    void FromJSON(const Json::Value & jsonValue) throw (std::runtime_error);
#endif // CISST_HAS_JSON

};

/* source line: 119 */

CMN_DECLARE_SERVICES_INSTANTIATION(prmFixtureGainCartesianSet);


#endif // _cisstParameterTypes_prmFixtureGainCartesianSet_h
