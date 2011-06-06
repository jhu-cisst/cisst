from RobotComponents import *
from ODE             import *
from OSG             import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

# create the world
world = ODEWorld()

# create the camera
cameara = OSGMono( 'camera', world )

# create the WAM
Rtw0 = vctFrm3()
Rw0  = Rtw0.Rotation();
Rw0[0][0] = 0.0; Rw0[0][1] = 0.0; Rw0[0][2] = -1.0;
Rw0[1][0] = 0.0; Rw0[1][1] = 1.0; Rw0[1][2] =  0.0;
Rw0[2][0] = 1.0; Rw0[2][1] = 0.0; Rw0[2][2] =  0.0;
tw0  = Rtw0.Translation();
tw0[0] = 0.75;
tw0[2] = 1.0;
wam = ODEWAM( 'WAM', world, Rtw0 )

# create the trajectory + setpoint generator
wamqinit = array( [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00] )
wamqdmax = array( [0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10] )
wamsetpoints = SetPoints( 'WAMsetpoints', wamqinit );
wamtrajectory = TrajectoryJoints( 'WAMtrajectory', wamqinit, wamqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( wamsetpoints.GetName(),  devSetPoints.Output,
                     wamtrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( wamtrajectory.GetName(), devTrajectory.Output,
                     wam.GetName(),           devManipulator.Input );

StartTasks()

print 'ENTER to move'
stdin.readline()

q = array( [1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00] )
wamsetpoints.Insert( q )
wamsetpoints.Latch()

print 'ENTER to exit.'
stdin.readline()

StopTasks()

