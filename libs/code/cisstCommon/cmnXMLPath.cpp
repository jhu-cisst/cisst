/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
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

#if CISST_HAS_QT
#define CISST_HAS_QT_XML 0 // not ready yet
#else
#define CISST_HAS_QT_XML 0
#endif

#if CISST_HAS_QT_XML

#include <QFile>
#include <QDomDocument>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

class cmnXMLPathData
{
public:
    // default ctor
    cmnXMLPathData(void):
        File(0),
        Document(0)
    {}

    // default dtor
    ~cmnXMLPathData()
    {
        if (this->Document) {
            delete this->Document;
        }
        if (this->File) {
            this->File->close();
            delete this->File;
        }
    }

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

    // File used by Qt
    QFile * File;

    // Dom Document
    QDomDocument * Document;

    // set input
    void SetInputSource(const char * filename)
    {
        std::string docName = std::string("cmnXMLPath") + filename;
        this->Document = new QDomDocument(docName.c_str());
        this->File = new QFile(filename);
        if (!this->File->open(QIODevice::ReadOnly | QIODevice::Text)) {
            CMN_LOG_CLASS_INIT_ERROR << "SetInputSource (Qt): an error occured while opening \"" << filename << "\"" << std::endl;
            return;
        }
        if (!this->Document->setContent(this->File)) {
            CMN_LOG_CLASS_INIT_ERROR << "SetInputSource (Qt): an error occured while loading \"" << filename << "\"" << std::endl;
            this->File->close();
            return;
        }
    }

    bool ValidateWithSchema(const char * filename)
    {
        QXmlSchema schema;
        if (!schema.load(QUrl(filename))) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (Qt): the schema cannot be loaded \""
                                     << filename << "\"" << std::endl;
        }
        if (!schema.isValid()) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (Qt): the schema itself is not valid \""
                                     << filename << "\"" << std::endl;
            return false;
        }
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(this->Document->toByteArray())) {
            return false;
        }
        return true;
    }

    bool SaveAs(const char * filename) const
    {
        std::ofstream file(filename);
        if (!file) {
            CMN_LOG_CLASS_INIT_ERROR << "SaveAs (Qt): failed to open file \"" << filename << "\"" << std::endl;
            return false;
        }
        file << this->Document->toString().toStdString();
        file.close();
        return true;
    }

    // generic string get
    bool GetXMLValueStdString(const char * context, const char * XPath, std::string & storage)
    {
        QXmlQuery query;
        QXmlResultItems results;
        query.setFocus(this->Document->toString());
        QString path = "data(";
        path += context;
        path += "/";
        path += XPath;
        path += ")";
        query.setQuery(path);
        if (!query.isValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "GetXMLValueStdString (Qt): invalid query for path [" << path.toStdString() << "]" << std::endl;
            std::cerr << "1" << std::endl;
            return false;
        }
        query.evaluateTo(&results);
        if (results.hasError()) {
            CMN_LOG_CLASS_RUN_ERROR << "GetXMLValueStdString (Qt): query result has errors for path [" << path.toStdString() << "]" << std::endl;
            std::cerr << "2" << std::endl;
            return false;
        }
        bool attributeFound = false;
        for (QXmlItem result = results.next(); !result.isNull(); result = results.next()) {
            storage = result.toAtomicValue().toString().toStdString();
            attributeFound = true;
            CMN_LOG_CLASS_RUN_VERBOSE << "GetXMLValueStdString (Qt): read path [" << path.toStdString()
                                      << "]  Content [" << storage << "]" << std::endl;
        }
        return attributeFound;
    }

    bool SetXMLValueStdString(const char * context, const char * XPath, const std::string & storage)
    {
        return true;
    }
};


#else

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlschemas.h>

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

    // set input
    void SetInputSource(const char * filename)
    {
        this->Document = xmlParseFile(filename);
        if (this->Document == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "SetInputSource (libxml2): an error occured while parsing \"" << filename << "\"" << std::endl;
        }
        CMN_ASSERT(this->Document != 0);

        this->XPathContext = xmlXPathNewContext(this->Document);
        if (this->XPathContext == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "SetInputSource (libxml2): an error occured while parsing \"" << filename << "\"" << std::endl;
            xmlFreeDoc(this->Document);
        }
        CMN_ASSERT(this->XPathContext != 0);
    }

    bool ValidateWithSchema(const char * filename)
    {
        xmlDocPtr schema_doc = xmlReadFile(filename, NULL, XML_PARSE_NONET);
        if (schema_doc == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (libxml2): the schema cannot be loaded or is not well-formed \""
                                     << filename << "\"" << std::endl;
            return false;
        }
        xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
        if (parser_ctxt == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (libxml2): unable to create a parser context for the schema \""
                                     << filename << "\"" << std::endl;
            xmlFreeDoc(schema_doc);
            return false;
        }
        xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
        if (schema == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (libxml2): the schema itself is not valid \""
                                     << filename << "\"" << std::endl;
            xmlSchemaFreeParserCtxt(parser_ctxt);
            xmlFreeDoc(schema_doc);
            return false;
        }
        xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
        if (valid_ctxt == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "ValidateWithSchema (libxml2): unable to create a validation context for the schema \""
                                     << filename << "\"" << std::endl;
            xmlSchemaFree(schema);
            xmlSchemaFreeParserCtxt(parser_ctxt);
            xmlFreeDoc(schema_doc);
            return false;
        }
        xmlSchemaSetValidErrors(valid_ctxt, (xmlValidityErrorFunc)fprintf, (xmlValidityWarningFunc)fprintf, stderr);
        int is_valid = (xmlSchemaValidateDoc(valid_ctxt, this->Document) == 0);
        xmlSchemaFreeValidCtxt(valid_ctxt);
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        xmlFreeDoc(schema_doc);
        return is_valid ? true : false;
    }

    bool SaveAs(const char * filename) const
    {
        if (xmlSaveFile(filename, this->Document) <= 0) {
            CMN_LOG_CLASS_INIT_ERROR << "SaveAs (libxml2): failed to open file \"" << filename << "\"" << std::endl;
            return false;
        }
        return true;
    }

    // generic string get
    bool GetXMLValueStdString(const char * context, const char * XPath, std::string & storage)
    {
        /* Evaluate xpath expression */
        /* first we need to concat the context and Xpath to fit libxml standard. context is fixed at
           document root in libxml2 */
        std::string xpathlibxml("/");
        xpathlibxml += context;
        xpathlibxml += "/";
        xpathlibxml += XPath;
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(reinterpret_cast<const xmlChar *>(xpathlibxml.c_str()),
                                                            this->XPathContext);
        bool attributeFound = false;
        if (xpathObj != 0) {
            xmlNodePtr currentNode;
            xmlNodeSetPtr nodes = xpathObj->nodesetval;
            unsigned int size = (nodes)? nodes->nodeNr: 0;
            unsigned int i;
            for (i = 0; i < size; ++i) {
                CMN_ASSERT(nodes->nodeTab[i] != 0);
                if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
                    currentNode = nodes->nodeTab[i];
                    storage = reinterpret_cast<char *>(xmlNodeGetContent(currentNode));
                    attributeFound = true;
                    CMN_LOG_CLASS_RUN_VERBOSE << "GetXMLValueStdString (libxml2): read Xpath [" << XPath << "] Node name ["
                                              << currentNode->name << "] Content [" << storage << "]" << std::endl;
                } else {
                    currentNode = nodes->nodeTab[i];
                    CMN_LOG_CLASS_RUN_WARNING << "GetXMLValueStdString (libxml2): node is not attribute node [" << XPath
                                              << "] Node name [" << currentNode->name << "]" << std::endl;
                }
            }
        }
        if (!attributeFound) {
            CMN_LOG_CLASS_RUN_WARNING << "GetXMLValueStdString (libxml2): unable to match the location path [" << XPath
                                      << "] in context [" << context << "]" << std::endl;
            return false;
        }
        return true;
    }

    // set value
    bool SetXMLValueStdString(const char * context, const char * XPath, const std::string & storage)
    {
        /* Evaluate xpath expression */
        /* first we need to concat the context and Xpath to fit libxml standard. context is fixed at
           document root in libxml2 */
        std::string xpathlibxml("/");
        xpathlibxml += context;
        xpathlibxml += "/";
        xpathlibxml += XPath;
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(reinterpret_cast<const xmlChar *>(xpathlibxml.c_str()),
                                                            this->XPathContext);
        if (xpathObj != 0) {
            xmlNodePtr currentNode;
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
            for (i = size - 1; i >= 0; i--) {
                CMN_ASSERT(nodes->nodeTab[i] != 0);
                if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
                    xmlNodeSetContent(nodes->nodeTab[i], reinterpret_cast<const xmlChar *>(storage.c_str()));
                    CMN_LOG_CLASS_RUN_VERBOSE << "SetXMLValueStdString: write Xpath [" << XPath << "] Node name ["
                                              << nodes->nodeTab[i]->name << "] Content [" << storage << "]" << std::endl;
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
                else if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL) {
                    nodes->nodeTab[i] = 0;
                } else {
                    currentNode = nodes->nodeTab[i];
                    CMN_LOG_CLASS_RUN_WARNING << "SetXMLValueStdString (libxml2): node is not attribute node [" << XPath
                                              << "] Node name [" << currentNode->name << "]" << std::endl;
                }
            }
            return true;
        }
        CMN_LOG_CLASS_RUN_WARNING << "SetXMLValueStdString (libxml2): enable to match the location path \[" << XPath
                                  << "]" << std::endl;
        return false;
    }
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
    this->Data->SetInputSource(filename);
}


void cmnXMLPath::SetInputSource(const std::string & fileName)
{
    this->SetInputSource(fileName.c_str());
}


bool cmnXMLPath::ValidateWithSchema(const char * filename)
{
    return this->Data->ValidateWithSchema(filename);
}


bool cmnXMLPath::ValidateWithSchema(const std::string & fileName)
{
    return this->ValidateWithSchema(fileName.c_str());
}


bool cmnXMLPath::SaveAs(const char * filename) const
{
    return this->Data->SaveAs(filename);
}


bool cmnXMLPath::SaveAs(const std::string & filename) const
{
    return this->SaveAs(filename.c_str());
}


void cmnXMLPath::PrintValue(std::ostream & outputStream, const char * context, const char * XPath)
{
    std::string str;
    if (GetXMLValue(context, XPath, str) == false) {
        CMN_LOG_CLASS_RUN_WARNING << "PrintValue: no nodes matched the location path [" << XPath
                                  << "]" << std::endl;
    } else {
        outputStream << str << std::endl;
    }
}


// -------------------- methods to set/get bool ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, bool & value)
{
    std::string storage;
    if (!this->Data->GetXMLValueStdString(context, XPath, storage)) {
        return false;
    }
    std::transform(storage.begin(), storage.end(), storage.begin(), ::toupper);
    if (storage == "FALSE") {
        value = false;
        return true;
    } else if (storage == "TRUE") {
        value = true;
        return true;
    } else {
        return false;
    }
    return false;
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
    std::string storage((value)?"true":"false");
    return this->Data->SetXMLValueStdString(context, XPath, storage);

}


// -------------------- methods to set/get int ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, int & value)
{
    std::string storage;
    if (this->Data->GetXMLValueStdString(context, XPath, storage)) {
        value = atoi(storage.c_str());
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
    std::stringstream storage;
    // this way we can control the printing format.
    storage << cmnPrintf("%d") << value;
    return this->Data->SetXMLValueStdString(context, XPath, storage.str());
}


// -------------------- methods to set/get double ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, double & value)
{
    std::string storage;
    if (!this->Data->GetXMLValueStdString(context, XPath, storage)) {
        return false;
    }

    // special treatment for select floating points eps, -Inf & Inf
    std::transform(storage.begin(), storage.end(), storage.begin(), ::toupper);
    if ((storage == "INF") || (storage == "1.#INF")) {
        value = std::numeric_limits<double>::infinity();
    } else if ((storage == "-INF") || (storage == "1.#INF")) {
        value = -1*std::numeric_limits<double>::infinity();
    } else if (storage == "EPS") {
        value = std::numeric_limits<double>::epsilon();
    } else {
        value = atof(storage.c_str());
    }
    return true;
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
    std::stringstream storage;
    // this way we can control the printing size.
    if (fabs(value) < 1e-3) {
        storage << cmnPrintf("%g") << value;
    } else {
        storage << cmnPrintf("%f") << value;
    }
    return this->Data->SetXMLValueStdString(context, XPath, storage.str());
}


// -------------------- methods to set/get std::string ---------------------
bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, std::string & storage)
{
    storage = "";
    return this->Data->GetXMLValueStdString(context, XPath, storage);
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
    return this->Data->SetXMLValueStdString(context, XPath, storage);
}
