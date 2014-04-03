#!/usr/bin/python
# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#

#  Author(s): Anton Deguet
#  Created on: 2004-10-05

# (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

print ">>>> In python script"

objectRegister = cmnObjectRegister_Instance()

print objectRegister

derived1 = objectRegister.FindObject("derived1")
print "Class of derived1: ", derived1.__class__

rederived1 = objectRegister.FindObject("rederived1")
print "Class of rederived1: ", rederived1.__class__

print "Create and register derived2"
derived2 = myDerivedClass()
objectRegister.Register("derived2", derived2)

print "modifying derived1.FixedSizeVector()"
fixedVector1 = derived1.FixedSizeVector()
fixedVector1 / 100

print objectRegister

print "<<<< Out of python script"

