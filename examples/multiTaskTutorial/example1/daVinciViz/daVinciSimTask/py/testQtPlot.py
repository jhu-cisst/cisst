#!/usr/bin/env python
# -*- mode: python; coding: utf-8-unix; -*-
'''
Author(s): Tian Xia
Created on: 2010-03-01

'''
import csv 
import sys
import random 
import time 

# requires numpy 
import numpy as np

# requires matplotlib 
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from mpl_toolkits.mplot3d import Axes3D 

# requires pyqt
from PyQt4.QtGui import * 
from PyQt4.QtCore import *

# requires cisst 
'''
import cisstCommonPython as cmn 
import cisstVectorPython as vct
import cisstMultiTaskPython as mts
#import cisstParameterTypesPython as prm 
#import cisstDevicesPython as dev
'''
# -----------------------------------------------------------------------------
def embed_ipython(w):
    from IPython.Shell import IPShellEmbed
    ipshell = IPShellEmbed(user_ns = dict(w = w))
    ipshell()

# -----------------------------------------------------------------------------
class Tracker():
    def __init__(self): 
        self.is_tracking = False 

    def __del__(self): 
        print 'del' 

    # left/right master/slave robots, cameras, etc
    # add_device will add the left/right master/slave robots as four devices 
    # each devices will have corresponding signals to be plotted
    def add_device(self, device): 
        print 'add device'
 
    # TODO: do task manager related init here ? 
    def initialize(self): 
        print 'initialize' 

    # update enabled signals from all devices
    # a signal can be enabled, or disabled 
    # a signal is a vector of joints or a cartesian frame
    # each joint can be enabled individually for plotting
    def update_signals(self, all_devices): 
        print 'get positions' 
        print 'get frames' 
        
class Canvas(FigureCanvas):

    def __init__(self): 
        self.figure = Figure()
        FigureCanvas.__init__(self, self.figure) 

        self.figure.add_subplot(2,1,1) 
        self.figure.add_subplot(2,1,2) 

        self.axes = self.figure.get_axes() 
        self.background1 = self.copy_from_bbox(self.axes[0].bbox)
        self.background2 = self.copy_from_bbox(self.axes[1].bbox)
        
        ################# TEMP ###############################################

        self.cnt = 0
        self.x = np.arange(0, 2 * np.pi, 0.01) 
        self.sin_line, = self.axes[0].plot(self.x, np.sin(self.x), animated = True) 
        self.cos_line, = self.axes[1].plot(self.x, np.cos(self.x), animated = True) 
        #self.sin_line.set_animated(True) 
        #self.cos_line.set_animated(True)
        self.draw() 
        
        ######################################################################

        # TODO: track is not right, just temporary, will have multiple plots... 
        #self.tracker, = self.axes[0].plot([], [], '.')
        #self.tracker.set_animated(True)
        #self.clear() 


    def clear(self): 
        self.axes.clear() 
        #self.axes[1].clear() 
        self.axes.grid(True)
        #self.axes[1].grid(True)
        self.draw()
        #self.axes[0].set_xlim(-500, 500)
        #self.axes[0].set_ylim(-500, 500)

    # TODO: update tracker     
    def update_tracker(self, position): 
        self.restore_region(self.background1, bbox=self.axes[0].bbox) 
        self.restore_region(self.background2, bbox=self.axes[1].bbox) 

        # dummy sine/cos position
        # update the data 
        self.sin_line.set_xdata(np.sin(self.x))
        self.cos_line.set_xdata(np.cos(self.x)) 
        
        # draw just the animated artist (animated region only) 
        self.axes[0].draw_artist(self.sin_line) 
        self.axes[1].draw_artist(self.cos_line) 
        self.blit(self.axes[0].bbox) 
        self.blit(self.axes[1].bbox) 

        self.cnt+=1 

        #self.tracker.set_xdata(position[0,3])
        #self.tracker.set_ydata(position[1,3])
        #self.axes[0].draw_artist(self.tracker)
        #self.blit(self.axes[0].bbox)
        
class MainWindow(QMainWindow): 

    def __init__(self): 
        QMainWindow.__init__(self) 

        self.setWindowTitle("Signal Visualizer") 
        self.create_widgets()
        self.setup_widgets()
        self.connect_signals()
        self.create_menu()
        self.create_layout()
        #self.tstart = time.time()
        #self.startTimer(10)

    def create_widgets(self):
        self.act_quit = QAction('Quit', self)
        self.act_fullscreen = QAction('Fullscreen', self)
        self.act_about = QAction('About', self)
        self.btn_track = QPushButton('Track')
        #self.btn_analyze = QPushButton('Analyze')
        #self.vtk_widget = QVTKInteractor()
        #self.canvas = Canvas()
        self.sizeLabel = QLabel() 
        self.sizeLabel.setFrameStyle(QFrame.StyledPanel|QFrame.Sunken)
        self.status = self.statusBar() 
        
    def setup_widgets(self):
        self.act_quit.setShortcut('Ctrl+Q')
        self.act_fullscreen.setCheckable(True)
        self.act_fullscreen.setShortcut('Ctrl+F')
        self.btn_track.setCheckable(True)
        #self.vtk_widget.setup()
        self.status.setSizeGripEnabled(False) 
        self.status.addPermanentWidget(self.sizeLabel) 
        self.status.showMessage("Ready") 

    def connect_signals(self):
        self.act_quit.triggered.connect(self.close)
        self.act_fullscreen.triggered[bool].connect(self.fullscreen)
        self.act_about.triggered.connect(qApp.aboutQt)
        #self.btn_track.toggled[bool].connect(self.toggle_tracker)
        #self.btn_analyze.clicked.connect(self.analyze)

    def create_menu(self):
        mnu_file = self.menuBar().addMenu('&File')
        mnu_file.addAction(self.act_quit)

        mnu_view = self.menuBar().addMenu('&View')
        mnu_view.addAction(self.act_fullscreen)

        mnu_help = self.menuBar().addMenu('Help')
        mnu_help.addAction(self.act_about)

    def create_layout(self):
        lyt_main = QGridLayout()
        #lyt_main.addWidget(self.vtk_widget, 0, 0, 1, 4)
        #lyt_main.addWidget(self.canvas, 1, 0, 1, 4)
        lyt_main.setColumnStretch(0, 1)
        lyt_main.addWidget(self.btn_track, 2, 1, 1, 1)
        #lyt_main.addWidget(self.btn_analyze, 2, 2, 1, 1)
        lyt_main.setColumnStretch(3, 1)

        wgt_main = QWidget()
        wgt_main.setLayout(lyt_main)
        self.setCentralWidget(wgt_main)

    def timerEvent(self, event):
        #position = self.tracker.get_position(TRACKER, 'Pointer')
        #self.vtk_widget.update_scene(position)
        position = 0
        self.canvas.update_tracker(position)
        
    def fullscreen(self, state):
        if state:
            self.showFullScreen()
        else:
            self.showNormal()
    
    #def toggle_tracker(self, toggle):
    #    if not self.is_initialized:
    #        self.is_initialized = True
    #        self.tracker.add_tracker(TRACKER)
    #        self.tracker.initialize()
    #        
    #    self.tracker.toggle_tracking(toggle)
    #    if toggle:
    #        self.timer = self.startTimer(20)
    #    else:
    #        self.killTimer(self.timer)

# -----------------------------------------------------------------------------
# main script 
'''
task_manager = cmn.cmnObjectRegister.FindObject("LocalManager") 
print task_manager 
'''
app = QApplication(sys.argv)
'''
# simple pyqt4 button
button = QPushButton("Hellow World", None) 
button.show() 
embed_ipython(button) 
'''
window = MainWindow() 
window.show() 


# embedding ipython in gui app 
# see http://ipython0.wordpress.com/2008/05/15/embedding-ipython-in-gui-apps-is-trivial/
#embed_ipython(window) 
app.exec_() 
#sys.exit(app.exec_())

'''
arr_a = np.array([1,2,3])
arr_c = np.hstack((arr_a, arr_b)) # of same row dimension, or can use concatenate  


lcm = mts.mtsManagerLocal_GetInstance()
lcm.GetNamesOfComponents()
disp = lcm.GetComponent('DISP:DISPProxy')
disp_provided = disp.GetProvidedInterface('Provided')
GetDouble = disp_provided.GetCommandRead('GetDouble')
a = GetDouble()
a.Data
disp_provided.GetNamesOfCommands()
ZeroAll = disp_provided.GetCommandVoid('ZeroAll')
'''
