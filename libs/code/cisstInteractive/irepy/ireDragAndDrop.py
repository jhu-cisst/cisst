# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id: ireDragAndDrop.py,v 1.8 2007/04/26 19:33:57 anton Exp $
#
#  Author(s):	Peter Kazanzides
#  Created on: 2005-12-31
#
#  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
#  Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

#  This file defines the classes that are needed for the drag and
#  drop implementation.  For now, there is just a single class that
#  is used by the shell to signify that it is a text drop target
#  (for commands dragged from the Command History list or variables
#   dragged from the Shell Variables list).
#
#  In the future, other drag and drop operations may be defined,
#  such as:
#
#    - dragging variables from the Shell Variables to the Register
#      Contents (this would add the Python variables to the
#      C++ object register).  See the AddVariablesToRegister function
#      currently in ireMain.py (not fully implemented).
#
#    - dragging entries from the Register Contents to the Shell
#      Variables or shell (this would fetch the C++ object from the
#      register and bind it to a Python object with the same name).
#      See the GetVariablesFromRegister function currently in
#      ireMain.py (not fully implemented).
#
#    - dragging Python source files to the shell (this would open
#      a new tab for editing the file).
#
# ireDragAndDrop.py

import wx

class ShellDropTarget(wx.TextDropTarget):
    dropHandler = None

    def __init__(self, window, handler):
        wx.TextDropTarget.__init__(self)
        self.window = window
        self.dropHandler = handler

    def OnDropText(self, x, y, text):
        self.dropHandler(text)
        return True


# $Log: ireDragAndDrop.py,v $
# Revision 1.8  2007/04/26 19:33:57  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.7  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.6  2006/01/02 04:17:44  pkaz
# IRE:  new (correct) implementation of drag and drop functionality.
# This check-in resolves item 6 in #174 (drag and drop on Linux).
#
# Revision 1.5  2005/12/29 16:35:38  pkaz
# ireDragAndDrop.py: documented current implementation of drag and drop.
# It is an unconventional implementation that does not seem to work on Linux and
# should be rewritten.
#
# Revision 1.4  2005/12/24 05:46:37  pkaz
# ireDragAndDrop.py: copied some code from ireDragAndDropMediator.py (which is being deleted). This code is probably not useful and is commented out for nw.
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
