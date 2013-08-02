// file automatically generated, do not modify
// cisst version: 1.0
// source file: C:/Users/MJ/project/cisst/cisst/cisstParameterTypes/code/../prmFixtureGainCartesianSet.ccc

#include <cisstParameterTypes/prmFixtureGainCartesianSet.h>

#include <cisstCommon/cmnDataFunctions.h>

/* source line: 1 */

/* source line: 14 */
prmFixtureGainCartesianSet::prmFixtureGainCartesianSet(void):
    mtsGenericObject(),
    ForceOrientationMember(),
    ForcePositionMember(),
    TorqueOrientationMember(),
    PositionStiffnessPosMember(),
    PositionStiffnessNegMember(),
    PositionDampingPosMember(),
    PositionDampingNegMember(),
    ForceBiasPosMember(),
    ForceBiasNegMember(),
    OrientationStiffnessPosMember(),
    OrientationStiffnessNegMember(),
    OrientationDampingPosMember(),
    OrientationDampingNegMember(),
    TorqueBiasPosMember(),
    TorqueBiasNegMember()
{}

prmFixtureGainCartesianSet::prmFixtureGainCartesianSet(const prmFixtureGainCartesianSet & other):
    mtsGenericObject(other),
    ForceOrientationMember(other.ForceOrientationMember),
    ForcePositionMember(other.ForcePositionMember),
    TorqueOrientationMember(other.TorqueOrientationMember),
    PositionStiffnessPosMember(other.PositionStiffnessPosMember),
    PositionStiffnessNegMember(other.PositionStiffnessNegMember),
    PositionDampingPosMember(other.PositionDampingPosMember),
    PositionDampingNegMember(other.PositionDampingNegMember),
    ForceBiasPosMember(other.ForceBiasPosMember),
    ForceBiasNegMember(other.ForceBiasNegMember),
    OrientationStiffnessPosMember(other.OrientationStiffnessPosMember),
    OrientationStiffnessNegMember(other.OrientationStiffnessNegMember),
    OrientationDampingPosMember(other.OrientationDampingPosMember),
    OrientationDampingNegMember(other.OrientationDampingNegMember),
    TorqueBiasPosMember(other.TorqueBiasPosMember),
    TorqueBiasNegMember(other.TorqueBiasNegMember)
{}

prmFixtureGainCartesianSet::~prmFixtureGainCartesianSet(void){}


void prmFixtureGainCartesianSet::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->ForceOrientationMember);
    cmnSerializeRaw(outputStream, this->ForcePositionMember);
    cmnSerializeRaw(outputStream, this->TorqueOrientationMember);
    cmnSerializeRaw(outputStream, this->PositionStiffnessPosMember);
    cmnSerializeRaw(outputStream, this->PositionStiffnessNegMember);
    cmnSerializeRaw(outputStream, this->PositionDampingPosMember);
    cmnSerializeRaw(outputStream, this->PositionDampingNegMember);
    cmnSerializeRaw(outputStream, this->ForceBiasPosMember);
    cmnSerializeRaw(outputStream, this->ForceBiasNegMember);
    cmnSerializeRaw(outputStream, this->OrientationStiffnessPosMember);
    cmnSerializeRaw(outputStream, this->OrientationStiffnessNegMember);
    cmnSerializeRaw(outputStream, this->OrientationDampingPosMember);
    cmnSerializeRaw(outputStream, this->OrientationDampingNegMember);
    cmnSerializeRaw(outputStream, this->TorqueBiasPosMember);
    cmnSerializeRaw(outputStream, this->TorqueBiasNegMember);
}


void prmFixtureGainCartesianSet::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ForceOrientationMember);
    cmnDeSerializeRaw(inputStream, this->ForcePositionMember);
    cmnDeSerializeRaw(inputStream, this->TorqueOrientationMember);
    cmnDeSerializeRaw(inputStream, this->PositionStiffnessPosMember);
    cmnDeSerializeRaw(inputStream, this->PositionStiffnessNegMember);
    cmnDeSerializeRaw(inputStream, this->PositionDampingPosMember);
    cmnDeSerializeRaw(inputStream, this->PositionDampingNegMember);
    cmnDeSerializeRaw(inputStream, this->ForceBiasPosMember);
    cmnDeSerializeRaw(inputStream, this->ForceBiasNegMember);
    cmnDeSerializeRaw(inputStream, this->OrientationStiffnessPosMember);
    cmnDeSerializeRaw(inputStream, this->OrientationStiffnessNegMember);
    cmnDeSerializeRaw(inputStream, this->OrientationDampingPosMember);
    cmnDeSerializeRaw(inputStream, this->OrientationDampingNegMember);
    cmnDeSerializeRaw(inputStream, this->TorqueBiasPosMember);
    cmnDeSerializeRaw(inputStream, this->TorqueBiasNegMember);
}


void prmFixtureGainCartesianSet::ToStream(std::ostream & outputStream) const
{
    outputStream << "prmFixtureGainCartesianSet" << std::endl;
    mtsGenericObject::ToStream(outputStream);
    outputStream
        << "  Force orientation matrix:" << this->ForceOrientationMember
        << "  Force position vector:" << this->ForcePositionMember
        << "  Torque orientation matrix.:" << this->TorqueOrientationMember
        << "  Positive position stiffness vector.:" << this->PositionStiffnessPosMember
        << "  Negative position stiffness vector.:" << this->PositionStiffnessNegMember
        << "  Positive position damping vector.:" << this->PositionDampingPosMember
        << "  Negative position damping vector.:" << this->PositionDampingNegMember
        << "  Positive force bias vector.:" << this->ForceBiasPosMember
        << "  Negative force bias vector.:" << this->ForceBiasNegMember
        << "  Positive orientation stiffness vector.:" << this->OrientationStiffnessPosMember
        << "  Negative orientation stiffness vector.:" << this->OrientationStiffnessNegMember
        << "  Positive orientation damping vector.:" << this->OrientationDampingPosMember
        << "  Positive orientation damping vector.:" << this->OrientationDampingNegMember
        << "  Positive torque bias vector.:" << this->TorqueBiasPosMember
        << "  Negative torque bias vector.:" << this->TorqueBiasNegMember;
}

void prmFixtureGainCartesianSet::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const
{
    if (headerOnly) {
        outputStream << cmnDataSerializeTextDescription(*this, delimiter, headerPrefix);
    } else {
        cmnDataSerializeText(outputStream, *this, delimiter);
    }
}
/* source line: 23 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetForceOrientation(vctMatRot3 & placeHolder) const
{
    placeHolder = this->ForceOrientationMember;
}

void prmFixtureGainCartesianSet::SetForceOrientation(const vctMatRot3 & newValue)
{
    this->ForceOrientationMember = newValue;
}


/* accessors is set to: all */
const vctMatRot3 & prmFixtureGainCartesianSet::ForceOrientation(void) const
{
    return this->ForceOrientationMember;
}

vctMatRot3 & prmFixtureGainCartesianSet::ForceOrientation(void)
{
    return this->ForceOrientationMember;
}

/* source line: 28 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetForcePosition(vct3 & placeHolder) const
{
    placeHolder = this->ForcePositionMember;
}

void prmFixtureGainCartesianSet::SetForcePosition(const vct3 & newValue)
{
    this->ForcePositionMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::ForcePosition(void) const
{
    return this->ForcePositionMember;
}

vct3 & prmFixtureGainCartesianSet::ForcePosition(void)
{
    return this->ForcePositionMember;
}

/* source line: 34 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetTorqueOrientation(vctMatRot3 & placeHolder) const
{
    placeHolder = this->TorqueOrientationMember;
}

void prmFixtureGainCartesianSet::SetTorqueOrientation(const vctMatRot3 & newValue)
{
    this->TorqueOrientationMember = newValue;
}


/* accessors is set to: all */
const vctMatRot3 & prmFixtureGainCartesianSet::TorqueOrientation(void) const
{
    return this->TorqueOrientationMember;
}

vctMatRot3 & prmFixtureGainCartesianSet::TorqueOrientation(void)
{
    return this->TorqueOrientationMember;
}

/* source line: 40 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetPositionStiffnessPos(vct3 & placeHolder) const
{
    placeHolder = this->PositionStiffnessPosMember;
}

void prmFixtureGainCartesianSet::SetPositionStiffnessPos(const vct3 & newValue)
{
    this->PositionStiffnessPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::PositionStiffnessPos(void) const
{
    return this->PositionStiffnessPosMember;
}

vct3 & prmFixtureGainCartesianSet::PositionStiffnessPos(void)
{
    return this->PositionStiffnessPosMember;
}

/* source line: 46 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetPositionStiffnessNeg(vct3 & placeHolder) const
{
    placeHolder = this->PositionStiffnessNegMember;
}

void prmFixtureGainCartesianSet::SetPositionStiffnessNeg(const vct3 & newValue)
{
    this->PositionStiffnessNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::PositionStiffnessNeg(void) const
{
    return this->PositionStiffnessNegMember;
}

vct3 & prmFixtureGainCartesianSet::PositionStiffnessNeg(void)
{
    return this->PositionStiffnessNegMember;
}

/* source line: 52 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetPositionDampingPos(vct3 & placeHolder) const
{
    placeHolder = this->PositionDampingPosMember;
}

void prmFixtureGainCartesianSet::SetPositionDampingPos(const vct3 & newValue)
{
    this->PositionDampingPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::PositionDampingPos(void) const
{
    return this->PositionDampingPosMember;
}

vct3 & prmFixtureGainCartesianSet::PositionDampingPos(void)
{
    return this->PositionDampingPosMember;
}

/* source line: 58 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetPositionDampingNeg(vct3 & placeHolder) const
{
    placeHolder = this->PositionDampingNegMember;
}

void prmFixtureGainCartesianSet::SetPositionDampingNeg(const vct3 & newValue)
{
    this->PositionDampingNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::PositionDampingNeg(void) const
{
    return this->PositionDampingNegMember;
}

vct3 & prmFixtureGainCartesianSet::PositionDampingNeg(void)
{
    return this->PositionDampingNegMember;
}

/* source line: 64 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetForceBiasPos(vct3 & placeHolder) const
{
    placeHolder = this->ForceBiasPosMember;
}

void prmFixtureGainCartesianSet::SetForceBiasPos(const vct3 & newValue)
{
    this->ForceBiasPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::ForceBiasPos(void) const
{
    return this->ForceBiasPosMember;
}

vct3 & prmFixtureGainCartesianSet::ForceBiasPos(void)
{
    return this->ForceBiasPosMember;
}

/* source line: 70 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetForceBiasNeg(vct3 & placeHolder) const
{
    placeHolder = this->ForceBiasNegMember;
}

void prmFixtureGainCartesianSet::SetForceBiasNeg(const vct3 & newValue)
{
    this->ForceBiasNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::ForceBiasNeg(void) const
{
    return this->ForceBiasNegMember;
}

vct3 & prmFixtureGainCartesianSet::ForceBiasNeg(void)
{
    return this->ForceBiasNegMember;
}

/* source line: 76 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetOrientationStiffnessPos(vct3 & placeHolder) const
{
    placeHolder = this->OrientationStiffnessPosMember;
}

void prmFixtureGainCartesianSet::SetOrientationStiffnessPos(const vct3 & newValue)
{
    this->OrientationStiffnessPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::OrientationStiffnessPos(void) const
{
    return this->OrientationStiffnessPosMember;
}

vct3 & prmFixtureGainCartesianSet::OrientationStiffnessPos(void)
{
    return this->OrientationStiffnessPosMember;
}

/* source line: 82 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetOrientationStiffnessNeg(vct3 & placeHolder) const
{
    placeHolder = this->OrientationStiffnessNegMember;
}

void prmFixtureGainCartesianSet::SetOrientationStiffnessNeg(const vct3 & newValue)
{
    this->OrientationStiffnessNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::OrientationStiffnessNeg(void) const
{
    return this->OrientationStiffnessNegMember;
}

vct3 & prmFixtureGainCartesianSet::OrientationStiffnessNeg(void)
{
    return this->OrientationStiffnessNegMember;
}

/* source line: 88 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetOrientationDampingPos(vct3 & placeHolder) const
{
    placeHolder = this->OrientationDampingPosMember;
}

void prmFixtureGainCartesianSet::SetOrientationDampingPos(const vct3 & newValue)
{
    this->OrientationDampingPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::OrientationDampingPos(void) const
{
    return this->OrientationDampingPosMember;
}

vct3 & prmFixtureGainCartesianSet::OrientationDampingPos(void)
{
    return this->OrientationDampingPosMember;
}

/* source line: 94 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetOrientationDampingNeg(vct3 & placeHolder) const
{
    placeHolder = this->OrientationDampingNegMember;
}

void prmFixtureGainCartesianSet::SetOrientationDampingNeg(const vct3 & newValue)
{
    this->OrientationDampingNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::OrientationDampingNeg(void) const
{
    return this->OrientationDampingNegMember;
}

vct3 & prmFixtureGainCartesianSet::OrientationDampingNeg(void)
{
    return this->OrientationDampingNegMember;
}

/* source line: 100 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetTorqueBiasPos(vct3 & placeHolder) const
{
    placeHolder = this->TorqueBiasPosMember;
}

void prmFixtureGainCartesianSet::SetTorqueBiasPos(const vct3 & newValue)
{
    this->TorqueBiasPosMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::TorqueBiasPos(void) const
{
    return this->TorqueBiasPosMember;
}

vct3 & prmFixtureGainCartesianSet::TorqueBiasPos(void)
{
    return this->TorqueBiasPosMember;
}

/* source line: 106 */

/* accessors is set to: all */
void prmFixtureGainCartesianSet::GetTorqueBiasNeg(vct3 & placeHolder) const
{
    placeHolder = this->TorqueBiasNegMember;
}

void prmFixtureGainCartesianSet::SetTorqueBiasNeg(const vct3 & newValue)
{
    this->TorqueBiasNegMember = newValue;
}


/* accessors is set to: all */
const vct3 & prmFixtureGainCartesianSet::TorqueBiasNeg(void) const
{
    return this->TorqueBiasNegMember;
}

vct3 & prmFixtureGainCartesianSet::TorqueBiasNeg(void)
{
    return this->TorqueBiasNegMember;
}

/* default functions */
void cmnSerializeRaw(std::ostream & outputStream, const prmFixtureGainCartesianSet & object)
{
    object.SerializeRaw(outputStream);
}
void cmnDeSerializeRaw(std::istream & inputStream, prmFixtureGainCartesianSet & placeHolder)
{
    placeHolder.DeSerializeRaw(inputStream);
}
/* data functions */
void cmnDataCopy(prmFixtureGainCartesianSet & destination, const prmFixtureGainCartesianSet & source) {
    cmnDataCopy(*(dynamic_cast<mtsGenericObject*>(&destination)), *(dynamic_cast<const mtsGenericObject*>(&source)));
    cmnDataCopy(destination.ForceOrientationMember, source.ForceOrientationMember);
    cmnDataCopy(destination.ForcePositionMember, source.ForcePositionMember);
    cmnDataCopy(destination.TorqueOrientationMember, source.TorqueOrientationMember);
    cmnDataCopy(destination.PositionStiffnessPosMember, source.PositionStiffnessPosMember);
    cmnDataCopy(destination.PositionStiffnessNegMember, source.PositionStiffnessNegMember);
    cmnDataCopy(destination.PositionDampingPosMember, source.PositionDampingPosMember);
    cmnDataCopy(destination.PositionDampingNegMember, source.PositionDampingNegMember);
    cmnDataCopy(destination.ForceBiasPosMember, source.ForceBiasPosMember);
    cmnDataCopy(destination.ForceBiasNegMember, source.ForceBiasNegMember);
    cmnDataCopy(destination.OrientationStiffnessPosMember, source.OrientationStiffnessPosMember);
    cmnDataCopy(destination.OrientationStiffnessNegMember, source.OrientationStiffnessNegMember);
    cmnDataCopy(destination.OrientationDampingPosMember, source.OrientationDampingPosMember);
    cmnDataCopy(destination.OrientationDampingNegMember, source.OrientationDampingNegMember);
    cmnDataCopy(destination.TorqueBiasPosMember, source.TorqueBiasPosMember);
    cmnDataCopy(destination.TorqueBiasNegMember, source.TorqueBiasNegMember);
}
void cmnDataSerializeBinary(std::ostream & outputStream, const prmFixtureGainCartesianSet & data) throw (std::runtime_error) {
     data.SerializeBinary(outputStream);
}
void prmFixtureGainCartesianSet::SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error) {
    cmnDataSerializeBinary(outputStream, *(dynamic_cast<const mtsGenericObject*>(this)));
    cmnDataSerializeBinary(outputStream, this->ForceOrientationMember);
    cmnDataSerializeBinary(outputStream, this->ForcePositionMember);
    cmnDataSerializeBinary(outputStream, this->TorqueOrientationMember);
    cmnDataSerializeBinary(outputStream, this->PositionStiffnessPosMember);
    cmnDataSerializeBinary(outputStream, this->PositionStiffnessNegMember);
    cmnDataSerializeBinary(outputStream, this->PositionDampingPosMember);
    cmnDataSerializeBinary(outputStream, this->PositionDampingNegMember);
    cmnDataSerializeBinary(outputStream, this->ForceBiasPosMember);
    cmnDataSerializeBinary(outputStream, this->ForceBiasNegMember);
    cmnDataSerializeBinary(outputStream, this->OrientationStiffnessPosMember);
    cmnDataSerializeBinary(outputStream, this->OrientationStiffnessNegMember);
    cmnDataSerializeBinary(outputStream, this->OrientationDampingPosMember);
    cmnDataSerializeBinary(outputStream, this->OrientationDampingNegMember);
    cmnDataSerializeBinary(outputStream, this->TorqueBiasPosMember);
    cmnDataSerializeBinary(outputStream, this->TorqueBiasNegMember);
}
void cmnDataDeSerializeBinary(std::istream & inputStream, prmFixtureGainCartesianSet & data,
                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    data.DeSerializeBinary(inputStream, remoteFormat, localFormat);
}
void prmFixtureGainCartesianSet::DeSerializeBinary(std::istream & inputStream,
                                            const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {
    cmnDataDeSerializeBinary(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->ForceOrientationMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->ForcePositionMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->TorqueOrientationMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionStiffnessPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionStiffnessNegMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionDampingPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->PositionDampingNegMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->ForceBiasPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->ForceBiasNegMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->OrientationStiffnessPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->OrientationStiffnessNegMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->OrientationDampingPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->OrientationDampingNegMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->TorqueBiasPosMember, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, this->TorqueBiasNegMember, remoteFormat, localFormat);
}
void cmnDataSerializeText(std::ostream & outputStream, const prmFixtureGainCartesianSet & data, const char delimiter) throw (std::runtime_error) {
     data.SerializeText(outputStream, delimiter);
}
void prmFixtureGainCartesianSet::SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error) {
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
    cmnDataSerializeText(outputStream, this->ForceOrientationMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->ForcePositionMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->TorqueOrientationMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->PositionStiffnessPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->PositionStiffnessNegMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->PositionDampingPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->PositionDampingNegMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->ForceBiasPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->ForceBiasNegMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->OrientationStiffnessPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->OrientationStiffnessNegMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->OrientationDampingPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->OrientationDampingNegMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->TorqueBiasPosMember, delimiter);
    if (someData) {
        outputStream << delimiter;
    }
    someData = true;
    cmnDataSerializeText(outputStream, this->TorqueBiasNegMember, delimiter);
}
std::string cmnDataSerializeTextDescription(const prmFixtureGainCartesianSet & data, const char delimiter, const std::string & userDescription) {
     return data.SerializeTextDescription(delimiter, userDescription);
}
std::string prmFixtureGainCartesianSet::SerializeTextDescription(const char delimiter, const std::string & userDescription) const {
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
    description << cmnDataSerializeTextDescription(this->ForceOrientationMember, delimiter, prefix + "ForceOrientation");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->ForcePositionMember, delimiter, prefix + "ForcePosition");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->TorqueOrientationMember, delimiter, prefix + "TorqueOrientation");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->PositionStiffnessPosMember, delimiter, prefix + "PositionStiffnessPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->PositionStiffnessNegMember, delimiter, prefix + "PositionStiffnessNeg");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->PositionDampingPosMember, delimiter, prefix + "PositionDampingPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->PositionDampingNegMember, delimiter, prefix + "PositionDampingNeg");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->ForceBiasPosMember, delimiter, prefix + "ForceBiasPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->ForceBiasNegMember, delimiter, prefix + "ForceBiasNeg");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->OrientationStiffnessPosMember, delimiter, prefix + "OrientationStiffnessPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->OrientationStiffnessNegMember, delimiter, prefix + "OrientationStiffnessNeg");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->OrientationDampingPosMember, delimiter, prefix + "OrientationDampingPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->OrientationDampingNegMember, delimiter, prefix + "OrientationDampingNeg");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->TorqueBiasPosMember, delimiter, prefix + "TorqueBiasPos");
    if (someData) {
        description << delimiter;
    }
    someData = true;
    description << cmnDataSerializeTextDescription(this->TorqueBiasNegMember, delimiter, prefix + "TorqueBiasNeg");
    return description.str();
}
void cmnDataDeSerializeText(std::istream & inputStream, prmFixtureGainCartesianSet & data,
                            const char delimiter) throw (std::runtime_error) {
    data.DeSerializeText(inputStream, delimiter);
}
void prmFixtureGainCartesianSet::DeSerializeText(std::istream & inputStream,
                                          const char delimiter) throw (std::runtime_error) {
    bool someData = false;
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, *(dynamic_cast<mtsGenericObject*>(this)), delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->ForceOrientationMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->ForcePositionMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->TorqueOrientationMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionStiffnessPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionStiffnessNegMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionDampingPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->PositionDampingNegMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->ForceBiasPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->ForceBiasNegMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->OrientationStiffnessPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->OrientationStiffnessNegMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->OrientationDampingPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->OrientationDampingNegMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->TorqueBiasPosMember, delimiter);
    if (someData) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "prmFixtureGainCartesianSet");
    }
    someData = true;
    cmnDataDeSerializeText(inputStream, this->TorqueBiasNegMember, delimiter);
}
bool cmnDataScalarNumberIsFixed(const prmFixtureGainCartesianSet & data) {
     return data.ScalarNumberIsFixed();
}
bool prmFixtureGainCartesianSet::ScalarNumberIsFixed(void) const {
    return true
           && cmnDataScalarNumberIsFixed(*(dynamic_cast<const mtsGenericObject*>(this)))
           && cmnDataScalarNumberIsFixed(this->ForceOrientationMember)
           && cmnDataScalarNumberIsFixed(this->ForcePositionMember)
           && cmnDataScalarNumberIsFixed(this->TorqueOrientationMember)
           && cmnDataScalarNumberIsFixed(this->PositionStiffnessPosMember)
           && cmnDataScalarNumberIsFixed(this->PositionStiffnessNegMember)
           && cmnDataScalarNumberIsFixed(this->PositionDampingPosMember)
           && cmnDataScalarNumberIsFixed(this->PositionDampingNegMember)
           && cmnDataScalarNumberIsFixed(this->ForceBiasPosMember)
           && cmnDataScalarNumberIsFixed(this->ForceBiasNegMember)
           && cmnDataScalarNumberIsFixed(this->OrientationStiffnessPosMember)
           && cmnDataScalarNumberIsFixed(this->OrientationStiffnessNegMember)
           && cmnDataScalarNumberIsFixed(this->OrientationDampingPosMember)
           && cmnDataScalarNumberIsFixed(this->OrientationDampingNegMember)
           && cmnDataScalarNumberIsFixed(this->TorqueBiasPosMember)
           && cmnDataScalarNumberIsFixed(this->TorqueBiasNegMember)
    ;
}
size_t cmnDataScalarNumber(const prmFixtureGainCartesianSet & data) {
     return data.ScalarNumber();
}
size_t prmFixtureGainCartesianSet::ScalarNumber(void) const {
    return 0
           + cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)))
           + cmnDataScalarNumber(this->ForceOrientationMember)
           + cmnDataScalarNumber(this->ForcePositionMember)
           + cmnDataScalarNumber(this->TorqueOrientationMember)
           + cmnDataScalarNumber(this->PositionStiffnessPosMember)
           + cmnDataScalarNumber(this->PositionStiffnessNegMember)
           + cmnDataScalarNumber(this->PositionDampingPosMember)
           + cmnDataScalarNumber(this->PositionDampingNegMember)
           + cmnDataScalarNumber(this->ForceBiasPosMember)
           + cmnDataScalarNumber(this->ForceBiasNegMember)
           + cmnDataScalarNumber(this->OrientationStiffnessPosMember)
           + cmnDataScalarNumber(this->OrientationStiffnessNegMember)
           + cmnDataScalarNumber(this->OrientationDampingPosMember)
           + cmnDataScalarNumber(this->OrientationDampingNegMember)
           + cmnDataScalarNumber(this->TorqueBiasPosMember)
           + cmnDataScalarNumber(this->TorqueBiasNegMember)
    ;
}
std::string cmnDataScalarDescription(const prmFixtureGainCartesianSet & data, const size_t index,
                                     const std::string & userDescription) throw (std::out_of_range) {
    return data.ScalarDescription(index, userDescription);
}
std::string prmFixtureGainCartesianSet::ScalarDescription(const size_t index, const std::string & userDescription) const throw (std::out_of_range) {
    std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    size_t baseIndex = 0;
    size_t currentMaxIndex = 0;
    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)));
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(*(dynamic_cast<const mtsGenericObject*>(this)), index - baseIndex, prefix);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceOrientationMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->ForceOrientationMember, index - baseIndex, prefix + "ForceOrientation");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForcePositionMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->ForcePositionMember, index - baseIndex, prefix + "ForcePosition");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueOrientationMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->TorqueOrientationMember, index - baseIndex, prefix + "TorqueOrientation");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionStiffnessPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->PositionStiffnessPosMember, index - baseIndex, prefix + "PositionStiffnessPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionStiffnessNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->PositionStiffnessNegMember, index - baseIndex, prefix + "PositionStiffnessNeg");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionDampingPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->PositionDampingPosMember, index - baseIndex, prefix + "PositionDampingPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionDampingNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->PositionDampingNegMember, index - baseIndex, prefix + "PositionDampingNeg");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceBiasPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->ForceBiasPosMember, index - baseIndex, prefix + "ForceBiasPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceBiasNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->ForceBiasNegMember, index - baseIndex, prefix + "ForceBiasNeg");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationStiffnessPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->OrientationStiffnessPosMember, index - baseIndex, prefix + "OrientationStiffnessPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationStiffnessNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->OrientationStiffnessNegMember, index - baseIndex, prefix + "OrientationStiffnessNeg");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationDampingPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->OrientationDampingPosMember, index - baseIndex, prefix + "OrientationDampingPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationDampingNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->OrientationDampingNegMember, index - baseIndex, prefix + "OrientationDampingNeg");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueBiasPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->TorqueBiasPosMember, index - baseIndex, prefix + "TorqueBiasPos");
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueBiasNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalarDescription(this->TorqueBiasNegMember, index - baseIndex, prefix + "TorqueBiasNeg");
    }
    baseIndex = currentMaxIndex;
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmFixtureGainCartesianSet index out of range"));
    return "";
}
double cmnDataScalar(const prmFixtureGainCartesianSet & data, const size_t index) throw (std::out_of_range) {
    return data.Scalar(index);
}
double prmFixtureGainCartesianSet::Scalar(const size_t index) const throw (std::out_of_range) {
    size_t baseIndex = 0;
    size_t currentMaxIndex = 0;
    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const mtsGenericObject*>(this)));
    if (index < currentMaxIndex) {
        return cmnDataScalar(*(dynamic_cast<const mtsGenericObject*>(this)), index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceOrientationMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->ForceOrientationMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForcePositionMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->ForcePositionMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueOrientationMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->TorqueOrientationMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionStiffnessPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->PositionStiffnessPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionStiffnessNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->PositionStiffnessNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionDampingPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->PositionDampingPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->PositionDampingNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->PositionDampingNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceBiasPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->ForceBiasPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->ForceBiasNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->ForceBiasNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationStiffnessPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->OrientationStiffnessPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationStiffnessNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->OrientationStiffnessNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationDampingPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->OrientationDampingPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->OrientationDampingNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->OrientationDampingNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueBiasPosMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->TorqueBiasPosMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    currentMaxIndex += cmnDataScalarNumber(this->TorqueBiasNegMember);
    if (index < currentMaxIndex) {
        return cmnDataScalar(this->TorqueBiasNegMember, index - baseIndex);
    }
    baseIndex = currentMaxIndex;
    cmnThrow(std::out_of_range("cmnDataScalarDescription: prmFixtureGainCartesianSet index out of range"));
    return 1.2345;
}
#if CISST_HAS_JSON
void cmnDataToJSON(const prmFixtureGainCartesianSet & data, Json::Value & jsonValue) {
    data.ToJSON(jsonValue);
}
void prmFixtureGainCartesianSet::ToJSON(Json::Value & jsonValue) const {
    cmnDataToJSON(*(dynamic_cast<const mtsGenericObject*>(this)), jsonValue);
    cmnDataToJSON(this->ForceOrientationMember, jsonValue["ForceOrientation"]);
    cmnDataToJSON(this->ForcePositionMember, jsonValue["ForcePosition"]);
    cmnDataToJSON(this->TorqueOrientationMember, jsonValue["TorqueOrientation"]);
    cmnDataToJSON(this->PositionStiffnessPosMember, jsonValue["PositionStiffnessPos"]);
    cmnDataToJSON(this->PositionStiffnessNegMember, jsonValue["PositionStiffnessNeg"]);
    cmnDataToJSON(this->PositionDampingPosMember, jsonValue["PositionDampingPos"]);
    cmnDataToJSON(this->PositionDampingNegMember, jsonValue["PositionDampingNeg"]);
    cmnDataToJSON(this->ForceBiasPosMember, jsonValue["ForceBiasPos"]);
    cmnDataToJSON(this->ForceBiasNegMember, jsonValue["ForceBiasNeg"]);
    cmnDataToJSON(this->OrientationStiffnessPosMember, jsonValue["OrientationStiffnessPos"]);
    cmnDataToJSON(this->OrientationStiffnessNegMember, jsonValue["OrientationStiffnessNeg"]);
    cmnDataToJSON(this->OrientationDampingPosMember, jsonValue["OrientationDampingPos"]);
    cmnDataToJSON(this->OrientationDampingNegMember, jsonValue["OrientationDampingNeg"]);
    cmnDataToJSON(this->TorqueBiasPosMember, jsonValue["TorqueBiasPos"]);
    cmnDataToJSON(this->TorqueBiasNegMember, jsonValue["TorqueBiasNeg"]);
}
void cmnDataFromJSON(prmFixtureGainCartesianSet & data, const Json::Value & jsonValue) throw (std::runtime_error) {
    data.FromJSON(jsonValue);
}
void prmFixtureGainCartesianSet::FromJSON(const Json::Value & jsonValue) throw (std::runtime_error) {
    cmnDataFromJSON(*(dynamic_cast<mtsGenericObject*>(this)), jsonValue);
    cmnDataFromJSON(this->ForceOrientationMember, jsonValue["ForceOrientation"]);
    cmnDataFromJSON(this->ForcePositionMember, jsonValue["ForcePosition"]);
    cmnDataFromJSON(this->TorqueOrientationMember, jsonValue["TorqueOrientation"]);
    cmnDataFromJSON(this->PositionStiffnessPosMember, jsonValue["PositionStiffnessPos"]);
    cmnDataFromJSON(this->PositionStiffnessNegMember, jsonValue["PositionStiffnessNeg"]);
    cmnDataFromJSON(this->PositionDampingPosMember, jsonValue["PositionDampingPos"]);
    cmnDataFromJSON(this->PositionDampingNegMember, jsonValue["PositionDampingNeg"]);
    cmnDataFromJSON(this->ForceBiasPosMember, jsonValue["ForceBiasPos"]);
    cmnDataFromJSON(this->ForceBiasNegMember, jsonValue["ForceBiasNeg"]);
    cmnDataFromJSON(this->OrientationStiffnessPosMember, jsonValue["OrientationStiffnessPos"]);
    cmnDataFromJSON(this->OrientationStiffnessNegMember, jsonValue["OrientationStiffnessNeg"]);
    cmnDataFromJSON(this->OrientationDampingPosMember, jsonValue["OrientationDampingPos"]);
    cmnDataFromJSON(this->OrientationDampingNegMember, jsonValue["OrientationDampingNeg"]);
    cmnDataFromJSON(this->TorqueBiasPosMember, jsonValue["TorqueBiasPos"]);
    cmnDataFromJSON(this->TorqueBiasNegMember, jsonValue["TorqueBiasNeg"]);
}
#endif // CISST_HAS_JSON


