# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

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


