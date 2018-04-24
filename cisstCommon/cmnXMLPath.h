/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Anton Deguet, Ali Uneri
  Created on: 2004-04-30

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Defines XMLPath
*/

#pragma once

#ifndef _cmnXMLPath_h
#define _cmnXMLPath_h

#include <cisstCommon/cmnConfigXML.h>
#include <cisstCommon/cmnGenericObject.h>

#if CISST_HAS_XML

// always include last
#include <cisstCommon/cmnExportXML.h>

// forward declaration of struct containing all data used by wrapper.
class cmnXMLPathData;

/*!
  \ingroup cisstCommon

  This class provides a simple but somewhat powerful XPath implementation
  for parsing and validating XML.  Validation is handed via XML Schemas.
  XPath query expressions are passed to a processor method (GetXMLValue),
  along with a context and typed output param reference, which processes
  the query and returns results as the output parameter type.

  Error checking is limited:
  ** SetInputSource() tries to catch as many exceptions as possible,
  and does no error checking whatsoever.  Data may be missing and
  misformated. Generates a GENERAL PROTECTION FAULT (via assertions)
  on error.
  ** SetInputSourceWithValidation() reports parsing error/warning
  events through the ParseErrorHandler object.  Data is verified
  against an XML Schema named in the XML source; no other data
  checking is necessary (if the schema is complete).  Also
  generates a GPF, until a new error policy is decided.
  This is very easy to use - set the input XML file to parse using
  SetInputSource, and Get any attribute value of a tag using
  GetXMLValue, which accepts a context and a XPath
  As an example consider the following config file

  <code> <config>
      <device name="mei">
          <axis name="x" axis-on-amp="1" logical-axis-no="1">
            <filter pgain="1000" ipgain="100" dgain="200" />
            <encoder out-units="mm" multiplier="0.123" />
        </axis>
        ...
        <axis name="rx" axis-on-amp="5" logical-axis-no="4">
             <filter pgain="100" ipgain="10" dgain="50"  />
             <encoder out-units="rad" multiplier="0.564" />
        </axis>
    </device>
    <task name="forceservo">
        <provides name="startservo" argtype="bool" />
        <provides name="stopservo" argtype="bool" />
        ...
        <state name="velocity" elementtype="std::vector[double]" />
        <state name="force" elementtype="cmnFNTNmData" />
        ...
    </task>
    </config>
  </code>
  Now the value of pgain for axis can be read by passing /config/device as context
  and axis[@name="x"]/filter/@pgain
  Yes that simple!

  One can also issue direct XPath queries using the Query method.
  This can be used to count nodes
  (eg. <code>path.Query("count(/data-1/data-1-2/data-1-2-x)",
  intValue);</code>).  We strongly recommend using GetXMLValue
  whenever possible as Qt and LibXml2 queries might slightly differ
  (e.g. Qt node content queries start with "data(").

  \todo Add tests in all methods to make sure input is defined
  \todo Add write Qt
  \todo Add tests to get nodes, not just attributes (might already work on libxml2)
  */
class CISST_EXPORT cmnXMLPath: public cmnGenericObject
{
    /*! Register this class with a default log mask */
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    cmnXMLPathData * Data;

public:
    /*! Constructor */
    cmnXMLPath();

    /*! Destructor */
    virtual ~cmnXMLPath();

    /*! Set the input source file */
    //@{
    void SetInputSource(const char * filename);
    void SetInputSource(const std::string & fileName);
    //@}

    /*! Set the input source file */
    //@{
    bool ValidateWithSchema(const char * filename);
    bool ValidateWithSchema(const std::string & fileName);
    //@}

    /*! Get the error reported by the last call to ValidateWithSchema.
      Both errors and warnings are listed, one per line.  The exact
      message depends on the underlying library used, i.e. Qt or
      libxml2. */
    const std::string & GetLastErrors(void) const;

    /*! Save to file */
    //@{
    bool SaveAs(const char * filename) const;
    bool SaveAs(const std::string & fileName) const;
    //@}

    /*! For debugging. Print the attribute value as a string on stream */
    void PrintValue(std::ostream & out, const char * context, const char * XPath);

    /*! Check if contest exists */
    bool Exists(const char * context);

    /*! Get/Set the XPath result and cast it as bool */
    //@{
    bool GetXMLValue(const char * context, const char * XPath, bool & value);
    bool GetXMLValue(const char * context, const char * XPath, bool & value, const bool & valueIfMissing);
    bool SetXMLValue(const char * context, const char * XPath, const bool & value);
    bool Query(const char * query, bool & value);
    //@}

    /*! Get/Set the XPath result and cast it as int */
    //@{
    bool GetXMLValue(const char * context, const char * XPath, int & value);
    bool GetXMLValue(const char * context, const char * XPath, int & value, const int & valueIfMissing);
    bool SetXMLValue(const char * context, const char * XPath, const int & value);
    bool Query(const char * query, int & value);
    //@}

    /*! Get/Set the XPath result and cast it as double */
    //@{
    bool GetXMLValue(const char * context, const char * XPath, double & value);
    bool GetXMLValue(const char * context, const char * XPath, double & value, const double & valueIfMissing);
    bool SetXMLValue(const char * context, const char * XPath, const double & value);
    bool Query(const char * query, double & value);
    //@}

    /*! Get/Set the XPath result and cast it as string */
    //@{
    bool GetXMLValue(const char * context, const char * XPath, std::string & value);
    bool GetXMLValue(const char * context, const char * XPath, std::string & value, const std::string & valueIfMissing);
    bool SetXMLValue(const char * context, const char * XPath, const std::string & value);
    bool Query(const char * query, std::string & value);
    //@}

    /*! Templated helpers to define context and path using std::string */
    //@{
    bool Exists(const std::string & context) {
        return this->Exists(context.c_str());
    }

    template <class __elementType>
    bool GetXMLValue(const std::string & context, const std::string & XPath, __elementType & value) {
        return this->GetXMLValue(context.c_str(), XPath.c_str(), value);
    }

    template <class __elementType>
    bool GetXMLValue(const std::string & context, const std::string & XPath, __elementType & value, const __elementType & valueIfMissing) {
        return this->GetXMLValue(context.c_str(), XPath.c_str(), value, valueIfMissing);
    }

    template <class __elementType>
    bool SetXMLValue(const std::string & context, const std::string & XPath, const __elementType & value)
    {
        return this->SetXMLValue(context.c_str(), XPath.c_str(), value);
    }

    template <class __elementType>
    bool Query(const std::string & query, __elementType & value) {
        return this->Query(query.c_str(), value);
    }
};


CMN_DECLARE_SERVICES_INSTANTIATION(cmnXMLPath);

#endif // CISST_HAS_XML

#endif // _cmnXMLPath_h
