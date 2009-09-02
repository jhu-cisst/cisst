# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id$
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

#create a class derived from wxFrame
class ireTaskTree( wx.Frame ):

	def __init__ (self, parent, id, title, taskManager):
	    #call the base class init method to create the frame
		wx.Frame.__init__(self, parent, id, title)

        self.taskManager = taskManager
        self.tree = wx.TreeCtrl(self)
        root = self.tree.AddRoot("TaskManager")
        self.AddTaskNodes(root, self.taskManager.GetNamesOfDevices(), self.taskManager.GetDevice)		
        self.AddTaskNodes(root, self.taskManager.GetNamesOfTasks(), self.taskManager.GetTask)
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
        for item in task.GetNamesOfProvidedInterfaces():
            provId = self.tree.AppendItem(parentId, item)
            provInterface = task.GetProvidedInterface(item)
            cmdId = self.tree.AppendItem(provId, "CommandVoid")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsVoid())
            cmdId = self.tree.AppendItem(provId, "CommandWrite")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsWrite())
            cmdId = self.tree.AppendItem(provId, "CommandRead")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsRead())
            cmdId = self.tree.AppendItem(provId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, provInterface.GetNamesOfCommandsQualifiedRead())

    def AddReqInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfRequiredInterfaces():
            reqId = self.tree.AppendItem(parentId, item)
            reqInterface = task.GetRequiredInterface(item)
            cmdId = self.tree.AppendItem(reqId, "CommandVoid")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfCommandPointersVoid())
            cmdId = self.tree.AppendItem(reqId, "CommandWrite")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfCommandPointersWrite())
            cmdId = self.tree.AppendItem(reqId, "CommandRead")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfCommandPointersRead())
            cmdId = self.tree.AppendItem(reqId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, reqInterface.GetNamesOfCommandPointersQualifiedRead())

    def AddCommandNodes(self, parentId, cmdList):
        for cmd in cmdList:
            cmdId = self.tree.AppendItem(parentId, cmd)


class ireSignalSelect( wx.Dialog ):

	def __init__ (self, parent, id, title, taskManager):
	    #call the base class init method to create the dialog
		wx.Dialog.__init__(self, parent, id, title)

        self.taskManager = taskManager

        self.tree = wx.TreeCtrl(self)
        root = self.tree.AddRoot("TaskManager")
        self.AddTaskNodes(root, self.taskManager.GetNamesOfDevices(), self.taskManager.GetDevice)		
        self.AddTaskNodes(root, self.taskManager.GetNamesOfTasks(), self.taskManager.GetTask)
        self.tree.ExpandAll()

        self.OKButton = wx.Button(self, wx.ID_OK)
        self.CancelButton = wx.Button(self, wx.ID_CANCEL)

        lay = wx.LayoutConstraints()
        lay.top.SameAs(self, wx.Top, 10)         # Top margin of 10
        lay.bottom.SameAs(self, wx.Bottom, 35)   # Bottom margin of 35, to leave room for a button
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

    def AddTaskNodes(self, parentId, items, getter):
        for item in items:
            taskId = self.tree.AppendItem(parentId, item)
            task = getter(item)
            self.AddProvInterfaceNodes(taskId, task)

    def AddProvInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfProvidedInterfaces():
            provId = self.tree.AppendItem(parentId, item)
            provInterface = task.GetProvidedInterface(item)
            self.AddCommandNodes(provId, provInterface, provInterface.GetNamesOfCommandsRead())

    def AddCommandNodes(self, parentId, interface, cmdList):
        for cmd in cmdList:
            cmdId = self.tree.AppendItem(parentId, cmd)
            self.tree.SetItemPyData(cmdId, interface.GetCommandRead(cmd))

    def GetSelection(self):
        return self.tree.GetItemPyData(self.tree.GetSelection())
