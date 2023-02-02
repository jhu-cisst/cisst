/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-08-27

  (C) Copyright 2012-2023 Johns Hopkins University (JHU), All Rights Reserved.

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
inline bool cmnCommandLineOptionsConvert<std::string>(const char * value, std::string & element) {
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
     std::list<double> someNumbers;
     options.AddOptionNoValue("v", "verbose", "more messages");
     options.AddOptionOneValue("f", "file", "file name", cmnCommandLineOptions::REQUIRED_OPTION, &filename);
     options.AddOptionOneValue("i", "iterations", "number of iterations", cmnCommandLineOptions::REQUIRED_OPTION, &iterations);
     options.AddOptionMultipleValues("n", "number", "one or many numbers", cmnCommandLineOptions::REQUIRED_OPTION, &someNumbers);

     std::string errorMessage;
     if (!options.Parse(argc, argv, errorMessage)) {
         std::cerr << "Error: " << errorMessage << std::endl;
         options.PrintUsage(std::cerr);
         return -1;
     }

     verbose = options.IsSet("v"); // or IsSet("verbose")
  \endcode

  In this example, note that the first option added doesn't require a
  value, i.e. the caller should just use the short option (-v) or the
  long one (--verbose).

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

#undef OPTIONAL   // work-around for Windows

class CISST_EXPORT cmnCommandLineOptions: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    typedef enum {REQUIRED_OPTION, OPTIONAL_OPTION, SQUASH_REQUIRED_OPTION} RequiredType;

 protected:
    class OptionBase {
        friend class cmnCommandLineOptions;
    protected:
#if (CISST_COMPILER == CISST_DOTNET2003)
        // Workaround for Visual Studio.NET 2003
    public:
#endif
        OptionBase(const std::string & shortOption, const std::string & longOption,
                   const std::string & description, RequiredType required);
        virtual ~OptionBase() {};
        virtual bool SetValue(const char * value) = 0;
        virtual std::string PrintValues(void) const = 0;
        std::string Short;
        std::string Long;
        std::string Description;
        RequiredType Required;
        bool Set;
    };

    class CISST_EXPORT OptionNoValue: public OptionBase {
        friend class cmnCommandLineOptions;
    protected:
        OptionNoValue(const std::string & shortOption, const std::string & longOption,
                      const std::string & description, RequiredType required = OPTIONAL_OPTION);
        virtual ~OptionNoValue() {};
        bool SetValue(const char * value);
        std::string PrintValues(void) const {
            return std::string();
        }
    };

    class CISST_EXPORT OptionOneValueBase: public OptionBase {
        friend class cmnCommandLineOptions;
    protected:
#if (CISST_COMPILER == CISST_DOTNET2003)
        // Workaround for Visual Studio.NET 2003
    public:
#endif
        OptionOneValueBase(const std::string & shortOption, const std::string & longOption,
                           const std::string & description, RequiredType required);
        virtual ~OptionOneValueBase() {};
    };
    template <typename _elementType>
    class OptionOneValue: public OptionOneValueBase {
        friend class cmnCommandLineOptions;
    protected:
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
        std::string PrintValues(void) const {
            std::stringstream stream;
            stream << *Value;
            return stream.str();
        }
        _elementType * Value;
    };

    class CISST_EXPORT OptionMultipleValuesBase: public OptionBase {
        friend class cmnCommandLineOptions;
    protected:
#if (CISST_COMPILER == CISST_DOTNET2003)
        // Workaround for Visual Studio.NET 2003
    public:
#endif
        OptionMultipleValuesBase(const std::string & shortOption, const std::string & longOption,
                                 const std::string & description, RequiredType required);
        virtual ~OptionMultipleValuesBase() {};
    };
    template <typename _elementType>
    class OptionMultipleValues: public OptionMultipleValuesBase {
        friend class cmnCommandLineOptions;
    protected:
        OptionMultipleValues(const std::string & shortOption, const std::string & longOption,
                       const std::string & description, RequiredType required,
                       std::list<_elementType> * value):
            OptionMultipleValuesBase(shortOption, longOption, description, required),
            Value(value)
        {
        }
        bool SetValue(const char * value) {
            _elementType temp;
            bool result = cmnCommandLineOptionsConvert(value, temp);
            if (result) {
                Value->push_back(temp);
                this->Set = true;
            }
            return result;
        }
        std::string PrintValues(void) const {
            typedef typename std::list<_elementType> ListType;
            typedef typename ListType::const_iterator const_iterator;
            const const_iterator end = Value->end();
            const_iterator iter;
            std::stringstream stream;
            for (iter = Value->begin(); iter != end; ++iter) {
                if (iter != Value->begin()) {
                    stream << ", ";
                }
                stream << *iter;
            }
            return stream.str();
        }
        std::list<_elementType> * Value;
    };

 public:
    cmnCommandLineOptions(void);

    ~cmnCommandLineOptions();

    bool AddOptionNoValue(const std::string & shortOption, const std::string & longOption,
                          const std::string & description, RequiredType required = OPTIONAL_OPTION);

    template <typename _elementType>
    bool AddOptionOneValue(const std::string & shortOption, const std::string & longOption,
                           const std::string & description, RequiredType required, _elementType * value) {
        std::string cleanedShort, cleanedLong;
        if (!this->ValidOptions(shortOption, longOption, cleanedShort, cleanedLong)) {
            return false;
        }
        typedef OptionOneValue<_elementType> OptionType;
        OptionType * option = new OptionOneValue<_elementType>(cleanedShort, cleanedLong, description,
                                                               required, value);
        this->Options.push_back(option);
        return true;
    }

    template <typename _elementType>
    bool AddOptionMultipleValues(const std::string & shortOption, const std::string & longOption,
                                 const std::string & description, RequiredType required, std::list<_elementType> * value) {
        std::string cleanedShort, cleanedLong;
        if (!this->ValidOptions(shortOption, longOption, cleanedShort, cleanedLong)) {
            return false;
        }
        typedef OptionMultipleValues<_elementType> OptionType;
        OptionType * option = new OptionMultipleValues<_elementType>(cleanedShort, cleanedLong, description,
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
    bool Parse(const std::vector<std::string> & arguments, std::string & errorMessage);

    /*! Parse and display error messages as well as usage if an error
      is found. */
    bool Parse(int argc, char * argv[], std::ostream & outputStream);
    bool Parse(const std::vector<std::string> & arguments, std::ostream & outputStream);

    /*! Print list of options with description to any C++ ostream.
      For example, `options.PrintUsage(std::cout)`. */
    void PrintUsage(std::ostream & outputStream);

    /*! Check if an option has been set.  This can be used after Parse
      to check if an optional value has been set or not.  The option
      name can be either the short or long one. */
    bool IsSet(const std::string & option);


    /* Print out the list of options parsed successfully. This can be
      used after Parse. */
    void PrintParsedArguments(std::string & parsedArguments) const;

 protected:
    std::string ProgramName;
    typedef std::list<OptionBase *> OptionsType;
    OptionsType Options;
    OptionBase * GetShortNoDash(const std::string & shortOption);
    OptionBase * GetLongNoDashDash(const std::string & longOption);
    bool ValidOptions(const std::string & shortOption, const std::string & longOption,
                      std::string & cleanedShort, std::string & cleanedLong);
    OptionBase * Get(const std::string & option);
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnCommandLineOptions)


#endif // _cmnCommandLineOptions_h
