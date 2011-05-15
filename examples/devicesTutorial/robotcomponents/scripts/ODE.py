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

def ODEWorld( period=0.001 ):
    odeworld = devODEWorld( period, OSA_CPU1 )
    taskManager = mtsManagerLocal.GetInstance();
    taskManager.AddComponent( odeworld )
    return odeworld

def ODEWAM( name, world, Rtw0, period=0.002 ):
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
    odewam = devODEManipulator( name,
                                period,
                                devRobotComponent.ENABLED,
                                OSA_CPU2,
                                world,
                                devManipulator.POSITION,
                                rob_model,
                                Rtw0,
                                qinit,
                                cad_models,
                                cad_base )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( odewam )
    return odewam

def ODEBH( name, world, Rtw0, period=0.002 ):
    cisst_source_root = os.environ['CISST_SOURCE_ROOT']
    odebh = devODEBH( name,
                      period,
                      devRobotComponent.ENABLED,
                      OSA_CPU2,
                      world,
                      devManipulator.POSITION,
                      cisst_source_root+'/libs/etc/cisstRobot/BH/f1f2.rob',
                      cisst_source_root+'/libs/etc/cisstRobot/BH/f3.rob',
                      Rtw0,
                      cisst_source_root+'/libs/etc/cisstRobot/BH/l0.obj',
                      cisst_source_root+'/libs/etc/cisstRobot/BH/l1.obj',
                      cisst_source_root+'/libs/etc/cisstRobot/BH/l2.obj',
                      cisst_source_root+'/libs/etc/cisstRobot/BH/l3.obj' )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( odebh )
    return odebh

def ODEWAMBH( wamname, bhname, world, Rtw0, period=0.002 ):

    WAM = ODEWAM( wamname, world, Rtw0, period )

    Rtwh = vctFrm3()
    q = array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    WAM.ForwardKinematics( q, Rtwh )

    BH = ODEBH( bhname, world, Rtwh, period );

    WAM.Attach( BH );

    return [ WAM, BH ]


def ODELWR( name, world, Rtw0, period=0.002 ):
#    cisst_source_root = os.environ['CISST_SOURCE_ROOT']
    cisst_source_root = "E:\\src\\cisst"
#    cisst_source_root = "E:\\src\\cisst"
    cad_base   =   cisst_source_root+'/libs/etc/cisstRobot/LWR/l0.obj'
    cad_models = [ cisst_source_root+'/libs/etc/cisstRobot/LWR/l1.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l2.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l3.obj',
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l4.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l5.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l6.obj', 
                   cisst_source_root+'/libs/etc/cisstRobot/LWR/l7.obj' ]
    rob_model = cisst_source_root+'/libs/etc/cisstRobot/LWR/lwr.rob'
    qinit = array([ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ])
    odelwr = devODEManipulator( name,
                                period,
                                devRobotComponent.ENABLED,
                                OSA_CPU2,
                                world,
                                devManipulator.POSITION,
                                rob_model,
                                Rtw0,
                                qinit,
                                cad_models,
                                cad_base )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( odelwr )
    return odelwr

def ODEGripper( name, world, Rtw0, period=0.002 ):
    cisst_source_root = os.environ['CISST_SOURCE_ROOT']
    odegrp=devODEGripper( name,
                          period,
                          devRobotComponent.ENABLED,
                          OSA_CPU2,
                          world,
                          devManipulator.POSITION,
                          cisst_source_root+'/libs/etc/cisstRobot/gripper/f1.rob',
                          cisst_source_root+'/libs/etc/cisstRobot/gripper/f2.rob',
                          Rtw0,
                          cisst_source_root+'/libs/etc/cisstRobot/gripper/l0.obj',
                          cisst_source_root+'/libs/etc/cisstRobot/gripper/l1.obj',
                          cisst_source_root+'/libs/etc/cisstRobot/gripper/l1.obj' )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( odegrp )
    return odegrp

def ODELWRGripper(lwrname, grpname, world, Rtw0, per=0.002):

    LWR = ODELWR( lwrname, world, Rtw0, per )

    Rtwh = vctFrm3()
    q = array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    LWR.ForwardKinematics( q, Rtwh )

    gripper = ODEGripper( grpname, world, Rtwh, per );

    LWR.Attach( gripper );

    return [ LWR, gripper ]


