# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author(s):  Anton Deguet
# Created on: 2018-05-10
#
# (C) Copyright 2018 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

import unittest

import cisstCommonPythonTestPython

class ThrowTest(unittest.TestCase):

    def setUp(self):
        pass

    def Test_cmnThrow(self):
        self.CObject = cisstCommonPythonTestPython.cmnThrowPythonTest()

        # test that we got a RuntimeError from SWIG
        exceptionOccurred = False
        exceptionWasRuntimeError = False
        try:
            self.CObject.ThisWillThrowRuntimeError()
        except (RuntimeError):
            exceptionOccurred = True
            exceptionWasRuntimeError = True
        assert(exceptionOccurred)
        assert(exceptionWasRuntimeError)

        # SWIG converts all error to RuntimeError
        exceptionOccurred = False
        exceptionWasRuntimeError = False
        try:
            self.CObject.ThisWillThrowLogicError()
        except (RuntimeError):
            exceptionOccurred = True
            exceptionWasRuntimeError = True
        except:
            print("other error?")
            exceptionOccurred = True
            exceptionWasRuntimeError = False
        assert(exceptionOccurred)
        assert(exceptionWasRuntimeError)

        # self.CObject.ThisWillThrowWithoutSpecification()
