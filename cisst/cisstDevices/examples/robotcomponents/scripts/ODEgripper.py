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
grp = ODEGripper( 'gripper', world, Rtw0 )

# create the trajectory + setpoint generator
grpqinit = array( [ 0.00 ] )
grpqdmax = array( [ 0.01 ] )
grpsetpoints = SetPoints( 'GRPsetpoints', grpqinit );
grptrajectory = TrajectoryJoints( 'GRPtrajectory', grpqinit, grpqdmax )

taskManager = mtsManagerLocal.GetInstance();

taskManager.Connect( grpsetpoints.GetName(),  devSetPoints.Output,
                     grptrajectory.GetName(), devTrajectory.Input );
taskManager.Connect( grptrajectory.GetName(), devTrajectory.Output,
                     grp.GetName(),           devManipulator.Input );

StartTasks()

print 'ENTER to move'
stdin.readline()

q = array( [ 1.0 ] )
grpsetpoints.Insert( q )
grpsetpoints.Latch()

print 'ENTER to exit.'
stdin.readline()

StopTasks()

