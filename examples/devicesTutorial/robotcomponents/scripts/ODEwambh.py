from RobotComponents import *
from ODE             import *
from OSG             import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

world = ODEWorld()

cameara = OSGMono( 'camera', world )

Rtw0 = vctFrm3()
Rw0  = Rtw0.Rotation();
Rw0[0][0] = 0.0; Rw0[0][1] = 0.0; Rw0[0][2] = -1.0;
Rw0[1][0] = 0.0; Rw0[1][1] = 1.0; Rw0[1][2] =  0.0;
Rw0[2][0] = 1.0; Rw0[2][1] = 0.0; Rw0[2][2] =  0.0;
tw0  = Rtw0.Translation();
tw0[0] = 0.75;
tw0[2] = 1.0;

[ wam, bh ] = ODEWAMBH( 'WAM', 'BH', world, Rtw0 )

# create the trajectory + setpoint generator
wamqinit = array( [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00] )
wamqdmax = array( [0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01] )
wamsetpoints = SetPoints( 'WAMsetpoints', wamqinit );
wamtrajectory = TrajectoryJoints( 'WAMtrajectory', wamqinit, wamqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( wamsetpoints.GetName(),  devSetPoints.Output,
                     wamtrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( wamtrajectory.GetName(), devTrajectory.Output,
                     wam.GetName(),           devManipulator.Input );

# create the trajectory + setpoint generator
bhqinit = array( [0.00, 0.00, 0.00, 0.00 ] )
bhqdmax = array( [0.10, 0.10, 0.10, 0.10 ] )
bhsetpoints = SetPoints( 'BHsetpoints', bhqinit );
bhtrajectory = TrajectoryJoints( 'BHtrajectory', bhqinit, bhqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( bhsetpoints.GetName(),  devSetPoints.Output,
                     bhtrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( bhtrajectory.GetName(), devTrajectory.Output,
                     bh.GetName(),           devManipulator.Input );

StartTasks()

print 'ENTER to move'
stdin.readline()

wamq = array( [1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00] )
wamsetpoints.Insert( wamq )
wamsetpoints.Latch()

bhq = array( [2.00, 2.00, 2.00, 2.00 ] )
bhsetpoints.Insert( bhq )
bhsetpoints.Latch()

print 'ENTER to exit.'
stdin.readline()

StopTasks()

