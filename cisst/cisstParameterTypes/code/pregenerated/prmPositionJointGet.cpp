// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmPositionJointGet.ccc

#include <cisstParameterTypes/prmPositionJointGet.h>

#include <cisstCommon/cmnDataFunctions.h>

/* source line: 1 */

/* source line: 11 */
prmPositionJointGet::prmPositionJointGet(void):
    mtsGenericObject(),
    PositionMember()
{}

prmPositionJointGet::prmPositionJointGet(const prmPositionJointGet & other):
    mtsGenericObject(other),
    PositionMember(other.PositionMember)
{}

prmPositionJointGet::~prmPositionJointGet(void){}


void prmPositionJointGet::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->PositionMember);
}


void prmPositionJointGet::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->PositionMember);
}


void prmPositionJointGet::ToStream(std::ostream & outputStream) const
{
    outputStream << "prmPositionJointGet" << std::endl;
    mtsGenericObject::ToStream(outputStream);
    outputStream
        << "  Desired position:" << this->PositionMember;
}

void prmPositionJointGet::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const
{
    if (headerOnly) {
        outputStream << cmnDataSerializeTextDescription(*this, delimiter, headerPrefix);
    } else {
        cmnDataSerializeText(outputStream, *this, delimiter);
    }
}
/* source line: 21 */

/* accessors is set to: all */
void prmPositionJointGet::GetPosition(vctDynamicVector<double> & placeHolder) const
{
    placeHolder = this->PositionMember;
}

void prmPositionJointGet::SetPosition(const vctDynamicVector<double> & newValue)
{
    this->PositionMember = newValue;
}


/* accessors is set to: all */
const vctDynamicVector<double> & prmPositionJointGet::Position(void) const
{
    return this->PositionMember;
}

vctDynamicVector<double> & prmPositionJointGet::Position(void)
{
    return this->PositionMember;
}

/* default functions */
void cmnSerializeRaw(std::ostream & outputStream, const prmPositionJointGet & object)
{
    object.SerializeRaw(outputStream);
}
void cmnDeSerializeRaw(std::istream & inputStream, prmPositionJointGet & placeHolder)
{
    placeHolder.DeSerializeRaw(inputStream);
}
/* data functions */
void cmnDataCopy(prmPositionJointGet & destination, const prmPositionJointGet & source) {
    cmnDataCopy(*(dynamic_cast<mtsGenericObject*>(&destination)), *(dynamic_cast<const mtsGenericObject*>(&source)));
    cmnDataCopy(destination.PositionMember, source.PositionMember);
}
void cmnDataSerializeBinary(std::ostream & outputStream, const prmPositionJointGet & data) throw (std::runtime_error) {
     data.SerializeBinary(outputStream);
}
void prmPositionJointGet::SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error) {
    cmnDataSerializeBinary(outputStream, *(dynamic_cast<const mtsGenericObject*>(this)));
    cmnDataSerializeBinary(outputStream, this->PositionMember);
}
void cmnDataDeSerializeBinary(std::istream & inputStream, prmPositionJointGet & data,
                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    data.DeSerializeBinary(inputStream, remoteFormat, localFormat);
}
void prmPositionJointGet::DeSerializeBinary(std::istream & inputStream,
                                            const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    cmnDataDeSerializeBinary(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionMember, remoteFormat, localFormat);
}
void cmnDataSerializeText(std::ostream & outputStream, const prmPositionJointGet & data, const char delimiter) throw (std::runtime_error) {
     data.SerializeText(outputStream, delimiter);
}
void prmPositionJointGet::SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error) {
    bool someData = false;
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, *(dynamic_cast<const mtsGenericObject*>(this)), delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->PositionMember, delimiter);
}
std::string cmnDataSerializeTextDescription(const prmPositionJointGet & data, const char delimiter, const std::string & userDescription) {
     return data.SerializeTextDescription(delimiter, userDescription);
}
std::string prmPositionJointGet::SerializeTextDescription(const char delimiter, const std::string & userDescription) const {
    bool someData = false;
    const std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    std::stringstream description;
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(*(dynamic_cast<const mtsGenericObject*>(this)), delimiter, userDescription);
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->PositionMember, delimiter, prefix + "Position");
    return description.str();
}
void cmnDataDeSerializeText(std::istream & inputStream, prmPositionJointGet & data,
                            const char delimiter) throw (std::runtime_error) {
    data.DeSerializeText(inputStream, delimiter);
}
void prmPositionJointGet::DeSerializeText(std::istream & inputStream,
                                          const char delimiter) throw (std::runtime_error) {
    bool someData = false;
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmPositionJointGet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmPositionJointGet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionMember, delimiter);
}
bool cmnDataScalarNumberIsFixed(const prmPositionJointGet & data) {
     return data.ScalarNumberIsFixed();
}
bool prmPositionJointGet::ScalarNumberIsFixed(void) const {
    return true
           && cmnDataScalarNumberIsFixed(*(dynamic_cast<const mtsGenericObject*>(this)))
           && cmnDataScalarNumberIsFixed(this->PositionMember)
    ;
}
size_t cmnDataScalarNumber(const prmPositionJointGet & data) {
     return data.ScalarNumber();
}
size_t prmPositionJointGet::ScalarNumber(void) const {
    return 0
           + cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)))
           + cmnDataScalarNumber(this->PositionMember)
    ;
}
std::string cmnDataScalarDescription(const prmPositionJointGet & data, const size_t index,
                                     const std::string & userDescription) throw (std::out_of_range) {
    return data.ScalarDescription(index, userDescription);
}
std::string prmPositionJointGet::ScalarDescription(const size_t index, const std::string & userDescription) const throw (std::out_of_range) {
    std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    size_t baseIndex = 0;
    size_t currentMaxIndex = 0;
    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)));
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(*(dynamic_cast<const mtsGenericObject*>(this)), index - baseIndex, prefix);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->PositionMember, index - baseIndex, prefix + "Position");
    }
    baseIndex = currentMaxIndex;
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmPositionJointGet index out of range"));
    return "";
}
double cmnDataScalar(const prmPositionJointGet & data, const size_t index) throw (std::out_of_range) {
    return data.Scalar(index);
}
double prmPositionJointGet::Scalar(const size_t index) const throw (std::out_of_range) {
    size_t baseIndex = 0;
    size_t currentMaxIndex = 0;
    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)));
    if (index < currentMaxIndex) {
        return cmnDataScalar(*(dynamic_cast<const mtsGenericObject*>(this)), index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->PositionMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmPositionJointGet index out of range"));
    return 1.2345;
}
#if CISST_HAS_JSON
void cmnDataToJSON(const prmPositionJointGet & data, Json::Value & jsonValue) {
    data.ToJSON(jsonValue);
}
void prmPositionJointGet::ToJSON(Json::Value & jsonValue) const {
    cmnDataToJSON(*(dynamic_cast<const mtsGenericObject*>(this)), jsonValue);
    cmnDataToJSON(this->PositionMember, jsonValue["Position"]);
}
void cmnDataFromJSON(prmPositionJointGet & data, const Json::Value & jsonValue) throw (std::runtime_error) {
    data.FromJSON(jsonValue);
}
void prmPositionJointGet::FromJSON(const Json::Value & jsonValue) throw (std::runtime_error) {
    cmnDataFromJSON(*(dynamic_cast<mtsGenericObject*>(this)), jsonValue);
    cmnDataFromJSON(this->PositionMember, jsonValue["Position"]);
}
#endif // CISST_HAS_JSON


