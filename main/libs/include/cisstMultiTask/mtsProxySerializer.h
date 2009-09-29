/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsProxySerializer.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2009-09-01

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines the base class for command proxy classes
*/

#ifndef _mtsProxySerializer_h
#define _mtsProxySerializer_h

#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstMultiTask/mtsGenericObject.h>

/*!
  \ingroup cisstMultiTask

  This class provides the feature of serialization and deserialization for
  command proxy and function proxy classes.
*/
class mtsProxySerializer {
private:
    /*! Internal buffer for serialization and deserialization. */
    std::stringstream SerializationBuffer;
    std::stringstream DeSerializationBuffer;

    /*! Serializer and Deserializer. */
    cmnSerializer * Serializer;
    cmnDeSerializer * DeSerializer;

public:
    mtsProxySerializer() {
        Serializer = new cmnSerializer(SerializationBuffer);
        DeSerializer = new cmnDeSerializer(DeSerializationBuffer);
    }

    ~mtsProxySerializer() {
        delete Serializer;
        delete DeSerializer;
    }

    void Serialize(const mtsGenericObject & originalObject, std::string & serializedObject) {
        try {
            SerializationBuffer.str("");
            Serializer->Serialize(originalObject);
            serializedObject = SerializationBuffer.str();
        } catch (std::runtime_error e) {
            CMN_LOG_RUN_ERROR << "Serialization failed: " << originalObject.ToString() << std::endl;
            CMN_LOG_RUN_ERROR << e.what() << std::endl;
            serializedObject = "";
        }        
    }

    void DeSerialize(const std::string & serializedObject, mtsGenericObject & originalObject) {
        try {
            DeSerializationBuffer.str("");
            DeSerializationBuffer << serializedObject;
            DeSerializer->DeSerialize(originalObject);
        }  catch (std::runtime_error e) {
            originalObject.SetValid(false);
            CMN_LOG_RUN_ERROR << "DeSerialization failed: " << e.what() << std::endl;
        }
    }

    mtsGenericObject * DeSerialize(const std::string & serializedObject) {
        cmnGenericObject * deserializedObject = 0;
        try {
            DeSerializationBuffer.str("");
            DeSerializationBuffer << serializedObject;
            deserializedObject = DeSerializer->DeSerialize();
        }  catch (std::runtime_error e) {            
            CMN_LOG_RUN_ERROR << "DeSerialization failed: " << e.what() << std::endl;
            return 0;
        }

        return dynamic_cast<mtsGenericObject*>(deserializedObject);
    }
};

#endif // _mtsProxySerializer_h
