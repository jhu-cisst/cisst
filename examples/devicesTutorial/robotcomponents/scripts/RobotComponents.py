from   numpy import *

from cisstCommonPython    import *;
from cisstMultiTaskPython import *;
from cisstVectorPython    import *;
from cisstDevicesPython   import *;

from sys import *

def StartTasks():
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance();
    # Create and start all the thread
    taskManager.CreateAll();
    taskManager.StartAll();

def StopTasks():
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance();
    # Stop everything
    taskManager.KillAll()
    taskManager.Cleanup()

def TrajectoryJoints( name, qinit, qdmax, period = 0.01 ):
    trajectory = devLinearRn( name,
                              period,
                              devRobotComponent.ENABLED,
                              OSA_CPU3,
                              devTrajectory.QUEUE,
                              devLinearRn.POSITION,
                              qinit,
                              qdmax );
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( trajectory )
    return trajectory


def TrajectoryInverseKinematics( name,      # task name
                                 qinit,     # initial joint positions
                                 robfile,   # file with kinematics
                                 Rtw0,      # base transformation
                                 period = 0.01 ):

    trajectory = devInverseKinematics( name,
                                       period,
                                       devTrajectory.ENABLED,
                                       OSA_CPUANY,
                                       devTrajectory.QUEUE,
                                       qinit,
                                       0.1,       # linear velocity
                                       0.1,       # angular velocity
                                       robfile,
                                       Rtw0 )

    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( trajectory )
    return trajectory

def SetPoints( name, dim ):
    # Create setpoints
    setpoints = devSetPoints( name, dim )
    # Get the task manager
    taskManager = mtsManagerLocal.GetInstance()
    # Add he camera task
    taskManager.AddComponent( setpoints )
    return setpoints

def CreateWAM( wamname, rtcan, qinit ):

    WAM = devWAM( wamname, 0.002, OSA_CPU4, rtcan, qinit )

    return WAM

def CreateGC( gcname, robfile ):

    Rtw0 = vctFrm3()
    Rw0 = Rtw0.Rotation()
    Rw0[0][0] = 0.0; Rw0[0][1] = 0.0; Rw0[0][2] = -1.0;
    Rw0[1][0] = 0.0; Rw0[1][1] = 1.0; Rw0[1][2] =  0.0;
    Rw0[2][0] = 1.0; Rw0[2][1] = 0.0; Rw0[2][2] =  0.0;

    GC = devGravityCompensation( gcname,
                                 0.002,
                                 devController.DISABLED,
                                 OSA_CPU2,
                                 robfile,
                                 Rtw0 );

    taskManager = mtsManagerLocal.GetInstance()
    taskManager.AddComponent( GC )

    return GC

def CreatePDGC( pdgcname, robfile ):

    Rtw0 = vctFrm3()
    Rw0 = Rtw0.Rotation()
    Rw0[0][0] = 0.0; Rw0[0][1] = 0.0; Rw0[0][2] = -1.0;
    Rw0[1][0] = 0.0; Rw0[1][1] = 1.0; Rw0[1][2] =  0.0;
    Rw0[2][0] = 1.0; Rw0[2][1] = 0.0; Rw0[2][2] =  0.0;

    Kp = zeros( [7, 7] )
    Kd = zeros( [7, 7] )

    Kp[0][0] = 80;    Kd[0][0] = 1.0;
    Kp[1][1] = 80;    Kd[1][1] = 1.0;
    Kp[2][2] = 50;    Kd[2][2] = 1.5;
    Kp[3][3] = 30;    Kd[3][3] = 1.5;
    Kp[4][4] =  5;    Kd[4][4] = 0.5;
    Kp[5][5] =  5;    Kd[5][5] = 0.5;
    Kp[6][6] =  1;    Kd[6][6] = 0.1;

    PDGC = devPDGC( pdgcname,
                    0.002,
                    devController.DISABLED,
                    OSA_CPU2,
                    robfile,
                    Rtw0,
                    Kp,
                    Kd);

    taskManager = mtsManagerLocal.GetInstance()
    taskManager.AddComponent( PDGC )

    return PDGC

def CreateBH( bhname, rtcan ):
    BH = devBH8_280( bhname, 0.1, OSA_CPU4, rtcan  )
    return BH

def CreateTrajectory( name, period, qinit, qdmax ):
    trajectory = devLinearRn( name,
                              period,
                              devRobotComponent.ENABLED,
                              OSA_CPU2,
                              devTrajectory.QUEUE,
                              devLinearRn.POSITION,
                              qinit,
                              qdmax )
    taskManager = mtsManagerLocal.GetInstance()
    taskManager.AddComponent( trajectory )
    return trajectory

def CreateSetPoints( name, qinit ):
    setpoints = devSetPoints( name, qinit )
    taskManager = mtsManagerLocal.GetInstance()
    taskManager.AddComponent( setpoints )
    return setpoints
    
