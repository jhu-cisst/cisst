# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: cmnClassRegisterTest.py 8 2009-01-04 21:13:48Z adeguet1 $
#

# Author: Anton Deguet
# Date: 2006-03-08
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

import unittest

import cisstCommonPython
from cisstCommonPython import cmnClassRegister


class ClassRegisterTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""
        self.classRegister = cisstCommonPython.cmnClassRegister_Instance()
        
    def tearDown(self):
        """Call after every test case."""
        
    def TestToString(self):
        """Test ToString."""
        content = self.classRegister.ToString()
        print "Class register: ", content

    def TestIterator(self):
        """Test iterator"""
        gotStopIteration = False
        found_cmnPath = False
        try:
            try:
                it = self.classRegister.__iter__()
                while True:
                    key, classInfo = it.next()
                    if classInfo.GetName() == "cmnPath":
                        found_cmnPath = True
            except StopIteration, e:
                gotStopIteration = True
        except:
            self.fail("Unexpected exception")
        self.failUnless(gotStopIteration)
        self.failUnless(found_cmnPath)

    def TestForLoop(self):
        """Test for loop"""
        found_cmnPath = False
        for key, classInfo in self.classRegister:
            if classInfo.GetName() == "cmnPath":
                found_cmnPath = True
        self.failUnless(found_cmnPath)

#
# $Log: cmnClassRegisterTest.py,v $
# Revision 1.5  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.4  2006/11/20 20:33:53  anton
# Licensing: Applied new license to tests.
#
# Revision 1.3  2006/05/10 18:40:25  anton
# cisstCommon class register: Updated to use a std::map to store class services
# and added std::type_info to the class services for a more reliable type
# identification of templated classes.
#
# Revision 1.2  2006/03/13 00:32:02  anton
# cisstCommon wrappers tests: Added "for loop" tests for objects and class
# registers.
#
# Revision 1.1  2006/03/10 14:37:27  anton
# cisstCommon wrapper tests: Added test for class and object registers.
#
#
