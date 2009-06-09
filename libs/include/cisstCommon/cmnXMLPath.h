/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):	Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

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

#ifndef _cmnXMLPath_h
#define _cmnXMLPath_h

#include <cisstConfig.h>

#if CISST_HAS_XML

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTokenizer.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnAssert.h>


#include <cassert>
#include <vector>
#include <string>

#include <iostream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <cisstCommon/cmnExport.h>


// At some point, windows.h is going to be included (OS abstraction).
// This instructs the compiler to include only the bare minimum of
// Windows header files.  Specifically, it prevents namespace 
// collisions between MsXml and Xerces/Xalan.
// #define WIN32_LEAN_AND_MEAN

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
  and axis/@name="x"/filter/@pgain
  Yes that simple!
  */


class CISST_EXPORT cmnXMLPath: public cmnGenericObject {
    /*! Register this class with a default level of detail 1.  Levels
      of details are 1 for errors, 2 for warnings and 3 for very
      verbose.  */
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

	/*! Libxml2 document source */
	xmlDocPtr Document;

	/*! Xpath context used by libxml2 */
	xmlXPathContextPtr XPathContext; 

protected:
	
	/*! Get the XPath result and cast it as internal storage type based on the library */
	bool GetXMLValue(const char * context, const char * XPath, xmlChar **storage);

	/*! set the value of attribute returned by XPath expression to value help by 
	internal storage type based on the library*/
	bool SetXMLValue(const char * context, const char * XPath, const xmlChar *storage);


public:
    /*! Constructor */
    cmnXMLPath();
    
    /*! Destructor */
    virtual ~cmnXMLPath();
	
    /*! Set the input source file2 */
    //@{
    void SetInputSource(const char *filename);
    inline void SetInputSource(const std::string & fileName) {
        this->SetInputSource(fileName.c_str());
    }
    //@}
	
    /*! For debugging. Print the attribute value as a string on stream */
    void PrintValue(std::ostream &out, const char *context, const char *XPath);
    
	/*! Get/Set the XPath result and cast it as bool */
    bool GetXMLValue(const char * context, const char * XPath, bool &value);
	bool GetXMLValue(const char * context, const char * XPath, bool &value, const bool &valueifmissing);
    bool SetXMLValue(const char * context, const char * XPath, bool value);

    /*! Get/Set the XPath result and cast it as int */
    bool GetXMLValue(const char * context, const char * XPath, int &value);
	bool GetXMLValue(const char * context, const char * XPath, int &value, const int &valueifmissing);
	bool SetXMLValue(const char * context, const char * XPath, int value);
    
    /*! Get/Set the XPath result and cast it as double */
    bool GetXMLValue(const char * context, const char * XPath, double &value);
	bool GetXMLValue(const char * context, const char * XPath, double &value, const double &valueifmissing);
	bool SetXMLValue(const char * context, const char * XPath, double value);

    /*! Get/Set the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, std::string &storage);
	bool GetXMLValue(const char * context, const char * XPath, std::string &storage, const std::string &valueifmissing);
    bool SetXMLValue(const char * context, const char * XPath, const std::string &storage);

};


CMN_DECLARE_SERVICES_INSTANTIATION(cmnXMLPath);

#endif // CISST_HAS_XML

#endif // _cmnXMLPath_h
