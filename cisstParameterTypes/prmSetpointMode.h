// -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:

#ifndef _prmSetpointMode_h
#define _prmSetpointMode_h

#include <cstdint>

// Always include last
#include <cisstParameterTypes/prmExport.h>

enum class prmSetpointMode : uint8_t {
    NONE = 0,
    POSITION = 1 << 0,
    VELOCITY = 1 << 1,
    EFFORT = 1 << 2
};

// operator to combine setpoint modes
inline prmSetpointMode operator|(prmSetpointMode a, prmSetpointMode b)
{
    return static_cast<prmSetpointMode>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

// operator to test setpoint modes
inline bool operator&(prmSetpointMode a, prmSetpointMode b)
{
    return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

template <>
inline void CISST_EXPORT cmnDataJSON<prmSetpointMode>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = static_cast<uint8_t>(data);
}

template <>
inline void CISST_EXPORT cmnDataJSON<prmSetpointMode>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<prmSetpointMode>::DeSerializeText: empty JSON value");
    }

    // check that value can be expressed as bitwise-or of known setpoint modes
    unsigned int raw_data = jsonValue.asUInt();
    constexpr size_t num_nonzero_modes = 3;
    if (raw_data >= (1 << num_nonzero_modes)) {
        cmnThrow("cmnDataJSON<prmSetpointMode>::DeSerializeText: JSON value does not correspond to combination of known modes");
    }

    data = static_cast<prmSetpointMode>(raw_data);
}

template <>
class cmnData<prmSetpointMode> {
public:
    enum {IS_SPECIALIZED = 1};

    static void Copy(prmSetpointMode & data, const prmSetpointMode & source) {
        data = source;
    }

    static std::string HumanReadable(const prmSetpointMode & data) {
        switch (data)
        {
        case prmSetpointMode::NONE:
            return "prmSetpointMode::NONE";
        case prmSetpointMode::POSITION:
            return "prmSetpointMode::POSITION";
        case prmSetpointMode::VELOCITY:
            return "prmSetpointMode::VELOCITY";
        case prmSetpointMode::EFFORT:
            return "prmSetpointMode::EFFORT";
        default:
            return "prmSetpointMode unknown";
        }
    }

    static void SerializeBinary(const prmSetpointMode & data,
                                std::ostream & outputStream) CISST_THROW(std::runtime_error) {
        cmnData<uint8_t>::SerializeBinary(static_cast<uint8_t>(data), outputStream);
    }

    static void DeSerializeBinary(prmSetpointMode & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat) CISST_THROW(std::runtime_error) {
        uint8_t value;
        cmnData<uint8_t>::DeSerializeBinary(value, inputStream, localFormat, remoteFormat);
        data = static_cast<prmSetpointMode>(value);
    }

    static void SerializeText(const prmSetpointMode & data, std::ostream & outputStream,
                              const char CMN_UNUSED(delimiter) = ',') CISST_THROW(std::runtime_error) {
        outputStream << static_cast<uint8_t>(data);
    }

    static std::string SerializeDescription(const prmSetpointMode & CMN_UNUSED(data),
                                            const char CMN_UNUSED(delimiter) = ',',
                                            const std::string & userDescription = "") {
        return (userDescription == "") ? "prmSetpointMode" : (userDescription + "prmSetpointMode");
    }

    static void DeSerializeText(prmSetpointMode & data, std::istream & inputStream,
                                const char CMN_UNUSED(delimiter) = ',') CISST_THROW(std::runtime_error) {
        uint8_t value;
        inputStream >> value;
        data = static_cast<prmSetpointMode>(value);
    }

    static bool ScalarNumberIsFixed(const prmSetpointMode & CMN_UNUSED(data)) {
        return true;
    }

    static size_t ScalarNumber(const prmSetpointMode & CMN_UNUSED(data)) {
        return 1;
    }

    static std::string ScalarDescription(const prmSetpointMode & CMN_UNUSED(data), const size_t index,
                                        const std::string & userDescription) CISST_THROW(std::out_of_range) {
        if (index == 0) {
            return (userDescription == "") ? "prmSetpointMode" : (userDescription + "prmSetpointMode");
        }
        cmnThrow(std::out_of_range("cmnDataScalarDescription: prmSetpointMode index out of range"));
        return "";
    }

    static double Scalar(const prmSetpointMode & data, const size_t index) CISST_THROW(std::out_of_range) {
        if (index == 0) {
            return static_cast<double>(data);
        }
        cmnThrow(std::out_of_range("cmnDataScalar: prmSetpointMode index out of range"));
        return 0.0;
    }
};

#endif // _prmSetpointMode_h
