#
# $Id: example4.py 75 2009-02-24 16:47:20Z adeguet1 $
#
# To be executed by multiTaskTutorialExample4
#
from cisstCommonPython import *
from cisstMultiTaskPython import *

taskManager = cmnObjectRegister.FindObject("TaskManager")
taskManager.UpdateFromC()

sineData = taskManager.SIN.MainInterface.GetCommandRead('GetData')
