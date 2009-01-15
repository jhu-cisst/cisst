# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id: ireShell.py,v 1.12 2007/04/26 19:33:57 anton Exp $
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

# ireShell.py
"""
This module controls the shell of the IRE.

This module should not be called or loaded directly.  
See ireMain.py to launch the IRE.
"""

import os, string
import wx
import wx.py as py
from ireDragAndDrop import *

class ireShell(py.shell.Shell):

	PythonCommandHandler = None
	DiaryOnHandler = None
	DiaryOffHandler = None

	#------------------------------------------------------
	# __init__
	#
	# Class Constructor.  Maps events.
	#------------------------------------------------------
	
	def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
		size=wx.DefaultSize, style=wx.CLIP_CHILDREN,
		introText='someshell', locals={}, InterpClass=None,
		*args, **kwds):
		py.shell.Shell.__init__(self, parent, id, pos, size, style,
		introText, locals, InterpClass, *args, **kwds)
		wx.EVT_LEFT_DOWN(self, self.OnMouseLeftDown)
		wx.EVT_LEFT_UP(self, self.OnMouseLeftUp)
		wx.EVT_KEY_DOWN(self, self.OnKeyUpHndlr)

        dt = ShellDropTarget(self, self.DragAndDropHandler)
        self.SetDropTarget(dt)
		
	#------------------------------------------------------
	# Events
	#
	# Handlers for the events trapped above (specified in 
	# the class constructor)
	#------------------------------------------------------

	def OnMouseLeftDown(self,event):
		event.Skip()

	def OnMouseLeftUp(self,event):
		event.Skip()
	
	def OnKeyUpHndlr(self, event):
		bCtrl = False
		if event.GetKeyCode()==308: bCtrl=True
		if bCtrl and event.GetKeyCode()==wx.WXK_UP:
			self.replaceFromHistory(1)
		else:
			bCtrl=False
		event.Skip()


	#------------------------------------------------------
	# Set callback functions.
	#
	# An alternative would be to define new events, using
    # wx.NewEventType and wx.PyEventBinder.  The previous
    # implementation (overriding the Bind method) does not
    # work as of wxPython 2.6.2.1.
	#------------------------------------------------------
    def SetPythonCommandHandler(self, Handler):
        self.PythonCommandHandler = Handler

    def SetDiaryHandlers(self, OnHandler, OffHandler):
        self.DiaryOnHandler = OnHandler
        self.DiaryOffHandler = OffHandler

	#------------------------------------------------------
	# Push
	#------------------------------------------------------
	def push(self, Command):
		if self.DiaryOnHandler and Command.strip().upper() == 'DIARY ON':
			self.DiaryOnHandler()
			Command = '\n'
		elif self.DiaryOffHandler and Command.strip().upper() == 'DIARY OFF':
			self.DiaryOffHandler()
			Command = '\n'

		py.shell.Shell.push(self, Command)


	#------------------------------------------------------
	# runfile:  modified version of wx.py.shell.runfile()
    #
    # This method was modified because the original version
    # did not ignore comments and it did not properly handle
    # blank lines within an indented block.  Note that indented
    # blocks are not stored in the history (although they are
    # stored if entered directly from the keyboard, hmmm...).
	#------------------------------------------------------
    def runfile(self, filename):
        """Execute all commands in file as if they were typed into the
        shell. Modified version that ignores comments and properly
        handles blank lines in indented blocks."""
        file = open(filename)
        try:
            self.prompt()
            for command in file.readlines():
                command = command.split('#', 2)[0]   # Remove comments
                if command[:6] == 'shell.':
                    # Run shell methods silently.
                    self.run(command, prompt=False, verbose=False)
                elif command.strip():
                    # IF command is not blank AND we are in the middle of an
                    # indented block AND the command is not indented, THEN
                    # output a blank line to finish the indented block.
                    if self.more and not command[0].isspace():
                        self.run('', prompt=False, verbose=True)
                    # Now, run the command.
                    self.run(command, prompt=False, verbose=True)
        finally:
            # If we are in the middle of an indented block, finish it
            # by outputting a blank line.
            if self.more:
                self.run('', prompt=False, verbose=True)
            file.close()

	#------------------------------------------------------
	# addHistory
	#
	# Add commands to the history processor.
	#------------------------------------------------------
	def addHistory(self, Command):
		py.shell.Shell.addHistory(self, Command)
		if self.PythonCommandHandler:
			self.PythonCommandHandler(Command)

	#------------------------------------------------------
	# DragAndDropHandler
	#
	# Handler for commands that are received via a drag and
    # drop operation (e.g., by dragging from the command
    # history).  For ease of implementation, multiple lines
    # of text are joined into a single string (separated by
    # os.linesep).  Therefore, this function first splits
    # Data into a list of strings (DataList).
	#------------------------------------------------------
    def DragAndDropHandler(self, Data):
        self.prompt()
        DataList = Data.split(os.linesep)
        for Line in DataList:
            self.run(Line+os.linesep, prompt=False, verbose=True)


# $Log: ireShell.py,v $
# Revision 1.12  2007/04/26 19:33:57  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.11  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.10  2006/02/06 20:35:11  pkaz
# IRE:  changed ireShell to explicitly use callback functions rather than overriding the Bind method
# (this was a bad implementation that broke in wxPython 2.6.2.1).  Also, did some minor display cleanup.
#
# Revision 1.9  2006/01/27 09:33:43  pkaz
# ireShell.py:  reimplemented shell runfile() method to ignore comments and properly
# handle blank lines.
#
# Revision 1.8  2006/01/20 06:08:26  pkaz
# IRE:  Better (more portable) implementation of view selections for left panel.
# Allow Object Register to be used when IRE is not embedded (when user loads cisstCommonPython).
#
# Revision 1.7  2006/01/11 16:22:18  pkaz
# IRE: Added character string parameter to LaunchIREShell(), which gets executed when Python shell is started.
#
# Revision 1.6  2006/01/02 04:17:44  pkaz
# IRE:  new (correct) implementation of drag and drop functionality.
# This check-in resolves item 6 in #174 (drag and drop on Linux).
#
# Revision 1.5  2005/12/20 20:57:16  pkaz
# ireShell.py:  removed import of ireSimpleDialogs.
#
# Revision 1.4  2005/12/12 06:37:12  pkaz
# ireShell.py: commented out some (apparently) dead code.
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
# Revision 1.3  2005/04/06 22:03:25  alamora
# Added history scroll to arrow-up
#
# Revision 1.2  2005/04/06 21:58:17  alamora
# Non-functional changes; cleaned up formatting
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
