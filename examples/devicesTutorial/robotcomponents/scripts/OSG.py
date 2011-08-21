import os
from   numpy import *
import numpy.linalg
import numpy.random

from cv import *;

from cisstCommonPython    import *;
from cisstMultiTaskPython import *;
from cisstVectorPython    import *;
from cisstDevicesPython   import *;

from sys import *

def OSGWorld():
    # Create the world (OSG node)
    osgworld = devOSGWorld()
    return osgworld

def OSGMono(camname, world, w=640, h=480, fov=55.0, Zn=0.1, Zf=10.0, x=0, y=0):
    aspect = double( w ) / double( h )
    # Create the camera thread
    osgmono = devOSGMono( camname, world, x, y, w, h, fov, aspect, Zn, Zf )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( osgmono )
    return osgmono


def OSGWAM( name, world, Rtw0, period=0.002 ):
    cisst_source_root = os.environ['CISST_SOURCE_ROOT']
    cad_base   =   cisst_source_root+'/libs/etc/cisstRobot/WAM/l0.obj'
    cad_models = [ cisst_source_root+'/libs/etc/cisstRobot/WAM/l1.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l2.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l3.obj',
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l4.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l5.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l6.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/WAM/l7.obj' ]
    rob_model = cisst_source_root+'/libs/etc/cisstRobot/WAM/wam7.rob'
    qinit = array([ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ])
    osgwam = devOSGManipulator( name,
                                period,
                                devRobotComponent.ENABLED,
                                OSA_CPU2,
                                world,
                                rob_model,
                                Rtw0,
                                qinit,
                                cad_models,
                                cad_base )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( osgwam )
    return osgwam

