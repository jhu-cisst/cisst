# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author: Anton Deguet
# Date: 2010-01-20
#
# (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

import unittest
import numpy
import time

import cisstCommonPython as cisstCommon
import cisstMultiTaskPython as cisstMultiTask
import cisstMultiTaskPythonTest # contains test classes

class PeriodicTaskTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""

    def tearDown(self):
        """Call after every test case."""

    def TestType(self):
        """Test constructor and types of mtsPeriodicTask"""
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(0.05)
        # check type
        self.assertTrue(isinstance(periodicTask, cisstMultiTaskPythonTest.mtsPeriodicTaskTest))
        self.assertTrue(isinstance(periodicTask, cisstMultiTask.mtsTaskPeriodic))
        self.assertTrue(isinstance(periodicTask, cisstMultiTask.mtsTaskContinuous))
        self.assertTrue(isinstance(periodicTask, cisstMultiTask.mtsComponent))
        self.assertTrue(isinstance(periodicTask, cisstCommon.cmnGenericObject))

    def TestUpdateFromC(self):
        """Test UpdateFromC for mtsPeriodicTask"""
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(0.05)
        periodicTask.UpdateFromC()
        # verify that both interfaces have been created
        self.assertTrue("MainInterface" in periodicTask.__dict__)
        self.assertTrue("EmptyInterface" in periodicTask.__dict__) # space should have been removed
        # test that MainInterface has been populated properly
        # command AddDouble(mtsDouble)
        self.assertTrue("AddDouble" in periodicTask.MainInterface.__dict__)
        self.assertTrue(isinstance(periodicTask.MainInterface.AddDouble, cisstMultiTask.mtsCommandWriteBase))
        proto = periodicTask.MainInterface.AddDouble.GetArgumentPrototype()
        self.assertTrue(isinstance(proto, cisstMultiTask.mtsDouble))
        # command ZeroAll(void)
        self.assertTrue("ZeroAll" in periodicTask.MainInterface.__dict__)
        self.assertTrue(isinstance(periodicTask.MainInterface.ZeroAll, cisstMultiTask.mtsCommandVoid))
        # command GetDouble(mtsDouble)
        self.assertTrue("GetDouble" in periodicTask.MainInterface.__dict__)
        self.assertTrue(isinstance(periodicTask.MainInterface.GetDouble, cisstMultiTask.mtsCommandRead))
        proto = periodicTask.MainInterface.GetDouble.GetArgumentPrototype()
        self.assertTrue(isinstance(proto, cisstMultiTask.mtsDouble))
        # command GetVector
        self.assertTrue("GetVector" in periodicTask.MainInterface.__dict__)
        self.assertTrue(isinstance(periodicTask.MainInterface.GetVector, cisstMultiTask.mtsCommandRead))

    def TestRun(self):
        """Test run mtsPeriodicTask"""
        # create the task
        period = 0.05
        periodicTask = cisstMultiTaskPythonTest.mtsPeriodicTaskTest(period)
        periodicTask.UpdateFromC()
        # get the task manager
        taskManager = cisstMultiTask.mtsManagerLocal.GetInstance()
        self.assertTrue(isinstance(taskManager, cisstMultiTask.mtsManagerLocal))
        # add the task
        self.assertTrue(taskManager.AddComponent(periodicTask))
        # Wait for asynchronous task initialization before starting it.
        taskManager.CreateAll()
        self.assertTrue(periodicTask.WaitToStart(10.0))
        taskManager.StartAll()

        def read_double_when(expected_value):
            deadline = time.monotonic() + 10.0
            last_result = None
            result = cisstMultiTask.mtsDouble()
            while time.monotonic() < deadline:
                last_result = periodicTask.MainInterface.GetDouble.Execute(result)
                if last_result.IsOK() and result.Data == expected_value:
                    return result
                time.sleep(min(period / 4.0, max(0.0, deadline - time.monotonic())))
            self.fail(f"GetDouble did not return {expected_value}; last result was {last_result}")

        # and now use the task ...
        # -1- set all to zero and check result
        periodicTask.MainInterface.ZeroAll()
        result1 = read_double_when(0.0)
        timestamp1 = result1.GetTimestamp()
        self.assertTrue(result1.Data == 0.0)
        resultVector = periodicTask.MainInterface.GetVector()
        data = resultVector.Data()
        size = data.size
        expected = numpy.zeros(size)
        self.assertTrue((data == expected).all())
        # -2- use the AddDouble to modify all and test
        periodicTask.MainInterface.AddDouble(1.0)
        result2 = read_double_when(1.0)
        timestamp2 = result2.GetTimestamp()
        self.assertTrue(result2.Data == 1.0)
        resultVector = periodicTask.MainInterface.GetVector()
        data = resultVector.Data()
        size = data.size
        expected = numpy.ones(size)
        self.assertTrue((data == expected).all())
        # -3- set to zero again and test
        periodicTask.MainInterface.ZeroAll()
        result3 = read_double_when(0.0)
        timestamp3 = result3.GetTimestamp()
        self.assertTrue(result3.Data == 0.0)
        resultVector = periodicTask.MainInterface.GetVector()
        data = resultVector.Data()
        size = data.size
        expected = numpy.zeros(size)
        self.assertTrue((data == expected).all())
        # ... and now check the timestamps
        self.assertTrue(timestamp1 < timestamp2)
        self.assertTrue(timestamp2 < timestamp3)
        # kill all
        taskManager.KillAll()
