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

# ireListCtrlPanel.py

import wx, sys, string, os, types
import wx.lib.mixins.listctrl as listmix
import ireImages


class ireListCtrl(wx.ListCtrl, listmix.ListCtrlAutoWidthMixin):
    def __init__(self, parent, ID, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0):
        wx.ListCtrl.__init__(self, parent, ID, pos, size, style)
        listmix.ListCtrlAutoWidthMixin.__init__(self)

# Class:  ireListCtrlPanel
#
# This class implements the panel with a list control that
# is used for the register contents, scope variables, and
# command history.
#
# It is a little complicated by the fact that the command history
# is a single column (list of strings), whereas the other
# displays consist of 2 columns (list of tuples of strings).
# 
# The ColumnSorterMixin allows the columns to be sorted by clicking
# on the column header.  For the command history, we set the
# itemDataMap so that the sort is done by the command index
# (which starts at 0 for the first command).  The result is that
# the command history can be sorted by chronological or reverse-
# chronological order, which is more sensible than sorting the
# commands alphabetically.  The columns for the register contents
# and the scope variables are sorted alphabetically.
#
# There are two methods for adding items to the list control:
#
#    AddIndexedItem:  this is used to add a single string
#                     (e.g., command) and set the itemDataMap
#                     to sort by (chronological) index.
#
#    AddTupleItem:    this is used to add a tuple of strings
#                     (the tuple size should correspond to the
#                     number of columns) and set the itemDataMap
#                     to sort the columns alphabetically.
#
# There are other implementation options, such as merging them
# into a single method, with a flag to indicate how to handle the
# sorting.  Also, note that currently there is no method that
# accepts a single string and sorts alphabetically or 
# that accepts a tuple and sorts chronologically, but these
# are not needed.
#
# The ireListCtrlPanel can also be a source for drag-and-drop
# operations.  This is enabled by calling the EnableDropSource
# method.  The drag-and-drop operation is initiated by the
# left mouse button click (see OnLeftDown event handler).

class ireListCtrlPanel(wx.Panel, listmix.ColumnSorterMixin):

    dropSource = None

    def __init__(self, parent, Label, ColumnTitles=[], Data=[]):
        wx.Panel.__init__(self, parent, -1, style=wx.WANTS_CHARS)
        
        tID = wx.NewId()

        # Create an image list for the up and down arrows.
        # For Windows, we also create a blank image as the first entry;
        # otherwise, every line in the list starts with a small up arrow
        # (this seems to be a wxWidgets bug on Windows).
        self.il = wx.ImageList(16, 16)
        if sys.platform == 'win32':
            self.blank = self.il.Add(wx.EmptyBitmap(16,16), wx.EmptyBitmap(16,16))
        self.sm_up = self.il.Add(ireImages.getSmallUpArrowBitmap())
        self.sm_dn = self.il.Add(ireImages.getSmallDnArrowBitmap())

        self.list = ireListCtrl(self, tID,
                                 style=wx.LC_REPORT 
                                 | wx.SUNKEN_BORDER
                                 | wx.LC_EDIT_LABELS
                                 #| wxLC_NO_HEADER
                                 #| wxLC_VRULES | wxLC_HRULES
                                 )

        self.numColumns = len(ColumnTitles)
        self.SetBackgroundColour(wx.BLUE)

        self.Text = wx.StaticText(self, -1, Label, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT)
        self.Text.SetForegroundColour(wx.WHITE)
        Font = wx.Font(9, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.Text.SetFont(Font)

        self.list.SetImageList(self.il, wx.IMAGE_LIST_SMALL)

        # self.itemDataMap is needed for ColumnSorterMixin
        self.itemDataMap = {}

        self.PopulateList(ColumnTitles, Data)

        # Now that the list exists we can init the other base class,
        # see wxPython/lib/mixins/listctrl.py
        listmix.ColumnSorterMixin.__init__(self, self.numColumns)

        # Start with list sorted in ascending order
        self.SortListItems(0)

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_LIST_DELETE_ITEM, self.OnItemDelete, self.list)
        self.Bind(wx.EVT_LIST_BEGIN_LABEL_EDIT, self.OnBeginEdit, self.list)

        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.list)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnItemActivated, self.list)
        self.Bind(wx.EVT_LIST_COL_CLICK, self.OnColClick, self.list)

        self.list.Bind(wx.EVT_LEFT_DCLICK, self.OnDoubleClick)

        # for Drag-and-Drop (only need OnLeftDown)
        self.list.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.list.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)

    def EnableDropSource(self):
        self.dropSource = wx.DropSource(self)


    #------------------------------------------------------
    # Populate the list on launch
    #------------------------------------------------------
    def PopulateList(self, ColumnTitles, Data):
        for x in range(self.numColumns):
            self.list.InsertColumn(x, ColumnTitles[x])
            self.list.SetColumnWidth(x, wx.LIST_AUTOSIZE)
        for line in Data:
            if isinstance(line, types.StringTypes):
               self.AddIndexedItem(line)
            elif isinstance(line, types.TupleType):
               self.AddTupleItem(line)


    #------------------------------------------------------
    # Add a tuple of strings to the list control.
    #
    # This is called for adding the scope variables and register
    # contents.
    #
    # The itemDataMap is updated so that the column sort is
    # based on the strings displayed in the selected column
    # (i.e., in alphabetical or reverse-alphabetical order).
    #------------------------------------------------------
    def AddTupleItem(self, Data):
        Index = self.list.GetItemCount()
        if isinstance(Data, types.TupleType) and len(Data) > 0 and isinstance(Data[0], types.StringTypes):
            self.list.InsertStringItem(Index, Data[0])
            for i in range(1, len(Data)):
                self.list.SetStringItem(Index, i, Data[i])
            # Following two lines are needed for ColumnSorterMixin
            self.list.SetItemData(Index, Index)
            self.itemDataMap.update( {Index:Data} )
        else:
            print "ireListCtrlPanel.AddTupleItem: invalid data: "
            print Data

    #------------------------------------------------------
    # Add a single string to the list control.
    #
    # This is called for adding commands to the command history.
    #
    # The itemDataMap is updated so that the sort is based
    # on the index, rather than the command.  Thus, it is
    # only possible to sort commands in chronological or
    # reverse-chronological order.
    #
    #------------------------------------------------------
    def AddIndexedItem(self, Data):
        Index = self.list.GetItemCount()
        if isinstance(Data, types.StringTypes):
            try:
               self.list.InsertStringItem(Index, Data)
               # Following two lines are needed for ColumnSorterMixin
               self.list.SetItemData(Index, Index)
               self.itemDataMap.update( {Index:(Index,)} )
               return Index
            except Exception, e:
               print 'AddIndexedItem exception:', e
        else:
            print "ireListCtrlPanel.AddIndexedItem: invalid data: "
            print Data
        return -1


    
    #------------------------------------------------------
    # Sort the items in the list control.  It calls the
    # SortListItems method in the ColumnSorterMixin.
    # If no parameters are specified, the sort is done
    # using the current sorting parameters.  Otherwise,
    # the column to be sorted (and the sort order) can
    # be specified.
    # 
    # This method should be called after items are added to
    # the list.
    #------------------------------------------------------
    def SortList(self, col = -1, ascending = 1):
        self.SortListItems(col, ascending)

    #------------------------------------------------------
    # Clear items from the list control
    #------------------------------------------------------
    def ClearItems(self):
        self.list.DeleteAllItems()
        self.itemDataMap = {}

    #------------------------------------------------------
    # Get all items from the list control.  The items
    # are returned in their indexed order (i.e., independent
    # of the current sorting status).
    #------------------------------------------------------
    def GetAllItems(self):
        Data = range(self.list.GetItemCount())
        for x in range(self.list.GetItemCount()):
           Data[self.list.GetItemData(x)] = self.list.GetItemText(x)
        return Data
    
    #------------------------------------------------------
    # Return an instance of the current list
    # Used by the ColumnSorterMixin, see wxPython/lib/mixins/listctrl.py
    #------------------------------------------------------ 
    def GetListCtrl(self):
        return self.list

    # Used by the ColumnSorterMixin, see wx/lib/mixins/listctrl.py
    def GetSortImages(self):
        return (self.sm_dn, self.sm_up)

    #------------------------------------------------------
    # Get Header text
    #------------------------------------------------------
    def getColumnText(self, index, col):
        item = self.list.GetItem(index, col)
        return item.GetText()


    #------------------------------------------------------
    # Get drag and drop data:  This function returns a string
    # that is formed by joining all lines of text that have
    # been selected.  This simplifies the drag and drop
    # implementation because wx.PyTextDataObject and
    # wx.TextDropTarget can be used.
    #------------------------------------------------------
    def GetDragAndDropData(self):
        Data = []
        Index = self.list.GetFirstSelected()

        while Index != -1:
            Data.append( self.getColumnText(Index, 0) )
            Index = self.list.GetNextSelected(Index)

        return string.join(Data, os.linesep)

    # Left mouse button pressed
    def OnLeftDown(self, event):
        if self.dropSource:
            data = wx.PyTextDataObject(self.GetDragAndDropData())
            self.dropSource.SetData(data)
            self.dropSource.DoDragDrop(wx.Drag_AllowMove)
        event.Skip()


    # Left mouse button released
    def OnLeftUp(self, event):
        event.Skip()


    def OnBeginEdit(self, event):
        event.Veto()


    def OnItemDelete(self, event):
        event.Veto()


    def OnItemActivated(self, event):
        event.Skip()


    def OnItemSelected(self, event):
        event.Skip()


    def OnColClick(self, event):
        event.Skip()


    def OnDoubleClick(self, event):
        event.Skip()


    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.Text.SetDimensions(2, 0, w-2, 15)
        self.list.SetDimensions(0, 15, w, h-15)

