/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "cdgData.h"


void cdgData::ParseFile(std::ifstream & input, const std::string & filename)
{
    std::stringstream content;
    size_t errorLine;
    std::string errorMessage;
    if (!this->StripComments(input, content, errorLine, errorMessage)) {
        std::cerr << filename << ":" << errorLine << ": error: " << errorMessage << std::endl;
        return;
    }

    cmnTokenizer tokenizer;
    std::string line;
    size_t lineNumber = 0;
    while (!content.eof()) {
        lineNumber++;
        getline(content, line);
        tokenizer.Parse(line);
        if (tokenizer.GetNumTokens() > 1) {
            const char * const * tokens = tokenizer.GetTokensArray();
            std::string keyword(*tokens);
            if (keyword == std::string("include")) {
                *tokens++;
                Includes.push_back(*tokens);
            } else if (keyword == std::string("typename")) {
                *tokens++;
                Name = *tokens;
            } else if (keyword == std::string("default-log-lod")) {
                *tokens++;
                DefaultLogLoD = *tokens;
            } else if (keyword == std::string("member")) {
                cdgDataMember dataMember;
                *tokens++;
                dataMember.Type = *tokens;
                *tokens++;
                dataMember.Name = *tokens;
                for (size_t index = 0; index < tokenizer.GetNumTokens() - 4; index++) {
                    *tokens++;
                    if (index == 0) {
                        dataMember.Description = *tokens;
                    } else {
                        dataMember.Description = dataMember.Description + "-" + *tokens;
                    }
                }
                DataMembers.push_back(dataMember);
            } else if (keyword == std::string("typedef")) {
                cdgTypedef typeDef;
                // pick last one as name, everything else is definition
                for (size_t index = 0; index < tokenizer.GetNumTokens() - 3; index++) {
                    *tokens++;
                    if (index == 0) {
                        typeDef.Definition = *tokens;
                    } else {
                        typeDef.Definition = typeDef.Definition + " " + *tokens;
                    }
                }
                *tokens++;
                typeDef.Name = *tokens;
                Typedefs.push_back(typeDef);
            } else {
                std::cerr << filename << ":" << lineNumber << ": error: Invalid keyword \"" << *tokens
                          << "\"" << std::endl;
            }
        }
    }
    // for each data member type, look if this is a typedef - could be much more efficient
    for (size_t memberIndex = 0; memberIndex < DataMembers.size(); memberIndex++) {
        for (size_t typedefIndex = 0; typedefIndex < Typedefs.size(); typedefIndex++) {
            if (DataMembers[memberIndex].Type == Typedefs[typedefIndex].Name) {
                DataMembers[memberIndex].UsesClassTypedef = true;
            }
        }
    }
}


bool cdgData::StripComments(std::istream & input, std::ostream & content,
                            size_t & errorLine, std::string & errorMessage)
{
    // this implementation is not the fastest possible but it should
    // be fine since it should be used on rather small files
    bool cCommentOpened = false;
    std::string line;
    size_t lineNumber = 0;
    size_t lineCommentOpened;
    size_t slashStarPos, starSlashPos, slashSlashPos;
    bool stripSlashSlash, foundWrongStarSlash;
    while (!input.eof()) {
        lineNumber++;
        getline(input, line);
        // if there is a C comment opened in previous line, try to find the end of it
        if (cCommentOpened) {
            // look for star slash comment close
            slashStarPos = line.find("*/"); 
            if (slashStarPos != std::string::npos) {
                line.replace(0, starSlashPos, starSlashPos + 1, ' '); // replace by n spaces
                cCommentOpened = false;
            } else {
                line.resize(0);
            }
        }

        // search for any form of comment in code
        do {
            // find first of slash star or slash slash comment
            stripSlashSlash = false;
            slashSlashPos = line.find("//");
            // if slash slash comes first, just remove the end of line
            if (slashSlashPos != std::string::npos) {
                slashStarPos = line.find("/*");
                if (slashStarPos != std::string::npos) {
                    if (slashSlashPos < slashStarPos) {
                        stripSlashSlash = true;
                    }
                } else {
                    stripSlashSlash = true;
                }
            }
            if (stripSlashSlash) {
                line.resize(slashSlashPos);
            }
            // now look for slash star and star slash
            slashStarPos = line.find("/*");
            starSlashPos = line.find("*/");
            foundWrongStarSlash = false;
            if (slashStarPos != std::string::npos) {
                if (starSlashPos != std::string::npos) {
                    if (starSlashPos < slashStarPos) {
                        foundWrongStarSlash = true;
                    } else {
                        line.replace(slashStarPos, starSlashPos - slashStarPos + 2, starSlashPos - slashStarPos + 2, ' ');
                    }
                } else {
                    cCommentOpened = true;
                    lineCommentOpened = lineNumber;
                    line.resize(slashStarPos);
                }
            } else {
                if (starSlashPos != std::string::npos) {
                    foundWrongStarSlash = true;
                }
            }
            if (foundWrongStarSlash) {
                errorLine = lineNumber;
                errorMessage = "Found comment closing \"*/\" before opening \"/*\"";
                return false;
            }
        } while ((slashStarPos != std::string::npos) &&
                 (slashSlashPos != std::string::npos) &&
                 !cCommentOpened);
        content << line << std::endl;
    }
    if (cCommentOpened) {
        errorLine = lineCommentOpened;
        errorMessage = "Comment opened with \"/*\" was never closed";
        return false;
    }
    return true;
}


void cdgData::GenerateHeader(std::ostream & output) const
{
    size_t index;
    output << "#ifndef _" << Name << "_h" << std::endl
           << "#define _" << Name << "_h" << std::endl
           << "#include <cisstMultiTask/mtsGenericObject.h>" << std::endl;

    for (index = 0; index < Includes.size(); index++) {
        output << "#include " << Includes[index] << std::endl;
    }
    output << "class " << Name << ": public mtsGenericObject" << std::endl
           << "{" << std::endl
           << "    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, " << DefaultLogLoD << ");" << std::endl;
    {
        output << " public:" << std::endl
               << "    typedef mtsGenericObject BaseType;" << std::endl;
        for (index = 0; index < Typedefs.size(); index++) {
            Typedefs[index].GenerateHeader(output);
        }

        for (index = 0; index < DataMembers.size(); index++) {
            DataMembers[index].GenerateHeaderAccessors(output);
        }

        GenerateStandardMethodsHeader(output);

        output << " protected:" << std::endl;
        for (index = 0; index < DataMembers.size(); index++) {
            DataMembers[index].GenerateHeaderDeclaration(output);
        }
    }
    output << "};" << std::endl
           << "CMN_DECLARE_SERVICES_INSTANTIATION(" << Name << ");" << std::endl
           << "#endif // _" << Name << "_h" << std::endl;
}


void cdgData::GenerateStandardMethodsHeader(std::ostream & output) const
{
        output << "    void SerializeRaw(std::ostream & outputStream) const;" << std::endl
               << "    void DeSerializeRaw(std::istream & inputStream);" << std::endl
               << "    void ToStream(std::ostream & outputStream) const;" << std::endl
               << "    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' '," << std::endl
               << "                     bool headerOnly = false, const std::string & headerPrefix = \"\") const;" << std::endl;
}


void cdgData::GenerateCode(std::ostream & output, const std::string & header) const
{
    size_t index;
    output << "#include <" << header << ">" << std::endl
           << "#include <cisstCommon/cmnSerializer.h>" << std::endl
           << "#include <cisstCommon/cmnDeSerializer.h>" << std::endl
           << "CMN_IMPLEMENT_SERVICES(" << Name << ");" << std::endl;
    GenerateSerializeRawCode(output);
    GenerateDeSerializeRawCode(output);
    GenerateToStreamCode(output);
    GenerateToStreamRawCode(output);

    for (index = 0; index < DataMembers.size(); index++) {
        DataMembers[index].GenerateCodeAccessors(output, Name);
    }
}


void cdgData::GenerateSerializeRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::SerializeRaw(std::ostream & outputStream) const" << std::endl
           << "{" << std::endl;
    for (index = 0; index < DataMembers.size(); index++) {
        output << "    cmnSerializeRaw(outputStream, this->" << DataMembers[index].Name << ");" << std::endl;
    }
    output << "}" << std::endl
           << std::endl;
}


void cdgData::GenerateDeSerializeRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::DeSerializeRaw(std::istream & inputStream)" << std::endl
           << "{" << std::endl;
    for (index = 0; index < DataMembers.size(); index++) {
        output << "    cmnDeSerializeRaw(inputStream, this->" << DataMembers[index].Name << ");" << std::endl;
    }
    output << "}" << std::endl
           << std::endl;
}


void cdgData::GenerateToStreamCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::ToStream(std::ostream & outputStream) const" << std::endl
           << "{" << std::endl
           << "    outputStream << \"" << Name << "\" << std::endl;" << std::endl
           << "    BaseType::ToStream(outputStream);" << std::endl
           << "    outputStream" << std::endl;
    for (index = 0; index < DataMembers.size(); index++) {
        output << "        << \"  " << DataMembers[index].Description << ":\" << this->" << DataMembers[index].Name;
        if (index == (DataMembers.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "}" << std::endl;
}


void cdgData::GenerateToStreamRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
           << "{" << std::endl
           << "    if (headerOnly) {" << std::endl
           << "        BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);" << std::endl
           << "        outputStream" << std::endl;
    for (index = 0; index < DataMembers.size(); index++) {
        output << "            << delimiter << headerPrefix << \"-" << DataMembers[index].Description << "\"";
        if (index == (DataMembers.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "    } else {" << std::endl
           << "        BaseType::ToStreamRaw(outputStream, delimiter);" << std::endl
           << "        outputStream" << std::endl;
    for (index = 0; index < DataMembers.size(); index++) {
        output << "            << delimiter << this->" << DataMembers[index].Name;
        if (index == (DataMembers.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "    }" << std::endl
           << "}" << std::endl;
}
