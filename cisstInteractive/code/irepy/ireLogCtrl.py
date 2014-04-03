# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):	Peter Kazanzides
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

#
#  The ireLogCtrl class defines the panel used for the logger.
#  Currently, it contains a header and a text control (wx.TextCtrl)
#  for the log output.  The header includes a checkbox to enable/disable
#  the logger and an IntCtrl box to allow the user to change the
#  channel log mask.
#
#  Currently, if the IRE is not embedded (i.e., not called from a
#  C++ program), the logger functionality is disabled because the
#  ireLogger module is not available.
#
#  Future enhancements could include:
#    - logging support for cases where the IRE is not embedded (need
#      to create an importable Python module for ireLogger).
#    - the ability to change the level of detail on a class-by-class
#      basis (via the Class Registry).
#    - the ability to change the global level of detail.
#


""" ireLogCtrl

"""

from cisstCommonPython import *

import wx
import wx.lib.intctrl as intctrl

import wx.lib.newevent
# This creates a new Event class and a EVT binder function
(UpdateLogEvent, EVT_UPDATE_LOG) = wx.lib.newevent.NewEvent()

# See if the IRE is embedded in a C++ application.
# This can be determined by checking whether the ireLogger
# module can be imported, because ireLogger is created
# by the cisstInteractive (C++) library.
ireEmbedded = True
try:
    import ireLogger
    from ireLogCtrl import *
except ImportError,e:
    ireEmbedded = False


class ireLogCtrl(wx.Panel):
	
	def __init__(self, parent, id, title='',
                 style=wx.TE_READONLY | wx.HSCROLL | wx.TE_MULTILINE | wx.TE_RICH):
		wx.Panel.__init__(self, parent, id)

        self.SetBackgroundColour(wx.BLUE)

        Font = wx.Font(10, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.Text = wx.StaticText(self, -1, title, style=wx.ALIGN_LEFT)
        self.Text.SetForegroundColour(wx.WHITE)
        self.Text.SetFont(Font)
        self.EnableBox = wx.CheckBox(self, -1, 'Enabled')
        self.EnableBox.SetForegroundColour(wx.WHITE)
        self.EnableBox.SetFont(Font)
        self.ChannelMaskText = wx.StaticText(self, -1, 'Channel Mask: ')
        self.ChannelMaskText.SetForegroundColour(wx.WHITE)
        self.ChannelMaskText.SetFont(Font)

        self.Bind(EVT_UPDATE_LOG, self.OnUpdateLog)

        Mask = CMN_LOG_ALLOW_ALL
        if ireEmbedded:
            Mask = ireLogger.GetMask()

        # use the IntCtrl control to get the log mask.  Since this is now a mask,
        # rather than a log level, it would make more sense to have a different
        # interface (e.g., check boxes to enable/disable each type of log message).
        self.ChannelMask= intctrl.IntCtrl(self, -1, Mask, min=CMN_LOG_ALLOW_NONE, max=CMN_LOG_ALLOW_ALL, limited=True)

        if not ireEmbedded:
            self.EnableBox.Enable(False)
            self.ChannelMaskText.Enable(False)
            self.ChannelMask.Enable(False)

        self.HeaderSizer = wx.BoxSizer(wx.HORIZONTAL)
        self.HeaderSizer.Add(self.Text, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),3)   # Add a stretchable space
        self.HeaderSizer.Add(self.EnableBox, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),3)   # Add a stretchable space
        self.HeaderSizer.Add(self.ChannelMaskText, 0, wx.ALIGN_RIGHT)
        self.HeaderSizer.Add(self.ChannelMask, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),1)   # Add a smaller stretchable space

        self.logText = wx.TextCtrl(self, -1, style=style)

        self.LogSizer = wx.BoxSizer(wx.VERTICAL)
        self.LogSizer.Add(self.HeaderSizer, 0, wx.EXPAND)
        self.LogSizer.Add(self.logText, 1, wx.EXPAND)
        self.SetSizer(self.LogSizer)

        self.Bind(wx.EVT_CHECKBOX, self.OnEnableBox, self.EnableBox)
        self.Bind(intctrl.EVT_INT, self.OnChannelMask, self.ChannelMask)

    def EnableLogger(self):
        global ireEmbedded
        if ireEmbedded:
            # Callback to a Python function (via interpreter)
            callback = wx.PostEvent
            # Try to be clever and pass the (wrapped) C function as the
            # callback.  If a C function is passed, the ireLogger module
            # can call it directly.  If we can't find the C function,
            # we'll use the Python callback function (wx.LogMessage).
            try:
                callback = wx._core_.PostEvent
            except AttributeError,e:
                print "EnableLogger warning: could not find C function:"
                print e
            ireLogger.SetTextOutput(callback, UpdateLogEvent, self)
            self.EnableBox.SetValue(True)


    def DisableLogger(self):
        global ireEmbedded
        if ireEmbedded:
            ireLogger.ClearTextOutput()
            self.EnableBox.SetValue(False)

    # Event handlers

    def OnEnableBox(self, event):
        if event.IsChecked():
            self.EnableLogger()
        else:
            self.DisableLogger()

    def OnChannelMask(self, event):
        if ireEmbedded:
            ireLogger.SetMask(event.GetValue())

    def OnUpdateLog(self, evt):
        self.logText.AppendText(evt.msg)

