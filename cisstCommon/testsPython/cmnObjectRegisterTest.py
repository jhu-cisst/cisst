# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author: Anton Deguet
# Date: 2006-03-08
#
# (C) Copyright 2006-2018 Johns Hopkins University (JHU), All Rights Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

import unittest

import cisstCommonPython
from cisstCommonPython import cmnObjectRegister
from cisstCommonPython import cmnGenericObject;
from cisstCommonPython import cmnPath;


class ObjectRegisterTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""
        self.objectRegister = cisstCommonPython.cmnObjectRegister_Instance()
        self.path1 = cmnPath()
        self.objectRegister.Register("path1", self.path1);
        self.path2 = cmnPath()
        self.objectRegister.Register("path2", self.path2);

    def tearDown(self):
        """Call after every test case."""

    def TestToString(self):
        """Test ToString."""
        content = self.objectRegister.__str__()
        print("Object register: ", content)

    def TestIterator(self):
        """Test iterator"""
        gotStopIteration = False
        path1found = False
        path2found = False
        try:
            try:
                it = self.objectRegister.__iter__()
                while True:
                    key, value = it.next()
                    if key == "path1":
                        path1found = True
                    if key == "path2":
                        path2found = True
            except StopIteration:
                gotStopIteration = True
        except:
            self.fail("Unexpected exception")
        self.failUnless(gotStopIteration)
        self.failUnless(path1found and path2found)

    def TestForLoop(self):
        """Test for loop"""
        path1found = False
        path2found = False
        for obj in self.objectRegister:
            key, value = obj
            if key == "path1":
                path1found = True
            if key == "path2":
                path2found = True
        self.failUnless(path1found and path2found)
