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
            self.AddCommandNodes(cmdId, provInterface, provInterface.GetNamesOfCommandsVoid())
            cmdId = self.tree.AppendItem(provId, "CommandWrite")
            self.AddCommandNodes(cmdId, provInterface, provInterface.GetNamesOfCommandsWrite())
            cmdId = self.tree.AppendItem(provId, "CommandRead")
            self.AddCommandNodes(cmdId, provInterface, provInterface.GetNamesOfCommandsRead())
            cmdId = self.tree.AppendItem(provId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, provInterface, provInterface.GetNamesOfCommandsQualifiedRead())

    def AddReqInterfaceNodes(self, parentId, task):
        for item in task.GetNamesOfRequiredInterfaces():
            reqId = self.tree.AppendItem(parentId, item)
            reqInterface = task.GetRequiredInterface(item)
            cmdId = self.tree.AppendItem(reqId, "CommandVoid")
            self.AddCommandNodes(cmdId, reqInterface, reqInterface.GetNamesOfCommandPointersVoid())
            cmdId = self.tree.AppendItem(reqId, "CommandWrite")
            self.AddCommandNodes(cmdId, reqInterface, reqInterface.GetNamesOfCommandPointersWrite())
            cmdId = self.tree.AppendItem(reqId, "CommandRead")
            self.AddCommandNodes(cmdId, reqInterface, reqInterface.GetNamesOfCommandPointersRead())
            cmdId = self.tree.AppendItem(reqId, "CommandQualifiedRead")
            self.AddCommandNodes(cmdId, reqInterface, reqInterface.GetNamesOfCommandPointersQualifiedRead())

    def AddCommandNodes(self, parentId, provInterface, cmdList):
        for cmd in cmdList:
            cmdId = self.tree.AppendItem(parentId, cmd)





