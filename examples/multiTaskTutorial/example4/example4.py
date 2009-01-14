#
# $Id: example4.py,v 1.2 2008/01/22 03:32:18 pkaz Exp $
#
# To be executed by multiTaskTutorialExample4
#
from cisstCommonPython import *
from cisstMultiTaskPython import *

taskManager = cmnObjectRegister.FindObject("TaskManager")
taskManager.UpdateFromC()

sineData = taskManager.SIN.MainInterface.GetCommandRead('GetData')
