// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmPositionCartesianGet.ccc

#include <cisstParameterTypes/prmPositionCartesianGet.h>

#include <cisstCommon/cmnDataFunctions.h>

/* source line: 1 */

/* source line: 20 */
prmPositionCartesianGet::prmPositionCartesianGet(void):
    mtsGenericObject(),
    PositionMember()
{}

prmPositionCartesianGet::prmPositionCartesianGet(const prmPositionCartesianGet & other):
    mtsGenericObject(other),
    PositionMember(other.PositionMember)
{}

prmPositionCartesianGet::~prmPositionCartesianGet(void){}


void prmPositionCartesianGet::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->PositionMember);
}


void prmPositionCartesianGet::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->PositionMember);
}


void prmPositionCartesianGet::ToStream(std::ostream & outputStream) const
{
    outputStream << "prmPositionCartesianGet" << std::endl;
    mtsGenericObject::ToStream(outputStream);
    outputStream
        << "  Desired position:" << this->PositionMember;
}

void prmPositionCartesianGet::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const
{
    if (headerOnly) {
        outputStream << cmnDataSerializeTextDescription(*this, delimiter, headerPrefix);
    } else {
        cmnDataSerializeText(outputStream, *this, delimiter);
    }
}
/* source line: 40 */

/* accessors is set to: all */
void prmPositionCartesianGet::GetPosition(vctFrm3 & placeHolder) const
{
    placeHolder = this->PositionMember;
}

void prmPositionCartesianGet::SetPosition(const vctFrm3 & newValue)
{
    this->PositionMember = newValue;
}


/* accessors is set to: all */
const vctFrm3 & prmPositionCartesianGet::Position(void) const
{
    return this->PositionMember;
}

vctFrm3 & prmPositionCartesianGet::Position(void)
{
    return this->PositionMember;
}

/* default functions */
void cmnSerializeRaw(std::ostream & outputStream, const prmPositionCartesianGet & object)
{
    object.SerializeRaw(outputStream);
}
void cmnDeSerializeRaw(std::istream & inputStream, prmPositionCartesianGet & placeHolder)
{
    placeHolder.DeSerializeRaw(inputStream);
}
/* data functions */
void cmnDataCopy(prmPositionCartesianGet & destination, const prmPositionCartesianGet & source) {
    cmnDataCopy(*(dynamic_cast<mtsGenericObject*>(&destination)), *(dynamic_cast<const mtsGenericObject*>(&source)));
    cmnDataCopy(destination.PositionMember, source.PositionMember);
}
void cmnDataSerializeBinary(std::ostream & outputStream, const prmPositionCartesianGet & data) throw (std::runtime_error) {
     data.SerializeBinary(outputStream);
}
void prmPositionCartesianGet::SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error) {
    cmnDataSerializeBinary(outputStream, *(dynamic_cast<const mtsGenericObject*>(this)));
    cmnDataSerializeBinary(outputStream, this->PositionMember);
}
void cmnDataDeSerializeBinary(std::istream & inputStream, prmPositionCartesianGet & data,
                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    data.DeSerializeBinary(inputStream, remoteFormat, localFormat);
}
void prmPositionCartesianGet::DeSerializeBinary(std::istream & inputStream,
                                            const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    cmnDataDeSerializeBinary(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionMember, remoteFormat, localFormat);
}
void cmnDataSerializeText(std::ostream & outputStream, const prmPositionCartesianGet & data, const char delimiter) throw (std::runtime_error) {
     data.SerializeText(outputStream, delimiter);
}
void prmPositionCartesianGet::SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error) {
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
std::string cmnDataSerializeTextDescription(const prmPositionCartesianGet & data, const char delimiter, const std::string & userDescription) {
     return data.SerializeTextDescription(delimiter, userDescription);
}
std::string prmPositionCartesianGet::SerializeTextDescription(const char delimiter, const std::string & userDescription) const {
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
void cmnDataDeSerializeText(std::istream & inputStream, prmPositionCartesianGet & data,
                            const char delimiter) throw (std::runtime_error) {
    data.DeSerializeText(inputStream, delimiter);
}
void prmPositionCartesianGet::DeSerializeText(std::istream & inputStream,
                                          const char delimiter) throw (std::runtime_error) {
    bool someData = false;
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmPositionCartesianGet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmPositionCartesianGet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionMember, delimiter);
}
bool cmnDataScalarNumberIsFixed(const prmPositionCartesianGet & data) {
     return data.ScalarNumberIsFixed();
}
bool prmPositionCartesianGet::ScalarNumberIsFixed(void) const {
    return true
           && cmnDataScalarNumberIsFixed(*(dynamic_cast<const mtsGenericObject*>(this)))
           && cmnDataScalarNumberIsFixed(this->PositionMember)
    ;
}
size_t cmnDataScalarNumber(const prmPositionCartesianGet & data) {
     return data.ScalarNumber();
}
size_t prmPositionCartesianGet::ScalarNumber(void) const {
    return 0
           + cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)))
           + cmnDataScalarNumber(this->PositionMember)
    ;
}
std::string cmnDataScalarDescription(const prmPositionCartesianGet & data, const size_t index,
                                     const std::string & userDescription) throw (std::out_of_range) {
    return data.ScalarDescription(index, userDescription);
}
std::string prmPositionCartesianGet::ScalarDescription(const size_t index, const std::string & userDescription) const throw (std::out_of_range) {
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
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmPositionCartesianGet index out of range"));
    return "";
}
double cmnDataScalar(const prmPositionCartesianGet & data, const size_t index) throw (std::out_of_range) {
    return data.Scalar(index);
}
double prmPositionCartesianGet::Scalar(const size_t index) const throw (std::out_of_range) {
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
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmPositionCartesianGet index out of range"));
    return 1.2345;
}
#if CISST_HAS_JSON
void cmnDataToJSON(const prmPositionCartesianGet & data, Json::Value & jsonValue) {
    data.ToJSON(jsonValue);
}
void prmPositionCartesianGet::ToJSON(Json::Value & jsonValue) const {
    cmnDataToJSON(*(dynamic_cast<const mtsGenericObject*>(this)), jsonValue);
    cmnDataToJSON(this->PositionMember, jsonValue["Position"]);
}
void cmnDataFromJSON(prmPositionCartesianGet & data, const Json::Value & jsonValue) throw (std::runtime_error) {
    data.FromJSON(jsonValue);
}
void prmPositionCartesianGet::FromJSON(const Json::Value & jsonValue) throw (std::runtime_error) {
    cmnDataFromJSON(*(dynamic_cast<mtsGenericObject*>(this)), jsonValue);
    cmnDataFromJSON(this->PositionMember, jsonValue["Position"]);
}
#endif // CISST_HAS_JSON


