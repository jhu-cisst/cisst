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
#include <cisstCommon/cmnPrintf.h>
#include <limits>

// required to register the class.  default level of detail is 1 as
// mentioned in header file
CMN_IMPLEMENT_SERVICES(cmnXMLPath);


cmnXMLPath::cmnXMLPath()
{}



cmnXMLPath::~cmnXMLPath()
{}



void cmnXMLPath::SetInputSource(const char *filename)
{
	/* Load XML document */
    Document = xmlParseFile(filename);
    if (Document == 0) {
        CMN_LOG_CLASS(1) << "An error occured while parsing \"" << filename << "\"" << std::endl;
    }
    CMN_ASSERT(Document != 0);

	XPathContext = xmlXPathNewContext(Document);
	if(XPathContext == NULL) {
		CMN_LOG_CLASS(1) << "An error occured while parsing \"" << filename << "\"" << std::endl;
		xmlFreeDoc(Document); 
	}
	CMN_ASSERT(XPathContext != 0);
}


void cmnXMLPath::PrintValue(std::ostream &out, const char *context, const char *XPath)
{
	std::string str;
    if (GetXMLValue(context, XPath, str) == false) {
        CMN_LOG_CLASS(2) << "Warning -- No nodes matched the location path \"" << XPath
                         << "\"" << std::endl;
    } else {
        out << str << std::endl;
    }
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, xmlChar **storage)
{
	/* Evaluate xpath expression */
	/* first we need to concat the context and Xpath to fit libxml standard. context is fixed at 
	document root in libxml2 */
	std::string xpathlibxml("/");
	xpathlibxml += context;
	xpathlibxml += "/";
	xpathlibxml += XPath;
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar *)(xpathlibxml.c_str()), XPathContext);
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
				CMN_LOG(5) << "Read Xpath: " << XPath << " Node name: " 
					<< cur->name << " Content: " << *storage << std::endl;
			} else {
				cur = nodes->nodeTab[i];    
				CMN_LOG_CLASS(2) << "Warning -- Node is not attribute node \"" << XPath
					<< "\" Node name: " << cur->name << std::endl;
			}
		}
		return true;
	}
	CMN_LOG_CLASS(2) << "Warning -- Unable to match the location path \"" << XPath
                         << "\"" << std::endl;
	return false;
}


bool cmnXMLPath::SetXMLValue(const char * context, const char * XPath, const xmlChar *storage)
{
	/* Evaluate xpath expression */
	/* first we need to concat the context and Xpath to fit libxml standard. context is fixed at 
	document root in libxml2 */
	std::string xpathlibxml("/");
	xpathlibxml += context;
	xpathlibxml += "/";
	xpathlibxml += XPath;
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar *)(xpathlibxml.c_str()), XPathContext);
	if(xpathObj != NULL) {
		xmlNodePtr cur;
		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		int size = (nodes)? nodes->nodeNr: 0;
		int i;

		/*
		* NOTE: the nodes are processed in reverse order, i.e. reverse document
		*       order because xmlNodeSetContent can actually free up descendant
		*       of the node and such nodes may have been selected too ! Handling
		*       in reverse order ensure that descendant are accessed first, before
		*       they get removed. Mixing XPath and modifications on a tree must be
		*       done carefully !
		*/
		for(i = size - 1; i >= 0; i--) {
			CMN_ASSERT(nodes->nodeTab[i] != 0);
			if (nodes->nodeTab[i]->type == XML_ATTRIBUTE_NODE) {
				xmlNodeSetContent(nodes->nodeTab[i], storage);
				CMN_LOG(5) << "Write Xpath: " << XPath << " Node name: " 
					<< nodes->nodeTab[i]->name << " Content: " << storage << std::endl;
			}
			/*
			* All the elements returned by an XPath query are pointers to
			* elements from the tree *except* namespace nodes where the XPath
			* semantic is different from the implementation in libxml2 tree.
			* As a result when a returned node set is freed when
			* xmlXPathFreeObject() is called, that routine must check the
			* element type. But node from the returned set may have been removed
			* by xmlNodeSetContent() resulting in access to freed data.
			* This can be exercised by running
			*       valgrind xpath2 test3.xml '//discarded' discarded
			* There is 2 ways around it:
			*   - make a copy of the pointers to the nodes from the result set 
			*     then call xmlXPathFreeObject() and then modify the nodes
			* or
			*   - remove the reference to the modified nodes from the node set
			*     as they are processed, if they are not namespace nodes.
			*/
			else if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
			{
				nodes->nodeTab[i] = NULL;
			}
			else
			{
				cur = nodes->nodeTab[i];    
				CMN_LOG_CLASS(2) << "Warning -- Node is not attribute node \"" << XPath
					<< "\" Node name: " << cur->name << std::endl;
			}
		}
		return true;
	}
	CMN_LOG_CLASS(2) << "Warning -- Unable to match the location path \"" << XPath
		<< "\"" << std::endl;
	return false;
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, std::string &storage)
{
	xmlChar* tmpStorage = 0;
	if (GetXMLValue(context, XPath, &tmpStorage) && tmpStorage != 0) {
		storage = (tmpStorage)?(char*)tmpStorage:"";
		return true;
	}
	return false;
}


bool cmnXMLPath::SetXMLValue(const char *context, const char *XPath, const std::string &storage)
{
	xmlChar* tmpStorage = (xmlChar*)storage.c_str();
	if (SetXMLValue(context, XPath, tmpStorage)) {
		return true;
	}
	return false;
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, double &value)
{
	bool result = false;
	xmlChar* tmpStorage = 0;
	if (GetXMLValue(context, XPath, &tmpStorage) && tmpStorage != 0) {
		// special treatment for select floating points eps, -Inf & Inf
		for (int i = 0; tmpStorage[i] != '\0'; tmpStorage[i]=toupper(tmpStorage[i]), i++);
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


bool cmnXMLPath::SetXMLValue(const char *context, const char *XPath, double value)
{
	bool result = false;
	std::stringstream tmpStorage;
	// this way we can control the printing size.
	if (fabs(value) < 1e-3) {
		tmpStorage << cmnPrintf("%g") << value;
	} else {
		tmpStorage << cmnPrintf("%f") << value;
	}
	if (SetXMLValue(context, XPath, (xmlChar*)tmpStorage.str().c_str())) {
		return true;
	}
    return result;
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, int &value)
{
	xmlChar* tmpStorage = 0;
	if (GetXMLValue(context, XPath, &tmpStorage)  && tmpStorage != 0) {
		value = atoi((char*) tmpStorage);
		return true;
	}
    return false;
}


bool cmnXMLPath::SetXMLValue(const char *context, const char *XPath, int value)
{
	bool result = false;
	std::stringstream tmpStorage;
	// this way we can control the printing format.
	tmpStorage << cmnPrintf("%d") << value;
	if (SetXMLValue(context, XPath,		(xmlChar*)tmpStorage.str().c_str())) {
		return true;
	}
    return result;
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, bool &value)
{
    bool result = false;
	xmlChar* tmpStorage = 0;
	if (GetXMLValue(context, XPath, &tmpStorage) && tmpStorage != 0) {
		for (int i = 0; tmpStorage[i] != '\0'; tmpStorage[i]=toupper(tmpStorage[i]), i++);
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


bool cmnXMLPath::SetXMLValue(const char *context, const char *XPath, bool value)
{
	bool result = false;
	std::string tmpStorage((value)?"true":"false");
	if (SetXMLValue(context, XPath, (xmlChar*)(tmpStorage.c_str()))) {
		return true;
	}
	return result;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, bool &value, const bool &valueifmissing)
{
	bool ret_value; 
	bool tmp_value;
	ret_value = GetXMLValue(context, XPath, tmp_value);
	if (ret_value) value = tmp_value;
	else value = valueifmissing;
	return ret_value;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, int &value, const int &valueifmissing)
{
	bool ret_value; 
	int tmp_value;
	ret_value = GetXMLValue(context, XPath, tmp_value);
	if (ret_value) value = tmp_value;
	else value = valueifmissing;
	return ret_value;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, double &value, const double &valueifmissing)
{
	bool ret_value; 
	double tmp_value;
	ret_value = GetXMLValue(context, XPath, tmp_value);
	if (ret_value) value = tmp_value;
	else value = valueifmissing;
	return ret_value;
}


bool cmnXMLPath::GetXMLValue(const char * context, const char * XPath, std::string &value, const std::string &valueifmissing)
{
	bool ret_value; 
	std::string tmp_value;
	ret_value = GetXMLValue(context, XPath, tmp_value);
	if (ret_value) value = tmp_value;
	else value = valueifmissing;
	return ret_value;
}

