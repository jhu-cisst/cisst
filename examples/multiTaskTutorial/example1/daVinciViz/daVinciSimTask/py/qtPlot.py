#!/usr/bin/env python
# -*- mode: python; coding: utf-8-unix; -*-

'''
Author(s): Tian Xia
Created on: 2010-03-01

'''

# define FILE NAME and PATH 
#example CT_FIDUCIALS = '/Users/auneri/Dropbox/MATLAB/CT_20100107.fcsv'

import csv 
import sys
import random 

# requires numpy 
import numpy as np

# requires matplotlib 
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas 
from matplotlib.figure import Figure 
from mpl_toolkits.mplot3d import Axes3D 

# requires pyqt
from PyQt4.QtGui import * 
from PyQt4.QtCore import *

#from PyQt4.QtCore import (Qt, SIGNAL)
#from PyQt4.QtGui import (QApplication, QComboBox, QDialog,
#        QDoubleSpinBox, QGridLayout, QLabel)

# requires cisst 
import cisstCommonPython as cmn 
import cisstVectorPython as vct
import cisstMultiTaskPython as mts
import cisstParameterTypesPython as prm 
import cisstDevicesPython as dev

# -----------------------------------------------------------------------------
# Setup mtsTaskManager 
# Process signals of joints/actuators values and cartesian position and orientation 
class Tracker():
    def __init__(self): 
        self.is_tracking = False 
        
    #def __del__(self): 

    def initialize(self): 
        task_manager = mts.mtsTaskManager.GetInstance()
        
        
# -----------------------------------------------------------------------------
# TODO: what to do about sys.argv? 
application = QApplication([''])
window = MainWindow() 
window.show() 
#form = Form()
#form.show()
application.exec_()

