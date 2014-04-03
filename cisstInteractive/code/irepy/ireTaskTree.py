# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):	Peter Kazanzides
#  Created on: 2009-08-09
#
#  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

#ireTaskTree.py
"""
The IRE Task Tree

"""

#import wxPython GUI package
import wx

import wx.py as py

#create a class derived from wxFrame
class ireTaskTree( wx.Frame ):

	def __init__ (self, parent, id, title, taskManager):
	    #call the base class init method to create the frame
		wx.Frame.__init__(self, parent, id, title)

        self.taskManager = taskManager
        self.tree = wx.TreeCtrl(self)
        root = self.tree.AddRoot("TaskManager")
        self.AddTaskNodes(root, self.taskManager.GetNamesOfComponents(), self.taskManager.GetComponent)		
        self.tree.Expand(root)

    def AddTaskNodes(self, parentId, items, getter):
        for item in items:
            taskId = self.tree.AppendItem(parentId, item)
            task = getter(item)
            provId = self.tree.AppendItem(taskId, "Provides")
            self.AddProvInterfaceNodes(provId, task)
            reqId = self.tree.AppendItem(taskId, "Requires")
            self.AddReqInterfaceNodes(reqId, task)

    def AddProvInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfInterfacesProvided():
            provId = self.tree.AppendItem(parentId, item)
            provInterface = task.GetInterfaceProvided(item)
            cmdId = self.tree.AppendItem(provId, "CommandVoid")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsVoid())
            cmdId = self.tree.AppendItem(provId, "CommandWrite")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsWrite())
            cmdId = self.tree.AppendItem(provId, "CommandRead")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsRead())
            cmdId = self.tree.AppendItem(provId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsQualifiedRead())
            cmdId = self.tree.AppendItem(provId, "CommandVoidReturn")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsVoidReturn())
            cmdId = self.tree.AppendItem(provId, "CommandWriteReturn")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsWriteReturn())

    def AddReqInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfInterfacesRequired():
            reqId = self.tree.AppendItem(parentId, item)
            reqInterface = task.GetInterfaceRequired(item)
            cmdId = self.tree.AppendItem(reqId, "CommandVoid")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsVoid())
            cmdId = self.tree.AppendItem(reqId, "CommandWrite")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsWrite())
            cmdId = self.tree.AppendItem(reqId, "CommandRead")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsRead())
            cmdId = self.tree.AppendItem(reqId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsQualifiedRead())
            cmdId = self.tree.AppendItem(reqId, "CommandVoidReturn")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsVoidReturn())
            cmdId = self.tree.AppendItem(reqId, "CommandWriteReturn")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfFunctionsWriteReturn())

    def AddCommandNodes(self, parentId, cmdList):
        for cmd in cmdList:
            cmdId = self.tree.AppendItem(parentId, cmd)


import sys

try:
    from cisstMultiTaskPython import *
except Exception, e:
    print "Could not load cisstMultiTaskPython: ", e
try:
    from cisstParameterTypesPython import *
except Exception, e:
    print "Could not load cisstParameterTypesPython: ", e

class MyShell(py.shell.Shell):

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize, locals={}):
        py.editwindow.EditWindow.__init__(self, parent, id, pos, size)

        # Grab these so they can be restored by self.redirect* methods.
        self.stdin = sys.stdin
        self.stdout = sys.stdout
        self.stderr = sys.stderr

        # Create a replacement for stdin.
        self.reader = py.pseudo.PseudoFileIn(self.readline, self.readlines)
        self.reader.input = ''
        self.reader.isreading = False

        # Set up the interpreter.
        self.interp = py.interpreter.Interpreter(locals=locals,
                                  rawin=self.raw_input,
                                  stdin=self.reader,
                                  stdout=py.pseudo.PseudoFileOut(self.writeOut),
                                  stderr=py.pseudo.PseudoFileErr(self.writeErr))

        # Find out for which keycodes the interpreter will autocomplete.
        self.autoCompleteKeys = self.interp.getAutoCompleteKeys()

        # Keep track of the last non-continuation prompt positions.
        self.promptPosStart = 0
        self.promptPosEnd = 0

        # Keep track of multi-line commands.
        self.more = False

        self.noteMode = 0

        self.history = []
        self.historyIndex = -1

        # Assign handlers for keyboard events.
        self.Bind(wx.EVT_CHAR, self.OnChar)
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)

        # Assign handler for idle time.
        self.waiting = False
        self.Bind(wx.EVT_IDLE, self.OnIdle)


class ireSignalSelect( wx.Dialog ):

	def __init__ (self, parent, id, title, taskManager):
	    #call the base class init method to create the dialog
		wx.Dialog.__init__(self, parent, id, title)

        self.taskManager = taskManager
        self.suffix = ''

        # For older versions of wxWidgets, wx.ExpandAll causes an assertion failure if wx.TR_HIDE_ROOT is specified.
        # This was fixed in wxWidgets ticket #4287, but I don't know the corresponding wxWindows version number.
        # It is known to fail with wxWindows 2.8.7.1, so we at least check for that.
        if wx.VERSION < (2,8,7,2):
            self.tree = wx.TreeCtrl(self, style = wx.TR_DEFAULT_STYLE)
        else:
            self.tree = wx.TreeCtrl(self, style = wx.TR_DEFAULT_STYLE | wx.TR_HIDE_ROOT)
        root = self.tree.AddRoot("TaskManager")
        self.AddTaskNodes(root, self.taskManager.GetNamesOfComponents(), self.taskManager.GetComponent)		
        self.tree.ExpandAll()

        self.OKButton = wx.Button(self, wx.ID_OK)
        self.CancelButton = wx.Button(self, wx.ID_CANCEL)

        lay = wx.LayoutConstraints()
        lay.top.SameAs(self, wx.Top, 10)         # Top margin of 10
        lay.bottom.SameAs(self, wx.Bottom, 85)   # Bottom margin of 35, to leave room for a button
        lay.left.SameAs(self, wx.Left, 10)       # Left margin of 10
        lay.right.SameAs(self, wx.Right, 10)     # Right margin of 10
        self.tree.SetConstraints(lay)

        lay = wx.LayoutConstraints()
        lay.top.SameAs(self, wx.Bottom, -30)     # Position the button at the bottom of the Dialog.
        lay.left.SameAs(self, wx.Left, 10  )
        lay.height.AsIs()
        lay.width.AsIs()
        self.OKButton.SetConstraints(lay)

        lay = wx.LayoutConstraints()
        lay.top.SameAs(self, wx.Bottom, -30)     # Position the button at the bottom of the Dialog.
        lay.right.SameAs(self, wx.Right, 10  )
        lay.height.AsIs()
        lay.width.AsIs()
        self.CancelButton.SetConstraints(lay)
        self.Layout()
        self.SetAutoLayout(True)
        self.shell = MyShell(self, pos=(10, self.tree.Size[1]+15), size=(self.tree.Size[0],50))
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnTreeItemActivated)

    def AddTaskNodes(self, parentId, items, getter):
        for item in items:
            task = getter(item)
            if len(task.GetNamesOfInterfacesProvided()) > 0:
                taskId = self.tree.AppendItem(parentId, item)
                self.AddProvInterfaceNodes(taskId, task)

    def AddProvInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfInterfacesProvided():
            provId = self.tree.AppendItem(parentId, item)
            provInterface = task.GetInterfaceProvided(item)
            if len(provInterface.GetNamesOfCommandsRead()) > 0:
                self.AddCommandNodes(provId, provInterface, provInterface.GetNamesOfCommandsRead())

    def AddCommandNodes(self, parentId, interface, cmdList):
        for cmd in cmdList:
            cmdId = self.tree.AppendItem(parentId, cmd)
            self.tree.SetItemPyData(cmdId, interface.GetCommandRead(cmd))

    def OnTreeItemActivated(self, evt):
        command = self.tree.GetItemText(self.tree.GetSelection())
        tmp = self.tree.GetItemPyData(self.tree.GetSelection())
        tmp.UpdateFromC()
        self.shell.interp.locals[command] = tmp.ArgumentType
        self.shell.write(command)

    def GetSelection(self):
        command = self.tree.GetItemText(self.tree.GetSelection())
        suffix = ''
        # First, check the current line (if user has not pressed enter)
        line = self.shell.GetText()
        if line[0:len(command)] == command:
            suffix = line[len(command):]
        else:
            # Otherwise, look through command history.
            # It would make more sense to loop through history.reverse() and break
            # after the first match, but for some reason that doesn't work.
            for item in self.shell.history:
                if item[0:len(command)] == command:
                    suffix = item[len(command):]
        commandObject = self.tree.GetItemPyData(self.tree.GetSelection())
        if commandObject:
            y = commandObject()
            try:
                y = float(eval('y'+suffix))
            except TypeError:
                msgdlg = wx.MessageDialog(self, "Invalid Type", "Select Signal", wx.OK | wx.ICON_ERROR)
                msgdlg.ShowModal()
                msgdlg.Destroy()
                commandObject = None
        return (commandObject,suffix)
