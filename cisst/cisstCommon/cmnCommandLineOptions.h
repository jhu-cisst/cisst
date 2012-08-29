/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-08-27

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _cmnCommandLineOptions_h
#define _cmnCommandLineOptions_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnTokenizer.h>

#include <string>
#include <list>
#include <sstream>
#include <iostream>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


/*!  \brief Set command line options and parse command line arguments 

  \ingroup cisstCommon
*/
class CISST_EXPORT cmnCommandLineOptions: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    typedef enum {REQUIRED, OPTIONAL} RequiredType;

 protected:
    class OptionNoValue {
        friend class cmnCommandLineOptions;
        OptionNoValue(const std::string & shortOption, const std::string & longOption,
                      const std::string & description, RequiredType required);
        virtual bool SetValue(char * value);
        std::string Short;
        std::string Long;
        std::string Description;
        RequiredType Required;
        bool Set;
    };

    class OptionOneValueBase: public OptionNoValue {
        friend class cmnCommandLineOptions;
        OptionOneValueBase(const std::string & shortOption, const std::string & longOption,
                           const std::string & description, RequiredType required);
    };

    template <typename _elementType>
    class OptionOneValue: public OptionOneValueBase {
        friend class cmnCommandLineOptions;
        OptionOneValue(const std::string & shortOption, const std::string & longOption,
                       const std::string & description, RequiredType required,
                       _elementType * value):
            OptionOneValueBase(shortOption, longOption, description, required)
        {
            this->Value = value;
        }
        bool SetValue(char * value) {
            *(this->Value) = value; // might work for strings, need to use >> operator
            this->Set = true;
            return true;
        }
        _elementType * Value;
    };
        
 public:
    cmnCommandLineOptions(void);

    bool AddOptionNoValue(const std::string & shortOption, const std::string & longOption,
                          const std::string & description, RequiredType required);

    template <typename _elementType>
    bool AddOptionOneValue(const std::string & shortOption, const std::string & longOption,
                           const std::string & description, RequiredType required, _elementType * value) {
        typedef OptionOneValue<_elementType> OptionType;
        OptionType * option = new OptionOneValue<_elementType>(shortOption, longOption, description,
                                                               required, value);
        if (this->GetShortNoDash(shortOption)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddOptionOneValue: option \"-" << shortOption << "\" already defined" << std::endl;
            return false;
        }
        this->Options.push_back(option);
        return true;
    }

    bool Parse(int argc, char * argv[], std::string & errorMessage);

    void PrintUsage(std::ostream & outputStream);

 protected:
    std::string ProgramName;
    typedef std::list<OptionNoValue *> OptionsType;
    OptionsType Options;
    OptionNoValue * GetShortNoDash(const std::string & shortOption);
    OptionNoValue * Get(const std::string & option);
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnCommandLineOptions)


#endif // _cmnCommandLineOptions_h

