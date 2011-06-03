from RobotComponents import *
from ODE             import *
from OSG             import *

cmnLogger.SetMask( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskFunction( CMN_LOG_ALLOW_ALL )
cmnLogger.SetMaskDefaultLog( CMN_LOG_ALLOW_ALL )

world = ODEWorld()

cameara = OSGMono( 'camera', world )

Rtw0 = vctFrm3()
tw0  = Rtw0.Translation();
tw0[2] = 0.1;
BH = ODEBH( 'gripper', world, Rtw0 )

# create the trajectory + setpoint generator
BHqinit = array( [0.00, 0.00, 0.00, 0.00 ] )
BHqdmax = array( [0.10, 0.10, 0.10, 0.10 ] )
BHsetpoints = SetPoints( 'BHsetpoints', BHqinit );
BHtrajectory = TrajectoryJoints( 'BHtrajectory', BHqinit, BHqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( BHsetpoints.GetName(),  devSetPoints.Output,
                     BHtrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( BHtrajectory.GetName(), devTrajectory.Output,
                     BH.GetName(),           devManipulator.Input );

StartTasks()

print 'ENTER to move'
stdin.readline()

q = array( [1.00, 1.00, 1.00, 1.00 ] )
BHsetpoints.Insert( q )
BHsetpoints.Latch()

print 'ENTER to exit.'
stdin.readline()

StopTasks()

