/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnXMLPath.h>

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnPrintf.h>

#include <limits>
#include <string.h> // for strncmp

#if CISST_HAS_QT_THIS_IS_NOT_READY_YET

#include <QFile>
#include <QDomDocument>

class cmnXMLPathData
{
public:
    /*! File used by Qt */
    QFile File;
    
    /*! Dom Document */
    QDomDocument Document;
};


#else

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

class cmnXMLPathData
{
public:
    // to allow log to be associate to owner class
    const cmnClassServicesBase * OwnerServices;

    // methods use to emulate the cmnGenericObject interface used by
    // CMN_LOG_CLASS macros.
    inline const cmnClassServicesBase * Services(void) const {
        return this->OwnerServices;
    }

    inline cmnLogger::StreamBufType * GetLogMultiplexer(void) const {
        return cmnLogger::GetMultiplexer();
    }

    // Libxml2 document source
    xmlDocPtr Document;

    // Xpath context used by libxml2
    xmlXPathContextPtr XPathContext;

    // set the XPath result and cast it as internal storage type based on the library
    bool GetXMLValueXMLChar(const char * context, const char * XPath, xmlChar **storage);

    // set the value of attribute returned by XPath expression to
    // value help by internal storage type based on the library
    bool SetXMLValueXMLChar(const char * context, const char * XPath, const xmlChar *storage);
};

#endif


cmnXMLPath::cmnXMLPath():
    Data(0)
{
    // create internal data and set pointers to services to control log
    this->Data = new cmnXMLPathData();
    this->Data->OwnerServices = this->Services();
}



cmnXMLPath::~cmnXMLPath()
{
    if (this->Data) {
        delete this->Data;
    }
}



void cmnXMLPath::SetInputSource(const char * filename)
{
    /* Load XML document */
    this->Data->Document = xmlParseFile(filename);
    if (this->Data->Document == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputSource: an error occured while parsing \"" << filename << "\"" << std::endl;
    }
    CMN_ASSERT(this->Data->Document != 0);

    this->Data->XPathContext = xmlXPathNewContext(this->Data->Document);
    if (this->Data->XPathContext == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputSource: an error occured while parsing \"" << filename << "\"" << std::endl;
        xmlFreeDoc(this->Data->Document);
    }
    CMN_ASSERT(this->Data->XPathContext != 0);
}


void cmnXMLPath::SetInputSource(const std::string & fileName)
{
    this->SetInputSource(fileName.c_str());
}


bool cmnXMLPath::SaveAs(const char * filename) const
{
    if (xmlSaveFile(filename, this->Data->Document) <= 0) {
        return false;
    }
    return true;
}


bool cmnXMLPath::SaveAs(const std::string & filename) const
{
    return this->SaveAs(filename.c_str());
}


void cmnXMLPath::PrintValue(std::ostream & out, const char * context, const char * XPath)
{
    std::string str;
    if (GetXMLValue(context, XPath, str) == false) {
        CMN_LOG_CLASS_RUN_WARNING << "PrintValue: no nodes matched the location path \"" << XPath
                                  << "\"" << std::endl;
    } else {
        out << str << std::endl;
    }
}


bool cmnXMLPathData::GetXMLValueXMLChar(const char * context, const char * XPath, xmlChar **storage)
{
    /* Evaluate xpath expression */
    /* first we need to concat the context and Xpath to fit libxml standard. context is fixed at
    document root in libxml2 */
    std::string xpathlibxml("/");
    xpathlibxml += context;
    xpathlibxml += "/";
    xpathlibxml += XPath;
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar *)(xpathlibxml.c_str()), this->XPathContext);
    if(xpathObj != NULL) {
        xmlNodePtr cur;
        xmlNodeSetPtr nodes = xpathObj->nodesetval;
        int size = (nodes)? nodes->nodeNr: 0;
        int i;

        for(i = 0; i < size; ++i) {
            CMN_ASSERT(nodes->nodeTab[i] != 0);
            if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
                cur = nodes->nodeTab[i];
                *storage = xmlNodeGetContent(cur);
                CMN_LOG_CLASS_RUN_VERBOSE << "GetXMLValueXMLChar: read Xpath: " << XPath << " Node name: "
                                          << cur->name << " Content: " << *storage << std::endl;
            } else {
                cur = nodes->nodeTab[i];
                CMN_LOG_CLASS_RUN_WARNING << "GetXMLValueXMLChar: node is not attribute node \"" << XPath
                                          << "\" Node name: " << cur->name << std::endl;
            }
        }
        return true;
    }
    CMN_LOG_CLASS_RUN_WARNING << "GetXMLValueXMLChar: unable to match the location path [" << XPath
                              << "] in context [" << context << "]" << std::endl;
    return false;
}


bool cmnXMLPathData::SetXMLValueXMLChar(const char * context, const char * XPath, const xmlChar *storage)
{
    /* Evaluate xpath expression */
    /* first we need to concat the context and Xpath to fit libxml standard. context is fixed at
    document root in libxml2 */
    std::string xpathlibxml("/");
    xpathlibxml += context;
    xpathlibxml += "/";
    xpathlibxml += XPath;
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar *)(xpathlibxml.c_str()), this->XPathContext);
    if(xpathObj != NULL) {
        xmlNodePtr cur;
        xmlNodeSetPtr nodes = xpathObj->nodesetval;
        int size = (nodes)? nodes->nodeNr: 0;
        int i;

        /*
          NOTE: the nodes are processed in reverse order, i.e. reverse
                document order because xmlNodeSetContent can actually
                free up descendant of the node and such nodes may have
                been selected too ! Handling in reverse order ensure
                that descendant are accessed first, before they get
                removed. Mixing XPath and modifications on a tree must
                be done carefully !
        */
        for(i = size - 1; i >= 0; i--) {
            CMN_ASSERT(nodes->nodeTab[i] != 0);
            if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
                xmlNodeSetContent(nodes->nodeTab[i], storage);
                CMN_LOG_CLASS_RUN_VERBOSE << "SetXMLValueXMLChar: write Xpath: " << XPath << " Node name: "
                                          << nodes->nodeTab[i]->name << " Content: " << storage << std::endl;
            }
            /*
              All the elements returned by an XPath query are pointers to
              elements from the tree *except* namespace nodes where the XPath
              semantic is different from the implementation in libxml2 tree.
              As a result when a returned node set is freed when
              xmlXPathFreeObject() is called, that routine must check the
              element type. But node from the returned set may have been removed
              by xmlNodeSetContent() resulting in access to freed data.
              This can be exercised by running
                valgrind xpath2 test3.xml '//discarded' discarded
            
              There is 2 ways around it:
                - make a copy of the pointers to the nodes from the result set
                  then call xmlXPathFreeObject() and then modify the nodes
                - remove the reference to the modified nodes from the node set
                  as they are processed, if they are not namespace nodes.
            */
            else if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            {
                nodes->nodeTab[i] = NULL;
            }
            else
            {
                cur = nodes->nodeTab[i];
                CMN_LOG_CLASS_RUN_WARNING << "SetXMLValueXMLChar: node is not attribute node \"" << XPath
                                          << "\" Node name: " << cur->name << std::endl;
            }
        }
        return true;
    }
    CMN_LOG_CLASS_RUN_WARNING << "SetXMLValueXMLChar: enable to match the location path \"" << XPath
                              << "\"" << std::endl;
    return false;
}


// -------------------- methods to set/get bool ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, bool & value)
{
    bool result = false;
    xmlChar * tmpStorage = 0;
    if (this->Data->GetXMLValueXMLChar(context, XPath, &tmpStorage) && tmpStorage != 0) {
        for (int i = 0;
             tmpStorage[i] != '\0';
             tmpStorage[i] = toupper(tmpStorage[i]), i++)
        {}
        if (strncmp((char*)tmpStorage, "FALSE", 5) == 0) {
            value = false;
            return true;
        } else if (strncmp((char*)tmpStorage, "TRUE", 4) == 0) {
            value = true;
            return true;
        } else {
            return false;
        }
    }
    return result;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, bool & value, const bool & valueIfMissing)
{
    bool ret_value;
    bool tmp_value;
    ret_value = GetXMLValue(context, XPath, tmp_value);
    if (ret_value) {
        value = tmp_value;
    } else {
        value = valueIfMissing;
    }
    return ret_value;
}


bool cmnXMLPath::SetXMLValue(const char * context, const char * XPath, const bool & value)
{
    bool result = false;
    std::string tmpStorage((value)?"true":"false");
    if (this->Data->SetXMLValueXMLChar(context, XPath, (xmlChar *)(tmpStorage.c_str()))) {
        return true;
    }
    return result;
}


// -------------------- methods to set/get int ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, int & value)
{
    xmlChar * tmpStorage = 0;
    if (this->Data->GetXMLValueXMLChar(context, XPath, &tmpStorage)  && tmpStorage != 0) {
        value = atoi((char*) tmpStorage);
        return true;
    }
    return false;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, int & value, const int & valueIfMissing)
{
    bool ret_value;
    int tmp_value;
    ret_value = GetXMLValue(context, XPath, tmp_value);
    if (ret_value) {
        value = tmp_value;
    } else {
        value = valueIfMissing;
    }
    return ret_value;
}


bool cmnXMLPath::SetXMLValue(const char * context, const char * XPath, const int & value)
{
    bool result = false;
    std::stringstream tmpStorage;
    // this way we can control the printing format.
    tmpStorage << cmnPrintf("%d") << value;
    if (this->Data->SetXMLValueXMLChar(context, XPath, (xmlChar *)tmpStorage.str().c_str())) {
        return true;
    }
    return result;
}


// -------------------- methods to set/get double ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, double & value)
{
    bool result = false;
    xmlChar * tmpStorage = 0;
    if (this->Data->GetXMLValueXMLChar(context, XPath, &tmpStorage) && tmpStorage != 0) {
        // special treatment for select floating points eps, -Inf & Inf
        for (int i = 0;
             tmpStorage[i] != '\0';
             tmpStorage[i] = toupper(tmpStorage[i]), i++)
        {}
        if ((strncmp((char*)tmpStorage, "INF", 3) == 0) || (strncmp((char*)tmpStorage, "1.#INF", 6) == 0)) {
            value = std::numeric_limits<double>::infinity();
        } else if ((strncmp((char*)tmpStorage, "-INF", 4) == 0) || (strncmp((char*)tmpStorage, "-1.#INF", 7) == 0)) {
            value = -1*std::numeric_limits<double>::infinity();
        } else if (strncmp((char*)tmpStorage, "EPS", 3) == 0) {
            value = std::numeric_limits<double>::epsilon();
        } else {
            value = atof((char*) tmpStorage);
        }
        return true;
    }
    return result;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, double & value, const double & valueIfMissing)
{
    bool ret_value;
    double tmp_value;
    ret_value = GetXMLValue(context, XPath, tmp_value);
    if (ret_value) {
        value = tmp_value;
    } else {
        value = valueIfMissing;
    }
    return ret_value;
}


bool cmnXMLPath::SetXMLValue(const char * context, const char * XPath, const double & value)
{
    bool result = false;
    std::stringstream tmpStorage;
    // this way we can control the printing size.
    if (fabs(value) < 1e-3) {
        tmpStorage << cmnPrintf("%g") << value;
    } else {
        tmpStorage << cmnPrintf("%f") << value;
    }
    if (this->Data->SetXMLValueXMLChar(context, XPath, (xmlChar *)tmpStorage.str().c_str())) {
        return true;
    }
    return result;
}


// -------------------- methods to set/get std::string ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, std::string & storage)
{
    xmlChar * tmpStorage = 0;
    if (this->Data->GetXMLValueXMLChar(context, XPath, &tmpStorage) && tmpStorage != 0) {
        storage = (tmpStorage)?(char*)tmpStorage:"";
        return true;
    }
    return false;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, std::string & value, const std::string & valueIfMissing)
{
    bool ret_value;
    std::string tmp_value;
    ret_value = GetXMLValue(context, XPath, tmp_value);
    if (ret_value) {
        value = tmp_value;
    } else {
        value = valueIfMissing;
    }
    return ret_value;
}


bool cmnXMLPath::SetXMLValue(const char * context, const char * XPath, const std::string & storage)
{
    xmlChar * tmpStorage = (xmlChar *)storage.c_str();
    if (this->Data->SetXMLValueXMLChar(context, XPath, tmpStorage)) {
        return true;
    }
    return false;
}
