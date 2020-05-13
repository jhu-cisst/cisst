/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnStrings.h>
#include <algorithm>
#include <sstream>
#include <iterator>

void cmnStringReplaceAll(std::string & userString,
                         const std::string & oldString, const std::string & newString)
{
    // from http://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
    const size_t oldSize = oldString.length();

    // do nothing if line is shorter than the string to find
    if (oldSize > userString.length()) return;

    const size_t newSize = newString.length();
    for (size_t pos = 0; ; pos += newSize) {
        // Locate the substring to replace
        pos = userString.find(oldString, pos);
        if (pos == std::string::npos) {
            return;
        }
        if (oldSize == newSize) {
            // if they're same size, use std::string::replace
            userString.replace(pos, oldSize, newString);
        } else {
            // if not same size, replace by erasing and inserting
            userString.erase(pos, oldSize);
            userString.insert(pos, newString);
        }
    }
}


std::string cmnStringToUnderscoreLower(const std::string & input)
{
    typedef std::string StringType;
    StringType result(1, tolower(input[0]));
    const StringType::const_iterator end = input.end();

    // place underscores between contiguous lower and upper case letters.
    for (StringType::const_iterator iter = input.begin() + 1;
         iter != end;
         ++iter) {
        if (isupper(*iter) && *(iter-1) != '_' && islower(*(iter-1))) {
            result += "_";
        }
        result += *iter;
    }

    // then convert it to lower case.
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

std::string cmnStringFromVectorOfStrings(const std::vector<std::string> & input,
                                         const std::string & separator)
{
    std::stringstream tempStream;
    std::ostream_iterator<std::string> streamIterator(tempStream, separator.c_str());
    std::copy(input.begin(), input.end(), streamIterator);
    return tempStream.str();
}
