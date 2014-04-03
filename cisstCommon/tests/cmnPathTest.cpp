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


#include "cmnPathTest.h"


void cmnPathTest::TestAdd(void) {
    cmnPath path1, path2, path3, path4;
    path1.Set("/val1");
    path1.Add("/val2", cmnPath::TAIL);
    path1.Add("/val3", cmnPath::TAIL);
    path2.Set("/val3");
    path2.Add("/val2", cmnPath::HEAD);
    path2.Add("/val1", cmnPath::HEAD);
    path3.Set("/val1");
    path3.Add("/val2;/val3", cmnPath::TAIL);
    path4.Set("/val3");
    path4.Add("/val1;/val2", cmnPath::HEAD);
    
    CPPUNIT_ASSERT(path1.ToString() == path2.ToString());
    CPPUNIT_ASSERT(path1.ToString() == path3.ToString());
    CPPUNIT_ASSERT(path1.ToString() == path4.ToString());
}


void cmnPathTest::TestFind(void) {
    // CISST_SOURCE_DIR should not be used for regular code, only defined for test programs!
    cmnPath path("/this/directory/is/not/valid;/this/one/either/but/it/helps");
    path.Add(CISST_SOURCE_DIR"cisstCommon", cmnPath::TAIL);
    // test a file that should exist
    std::string fullName(CISST_SOURCE_DIR);
    fullName += "cisstCommon/cmnPath.h";
    CPPUNIT_ASSERT(fullName == path.Find("cmnPath.h", cmnPath::READ));
    // test with a file that shouldn't exist
    std::string thisFileCantExist = "/aBadPath/QwErTy.hohoho";
    CPPUNIT_ASSERT("" == path.Find(thisFileCantExist, cmnPath::READ));
}


void cmnPathTest::TestRemove(void) {
    cmnPath path("/val1;/val2;/val3;/val1");
    CPPUNIT_ASSERT(std::string("/val1;/val2;/val3;/val1") == path.ToString());
    CPPUNIT_ASSERT(path.Has("/val1") && path.Has("/val2") && path.Has("/val3"));
    path.Remove("/val1");
    CPPUNIT_ASSERT(std::string("/val2;/val3;/val1") == path.ToString());
    CPPUNIT_ASSERT(path.Has("/val1") && path.Has("/val2") && path.Has("/val3"));
    path.Remove("/val1");
    CPPUNIT_ASSERT(std::string("/val2;/val3") == path.ToString());
    CPPUNIT_ASSERT((!path.Has("/val1")) && path.Has("/val2") && path.Has("/val3"));
}

