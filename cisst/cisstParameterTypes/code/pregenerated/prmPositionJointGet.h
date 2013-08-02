// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmPositionJointGet.ccc

#ifndef _cisstParameterTypes_prmPositionJointGet_h
#define _cisstParameterTypes_prmPositionJointGet_h

/* source line: 1 */
/* source line: 4 */

#include <cisstVector/vctDataFunctions.h>
#include <cisstMultiTask/mtsGenericObject.h>
// Always include last
#include <cisstParameterTypes/prmExport.h>

/* source line: 11 */
class prmPositionJointGet;
/* default functions */
void CISST_EXPORT cmnSerializeRaw(std::ostream & outputStream, const prmPositionJointGet & object);
void CISST_EXPORT cmnDeSerializeRaw(std::istream & inputStream, prmPositionJointGet & placeHolder);
/* data functions */
void CISST_EXPORT cmnDataCopy(prmPositionJointGet & destination, const prmPositionJointGet & source);
void CISST_EXPORT cmnDataSerializeBinary(std::ostream & outputStream, const prmPositionJointGet & data) throw (std::runtime_error);
void CISST_EXPORT cmnDataDeSerializeBinary(std::istream & inputStream, prmPositionJointGet & data, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);
void CISST_EXPORT cmnDataSerializeText(std::ostream & outputStream, const prmPositionJointGet & data, const char delimiter) throw (std::runtime_error);
std::string CISST_EXPORT cmnDataSerializeTextDescription(const prmPositionJointGet & data, const char delimiter, const std::string & userDescription = "");
void CISST_EXPORT cmnDataDeSerializeText(std::istream & inputStream, prmPositionJointGet & data, const char delimiter) throw (std::runtime_error);
bool CISST_EXPORT cmnDataScalarNumberIsFixed(const prmPositionJointGet & data);
size_t CISST_EXPORT cmnDataScalarNumber(const prmPositionJointGet & data);
std::string CISST_EXPORT cmnDataScalarDescription(const prmPositionJointGet & data, const size_t index, const std::string & userDescription = "") throw (std::out_of_range);
double CISST_EXPORT cmnDataScalar(const prmPositionJointGet & data, const size_t index) throw (std::out_of_range);
#if CISST_HAS_JSON
#include <json/json.h>
void CISST_EXPORT cmnDataToJSON(const prmPositionJointGet & data, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(prmPositionJointGet & data, const Json::Value & jsonValue) throw (std::runtime_error);
#endif // CISST_HAS_JSON
class CISST_EXPORT prmPositionJointGet: public mtsGenericObject
{
 friend void CISST_EXPORT cmnDataCopy(prmPositionJointGet & destination, const prmPositionJointGet & source);
 /* default constructors and destructors. */
 public:
    prmPositionJointGet(void);
    prmPositionJointGet(const prmPositionJointGet & other);
    ~prmPositionJointGet();

/* source line: 21 */
 protected:
    vctDynamicVector<double> PositionMember; // Desired position
 public:
    /* accessors is set to: all */
    void GetPosition(vctDynamicVector<double> & placeHolder) const;
    void SetPosition(const vctDynamicVector<double> & newValue);
    /* accessors is set to: all */
    const vctDynamicVector<double> & Position(void) const;
    vctDynamicVector<double> & Position(void);
/* source line: 27 */

    public:
        inline prmPositionJointGet(size_t size) {
            this->SetSize(size);
        }

        inline void SetSize(size_t size) {
            this->PositionMember.SetSize(size);
        }

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

/* source line: 43 */

CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionJointGet);


#endif // _cisstParameterTypes_prmPositionJointGet_h
