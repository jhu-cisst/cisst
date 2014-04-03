# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#

# Author: Anton Deguet
# Date: 2010-01-20
#
# (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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
import cisstMultiTaskPython

class GenericObjectProxyTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""

    def tearDown(self):
        """Call after every test case."""
        
    def Test_mtsInt(self):
        """Test mtsInt"""
        variable = cisstMultiTaskPython.mtsInt(4)
        # check type
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsInt))
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsGenericObject))
        self.failUnless(isinstance(variable, cisstCommonPython.cmnGenericObject))
        # check default value and modify
        self.failUnless(variable.Data == 4)
        variable.Data = 8
        self.failUnless(variable.Data == 8)

    def Test_mtsDouble(self):
        """Test mtsDouble"""
        variable = cisstMultiTaskPython.mtsDouble(4.0)
        # check type
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsDouble))
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsGenericObject))
        self.failUnless(isinstance(variable, cisstCommonPython.cmnGenericObject))
        # check default value and modify
        self.failUnless(variable.Data == 4.0)
        variable.Data = 8.0
        self.failUnless(variable.Data == 8.0)

    def Test_mtsGenericObject(self):
        """Test mtsGenericObject methods"""
        variable = cisstMultiTaskPython.mtsInt(4)
        # check type
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsInt))
        self.failUnless(isinstance(variable, cisstMultiTaskPython.mtsGenericObject))
        # check default timestamp and modify
        timestamp = variable.GetTimestamp()
        self.failUnless(timestamp == 0.0)
        variable.SetTimestamp(10.0)
        timestamp = variable.GetTimestamp()
        self.failUnless(timestamp == 10.0)
        # check default valid flag and modify
        valid = variable.GetValid()
        self.failUnless(valid == True)
        variable.SetValid(False)
        valid = variable.GetValid()
        self.failUnless(valid == False)
        

