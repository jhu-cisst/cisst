# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#  $Id: ireInputBox.py,v 1.6 2007/04/26 19:33:57 anton Exp $
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

#ireInputBox.py
"""
The IRE Simple Input Box

Creates a wx dialog box that prompts the user for input:

Input: Two dictionary objects, of labels and data, keyed identically.
    ireInputBox(None, -1, Demo Input, dictLabels, dictData
    ... with Demo Input in quotes of course

Note: Currently, this dialog box displays the dialog size and position and
      updates them as you move or resize the dialog box.
"""

#import wxPython GUI package
import wx

#create a class derived from wxFrame
class ireInputBox( wx.Frame ):

	def __init__ (self, parent, id, title):
	    #call the base class init method to create the frame
		wx.Frame.__init__(self, parent, id, title)
		
		# Associate some events with methods of this class
		self.Bind(wx.EVT_SIZE, self.OnSize)
		self.Bind(wx.EVT_MOVE, self.OnMove)
		#Add the panel and some controls
		panel = wx.Panel(self, -1)
		label1 = wx.StaticText(panel, -1, "Size:")
		label2 = wx.StaticText(panel, -1, "Pos:")
		self.sizeCtrl = wx.TextCtrl(panel, -1, "", style = wx.TE_READONLY)
		self.posCtrl = wx.TextCtrl(panel, -1, "", style = wx.TE_READONLY)
		self.panel = panel
		
		# Use some sizers for layout of the widgets
		sizer = wx.FlexGridSizer(2, 2, 5, 5)
		sizer.Add(label1)
		sizer.Add(self.sizeCtrl)
		sizer.Add(label2)
		sizer.Add(self.posCtrl)
		
		border = wx.BoxSizer()
		border.Add(sizer, 0, wx.ALL, 15)
		panel.SetSizerAndFit(border)
		self.Fit()
        
    
	# This method is called by the System when the window is resized,
	# because of the association above.
	def OnSize(self, event):
		size = event.GetSize()
		self.sizeCtrl.SetValue("%s, %s" % (size.width, size.height))
		# tell the event system to continue looking for an event handler,
		# so the default handler will get called.
		event.Skip()

	# This method is called by the System when the window is moved,
	# because of the association above.
	def OnMove(self, event):
		pos = event.GetPosition()
		self.posCtrl.SetValue("%s, %s" % (pos.x, pos.y))



##############################################################################
# $Log: ireInputBox.py,v $
# Revision 1.6  2007/04/26 19:33:57  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.5  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.4  2005/12/20 21:00:21  pkaz
# ireInputBox.py:  Minor cleanup.  This appears to have been a work in process.
#
# Revision 1.3  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.2  2005/09/06 22:43:33  alamora
# irepy module: updated for cvs logging info, cisst license
#
