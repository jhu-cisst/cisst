/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Peter Kazanzides

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Defines cmnStreamRawParser
*/
#pragma once

#ifndef _cmnStreamRawParser_h
#define _cmnStreamRawParser_h

#include <iostream>
#include <string>
#include <set>
#include <functional>

#include <cisstCommon/cmnLogger.h>

#include <cisstCommon/cmnExport.h>

/*!  cmnStreamRawParser provides the ability to parse {keyword, value} pairs from a stream.
     For example, consider the case where we have the following configuration variables:

     \code
     cmnDouble s;
     vct3 v1, v2;
     double d;
     \endcode

     These variables can be initialized from the following input file (called "test.txt"):

     \code
     S 10
     D 5
     V1 1,2,3
     V2 4,5,6
     \endcode

     The code to do this is as follows:

     \code
     cmnStreamRawParser Config;
     Config.AddEntry("S", s);
     Config.AddEntry("V1", v1, ',');
     Config.AddEntry("V2", v2, ',');
     Config.AddEntryStreamable("D", d);
     std::ifstream file("test.txt");
     bool success = Config.Parse(file);
     \endcode

     If successful, the variables S, V1, V2, and D will be assigned the
     values from the file.  Note that the variable names in the code
     do not have to be the same as the string names (in this example,
     the variables names are lower case and the strings are upper case).

     For debugging (or to recreate the text file) call:

     \code
     Config.ToStream(cout);
     \endcode

     Note that the AddEntry method is templated, so that any data type can
     be used, as long as it contains the FromStreamRaw and ToStreamRaw methods.
     Similarly, the AddEntryStreamable method is templated, so that any data
     type can be used that defines the stream in (>>) and stream out (<<) operators.

     Current limitations:

       - the # character indicates a comment line (hard-coded).

       - the code does not support a delimiter (other than whitespace) between the keyword and data.
*/

class CISST_EXPORT cmnStreamRawParser
{
   class EntryBase {
   protected:
       std::string key;
       char delimiter;
       bool valid;
    public:
       EntryBase(const std::string &name, char delim = ' ') : key(name), delimiter(delim), valid(false) {}
       virtual ~EntryBase() {}

       std::string GetKey() const { return key; }
       bool isValid() const { return valid; }
       void SetValid(bool val) { valid = val; }

       // Following should be implemented in the derived classes
       virtual bool Parse(std::istream &CMN_UNUSED(inputStream)) { return false; }
       virtual void ToStream(std::ostream & outputStream) const { outputStream << key << " (default)"; }
   };

   template <class _elementType>
   class Entry : public EntryBase {
       _elementType *valuePtr;
   public:
       Entry(const std::string &name, _elementType &data, char delim = ' ') : EntryBase(name, delim), valuePtr(&data) {}
       ~Entry() {}

       bool Parse(std::istream &inputStream) {
           if (valid) CMN_LOG_INIT_WARNING << "cmnStreamRawParser: duplicate entry for " << key << std::endl;
           if (valuePtr)
               valid = valuePtr->FromStreamRaw(inputStream, delimiter);
           return valid;
       }

       void ToStream(std::ostream & outputStream) const {
           outputStream << key << " ";
           if (valuePtr && valid)
               valuePtr->ToStreamRaw(outputStream, delimiter);
           else
               outputStream << "(invalid)";
       }
   };

   template <class _elementType>
   class EntryStreamable : public EntryBase {
       _elementType *valuePtr;
   public:
       EntryStreamable(const std::string &name, _elementType &data) : EntryBase(name, ' '), valuePtr(&data) {}
       ~EntryStreamable() {}

       bool Parse(std::istream &inputStream) {
           if (valid) CMN_LOG_INIT_WARNING << "cmnStreamRawParser: duplicate entry for " << key << std::endl;
           if (valuePtr)
               inputStream >> *valuePtr;
           valid = inputStream.good();
           return valid;
       }

       void ToStream(std::ostream & outputStream) const {
           outputStream << key << " ";
           if (valuePtr && valid)
               outputStream << *valuePtr;
           else
               outputStream << "(invalid)";
       }
   };

   // Comparison operator for std::set
   struct KeyListLess: public std::binary_function<const cmnStreamRawParser::EntryBase*, const cmnStreamRawParser::EntryBase*, bool>
   {
       result_type operator()(first_argument_type p1, second_argument_type p2) const
      {
          return p1->GetKey() < p2->GetKey();
      }
   };

   typedef std::set<cmnStreamRawParser::EntryBase *, KeyListLess> KeyListType;
   KeyListType KeyList;

public:
   cmnStreamRawParser() {}
   ~cmnStreamRawParser();

   /*! Add a {keyword,value} pair to be parsed from the input stream.
       \param name Variable name (key)
       \param data Variable to be assigned when input stream is parsed
       \param delim Delimiter between data elements (but not between keyword and data)
       \returns true if successful; false if not (e.g., duplicate entry)
   */
   template <class _elementType>
   bool AddEntry(const std::string &name, _elementType &data, char delim = ' ')
   {
       cmnStreamRawParser::EntryBase *newEntry = new Entry<_elementType>(name, data, delim);
       bool result = KeyList.insert(newEntry).second;
       if (!result) delete newEntry;  // if not inserted, delete entry
       return result;
   }

   /*! Add a {keyword,value} pair to be parsed from the input stream. This method is for
       data types where the stream operators (<< and >>) can be used; in particular, for
       primitive C data types (e.g., int, double) and std::string.
       \param name Variable name (key)
       \param data Variable to be assigned when input stream is parsed
       \returns true if successful; false if not (e.g., duplicate entry)
   */
   template <class _elementType>
   bool AddEntryStreamable(const std::string &name, _elementType &data)
   {
       cmnStreamRawParser::EntryBase *newEntry = new EntryStreamable<_elementType>(name, data);
       bool result = KeyList.insert(newEntry).second;
       if (!result) delete newEntry;  // if not inserted, delete entry
       return result;
   }

   /*! Set all entries valid or invalid. */
   void SetAllValid(bool val = true);

   /*! Parse the input stream, extracting the data for all keywords that are found.
     Returns true if valid data extracted for all keywords. */
   bool Parse(std::istream & inputStream);

   /*! Write all {keyword,value} pairs to the output stream. If valid data was not
     parsed, will write "(invalid)". */
   void ToStream(std::ostream & outputStream) const;
};

#endif // _cmnStreamRawParser_h
