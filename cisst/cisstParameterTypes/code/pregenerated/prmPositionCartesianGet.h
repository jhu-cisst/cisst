// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmPositionCartesianGet.ccc

#ifndef _cisstParameterTypes_prmPositionCartesianGet_h
#define _cisstParameterTypes_prmPositionCartesianGet_h

/* source line: 1 */
/* source line: 9 */

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctDataFunctions.h>

#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/* source line: 20 */
class prmPositionCartesianGet;
/* default functions */
void CISST_EXPORT cmnSerializeRaw(std::ostream & outputStream, const prmPositionCartesianGet & object);
void CISST_EXPORT cmnDeSerializeRaw(std::istream & inputStream, prmPositionCartesianGet & placeHolder);
/* data functions */
void CISST_EXPORT cmnDataCopy(prmPositionCartesianGet & destination, const prmPositionCartesianGet & source);
void CISST_EXPORT cmnDataSerializeBinary(std::ostream & outputStream, const prmPositionCartesianGet & data) throw (std::runtime_error);
void CISST_EXPORT cmnDataDeSerializeBinary(std::istream & inputStream, prmPositionCartesianGet & data, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);
void CISST_EXPORT cmnDataSerializeText(std::ostream & outputStream, const prmPositionCartesianGet & data, const char delimiter) throw (std::runtime_error);
std::string CISST_EXPORT cmnDataSerializeTextDescription(const prmPositionCartesianGet & data, const char delimiter, const std::string & userDescription = "");
void CISST_EXPORT cmnDataDeSerializeText(std::istream & inputStream, prmPositionCartesianGet & data, const char delimiter) throw (std::runtime_error);
bool CISST_EXPORT cmnDataScalarNumberIsFixed(const prmPositionCartesianGet & data);
size_t CISST_EXPORT cmnDataScalarNumber(const prmPositionCartesianGet & data);
std::string CISST_EXPORT cmnDataScalarDescription(const prmPositionCartesianGet & data, const size_t index, const std::string & userDescription = "") throw (std::out_of_range);
double CISST_EXPORT cmnDataScalar(const prmPositionCartesianGet & data, const size_t index) throw (std::out_of_range);
#if CISST_HAS_JSON
#include <json/json.h>
void CISST_EXPORT cmnDataToJSON(const prmPositionCartesianGet & data, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(prmPositionCartesianGet & data, const Json::Value & jsonValue) throw (std::runtime_error);
#endif // CISST_HAS_JSON
class CISST_EXPORT prmPositionCartesianGet: public mtsGenericObject
{
 friend void CISST_EXPORT cmnDataCopy(prmPositionCartesianGet & destination, const prmPositionCartesianGet & source);
 /* default constructors and destructors. */
 public:
    prmPositionCartesianGet(void);
    prmPositionCartesianGet(const prmPositionCartesianGet & other);
    ~prmPositionCartesianGet();

/* source line: 40 */
 protected:
    vctFrm3 PositionMember; // Desired position
 public:
    /* accessors is set to: all */
    void GetPosition(vctFrm3 & placeHolder) const;
    void SetPosition(const vctFrm3 & newValue);
    /* accessors is set to: all */
    const vctFrm3 & Position(void) const;
    vctFrm3 & Position(void);
/* source line: 51 */

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

/* source line: 58 */

CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionCartesianGet);


#endif // _cisstParameterTypes_prmPositionCartesianGet_h
