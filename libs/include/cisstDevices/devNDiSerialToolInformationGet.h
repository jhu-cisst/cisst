/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Eric Lin, Joseph Vidalis
  Created on: 2008-09-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _devNDiSerialToolInformationGet_h
#define _devNDiSerialToolInformationGet_h


#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsStateIndex.h>

#include <string>

// Always include last
#include <cisstDevices/devExport.h>


class CISST_EXPORT devNDiSerialToolInformationGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
 private:
    /* The following three functions are a private method for storing
       information as strings. The string itself is stored as a
       character array of fixed size 256. Anything larger is
       truncated. This allows for easy usage for this parameter by the
       users of cisstTracker. */
    typedef struct
    { 
        char Value[256];
    } CHAR_256;

    CHAR_256 StringToChar256(std::string in) // Should be a reference ADV
    {
        CHAR_256 temp;
        // Handle string lengths > 256 but adding "Truncated"
        if (in.length() > 255) {
            std::string truncated("Truncated\0");
            in.replace(247,10,truncated);
        }
        // Convert // BAD, there must be a faster way, at least memcopy.  ADV
        const char *inCharPointer = in.c_str();
        for (unsigned int i = 0; i <= in.length(); i++) {
            temp.Value[i] = inCharPointer[i];   
        }
        return temp;
    }
    std::string Char256ToString(const CHAR_256 & in)
    {
        //Convert
        std::string temp(in.Value);
        return temp;
    }

 public:
    /*! default constructor */
    inline devNDiSerialToolInformationGet(void) {}
    
    /*! destructor */
    virtual ~devNDiSerialToolInformationGet();
    
    // Tool Identifiers
    /*! Declares Set and Get methods for Name of the Tool */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(CHAR_256, Name);
    
    /*! Overloads the SetName function for string arguments */
    inline void SetName(const std::string & name) { 
        this->SetName(StringToChar256(name)); 
    }
    
    /*! Overloads the GetName function for string arguments */
    inline void GetName(std::string &name)
    {
        CHAR_256 temp;
        GetName(temp);
        name = Char256ToString(temp); 
    }
    
    /*! Declares Set and Get methods for Serial Number of the Tool */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(CHAR_256, SerialNumber);
    
    /*! Overloads the SetSerialNumber function for string arguments */
    inline void SetSerialNumber(const std::string & serial)
    { 
        this->SetSerialNumber(StringToChar256(serial)); 
    }
    
    /*! Overloads the GetSerialNumber function for string arguments */
    inline void GetSerialNumber(std::string &serial)
    {
        CHAR_256 temp;
        GetSerialNumber(temp);
        serial = Char256ToString(temp); 
    }
    
    // Tool Information Data
    MTS_DECLARE_MEMBER_AND_ACCESSORS(double, Error);
    
    // Tool Information Flags
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, Disabled);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, Initialized);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, Enabled);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, Missing);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, OutOfVolume);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, PartiallyOutOfVolume); 

    /* NOTE: For some unknown reason a segfault would be cause when acsessing the final
       two parameters. Each is stored as a word in length. By placing following variable
       it causes the following variable to be a buffer for the segfault for the parameter. */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(int, UNUSEDBUFFER); 
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDiSerialToolInformationGet);


#endif //_devNDiSerialToolInformationGet_h

