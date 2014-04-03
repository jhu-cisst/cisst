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
import numpy

import cisstVectorPython
from cisstVectorPython import vctFrm3

class FrameTest(unittest.TestCase):
    def setUp(self):
        """Call before every test case."""
        self.ZeroTranslation = numpy.zeros(3, numpy.float)
        self.IdentityRotation = numpy.zeros((3, 3), numpy.float)
        self.IdentityRotation[0, 0] = 1.0
        self.IdentityRotation[1, 1] = 1.0
        self.IdentityRotation[2, 2] = 1.0

    def tearDown(self):
        """Call after every test case."""
        
    def TestCreation(self):
        """Test default constructor."""
        frame = vctFrm3()
        translation = frame.Translation()
        translationIsZero = (self.ZeroTranslation == translation).all()
        self.failUnless(translationIsZero)
        rotation = frame.Rotation()
        rotationIsIdentity = (self.IdentityRotation == rotation).all()
        self.failUnless(rotationIsIdentity)

    def TestTranslationModification(self):
        """Test translation modification."""
        frame = vctFrm3()
        translation = frame.Translation()
        translation.fill(10.0)
        translationOtherRef = frame.Translation()
        translationsAreEqual = (translation == translationOtherRef).all()
        self.failUnless(translationsAreEqual)

    def TestRotationModification(self):
        """Test translation modification."""
        frame = vctFrm3()
        rotation = frame.Rotation()
        rotation[0, 0] = 0.0
        rotation[0, 1] = -1.0
        rotation[1, 0] = 1.0
        rotation[1, 1] = 0.0
        rotationOtherRef = frame.Rotation()
        rotationsAreEqual = (rotation == rotationOtherRef).all()
        self.failUnless(rotationsAreEqual)
        self.failUnless(numpy.linalg.det(rotation) == 1.0)
