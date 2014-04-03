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
#
#ireEditorNotebook

import wx
import wx.py as py

from ireShell import *

TestVar = 'hi'

class ireEditorNotebook(wx.Notebook):

    def __init__(self, parent, locals={}):
        wx.Notebook.__init__(self, parent, id=-1)

        self.buffers = {}
        self.buffer = None
        self.editor = None

        ShellPanel = wx.Panel(self, -1)

        self.Shell = ireShell(ShellPanel, -1, style=wx.CLIP_CHILDREN | wx.SUNKEN_BORDER,
            introText='Welcome to the IRE Shell.\nCtrl-Up for history!', locals=locals)

        self.AddPage(page=ShellPanel, text='*Shell*', select=True)

        ShellPanelSizer = wx.BoxSizer(wx.VERTICAL)
        ShellPanelSizer.Add(self.Shell, 1, wx.EXPAND)
        ShellPanel.SetSizer(ShellPanelSizer)
        ShellPanel.SetAutoLayout(True)

        self.Shell.SetFocus()

        wx.EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)
        #wx.EVT_IDLE(self, self.OnIdle)



    def setEditor(self, editor):
        self.editor = editor
        self.buffer = self.editor.buffer
        self.buffers[self.buffer.id] = self.buffer


    def OnClose(self, event):
        """Event handler for closing."""
        for buffer in self.buffers.values():
            self.buffer = buffer
            if buffer.hasChanged():
                cancel = self.bufferSuggestSave()
                if cancel and event.CanVeto():
                    event.Veto()
                    return
        self.Destroy()

    def OnIdle(self, event):
        """Event handler for idle time."""
        #self._updateStatus()
        self._updateTabText()
        #self._updateTitle()
        event.Skip()
        

    #def _updateTabText(self):
    #    """Show current buffer information on notebook tab."""
    #    suffix = ' **'
    #    notebook = self
    #    selection = notebook.GetSelection()
    #    if selection < 1:
    #        return
    #    text = notebook.GetPageText(selection)
    #    window = notebook.GetPage(selection)
    #    if window.editor and window.editor.buffer.hasChanged():
    #        if text.endswith(suffix):
    #            pass
    #        else:
    #            notebook.SetPageText(selection, text + suffix)
    #    else:
    #        if text.endswith(suffix):
    #            notebook.SetPageText(selection, text[:len(suffix)])
    #    window.Update()
        

    def _updateTabText(self):
        """Show current buffer information on notebook tab."""
        selection = self.GetSelection()
        if selection < 1:
            return
        if self.GetPageText(selection) != self.buffer.name:
            self.SetPageText(selection, self.buffer.name)
            self.GetPage(selection).editor.window.Update()


    def hasBuffer(self):
        """Return True if there is a current buffer."""
        if self.buffer:
            return True
        else:
            return False

    def bufferCreate(self, filename=None):
        """Create new buffer."""
        buffer = py.editor.Buffer()
        panel = wx.Panel(parent=self, id=-1)
        wx.EVT_ERASE_BACKGROUND(panel, lambda x: x)        
        editor = py.editor.Editor(parent=panel)
        panel.editor = editor
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(editor.window, 1, wx.EXPAND)
        panel.SetSizer(sizer)
        panel.SetAutoLayout(True)
        sizer.Layout()
        buffer.addEditor(editor)
        buffer.open(filename)
        buffer.confirmed = True
        self.setEditor(editor)
        self.AddPage(page=panel, text=self.buffer.name, select=True)
        self.editor.setFocus()

    def bufferDestroy(self):
        """Destroy the current buffer."""
        selection = self.GetSelection()
        if selection > 0:  # Don't destroy the PyCrust tab.
            if self.buffer:
                del self.buffers[self.buffer.id]
                self.buffer = None  # Do this before DeletePage().
            self.DeletePage(selection)

    def bufferNew(self):
        """Create new buffer."""
        self.bufferCreate()
        cancel = False
        return cancel

    def bufferOpen(self):
        """Open file in buffer."""
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        result = py.editor.openMultiple(None, 'Open file', filedir, '', 'Python files (*.py)|*.py|All Files (*.*)|*.*')
        for path in result.paths:
            self.bufferCreate(path)
        cancel = False
        return cancel

    def bufferClose(self):
        """Close buffer."""
        if self.bufferHasChanged():
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        self.bufferDestroy()
        cancel = False
        return cancel


    def bufferHasChanged(self):
        """Return True if buffer has changed since last save."""
        if self.buffer:
            return self.buffer.hasChanged()
        else:
            return False


##     def bufferPrint(self):
##         """Print buffer."""
##         pass

##     def bufferRevert(self):
##         """Revert buffer to version of file on disk."""
##         pass

    def bufferSave(self):
        """Save buffer to its file."""
        if self.buffer.doc.filepath:
            self.buffer.save()
            cancel = False
        else:
            cancel = self.bufferSaveAs()
        self._updateTabText()
        return cancel

    def bufferSaveAs(self):
        """Save buffer to a new filename."""
        if self.bufferHasChanged() and self.buffer.doc.filepath:
            cancel = self.bufferSuggestSave()
            if cancel:
                return cancel
        filedir = ''
        if self.buffer and self.buffer.doc.filedir:
            filedir = self.buffer.doc.filedir
        fn = py.editor.saveSingle(None, 'Save file', filedir, '', 'Python files (*.py)|*.py|All Files (*.*)|*.*').path
        if fn:
            # Some systems don't automatically append the .py extension
            (name, ext) = os.path.splitext(fn)
            if not ext:
               fn += '.py'
            self.buffer.saveAs(fn)
            cancel = False
        else:
            cancel = True
        return cancel

    def bufferSuggestSave(self):
        """Suggest saving changes.  Return True if user selected Cancel."""
        result = messageDialog(parent=None,
                               message='%s has changed.\n'
                                       'Would you like to save it first'
                                       '?' % self.buffer.name,
                               title='Save current file?')
        if result.positive:
            cancel = self.bufferSave()
        else:
            cancel = result.text == 'Cancel'
        return cancel

    def updateNamespace(self):
        """Update the buffer namespace for autocompletion and calltips."""
        if self.buffer.updateNamespace():
            self.SetStatusText('Namespace updated')
        else:
            self.SetStatusText('Error executing, unable to update namespace')

    #def OnPageChanged(self, event):
    #    selection = event.GetSelection()
    #    if selection == 0:
    #        self.Shell.SetFocus()
    #    event.Skip()

    def OnPageChanged(self, event):
        new = event.GetSelection()
        window = self.GetPage(new)
        if hasattr(window, 'editor'):
            self.setEditor(window.editor)
        window.SetFocus()
        event.Skip()


    def SetFocus(self):
        wx.Notebook.SetFocus(self)
        selection = self.GetSelection()
        if selection == 0:
            self.Shell.SetFocus()



