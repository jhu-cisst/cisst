#
# $Id$
#
# To be executed by multiTaskTutorialExample4
#
from cisstCommonPython import *
from cisstMultiTaskPython import *

taskManager = cmnObjectRegister.FindObject("TaskManager")
taskManager.UpdateFromC()

sineData = taskManager.SIN.MainInterface.GetData
SetAmplitude = taskManager.SIN.MainInterface.SetAmplitude
