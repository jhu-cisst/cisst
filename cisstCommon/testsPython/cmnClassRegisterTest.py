# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author: Anton Deguet
# Date: 2006-03-08
#
# (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
        content = self.classRegister.__str__()
        print("Class register: ", content)

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
            except StopIteration:
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
