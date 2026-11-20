/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-04-24
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnSearchPathTest.h"

#include <string>
using namespace std::string_literals;

void cmnSearchPathTest::TestAdd(void) {
    cmnSearchPath path1, path2, path3, path4;
    path1.Set("/val1"s);
    path1.Add("/val2"s, cmnSearchPath::Position::TAIL);
    path1.Add("/val3"s, cmnSearchPath::Position::TAIL);
    path2.Set("/val3"s);
    path2.Add("/val2"s, cmnSearchPath::Position::HEAD);
    path2.Add("/val1"s, cmnSearchPath::Position::HEAD);

    CPPUNIT_ASSERT(path1.ToString() == path2.ToString());
}


void cmnSearchPathTest::TestFind(void) {
    // CISST_SOURCE_DIR should not be used for regular code, only defined for test programs!
    cmnSearchPath path("/this/directory/is/not/valid"s);
    path.Add("/this/one/either/but/it/helps"s);
    path.Add(CISST_SOURCE_DIR"cisstCommon"s, cmnSearchPath::Position::TAIL);
    // test a file that should exist
    std::string fullName(CISST_SOURCE_DIR);
    fullName += "cisstCommon/cmnSearchPath.h";
    CPPUNIT_ASSERT(fullName == path.Find("cmnSearchPath.h"s));
    // test with a file that shouldn't exist
    std::string thisFileCantExist = "/aBadPath/QwErTy.hohoho";
    CPPUNIT_ASSERT("" == path.Find(thisFileCantExist));
}


void cmnSearchPathTest::TestRemove(void) {
    cmnSearchPath path;
    path.Add("/val1"s, cmnSearchPath::Position::TAIL);
    path.Add("/val2"s, cmnSearchPath::Position::TAIL);
    path.Add("/val3"s, cmnSearchPath::Position::TAIL);
    path.Add("/val1"s, cmnSearchPath::Position::TAIL);
    CPPUNIT_ASSERT(std::string("/val1;/val2;/val3;/val1") == path.ToString());
    CPPUNIT_ASSERT(path.Has("/val1"s) && path.Has("/val2"s) && path.Has("/val3"s));
    path.Remove("/val1"s);
    CPPUNIT_ASSERT(std::string("/val2;/val3;/val1") == path.ToString());
    CPPUNIT_ASSERT(path.Has("/val1"s) && path.Has("/val2"s) && path.Has("/val3"s));
    path.Remove("/val1"s);
    CPPUNIT_ASSERT(std::string("/val2;/val3") == path.ToString());
    CPPUNIT_ASSERT((!path.Has("/val1"s)) && path.Has("/val2"s) && path.Has("/val3"s));
}

