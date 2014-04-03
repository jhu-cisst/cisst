# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):	Chris Abidin, Andrew LaMora
#  Created on: 2004-04-30
#
#  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
#  Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# ireWorkspace.py
""" 
This module initializes the IRE workspace.

This module should not be directly loaded or called.
See ireMain.py
"""

import sys
import exceptions
import cPickle

class NoOutput:
	def write(self, String):
		pass

def IsPicklable(Object):
	try:
		cPickle.dump(Object, NoOutput())
	except exceptions.Exception, error: #cPickle.PicklingError, error:
		return False
	return True

#------------------------------------------------------
# SaveWorkspaceToFile
#
# Save workspace variables and statements to a file
#------------------------------------------------------
def SaveWorkspaceToFile(ObjectDictionary, File):
	Workspace = {}
	for ObjectName in ObjectDictionary:
		if IsPicklable(ObjectDictionary[ObjectName]):
			Workspace.update({ObjectName:ObjectDictionary[ObjectName]})
	cPickle.dump(Workspace,File)

#------------------------------------------------------
# LoadWorkspaceFile
#
# Load workspace variables and statements from a file
#------------------------------------------------------
def LoadWorkspaceFile(ObjectDictionary, File):
	Workspace = cPickle.load(File)
	for Variable in Workspace:
		ObjectDictionary[Variable] = Workspace[Variable]

#------------------------------------------------------
# main()
#
# Load the workspace, parse the serialized file for
# variables and load them into the variable list.

def main():
	v1 = 1234
	v2 = 'string'
	print 'v1 = ', v1, '\nv2 = ', v2
	SaveWorkspaceToFile(vars(), open('workspace','w'))
	v1 = v2 = None
	print 'v1 = ', v1, '\nv2 = ', v2
	LoadFile(open('workspace'))
	Workspace = cPickle.load(open('workspace'))
	for Variable in Workspace:
		exec(Variable + " = Workspace['" + Variable + "']")
	print 'v1 = ', v1, '\nv2 = ', v2

if __name__ == "__main__":
	sys.modules['ireWorkspace'] = sys.modules['__main__']
	main()
    
