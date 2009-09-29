#!/usr/bin/python
# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: pythonEmbedded.py 75 2009-02-24 16:47:20Z adeguet1 $
#

#  Author(s): Anton Deguet
#  Created on: 2004-10-05

# (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

print ">>>> In python script"

print cmnObjectRegister.ToString()

derived1 = cmnObjectRegister.FindObject("derived1")
print "Class of derived1: ", derived1.__class__

rederived1 = cmnObjectRegister.FindObject("rederived1")
print "Class of rederived1: ", rederived1.__class__

print "Create and register derived2"
derived2 = myDerivedClass()
cmnObjectRegister.Register("derived2", derived2)

print "modifying derived1.FixedVector()"
fixedVector1 = derived1.FixedVector()
fixedVector1.Divide(derived1.FixedVector().Norm())

print cmnObjectRegister.ToString()

print "<<<< Out of python script"

