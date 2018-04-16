/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky
  Created on: 2006-02-15

  (C) Copyright 2006-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPrintf.h>
#include <cisstCommon/cmnLogger.h>
#include <stdio.h>
#include <string.h>

const char * cmnPrintfParser::TypeIdCharset = "cCdiouxXeDfgGnpsS%";
const char * cmnPrintfParser::IntegerTypeIds = "cCdiouxX";
const char * cmnPrintfParser::FloatTypeIds = "eEfgG";
const char * cmnPrintfParser::StringTypeIds = "sS";

#ifdef CISST_COMPILER_IS_MSVC
#define snprintf _snprintf
#endif

cmnPrintfParser::cmnPrintfParser(std::ostream & output, const cmnPrintf & outputFormatter):
    OutputStream(output),
    OutputFormatter(outputFormatter)
{
    NextFormatTextPosition = const_cast<char *>(OutputFormatter.GetFormat().c_str());
    NextFormatTextCharacter = *NextFormatTextPosition;
    FormatSequence = 0;
    NextTypeIdCharacter = 0;

    DumpUntilPercent();
    ProcessPercent();
}

void cmnPrintfParser::DumpUntilPercent(void)
{
    if (NextFormatTextPosition == 0)
        return;
    char * firstPosition = NextFormatTextPosition;
    *firstPosition = NextFormatTextCharacter;
    NextFormatTextPosition = strchr(firstPosition, '%');
    if (NextFormatTextPosition != 0) {
        *NextFormatTextPosition = 0;
        NextFormatTextCharacter = '%';
    }
    OutputStream << firstPosition;
}

void cmnPrintfParser::ProcessPercent(void)
{
    if (NextFormatTextPosition == 0)
        return;
    *NextFormatTextPosition = NextFormatTextCharacter;
    char * firstPosition = NextFormatTextPosition;
    NextFormatTextPosition = strpbrk(NextFormatTextPosition+1, TypeIdCharset);
    if (NextFormatTextPosition == 0) {
        return;
    }

    if (*NextFormatTextPosition == '%') {
        OutputStream << "%";
        ++NextFormatTextPosition;
        NextFormatTextCharacter = *NextFormatTextPosition;
        DumpUntilPercent();
        ProcessPercent();
        return;
    }

    NextTypeIdCharacter = *NextFormatTextPosition;
    ++NextFormatTextPosition;
    NextFormatTextCharacter = *NextFormatTextPosition;
    *NextFormatTextPosition = 0;
    FormatSequence = firstPosition;
    return;
}

bool cmnPrintfParser::NextTypeIdCharIsOneOf(const char * typeIdCharset) const
{
    if (strchr(typeIdCharset, NextTypeIdCharacter)) {
        return true;
    }
    return false;
}

template<class _intType>
bool cmnTypePrintfForIntegers(cmnPrintfParser & parser, const _intType number)
{
    if (parser.NextTypeIdCharIsOneOf(cmnPrintfParser::FloatTypeIds)) {
        return cmnTypePrintf(parser, static_cast<double>(number));
    }

    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::IntegerTypeIds)) {
        CMN_LOG_RUN_ERROR << "cmnTypePrintfForIntegers: expected type identified by <"
                          << parser.GetNextTypeIdCharacter()
                          << ">, received int instead.  Suspending output"
                          << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
                                     parser.GetNextFormatSequence(), number);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}


bool cmnTypePrintf(cmnPrintfParser & parser, const int number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const unsigned int number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const short number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const unsigned short number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const long number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const unsigned long number)
{
    return cmnTypePrintfForIntegers(parser, number);
}


bool cmnTypePrintf(cmnPrintfParser & parser, const double number)
{
    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::FloatTypeIds)) {
        CMN_LOG_RUN_ERROR << "cmnTypePrintf: expected type identified by <"
                          << parser.GetNextTypeIdCharacter()
                          << ">, received double instead.  Suspending output"
                          << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
                                     parser.GetNextFormatSequence(), number);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}


bool cmnTypePrintf(cmnPrintfParser & parser, const char * text)
{
    if (!parser.NextTypeIdCharIsOneOf(cmnPrintfParser::StringTypeIds)) {
        CMN_LOG_RUN_ERROR << "cmnTypePrintf: expected type identified by <"
                          << parser.GetNextTypeIdCharacter()
                          << ">, received char * instead.  Suspending output"
                          << std::endl;
        parser.SuspendOutput();
        return false;
    }

    char formatBuffer[cmnPrintfParser::BUFFER_SIZE + 1];
    formatBuffer[cmnPrintfParser::BUFFER_SIZE] = 0;
    const int printLength = snprintf(formatBuffer, cmnPrintfParser::BUFFER_SIZE,
                                     parser.GetNextFormatSequence(), text);
    parser.RawOutput(formatBuffer);

    if ( (printLength < 0) || (printLength > cmnPrintfParser::BUFFER_SIZE) ) {
        parser.RawOutput(" (truncated)");
    }

    return true;
}
