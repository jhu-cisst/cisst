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



// utility to convert const char * to a C type
template <typename _elementType>
bool cmnCommandLineOptionsConvert(const char * value, _elementType & element) {
    std::stringstream localStream;
    localStream << value;
    bool error = false;
    try {
        localStream >> element;
    } catch (...) {
        error = true;
    }
    if (!error) {
        return true;
    }
    return false;
}


// template specialization to make sure spaces in strings (\ ) don't
// act as separators using >>
template <>
bool cmnCommandLineOptionsConvert<std::string>(const char * value, std::string & element) {
    element = value;
    return true;
}


/*!  \brief Set command line options and parse command line arguments

  This class should facilitate the development of command line tools
  requiring arguments.  For example, it can be used to parse
  parameters such as "my_program -v --file path/file.h --iterations 2".

  \code
     cmnCommandLineOptions options;
     bool verbose;
     std::string filename;
     int iterations;
     options.AddOptionNoValue("v", "verbose", "more messages", cmnCommandLineOptions::OPTIONAL, &verbose);
     options.AddOptionOneValue("f", "file", "file name", cmnCommandLineOptions::REQUIRED, &filename));
     options.AddOptionOneValue("i", "iterations", "number of iterations", cmnCommandLineOptions::REQUIRED, &iterations));

     std::string errorMessage;
     if (!options.Parse(argc, argv, errorMessage)) {
         std::cerr << "Error: " << errorMessage << std::endl;
         options.PrintUsage(std::cerr);
         return -1;
     }
  \endcode

  In this example, note that the first option added doesn't require a
  value, i.e. the caller should just use the short option (-v) or the
  long one (--verbose).  The option is set as optional, i.e. the
  "Parse" call will succeed even if the option has not been provided
  on the command line.

  The second and third options are required, i.e. the "Parse" call
  will return false if one or more required option is not found.  Both
  options are added as "option with one value" which means that the
  parser expect a value and this option can not be use twice (such as
  "my_program -i 2 -i 3).  The last parameter of "AddOptionOneValue"
  can be a string, an integer, a double or anything the C++ operator
  >> can stream in using a single argument of the command line.

  The "Parse" method takes the argc and argv of the "main" function.
  It also requires an empty string passed by reference to generate an
  error message in case the parse fails.  If parsing the command line
  options fails, one can use PrintUsage to display all the valid
  options.

  \ingroup cisstCommon
*/
class CISST_EXPORT cmnCommandLineOptions: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    typedef enum {REQUIRED, OPTIONAL} RequiredType;

 protected:
    class OptionBase {
        friend class cmnCommandLineOptions;
        OptionBase(const std::string & shortOption, const std::string & longOption,
                   const std::string & description, RequiredType required);
        virtual bool SetValue(const char * value) = 0;
        std::string Short;
        std::string Long;
        std::string Description;
        RequiredType Required;
        bool Set;
    };

    class CISST_EXPORT OptionNoValue: public OptionBase {
        friend class cmnCommandLineOptions;
        OptionNoValue(const std::string & shortOption, const std::string & longOption,
                      const std::string & description, RequiredType required, bool * result);
        bool SetValue(const char * value);
        bool * Value;
    };

    class CISST_EXPORT OptionOneValueBase: public OptionBase {
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
            OptionOneValueBase(shortOption, longOption, description, required),
            Value(value)
        {
        }
        bool SetValue(const char * value) {
            bool result = cmnCommandLineOptionsConvert(value, *(this->Value));
            if (result) {
                this->Set = true;
            }
            return result;
        }
        _elementType * Value;
    };

 public:
    cmnCommandLineOptions(void);

    bool AddOptionNoValue(const std::string & shortOption, const std::string & longOption,
                          const std::string & description, RequiredType required, bool * value);

    template <typename _elementType>
    bool AddOptionOneValue(const std::string & shortOption, const std::string & longOption,
                           const std::string & description, RequiredType required, _elementType * value) {
        if (this->GetShortNoDash(shortOption)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddOptionOneValue: option \"-" << shortOption << "\" already defined" << std::endl;
            return false;
        }
        typedef OptionOneValue<_elementType> OptionType;
        OptionType * option = new OptionOneValue<_elementType>(shortOption, longOption, description,
                                                               required, value);
        this->Options.push_back(option);
        return true;
    }

    /*! Parse using an array of const C strings.  This class doesn't
      modify any of the parameters so it is a bit safer.  This also
      allows to create unit tests using static arrays of strings
      without warnings. */
    bool Parse(int argc, const char * argv[], std::string & errorMessage);

    /*! Parse using an array of non const C strings.  This is the
      default for most applications using C like main function.
      Internally it created an array of const char * pointing to the
      existing arrays and then call the other Parse method. */
    bool Parse(int argc, char * argv[], std::string & errorMessage);

    /*! Print list of options with description to any C++ ostream.
      For example, `options.PrintUsage(std::cout)`. */
    void PrintUsage(std::ostream & outputStream);

 protected:
    std::string ProgramName;
    typedef std::list<OptionBase *> OptionsType;
    OptionsType Options;
    OptionBase * GetShortNoDash(const std::string & shortOption);
    OptionBase * Get(const std::string & option);
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnCommandLineOptions)


#endif // _cmnCommandLineOptions_h
