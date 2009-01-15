# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id: ireWorkspace.py,v 1.5 2007/04/26 19:33:57 anton Exp $
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
	
# $Log: ireWorkspace.py,v $
# Revision 1.5  2007/04/26 19:33:57  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.4  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.3  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.2  2005/09/06 22:43:33  alamora
# irepy module: updated for cvs logging info, cisst license
#
# Revision 1.1  2005/09/06 22:30:03  alamora
# irepy module: revision 0.10 initial checkin
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
