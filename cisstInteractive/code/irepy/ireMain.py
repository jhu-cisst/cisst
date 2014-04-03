# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):	Chris Abidin, Andrew LaMora, Peter Kazanzides
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

""" ireMain.py
This module launches the Interactive Research Environment (IRE).

To launch the IRE, import this module into python and call:
>>>ireMain.launchIrePython()

To launch the IRE without the CISST Object Register, call:
>>>ireMain.launchIrePython( False )

Authors: Chris Abidin, Andrew LaMora, Peter Kazanzides
Copyright 2004-2010
ERC CISST
Johns Hopkins University
Baltimore, MD 21218
"""
###############################   SETUP  ######################################
#------------------------------------------------------
# Import standard libraries
#------------------------------------------------------
import os, sys, imp, time, cPickle, exceptions, types, warnings
import string as String

#-----------------------------------------------------
# Ignore Python future warnings (nuissances)
#-----------------------------------------------------
warnings.simplefilter('ignore', exceptions.FutureWarning)

#------------------------------------------------------
# Import what we need from the wx toolkit
#------------------------------------------------------
try:
    import wx
    import wx.py as py
except Exception, e:
    print "Could not load wxPython: ", e
    sys.exit(1)


#------------------------------------------------------
# Import IRE specific subclass modules
#------------------------------------------------------
from ireEditorNotebook import *
from ireListCtrlPanel import *
from ireLogCtrl import *
from ireWorkspace import *
from ireTaskTree import *
from ireInputBox import *
import ireImages

ireScope = False
try:
   from wxOscilloscope import *
   ireScope = True
except ImportError,e:
   print 'Could not import wxOscilloscope:', e


# Now, see if the IRE is embedded in a C++ application.
# This can be determined by checking whether the ireLogger
# module can be imported, because ireLogger is created
# by the cisstInteractive (C++) library.
ireEmbedded = True
try:
    import ireLogger
except ImportError,e:
    ireEmbedded = False

###############################  CLASSES ######################################

#------------------------------------------------------
# ireMain
# This is the main container, it extends wx.Frame
#------------------------------------------------------
class ireMain(wx.Frame):
    
    #This is ireMain.  It loads the main window of the IRE.

    LogToDiary = False

    ObjectRegister = None
    
    Register = None
    ScopeVariables = None

    HISTORY_FILENAME = 'history'
    DIARY_FILENAME = 'diary'
    
    TYPES_TO_EXCLUDE = ['module','builtin_function_or_method','type','NoneType','classobj']

    ID_VIEW_REGISTER = wx.NewId()
    ID_VIEW_VARIABLES = wx.NewId()
    ID_VIEW_HISTORY = wx.NewId()
    ID_VIEW_LOGGER = wx.NewId()
    ID_LOADWORKSPACE = wx.NewId()
    ID_SAVEWORKSPACE = wx.NewId()
    ID_RUNINSHELL = wx.NewId()
    ID_RUNINNEWPROCESS = wx.NewId()
    ID_TRUNCATEHISTORY = wx.NewId()
    ID_LOADHISTORY = wx.NewId()
    ID_CLEARHISTORY = wx.NewId()
    ID_TASKTREE = wx.NewId()
    ID_TESTINPUTBOX = wx.NewId()
    ID_OSCILLOSCOPE = wx.NewId()
    ID_LOAD_CISSTCOMMON = wx.NewId()
    ID_LOAD_CISSTOSABSTRACTION = wx.NewId()
    ID_LOAD_CISSTVECTOR = wx.NewId()
    ID_LOAD_CISSTNUMERICAL = wx.NewId()
    ID_LOAD_CISSTMULTITASK = wx.NewId()
    ID_LOAD_CISSTPARAMETERTYPES = wx.NewId()

    #------------------------------------------------------
    #IRE CONSTRUCTOR
    #------------------------------------------------------
    def __init__(self, parent, id, title):

        
        # Create a frame that is 750 x 500 pixels in size (if not embedded) or
        # 750 x 750 pixels in size (if embedded).  The larger size provides more
        # room for the Register Contents and Logger Output windows, which are enabled
        # by default when the IRE is embedded in a C++ program.
        ireSize = (750, 500)
        if ireEmbedded:
           ireSize = (750, 750)
        wx.Frame.__init__(self, parent, -1, title, pos=(150, 150), size=ireSize)

        #------------------------------------------------------
        # Define and set up the menu labels
        #------------------------------------------------------
        
        menu = self.FileMenu = wx.Menu()
        menu.Append(wx.ID_NEW,    "&New \tCtrl-N",            "New file")
        menu.Append(wx.ID_OPEN,   "&Open... \tCtrl-O",        "Open file")
        menu.AppendSeparator()
        menu.Append(wx.ID_REVERT, "&Revert \tCtrl-R",         "Revert to last saved version")
        menu.Append(wx.ID_CLOSE,  "&Close \tCtrl-W",          "Close current file")
        menu.AppendSeparator()
        menu.Append(wx.ID_SAVE,   "&Save \tCtrl-S",           "Save file")
        menu.Append(wx.ID_SAVEAS, "Save &As...\tShift+Ctrl+S","Save as a different filename")
        menu.AppendSeparator()
        menu.Append(wx.ID_PRINT,  "&Print... \tCtrl+P",       "Print")
        menu.AppendSeparator()
        menu.Append(wx.ID_EXIT,   "E&xit \tAlt-X",            "Exit the IRE")
        
        menu = self.EditMenu = wx.Menu()
        menu.Append(wx.ID_UNDO,      "&Undo \tCtrl+Z",       "Undo the last action")
        menu.Append(wx.ID_REDO,      "&Redo \tCtrl+Y",       "Redo the last undone action")
        menu.AppendSeparator()
        menu.Append(wx.ID_CUT,       "Cu&t \tCtrl+X",        "Cut the selection")
        menu.Append(wx.ID_COPY,      "&Copy \tCtrl+C",       "Copy the selection")
        menu.Append(wx.ID_PASTE,     "Paste \tCtrl+V",       "Paste from clipboard")
        menu.AppendSeparator()
        menu.Append(wx.ID_SELECTALL, "Select A&ll \tCtrl+A", "Select all text")
        
        menu = self.ViewMenu = wx.Menu()
        self.MENU_VIEW_REGISTER = wx.MenuItem(menu, self.ID_VIEW_REGISTER, "Register Contents",
                                                    "View Register Contents", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_REGISTER)
        self.MENU_VIEW_REGISTER.Check(ireEmbedded)
        self.MENU_VIEW_VARIABLES = wx.MenuItem(menu, self.ID_VIEW_VARIABLES, "Shell Variables",
                                                     "View Shell Variables", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_VARIABLES)
        self.MENU_VIEW_VARIABLES.Check(True)
        self.MENU_VIEW_HISTORY = wx.MenuItem(menu, self.ID_VIEW_HISTORY,   "Command History",
                                                   "View Command History", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_HISTORY)
        self.MENU_VIEW_HISTORY.Check(True)
        menu.Append(self.ID_VIEW_LOGGER,   "Logger Output",   "View C++ Logger Output",   wx.ITEM_CHECK)
        menu.Check(self.ID_VIEW_LOGGER, ireEmbedded)

        menu = self.ToolsMenu = wx.Menu()
        menu.Append(self.ID_LOADWORKSPACE, "&Load workspace... \tCtrl+1", "Load workspace from file")
        menu.Append(self.ID_SAVEWORKSPACE, "&Save workspace... \tCtrl+2", "Save workspace to file")
        menu.AppendSeparator()
        menu.Append(self.ID_RUNINSHELL, "&Run in shell \tCtrl+3", "Run script in shell")
        menu.Append(self.ID_RUNINNEWPROCESS, "Run in &new process \tCtrl+4", "Run script file in separate process")

        menu.AppendSeparator()
        menu.Append(self.ID_TRUNCATEHISTORY, "Truncate command &history... \tCtrl+5", "Archive history by line")
        menu.Append(self.ID_LOADHISTORY, "Load &command history... \tCtrl+6", "Load archived commands")
        menu.Append(self.ID_CLEARHISTORY, "C&lear command history... \tCtrl+7", "Clear your command history")
        menu.AppendSeparator()
        menu.Append(self.ID_TASKTREE, "&Task Manager", "Show Task Manager browser")
        menu.Append(self.ID_TESTINPUTBOX, "Test &input box", "Create test input box")
        if ireScope:
            menu.Append(self.ID_OSCILLOSCOPE, "&Oscilloscope", "Show oscilloscope")
        
        menu = self.ImportMenu = wx.Menu()
        menu.Append(self.ID_LOAD_CISSTCOMMON, "Import cisst&Common", "Import cisstCommon")
        menu.Append(self.ID_LOAD_CISSTOSABSTRACTION, "Import cisst&OSAbstraction", "Import cisstOSAbstraction")
        menu.Append(self.ID_LOAD_CISSTVECTOR, "Import cisst&Vector", "Import cisstVector")
        menu.Append(self.ID_LOAD_CISSTNUMERICAL, "Import cisst&Numerical", "Import cisstNumerical")
        menu.Append(self.ID_LOAD_CISSTMULTITASK, "Import cisst&MultiTask", "Import cisstMultiTask")
        menu.Append(self.ID_LOAD_CISSTPARAMETERTYPES, "Import cisst&ParameterTypes", "Import cisstParameterTypes")

        menu = self.HelpMenu = wx.Menu()
        menu.Append(wx.ID_ABOUT, "&About...", "About this program")

        menubar = self.MenuBar = wx.MenuBar()
        menubar.Append(self.FileMenu, "&File")
        menubar.Append(self.EditMenu, "&Edit")
        menubar.Append(self.ViewMenu, "&View")
        menubar.Append(self.ToolsMenu,"&Tools")
        menubar.Append(self.ImportMenu,"&Import")
        menubar.Append(self.HelpMenu, "&Help")
        self.SetMenuBar(menubar)

        wx.EVT_MENU(self, wx.ID_NEW, self.OnFileNew)
        wx.EVT_MENU(self, wx.ID_OPEN, self.OnFileOpen)
        wx.EVT_MENU(self, wx.ID_REVERT, self.OnFileRevert)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.OnFileClose)
        wx.EVT_MENU(self, wx.ID_SAVE, self.OnFileSave)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.OnFileSaveAs)
        #wx.EVT_MENU(self, wx.ID_NAMESPACE, self.OnFileUpdateNamespace)
        wx.EVT_MENU(self, wx.ID_PRINT, self.OnFilePrint)
        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU(self, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.ID_CUT, self.OnCut)
        wx.EVT_MENU(self, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(self, wx.ID_PASTE, self.OnPaste)
        wx.EVT_MENU(self, wx.ID_SELECTALL, self.OnSelectAll)
        wx.EVT_MENU(self, self.ID_VIEW_REGISTER, self.OnViewRegister)
        wx.EVT_MENU(self, self.ID_VIEW_VARIABLES, self.OnViewVariables)
        wx.EVT_MENU(self, self.ID_VIEW_HISTORY, self.OnViewHistory)
        wx.EVT_MENU(self, self.ID_VIEW_LOGGER, self.OnViewLogger)
        wx.EVT_MENU(self, self.ID_LOADWORKSPACE, self.OnLoadWorkspace)
        wx.EVT_MENU(self, self.ID_SAVEWORKSPACE, self.OnSaveWorkspace)
        wx.EVT_MENU(self, self.ID_RUNINSHELL, self.OnRunInShell)
        wx.EVT_MENU(self, self.ID_RUNINNEWPROCESS, self.OnRunInNewProcess)
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_MENU(self, self.ID_TRUNCATEHISTORY, self.OnTruncateHistory)
        wx.EVT_MENU(self, self.ID_LOADHISTORY, self.OnLoadHistory)
        wx.EVT_MENU(self, self.ID_CLEARHISTORY, self.OnClearHistory)
        wx.EVT_MENU(self, self.ID_TASKTREE, self.OnTaskTree)
        wx.EVT_MENU(self, self.ID_TESTINPUTBOX, self.OnTestInputBox)
        wx.EVT_MENU(self, self.ID_OSCILLOSCOPE, self.OnOscilloscope)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTCOMMON,  self.OnImportCisstCommon)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTOSABSTRACTION,  self.OnImportCisstOSAbstraction)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTVECTOR,  self.OnImportCisstVector)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTNUMERICAL,  self.OnImportCisstNumerical)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTMULTITASK,  self.OnImportCisstMultiTask)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTPARAMETERTYPES,  self.OnImportCisstParameterTypes)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_CUT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_COPY, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_PASTE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SELECTALL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_RUNINSHELL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_RUNINNEWPROCESS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_TASKTREE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTCOMMON, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTVECTOR, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTNUMERICAL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTMULTITASK, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTPARAMETERTYPES, self.OnUpdateMenu)
        #wx.EVT_UPDATE_UI(self, self.ID_TRUNCATEHISTORY, self.OnTruncateHistory)
        
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.CreateStatusBar()
        
        #------------------------------------------------------
        # Build and instantiate the toolbar
        #------------------------------------------------------

        ToolBar = self.CreateToolBar( wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT | wx.TB_TEXT )
        # Set up the toolbar, using bitmaps from ireImages.py.  Note that an alternative is to 
        # use wx.ArtProvider.GetBitmap to get the wxWidgets default bitmaps:
        #    tsize = (16,16)
        #    new_bmp =  wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR, tsize)
        #    open_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR, tsize)
        #    save_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_TOOLBAR, tsize)
        ToolBar.AddSimpleTool(wx.ID_NEW,  ireImages.getNewItemBitmap(), "New", "New file")
        ToolBar.AddSimpleTool(wx.ID_OPEN, ireImages.getOpenItemBitmap(), "Open", "Open file...")
        ToolBar.AddSimpleTool(wx.ID_SAVE, ireImages.getSaveItemBitmap(), "Save", "Save file")

        #ToolBar.AddSeparator()
        # Could add other tools here, such as ones for loading cisstCommon,
        # cisstVector, cisstNumerical, ...

        ToolBar.Realize()

  
        #*************** Subdivide the frame into multiple windows ***************************
        #

        # First, create a horizontal splitter window (MainSplitter).  The bottom pane is
        # used for the logger output.  The top pane (TopSplitter) will be split further.

        self.MainSplitter = wx.SplitterWindow(self, -1)
        self.TopSplitter = wx.SplitterWindow(self.MainSplitter, -1)
        self.LoggerPanel = ireLogCtrl(self.MainSplitter, -1, 'Logger Output', 
                              style = wx.TE_READONLY | wx.HSCROLL | wx.TE_MULTILINE | wx.TE_RICH)
        # Only show logger window if IRE is embedded.
        if ireEmbedded:
            self.MainSplitter.SplitHorizontally(self.TopSplitter, self.LoggerPanel, -150)
        else:
            self.MainSplitter.Initialize(self.TopSplitter)
            self.LoggerPanel.Show(False)
        self.MainSplitter.SetMinimumPaneSize(100)
        # Sash gravity of 1.0 means that only the TopSplitter will be resized when the entire
        # frame is resized.  Note that SetSashGravity is not available in older versions
        # of wxWidgets (before 2.5.4).
        if 'SetSashGravity' in dir(self.MainSplitter):
            self.MainSplitter.SetSashGravity(1.0)

        # Now, split the top splitter into left and right panes.  The right
        # pane is used for the shell/notebook.  The left pane (panel) will contain three
        # sub-panels for the Register Contents, Shell Variables and Command History.

        self.LeftPanel = wx.Panel(self.TopSplitter, -1)
        self.EditorNotebook = ireEditorNotebook(self.TopSplitter, { 'ireFrame':self })
        self.TopSplitter.SplitVertically(self.LeftPanel, self.EditorNotebook, 200)
        self.TopSplitter.SetMinimumPaneSize(200)

        # Finally, "split" the left panel into the three sub-panels.  This is done using wx.BoxSizer,
        # rather than wx.SplitterWindow because there were a lot of problems getting the latter to work
        # correctly on all platforms.  Note that the three sub-panels are not split evenly:  the Command
        # History is allocated 40% of the space, whereas the other two panels are each allocated 30%.

        self.RegisterContentsListCtrl = ireListCtrlPanel(self.LeftPanel, 'Register Contents', ['Identifier', 'Type'])
        self.ScopeVariablesListCtrl = ireListCtrlPanel(self.LeftPanel, 'Shell Variables',  ['Identifier', 'Type'])
        self.CommandHistoryListCtrl = ireListCtrlPanel(self.LeftPanel, 'Command History', ['Commands'], self.LoadHistoryFromFile())
        self.LeftBoxSizer = wx.BoxSizer(wx.VERTICAL)
        self.LeftBoxSizer.Add(self.RegisterContentsListCtrl, 3, wx.EXPAND)
        self.LeftBoxSizer.Add(self.ScopeVariablesListCtrl, 3, wx.EXPAND)
        self.LeftBoxSizer.Add(self.CommandHistoryListCtrl, 4, wx.EXPAND)
        self.LeftPanel.SetSizer(self.LeftBoxSizer)

        # Don't show Register Contents unless IRE is embedded (the user
        # can choose to display it later).
        self.LeftBoxSizer.Show(self.RegisterContentsListCtrl, ireEmbedded)

        #*************************** Set up Drag and Drop ***************************

        # Enable the Scope Variables list as a source for drag and drop
        # (i.e., to drag variable names to the shell in order to print their
        # current values).
        self.ScopeVariablesListCtrl.EnableDropSource()
        
        # Enable the Command History list as a source for drag and drop
        # (i.e., to drag lines from the command history list to the shell)
        self.CommandHistoryListCtrl.EnableDropSource()

        #*********************** Initialize the Command History***********************

        self.CommandHistoryListCtrl.AddIndexedItem(time.strftime("--- %a, %d %b %Y, %I:%M %p ---", time.localtime()))
	
        #******************** Set up callback functions for the shell ****************

        self.Shell = self.EditorNotebook.Shell
        self.Shell.SetPythonCommandHandler(self.PythonCommandEntered)
        self.Shell.SetDiaryHandlers(self.DiaryOn, self.DiaryOff)

        #********************************* Set up logger *****************************

        if ireEmbedded:
            self.LoggerPanel.EnableLogger()
    
        #******************************** Misc. other setup **************************

        # Set minimum size for entire frame to be at least twice the minimum pane size.
        # Note that (ireSize[x]-clientSize[x]) is added to account for the borders,
        # menu bar, tool bar, and status line.
        clientSize = self.GetClientSizeTuple()
        self.SetSizeHints(2*self.TopSplitter.GetMinimumPaneSize() + (ireSize[0]-clientSize[0]),
                          2*self.MainSplitter.GetMinimumPaneSize() + (ireSize[1]-clientSize[1]))

        self.CheckLists()
        
        Width = 10*(self.RegisterContentsListCtrl.list.GetColumnWidth(0) +
            self.RegisterContentsListCtrl.list.GetColumnWidth(1))/2
        self.RegisterContentsListCtrl.list.SetColumnWidth(0, Width)
        self.ScopeVariablesListCtrl.list.SetColumnWidth(0, Width)

        
	#------------------------------------------------------
	# addVariablesToRegister
	#
	# Parse the input Data for variables that are to be added
	# to the Object Register.
	# NOTE: affected by changes to the Object Register
	#------------------------------------------------------
    def AddVariablesToRegister(self, Data):
        #for VariableName in Data:
            #self.ObjectRegister.Register(VariableName, self.Shell.interp.locals[VariableName])
        self.CheckRegisterContents()

	#------------------------------------------------------
	# GetVariablesFromRegister
	#
	# Calls RegisterGet to explicitly fetch variables 
	# contained in the input argument "Data" (which is
	# itself populated by List() or used to be till anton killed it)
	#------------------------------------------------------
    def GetVariablesFromRegister(self, Data):
        for VariableName in Data:
            self.Shell.interp.locals[VariableName] = FindObject(VariableName)
        self.CheckScopeVariables()

	#------------------------------------------------------
	# The following functions handle command events
	# in the notebook shell.
	#------------------------------------------------------

    def PythonCommandEntered(self, Command):
        if len(Command.strip()) > 0:
            Lines = Command.splitlines()
            for Line in Lines:
                Line = Line.split('#', 1)[0]  # remove comment
                Line = Line.rstrip()          # remove trailing whitespace
                if Line:
                    index = self.CommandHistoryListCtrl.AddIndexedItem(Line)
                    if index < 0:
                       print 'Failed to add command to history: ', Line
                    elif self.IsShown():
                       self.CommandHistoryListCtrl.list.EnsureVisible(index)
            if self.LogToDiary:
                self.WriteToDiary(Lines)
    	    self.CheckLists()

    def OnImportCisstCommon(self, event):
        self.ImportCisstCommon()
    def ImportCisstCommon(self):
        print "importing cisstCommon"
		self.Shell.push("from cisstCommonPython import *")

    def OnImportCisstOSAbstraction(self, event):
        self.ImportCisstOSAbstraction()
    def ImportCisstOSAbstraction(self):
        print "importing cisstOSAbstraction"
		self.Shell.push("from cisstOSAbstractionPython import *")
            
    def OnImportCisstVector(self, event):
        self.ImportCisstVector()
    def ImportCisstVector(self):
        print "importing cisstVector"
		self.Shell.push("from cisstVectorPython import *")

    def OnImportCisstNumerical(self, event):
        self.ImportCisstNumerical()
    def ImportCisstNumerical(self):
        print "importing cisstNumerical"
		self.Shell.push("from cisstNumericalPython import *")

    def OnImportCisstMultiTask(self, event):
        self.ImportCisstMultiTask()
    def ImportCisstMultiTask(self):
        print "importing cisstMultiTask"
		self.Shell.push("from cisstMultiTaskPython import *")

    def OnImportCisstParameterTypes(self, event):
        self.ImportCisstParameterTypes()
    def ImportCisstParameterTypes(self):
        print "importing cisstParameterTypes"
		self.Shell.push("from cisstParameterTypesPython import *")

	#------------------------------------------------------
	# Diary Functions
	#
	# Handle enabling, recording to and fetching from the 
	# statement diary.  Currently not used.
	#------------------------------------------------------
    def WriteToDiary(self, Lines):
        DiaryFile = open(self.DIARY_FILENAME, 'a')
        for Line in Lines:
            DiaryFile.write(Line + os.linesep)
        DiaryFile.close()


    def DiaryOn(self):
        self.Shell.write('\nDiary logging enabled')
        DiaryFile = open(self.DIARY_FILENAME, 'a')
        DiaryFile.write(
            os.linesep + '# Diary started ' +
            time.strftime("%a, %d %b %Y, %H:%M:%S", time.localtime()) +
            os.linesep
        )
        DiaryFile.close()
        self.LogToDiary = True;


    def DiaryOff(self):
        self.Shell.write('\nDiary logging disabled')
        self.LogToDiary = False;


    def CheckLists(self):
        if not self.ObjectRegister:
            if sys.modules.has_key('cisstCommonPython'):
                import cisstCommonPython
                self.ObjectRegister = cisstCommonPython.cmnObjectRegister_Instance()
        if self.ObjectRegister:
            self.CheckRegisterContents()
        self.CheckScopeVariables()
        self.CommandHistoryListCtrl.SortList()


    def CheckRegisterContents(self):
        CurrentRegister = String.split(self.ObjectRegister.__str__())
        if CurrentRegister != self.Register:
            self.UpdateList(self.RegisterContentsListCtrl, self.GetRegisterContents(self.TYPES_TO_EXCLUDE))
            self.Register = CurrentRegister

    def CheckScopeVariables(self):
        CurrentScopeVariables = self.Shell.interp.locals.keys()   
        if CurrentScopeVariables != self.ScopeVariables:
            self.UpdateList(self.ScopeVariablesListCtrl, self.GetScopeVariables(self.TYPES_TO_EXCLUDE))
            self.ScopeVariables = CurrentScopeVariables


    def UpdateList(self, ListCtrl, Data):
        ListCtrl.ClearItems()
        for i in Data.keys():
            ListCtrl.AddTupleItem( (i,Data[i]) )
            ListCtrl.SortList()


    def GetRegisterContents(self, TypesToExclude=[]):
        Contents = {}
        VariableName = ""
        VariableType = ""
        for tok in String.split(self.ObjectRegister.__str__(), " "):
            #if starts with lparen, is type, else is name
            if tok.find('(')==0:  #this is a variabletype
                VariableType = tok[1:tok.rfind(')')]
                if VariableType not in TypesToExclude:
                    Contents.update( {VariableName:VariableType} )
            else:   #this is a variablename
                VariableName = tok
        return Contents


    def GetScopeVariables(self, TypesToExclude=[]):
        Variables = {}
        for VariableName in self.Shell.interp.locals.keys():
            if not self.IsDecorated(VariableName):
                VariableType = self.GetVariableTypeString(VariableName)
                if VariableType not in TypesToExclude:
                	Variables.update({VariableName:VariableType})
        return Variables


    def IsDecorated(self, String):
        return String[0:2] == '__' and String[len(String)-2:len(String)] == '__'


    def GetRegisterTypeString(self, VariableName):
        return self.GetTypeString("RegisterGet('" + VariableName + "')")


    def GetVariableTypeString(self, VariableName):
        return self.GetTypeString("vars(self.Shell.interp)['locals']['" + VariableName + "']")


    def GetTypeString(self, _Str):
        exec("VariableType = str(type(" + _Str + "))")
        return VariableType[VariableType.find("'")+1:VariableType.rfind("'")]

	
	#-------------------------------------------------
	# Methods for manipulating/displaying/truncating the 
	# command history 
	#-------------------------------------------------

    def SaveHistoryToFile(self):
        cmdlist = self.CommandHistoryListCtrl.GetAllItems()
        f = open(self.HISTORY_FILENAME, 'w')
        cPickle.dump(cmdlist, f)
    
    def LoadHistoryFromFile(self, fn=HISTORY_FILENAME):
        Data = []
        if os.path.isfile(fn):
            try:
                Data = cPickle.load(open(fn))
            except exceptions.Exception, error:
                msgdlg = wx.MessageDialog(self, str(error), "Load History", wx.OK | wx.ICON_ERROR)
                msgdlg.ShowModal()
                msgdlg.Destroy()
        # Make sure command history is a list of strings.  Note that an empty
        # list is not considered to be an error.
        file_error = False
        if isinstance(Data, types.ListType):
            if len(Data) > 0 and not isinstance(Data[0], types.StringTypes):
                file_error = True   # list does not contain strings
        else:
            file_error = True       # not a list
        if file_error:
            text = "Invalid command history file: " + fn
            msgdlg = wx.MessageDialog(self, text, "Load History", wx.OK | wx.ICON_ERROR)
            msgdlg.ShowModal()
            msgdlg.Destroy()
            Data = []
        return Data
    
    def ClearHistoryFile(self):
        """Remove all command history entries from the display and file."""
        text = "Are you sure you want to clear your command history?"
        title = "Clear Command History"
        dlg = wx.MessageDialog( self, text, title, wx.OK|wx.CANCEL)
        if dlg.ShowModal() == wx.ID_OK:
            #clear the history panel
            self.CommandHistoryListCtrl.ClearItems()
            #clear the history file
            self.SaveHistoryToFile()
        dlg.Destroy()
        
            
    def QueryHistoryFile(self):
        """Append to the command history from the selected history file (*.hist)."""
        # Use os.getcwd() instead of '.' to avoid gtk assertion on Linux.
        dlg = wx.FileDialog( self, "Select a Command Archive to Load:", os.getcwd(), "", \
                             "IRE Command Histories (*.hist)|*.hist|All Files (*.*)|*.*", wx.OPEN)
        # show the dialog and process it
        if dlg.ShowModal() == wx.ID_OK:
            fn = dlg.GetFilename()
            Data = self.LoadHistoryFromFile( fn )
            for n in Data:
                self.CommandHistoryListCtrl.AddIndexedItem(n)
            self.CommandHistoryListCtrl.SortList()
        dlg.Destroy()
        
        
    def TruncateHistoryFile(self):
        """Delete specified commands from Command History.  Deleted commands can (optionally)
           be archived to a new or existing history file (*.hist)."""
        #some vars
        title = "Truncate Command History"
        archlist = []
        # fetch the command history list
        cmdlist = self.CommandHistoryListCtrl.GetAllItems()
        # query the user for a choice using the simple choice dialog
        # Archived commands can be saved to a file, but will be deleted from Command History window
        cdlg = wx.SingleChoiceDialog(self, "Archive/delete up to this line, inclusive:", \
                                     title, cmdlist, wx.OK|wx.CANCEL)
        # display dlg but exit, if user pressed cancel
        if cdlg.ShowModal() == wx.ID_CANCEL:
            cdlg.Destroy()
            return
            
        # fetch the user's choice
        choice = cdlg.GetSelection()
        choice = int(choice + 1)
        cdlg.Destroy()

        # query the user for a file name, and save it if one is entered.
        # Use os.getcwd() instead of '.' to avoid gtk assertion on Linux.
        idlg = wx.FileDialog( None, "Save/append to archive file:", os.getcwd(), "", \
                              "IRE Command Histories (*.hist)|*.hist|All Files (*.*)|*.*", wx.SAVE)
        if idlg.ShowModal() == wx.ID_OK:
            fn = idlg.GetFilename()
            # Some systems don't automatically append the .hist extension
            (name, ext) = os.path.splitext(fn)
            if not ext:
               fn += '.hist'
            # write the command file up to the selected line into the named file
            # fetch any existing list first and append to it
            try:
                archlist = cPickle.load(open(fn))
            except IOError:
                archlist = []
            n=0
            for n in range(choice):
                archlist.append( cmdlist[n] )
            cPickle.dump( archlist, open(fn, 'w'))
            text = "Your command history was archived to " + fn
            msgdlg = wx.MessageDialog(self, text, title, wx.OK | wx.ICON_INFORMATION)
            msgdlg.ShowModal()
            msgdlg.Destroy()
            
        #now delete the entries from the list
        del cmdlist[0:choice]

        #clear the panel
        self.CommandHistoryListCtrl.ClearItems()
        #add only the new items back
        for line in cmdlist:
            self.CommandHistoryListCtrl.AddIndexedItem(line)
        self.CommandHistoryListCtrl.SortList()
            
        #clobber the main history file
        self.SaveHistoryToFile()
        idlg.Destroy()
        
    
    #-------------------------------------------------
    # Event handlers for menu selections
    #-------------------------------------------------
    
    def OnCloseWindow(self, evt):
        self.SaveHistoryToFile()
        self.LoggerPanel.DisableLogger()
        #self.Close(True)
        self.Destroy()

    # Disable changes to the sash (if this handler
    # is bound to EVT_SPLITTER_SASH_POS_CHANGING)
    def OnSashChanging(self, Event):
        Event.SetSashPosition(-1)

    def OnToolClick(self, Event):
        pass

    def OnFileNew(self, event):
        self.EditorNotebook.bufferNew()

    def OnFileOpen(self, event):
        self.EditorNotebook.bufferOpen()

    def OnFileRevert(self, event):
        self.EditorNotebook.bufferRevert()

    def OnFileClose(self, event):
        self.EditorNotebook.bufferClose()

    def OnFileSave(self, event):
        self.EditorNotebook.bufferSave()

    def OnFileSaveAs(self, event):
        self.EditorNotebook.bufferSaveAs()

    def OnFileUpdateNamespace(self, event):
        self.EditorNotebook.updateNamespace()

    def OnFilePrint(self, event):
        self.EditorNotebook.bufferPrint()

    def OnExit(self, event):
        self.LoggerPanel.DisableLogger()
        self.Close(False)
        #self.Destroy()

    def OnUndo(self, event):
        wx.Window_FindFocus().Undo()

    def OnRedo(self, event):
        wx.Window_FindFocus().Redo()

    def OnCut(self, event):
        wx.Window_FindFocus().Cut()

    def OnCopy(self, event):
        wx.Window_FindFocus().Copy()

    def OnPaste(self, event):
        wx.Window_FindFocus().Paste()

    def OnSelectAll(self, event):
        wx.Window_FindFocus().SelectAll()

	#-------------------------------------------------
	# View menu handlers
	#-------------------------------------------------

    # Check the status of the left panel (Register Contents, Shell Variables,
    # and Command History).  If all are disabled, then unsplit TopSplitter so
    # that there is more room for the shell.  If any of them is enabled, make
    # sure that TopSplitter is split.
    def CheckLeftPanel(self):
        if self.MENU_VIEW_REGISTER.IsChecked() or \
           self.MENU_VIEW_VARIABLES.IsChecked() or \
           self.MENU_VIEW_HISTORY.IsChecked():
            if not self.TopSplitter.IsSplit():
                self.TopSplitter.SplitVertically(self.LeftPanel, self.EditorNotebook, 200)
                # Following needed for Mac OS X
                self.LeftPanel.Show()
        else:
            if self.TopSplitter.IsSplit():
                self.TopSplitter.Unsplit(self.LeftPanel)
	
    def OnViewRegister(self, event):
        self.LeftBoxSizer.Show(self.RegisterContentsListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()

    def OnViewVariables(self, event):
        self.LeftBoxSizer.Show(self.ScopeVariablesListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()
    
    def OnViewHistory(self, event):
        self.LeftBoxSizer.Show(self.CommandHistoryListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()
    
    def OnViewLogger(self, event):
        if event.IsChecked():
            self.MainSplitter.SplitHorizontally(self.TopSplitter, self.LoggerPanel, -150)
            # Following needed for Mac OS X
            self.LoggerPanel.Show()
        else:
            self.MainSplitter.Unsplit(self.LoggerPanel)

	#-------------------------------------------------
	# Workspace maintenance methods
	# (also event handlers)
	#-------------------------------------------------
	
    def OnLoadWorkspace(self, event):
        Filepath = py.editor.openSingle(directory='',wildcard='IRE Workspace (*.ws)|*.ws|All Files (*.*)|*.*').path
        if Filepath:
            File = open(Filepath)
            LoadWorkspaceFile(self.Shell.interp.locals, File)
            File.close()
            self.CheckScopeVariables()

    def OnSaveWorkspace(self, event):
        Filepath = py.editor.saveSingle(directory='',wildcard='IRE Workspace (*.ws)|*.ws|All Files (*.*)|*.*').path
        if Filepath:
            # Some systems don't automatically append the .ws extension
            (name, ext) = os.path.splitext(Filepath)
            if not ext:
               Filepath += '.ws'
            File = open(Filepath,'w')
            SaveWorkspaceToFile(self.Shell.interp.locals, File)
            File.close()
	
    def OnLoadHistory(self, event):
        self.QueryHistoryFile()
      
    def OnTruncateHistory(self, event):
        self.TruncateHistoryFile()
    
    def OnClearHistory(self, event):
        self.ClearHistoryFile()

    def OnTestInputBox(self, event):        
        frame = ireInputBox(None, -1, "Test Size/Position")
        frame.Show(True)
        
    def OnTaskTree(self, event):        
        #taskManager = self.ObjectRegister.FindObject("TaskManager")
        taskManager = mtsManagerLocal.GetInstance()
        if taskManager:
            frame = ireTaskTree(None, -1, "Task Manager", taskManager)
            frame.Show(True)
        else:
            text = "Task Manager not found"
            msgdlg = wx.MessageDialog(self, text, "Task Manager", wx.OK | wx.ICON_ERROR)
            msgdlg.ShowModal()
            msgdlg.Destroy()
        
    def OnOscilloscope(self, event):        
        import gettext
        gettext.install("irepy")
        #taskManager = self.ObjectRegister.FindObject("TaskManager")
        taskManager = mtsManagerLocal.GetInstance()
        if taskManager:
            self.scopeFrame = COscilloscope(self, taskManager)
            self.scopeFrame.Show()
        else:
            text = "Task Manager not found"
            msgdlg = wx.MessageDialog(self, text, "Oscilloscope", wx.OK | wx.ICON_ERROR)
            msgdlg.ShowModal()
            msgdlg.Destroy()
        
    #-------------------------------------------------
	# Methods for running scripts in the main or 
	# separate threads.
	#-------------------------------------------------
	
    def OnRunInShell(self, event):
        cancel = self.EditorNotebook.bufferSave()
        if not cancel:
           self.Shell.runfile(self.EditorNotebook.buffer.doc.filepath)

    def OnRunInNewProcess(self, event):
        # Don't do this when embedded -- we may not really want to run
        # another copy of the entire program.
        if not ireEmbedded:
            cancel = self.EditorNotebook.bufferSave()
            if not cancel:
                str = os.sys.executable + ' "' + self.EditorNotebook.buffer.doc.filepath + '"'
                os.system(str)

		
	#-------------------------------------------------
	# The obligatory About
	#-------------------------------------------------
    def OnAbout(self, event):
        """Display an About window."""
        title = 'About'
        twidth = 55
        text = \
'Interactive Research Environment (IRE)'.center(twidth) + '\n' + \
'The IRE provides a Python-based command interpreter'.center(twidth) + '\n\n' + \
'Developed by the Engineering Research Center for'.center(twidth) + '\n' + \
'Computer-Integrated Surgical Systems & Technology (CISST)'.center(twidth) + '\n' + \
'http://cisst.org'.center(twidth) + '\n\n' + \
'Copyright (c) 2004-2011, The Johns Hopkins University'.center(twidth) + '\n' + \
'All Rights Reserved.\n\n'.center(twidth) + '\n\n' + \
'Based on the Py module of wxPython:\n' + \
'  Shell Revision: %s\n' % self.Shell.revision + \
'  Interpreter Revision: %s\n\n' % self.Shell.interp.revision + \
'  Platform: %s\n' % sys.platform + \
'  Python Version: %s\n' % sys.version.split()[0] + \
'  wxPython Version: %s\n' % wx.VERSION_STRING

        dialog = wx.MessageDialog(self, text, title, wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnUpdateMenu(self, event):
        win = wx.Window_FindFocus()
        id = event.GetId()
        event.Enable(True)
        try:
            if id == wx.ID_NEW:
                event.Enable(hasattr(self.EditorNotebook, 'bufferNew'))
            elif id == wx.ID_OPEN:
                event.Enable(hasattr(self.EditorNotebook, 'bufferOpen'))
            elif id == wx.ID_REVERT:
                event.Enable(hasattr(self.EditorNotebook, 'bufferRevert') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_CLOSE:
                event.Enable(hasattr(self.EditorNotebook, 'bufferClose') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_SAVE:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSave') and self.EditorNotebook.bufferHasChanged())
            elif id == wx.ID_SAVEAS:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer())
            #elif id == wx.ID_NAMESPACE:
            #    event.Enable(hasattr(self.EditorNotebook, 'updateNamespace') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_PRINT:
                event.Enable(hasattr(self.EditorNotebook, 'bufferPrint') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_UNDO:
                event.Enable(win.CanUndo())
            elif id == wx.ID_REDO:
                event.Enable(win.CanRedo())
            elif id == wx.ID_CUT:
                event.Enable(win.CanCut())
            elif id == wx.ID_COPY:
                event.Enable(win.CanCopy())
            elif id == wx.ID_PASTE:
                event.Enable(win.CanPaste())
            elif id == wx.ID_SELECTALL:
                event.Enable(hasattr(win, 'SelectAll'))
            elif id == self.ID_RUNINSHELL:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer())
            # Don't allow the "Run in New Thread" option when embedded
            # (otherwise, tries to start a new copy of the entire program).
            elif id == self.ID_RUNINNEWPROCESS:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer() and not ireEmbedded)
            elif id == self.ID_TASKTREE:
                event.Enable(ireEmbedded)  # Really, should check for existence of TaskManager
            elif id == self.ID_LOAD_CISSTCOMMON:
                event.Enable(ModuleAvailable('cisstCommonPython'))
            elif id == self.ID_LOAD_CISSTOSABSTRACTION:
                event.Enable(ModuleAvailable('cisstOSAbstractionPython'))
            elif id == self.ID_LOAD_CISSTVECTOR:
                event.Enable(ModuleAvailable('cisstVectorPython'))
            elif id == self.ID_LOAD_CISSTNUMERICAL:
                event.Enable(ModuleAvailable('cisstNumericalPython'))
            elif id == self.ID_LOAD_CISSTMULTITASK:
                event.Enable(ModuleAvailable('cisstMultiTaskPython'))
            elif id == self.ID_LOAD_CISSTPARAMETERTYPES:
                event.Enable(ModuleAvailable('cisstParameterTypesPython'))
            else:
                event.Enable(False)
        except AttributeError:
            event.Enable(False)

# ModuleAvailable:  returns true if the module 'name' is present on the path.
def ModuleAvailable(name):
    try:
       imp.find_module(name)
    except ImportError:
       return False
    return True

def launchIrePython():

    if wx.VERSION[0] <= 2 and wx.VERSION[1] < 5:
        print 'IRE requires wxPython version 2.5 or greater'
        print 'Currently installed version is ', wx.VERSION_STRING
    else:
        print "starting window "
        app = wx.PySimpleApp()
        try:
            frame = ireMain(None, -1, "CISST Interactive Research Environment")
        except Exception, e:
            print e
        try:
            # Load cisstCommon by default (if IRE embedded)
            if ireEmbedded:
                frame.ImportCisstCommon()
            # Run startup string (if provided)
            if len(sys.argv) > 1 and sys.argv[1]:
                frame.Shell.push(sys.argv[1])
        except Exception, e:
            print e
        frame.Show(True)
        lastcmd = frame.CommandHistoryListCtrl.list.GetItemCount()-1
        frame.CommandHistoryListCtrl.list.EnsureVisible( lastcmd  )
        # Redraw the left panel.  This fixes some minor display problems
        # (e.g., stray characters) on some platforms.
        frame.LeftBoxSizer.Layout()
        if ireEmbedded:
            ireLogger.SetActiveFlag(True)
        app.MainLoop()
        if ireEmbedded:
            ireLogger.SetActiveFlag(False)

