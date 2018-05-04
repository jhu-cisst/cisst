/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2003-06-09

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnTokenizer.h>
#include <string.h>

cmnTokenizer::cmnTokenizer()
    : Delimiters(DefaultDelimiters)
    , QuoteMarkers(DefaultQuoteMarkers)
    , EscapeMarkers(DefaultEscapeMarkers)
{}

cmnTokenizer::~cmnTokenizer()
{}

const char * const cmnTokenizer::DefaultDelimiters = " \t\n\012";
const char * const cmnTokenizer::DefaultEscapeMarkers = "\\";
const char * const cmnTokenizer::DefaultQuoteMarkers = "\"'";

void cmnTokenizer::Parse(const char * string) CISST_THROW(std::runtime_error)
{
    const size_t stringLength = strlen(string);
    StringBuffer.clear();
    StringBuffer.reserve(stringLength+1);
    StringBuffer.push_back(0);
    Tokens.clear();

    bool inToken = false;
    int lastQuote = 0;
    const char * currentCharPtr = string;
    const char * lastCharPtr = string + stringLength;

    for (; currentCharPtr < lastCharPtr; ++currentCharPtr) {
        int currentChar = *currentCharPtr;
        // skip all the delimiters outside of a token
        if (!inToken && (strchr(Delimiters, currentChar) != 0))
            continue;

        if (!inToken)
            inToken = true;

        // If encountered an escaped character, push the following character to the buffer,
        // and continue to the one after.  It follows that escaped characters are always
        // pushed.
        if (strchr(EscapeMarkers, currentChar) != 0) {
            ++currentCharPtr;
            currentChar = *currentCharPtr;
            StringBuffer.push_back(currentChar);
            continue;
        }

        // If encountered delimiter outside of quotes and not escaped, end the token and
        // switch the state of inToken.
        if ( (lastQuote == 0) && (strchr(Delimiters, currentChar) != 0) ) {
            inToken = false;
            StringBuffer.push_back(0);
            continue;
        }

        // If encountered quote mark outside of a quote, move to the following character
        // and store the quote mark.
        if ( (lastQuote == 0) && (strchr(QuoteMarkers, currentChar) != 0) ) {
            lastQuote = currentChar;
            continue;
        }

        // If encountered the opening quote mark, close the quote.  If the character immediately
        // following the quote is a delimiter, terminate the token.
        if (lastQuote == currentChar) {
            lastQuote = 0;
            if (strchr(Delimiters, *(currentCharPtr+1)) != 0)
                StringBuffer.push_back(0);
            inToken = false;
            continue;
        }

        // Otherwise, just push the next character to the buffer
        StringBuffer.push_back(currentChar);
    }
    StringBuffer.push_back(0);

    if (lastQuote != 0) {
        cmnThrow(std::runtime_error("mscTokenizer::Parse -- unclosed quotes"));
    }

    // Now after parsing the string, create a list of pointers to the tokens
    size_t bufferSize = StringBuffer.size();
    size_t index = 0;
    while (index < bufferSize) {
        if (StringBuffer[index] == 0) {
            ++index;
            continue;
        }

        Tokens.push_back(&(StringBuffer[index]));
        while (StringBuffer[index] != 0) {
            ++index;
        }
    }

    // Insert a null pointer at the end of the list.
    Tokens.push_back(NULL);
}


void cmnTokenizer::GetArgvTokens(std::vector<const char *> & argvTokens) const
{
    const size_type numTokens = GetNumTokens();
    argvTokens.resize(numTokens+1);
    argvTokens[0] = 0;
    std::copy( &(Tokens[0]), &(Tokens[0]) + numTokens, &(argvTokens[1]) );
}


const char * cmnTokenizer::GetToken(size_type num) const
{
    if (num < Tokens.size()) {
        return Tokens[num];
    } else {
        return 0;
    }
}


const char * const * cmnTokenizer::GetTokensArray(void) const
{
    if (Tokens.empty()) {
        return 0;
    }
    return &(Tokens[0]);
}

